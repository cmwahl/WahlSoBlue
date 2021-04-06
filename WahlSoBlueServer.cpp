#include "WahlSoBlue.h"
#include "GuidHelper.h"

namespace WahlBlues {
    Server::Server(std::string _name, std::string _uuid, std::string _description, int numberOfConnections, int _port) {

        readyForConnections = true;

        maxConnections = numberOfConnections;

        clientSockets = new SOCKET[numberOfConnections];
        clientInfo = new SOCKADDR_BTH[numberOfConnections];
        messageBuffers = new char* [numberOfConnections];
        messageBufferChars = new char[numberOfConnections * (MAX_MESSAGE_LENGTH_SERVER + 1)];
        connected = new bool[numberOfConnections];
        messages = new std::queue<std::string>[numberOfConnections];
        for (int i = 0; i < numberOfConnections; ++i) {
            messageBuffers[i] = messageBufferChars + i * (MAX_MESSAGE_LENGTH_SERVER + 1);
            connected[i] = false;
            clientSockets[i] = SOCKET_ERROR;
        }



        name = _name;
        uuidstr = _uuid;
        uuid = GuidHelper::StringToGuid(uuidstr);
        description = _description;
        port = _port;
        listening = false;
    }

    Server::~Server() {
        listening = false;
        stopListening();
        advertising = false;

        for (int client = 0; client < maxConnections; ++client) {
            if (connectionExists(client)) {
                disconnect(client);
            }
        }

        delete[] clientSockets;
        delete[] clientInfo;
        delete[] messageBuffers;
        delete[] messageBufferChars;
        delete[] connected;
        delete[] messages;

    }


    bool  Server::startAdvertising() {
        setCSADDR_INFOAdvert();
        setWSAQUERYSETAdvert();
        if (SOCKET_ERROR == WSASetService(&svcInfo, RNRSERVICE_REGISTER, 0)) {
            return false;
        }
        return true;
    }

    bool Server::stopAdvertising() {
        if (SOCKET_ERROR == WSASetService(&svcInfo, RNRSERVICE_DELETE, 0)) {
            return false;
        }

        return true;
    }

    void Server::setCSADDR_INFOAdvert() {
        sockInfo.iProtocol = BTHPROTO_RFCOMM; // Hey, look, RFCOMM!
        sockInfo.iSocketType = SOCK_STREAM;
        sockInfo.LocalAddr.lpSockaddr = (LPSOCKADDR)&listeningInfo;
        sockInfo.LocalAddr.iSockaddrLength = sizeof(listeningInfo);
        sockInfo.RemoteAddr.lpSockaddr = (LPSOCKADDR)&listeningInfo;
        sockInfo.RemoteAddr.iSockaddrLength = sizeof(listeningInfo);
    }

    void Server::setWSAQUERYSETAdvert() {
        svcInfo.dwSize = sizeof(svcInfo);
        svcInfo.dwNameSpace = NS_BTH;
        svcInfo.lpszServiceInstanceName = (LPSTR)name.c_str();
        svcInfo.lpszComment = (LPSTR)description.c_str();
        svcInfo.lpServiceClassId = (LPGUID)&uuid;
        svcInfo.dwNumberOfCsAddrs = 1;
        svcInfo.lpcsaBuffer = &sockInfo;
    }

    bool Server::startListening() {
        bool listeningGood = setUpListeningSocket();
        if (listeningGood) {
            listening = true;
            std::thread x(&Server::listenThread, this);
            x.detach();
        }
        return listeningGood;
    }

    bool Server::setUpListeningSocket() {
        listeningSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        listeningInfo.addressFamily = AF_BTH;
        listeningInfo.btAddr = 0; // Putting 0 allows for dynamic 
        listeningInfo.port = port;
        if (SOCKET_ERROR == listeningSocket) {
            return false;
        }

        if (SOCKET_ERROR == bind(listeningSocket, (const sockaddr*)&listeningInfo, sizeof(SOCKADDR_BTH))) {
            return false;
        }
        return true;
    }

    void Server::listenThread() {
        listen(listeningSocket, maxConnections);
        while (listening) {
            SOCKADDR_BTH ca;
            int caSize = sizeof(SOCKADDR_BTH);
            SOCKET client = accept(listeningSocket, (LPSOCKADDR)&ca, &caSize);
            if (!establishConnection(client, ca)) {
                send(client, "Too many connections!", 21, 0);
                closesocket(client);
            }
        }

    }

    bool Server::establishConnection(SOCKET& client, SOCKADDR_BTH& ca) {
        for (int connection = 0; connection < maxConnections; ++connection) {
            if (!connectionExists(connection)) {
                clientSockets[connection] = client;
                clientInfo[connection] = ca;
                std::thread x(&Server::recvThread, this, connection);
                x.detach();
                ++clientCount;
                connected[connection] = true;
                return true;
            }
        }
        return false;
    }

    void Server::stopListening() {
        listening = false;
        closesocket(listeningSocket);
    }

    void Server::recvThread(int _target) {
        int target = _target;
        int length;
        while (connected[target]) {
            ZeroMemory(messageBuffers[target], MAX_MESSAGE_LENGTH_SERVER);
            length = recv(clientSockets[target], messageBuffers[target], MAX_MESSAGE_LENGTH_SERVER, 0);
            if (length > 0) {
                std::string messageIn = messageBuffers[target];
                messages[target].push(messageIn);
            }
            else {
                disconnect(target);
                break;
            }

        }
    }

    bool Server::disconnect(int target) {
        if (connectionExists(target)) {
            closesocket(clientSockets[target]);
            clientSockets[target] = SOCKET_ERROR;
            connected[target] = false;
            --clientCount;
            return true;
        }
        return false;
    }

    int Server::getNumberOfConnections() {
        return clientCount;
    }

    int Server::getConnections(int*& targets) {
        targets = new int[clientCount];
        int count = 0;
        for (int i = 0; i < maxConnections; ++i) {
            if (connectionExists(i)) {
                targets[count] = i;
                ++count;
                if (count == maxConnections - 1) {
                    break;
                }
            }
        }
        return clientCount;
    }

    std::string Server::getNextMessage(int target) {
        if (connectionExists(target) && messages[target].size()) {
            std::string returnStr = messages[target].front();
            messages[target].pop();
            return returnStr;
        }
        return "";
    }

    int Server::getNumberOfMessages(int target) {
        if (connectionExists(target)) {
            return messages[target].size();
        }
        return -1;
    }

    bool Server::sendMessage(std::string message, int target) {
        if (connectionExists(target)) {
            send(clientSockets[target], message.c_str(), message.length() + 1, 0);
            return true;
        }
        return false;
    }

    bool Server::connectionExists(int target) {
        return connected[target];
    }

    BTDevice Server::getClientInfo(int target) {
        BTDevice client;
        ZeroMemory(&client, sizeof(client));
        if (connectionExists(target)) {
            char buffer[50];
            DWORD bufferSize = sizeof(buffer);
            ZeroMemory(buffer, sizeof(buffer));
            WSAAddressToString((LPSOCKADDR)(clientInfo + target), sizeof(SOCKADDR_BTH), NULL, buffer, &bufferSize);
            client.address = buffer;
            client.port = clientInfo[target].port;
        }
        return client;

    }
}