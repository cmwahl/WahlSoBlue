#include "WahlSoBlue.h"
#include "GuidHelper.h"

namespace WahlBlues {

    Client::Client() {
        readyToConnect = true;
        connected = false;
        createClientSocket();

    }

    Client::Client(BTDevice& server) {
        readyToConnect = true;
        connected = false;
        createClientSocket();
        setServerInfo(server);

    }

    void Client::createClientSocket() {
        clientSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        if (SOCKET_ERROR == clientSocket) {
            std::cout << "Failed to create client socket" << std::endl;
            readyToConnect = false;
        }
    }

    void Client::setServerInfo(BTDevice& server) {
        if (SOCKET_ERROR == WSAStringToAddress((LPSTR)server.address.c_str(), AF_BTH, NULL, (LPSOCKADDR)&serverInfo, &serverInfoLength)) {
            std::cout << "Bad address" << std::endl;
            readyToConnect = false;
        }
        serverInfo.port = server.port;
    }

    bool Client::connectToServer() {
        if (readyToConnect && !connected) {

            if (SOCKET_ERROR != connect(clientSocket, (LPSOCKADDR)&serverInfo, serverInfoLength)) {
                startReceiving();
                connected = true;
                return true;
            }
            else {
                std::cout << "Failed to connect to server" << std::endl;
                return false;
            }
        }

        return false;
    }

    void Client::startReceiving() {
        std::thread receiver(&Client::recvThread, this);
        receiver.detach();
    }

    bool Client::isReady() {
        return readyToConnect;
    }

    void Client::recvThread() {
        int length;
        while (connected) {
            ZeroMemory(messageBuffer, sizeof(messageBuffer));
            length = recv(clientSocket, messageBuffer, sizeof(messageBuffer), 0);
            if (length > 0) {
                std::string messageIn = messageBuffer;
                messages.push(messageIn);
            }
            else if (length < 0) {
                disconnect();
            }

        }
    }

    int Client::getNumberOfMessages() {
        return messages.size();
    }

    std::string Client::getNextMessage() {
        std::string messageStr = messages.front();
        messages.pop();
        return messageStr;

    }

    bool Client::sendMessage(std::string message) {
        if (connected) {
            send(clientSocket, message.c_str(), message.length() + 1, 0);
            return true;
        }
        return false;
    }

    Client::~Client() {
        disconnect();
    }

    bool Client::disconnect() {
        if (connected) {
            connected = false;
            closesocket(clientSocket);
            readyToConnect = false;
        }
        return true;
    }

    bool Client::newServer(BTDevice& server) {
        readyToConnect = true;
        createClientSocket();
        setServerInfo(server);
        return readyToConnect;
    }
}