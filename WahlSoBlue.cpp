
#include "WahlSoBlue.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "irprops.lib")
// The WINSOCK headers
#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothApis.h>

#include <queue>

#include "GuidHelper.h"

namespace WahlBlues {
    

	void init() {
		WSADATA wsd;
		if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
			printf("Unable to load Winsock! Error code is %d\n", WSAGetLastError());
			return;
		}
	}

	void shutdown() {
		WSACleanup();
	}

    int discoverServices(BTDevice*& devices, std::string& addr_in, std::string& guidstr) {

        DWORD qs_len = sizeof(WSAQUERYSET);
        WSAQUERYSET* qs = (WSAQUERYSET*)malloc(qs_len);

        //These are the only two fields in the QuerySet struct that matter for a Bluetooth inquiry, must reset set to 0
        ZeroMemory(qs, qs_len);
        qs->dwSize = sizeof(WSAQUERYSET);
        qs->dwNameSpace = NS_BTH;

        std::string piAddr = "DC:A6:32:A3:49:79";
        qs->lpszContext = (LPSTR)addr_in.c_str();
        GUID guid = GuidHelper::StringToGuid(guidstr);
        qs->lpServiceClassId = &guid;
        qs->dwNumberOfCsAddrs = 0;

        DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_ADDR | LUP_RETURN_NAME;
        //DWORD flags = LUP_CONTAINERS; // LUP_CONTAINERS is specified for device discoveries, FLUSHCACHE clears old detected devices, rest are self explanatory
        //flags |= LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_RETURN_COMMENT | LUP_RETURN_TYPE;

        HANDLE h;

        // start the device inquiry
        if (SOCKET_ERROR == WSALookupServiceBegin(qs, flags, &h)) { //WSALookupServiceBegin does the actual Service Discovery Protocol
            return 0;
        }

        std::queue<BTDevice> devicesQueue;

        // iterate through the inquiry results
        bool done = false;
        while (!done) {

            int result = WSALookupServiceNext(h, flags, &qs_len, qs);
            if (NO_ERROR == result) {
                char buf[40] = { 0 };
                DWORD bufsize = sizeof(buf);
                WSAAddressToString(qs->lpcsaBuffer->RemoteAddr.lpSockaddr, sizeof(SOCKADDR_BTH), NULL, buf, &bufsize); // Take the stored 64bit uint address and turn it to a readable string
                //printf("found: %s - %s - Port: %d\nDescr: %s\n", buf, qs->lpszServiceInstanceName, ((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->port, qs->lpszComment);

                std::string address = buf;
                std::string name = qs->lpszServiceInstanceName;
                //std::string descr = "(Null)";
                //std::string uuid = GuidHelper::GuidToString(*(qs->lpServiceClassId));
                //if (qs->lpszComment != nullptr) {
                //    descr = qs->lpszComment;
                //}

                int port = ((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->port;

                BTDevice device = { address, port, name, "", "" };
                devicesQueue.push(device);

            }
            else {
                int error = WSAGetLastError();
                if (error == WSAEFAULT) {
                    free(qs);
                    qs = (WSAQUERYSET*)malloc(qs_len);
                }
                else if (error == WSA_E_NO_MORE) {
                    done = true;
                }
                else {
                    std::cout << "Bad error" << std::endl;
                    done = true;
                }
            }
        }

        WSALookupServiceEnd(h);
        free(qs);

        int numDevices = devicesQueue.size();
        devices = new BTDevice[numDevices];
        for (int i = 0; i < numDevices; ++i) {
            devices[i] = devicesQueue.front();
            devicesQueue.pop();
        }

        return numDevices;

    }

	int discoverDevices(BTDevice* &devices) {

		DWORD qs_len = sizeof(WSAQUERYSET);
		WSAQUERYSET* qs = (WSAQUERYSET*)malloc(qs_len);

		//These are the only two fields in the QuerySet struct that matter for a Bluetooth inquiry, must reset set to 0
		ZeroMemory(qs, qs_len);
		qs->dwSize = sizeof(WSAQUERYSET);
		qs->dwNameSpace = NS_BTH;
		DWORD flags = LUP_CONTAINERS; // LUP_CONTAINERS is specified for device discoveries, FLUSHCACHE clears old detected devices, rest are self explanatory
		flags |= LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_RETURN_COMMENT | LUP_RETURN_TYPE;

		HANDLE h;

        // start the device inquiry
        if (SOCKET_ERROR == WSALookupServiceBegin(qs, flags, &h)) { //WSALookupServiceBegin does the actual Service Discovery Protocol
            return 0;
        }

        std::queue<BTDevice> devicesQueue;

        // iterate through the inquiry results
        bool done = false;
        while (!done) {

            int result = WSALookupServiceNext(h, flags, &qs_len, qs);
            if (NO_ERROR == result) {
                char buf[40] = { 0 };
                DWORD bufsize = sizeof(buf);
                WSAAddressToString(qs->lpcsaBuffer->RemoteAddr.lpSockaddr, sizeof(SOCKADDR_BTH), NULL, buf, &bufsize); // Take the stored 64bit uint address and turn it to a readable string
                //printf("found: %s - %s - Port: %d\nDescr: %s\n", buf, qs->lpszServiceInstanceName, ((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->port, qs->lpszComment);

                std::string address = buf;
                std::string name = qs->lpszServiceInstanceName;
                std::string descr = "(Null)";
                std::string uuid = GuidHelper::GuidToString(*(qs->lpServiceClassId));
                if (qs->lpszComment != nullptr) {
                    descr = qs->lpszComment;
                }

                int port = ((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->port;

                BTDevice device = { address, port, name, descr, uuid };
                devicesQueue.push(device);

            }
            else {
                int error = WSAGetLastError();
                if (error == WSAEFAULT) {
                    free(qs);
                    qs = (WSAQUERYSET*)malloc(qs_len);
                }
                else if (error == WSA_E_NO_MORE) {
                    done = true;
                }
                else {
                    done = true;
                }
            }
        }

        WSALookupServiceEnd(h);
        free(qs);
        
        int numDevices = devicesQueue.size();
        devices = new BTDevice[numDevices];
        for (int i = 0; i < numDevices; ++i) {
            devices[i] = devicesQueue.front();
            devicesQueue.pop();
        }

        return numDevices;

	}

    void printDevice(BTDevice& device) {
        std::cout << "Name: " << device.name << std::endl;
        std::cout << "Description: " << device.description << std::endl;
        std::cout << "Address: " << device.address << std::endl;
        std::cout << "Port: " << device.port << std::endl;
        std::cout << "UUID: " << device.guid << std::endl;
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

    void Client::setServerInfo(BTDevice &server) {
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
            send(clientSocket, message.c_str(), message.length(), 0);
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