// WahlSoBlue.h : Include file for standard system include files,
// or project specific include files.

#pragma once

// For the unique ID
#include <initguid.h>

#include <iostream>
#include <string>
#include <thread>

#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothApis.h>

#include <queue>

#ifdef MAX_MESSAGE_LENGTH_CLIENT
#else
#define MAX_MESSAGE_LENGTH_CLIENT 1024
#endif

#ifdef MAX_MESSAGE_LENGTH_SERVER
#else
#define MAX_MESSAGE_LENGTH_SERVER 1024
#endif

namespace WahlBlues {
	
	void init();
	void shutdown();

	struct BTDevice {
		std::string address = "";
		int port = 0;
		std::string name = "";
		std::string description = "";
		std::string guid = "";
	};

	int discoverDevices(BTDevice*& devices);
	int discoverDevices(BTDevice* &devices, std::string uuid);

	void printDevice(BTDevice& device);

	class Client {
	public:
		Client(BTDevice& server);
		~Client();

		bool connectToServer();
		bool isReady();
		bool sendMessage(std::string message);
		int getNumberOfMessages();
		std::string getNextMessage();
		bool disconnect();
		bool newServer(BTDevice& server);

	private:
		SOCKET clientSocket;
		void createClientSocket();

		void setServerInfo(BTDevice &server);
		SOCKADDR_BTH serverInfo;
		int serverInfoLength = sizeof(serverInfo);
		
		bool readyToConnect;
		bool connected;

		void startReceiving();
		std::queue<std::string> messages;
		char messageBuffer[MAX_MESSAGE_LENGTH_CLIENT];

		void recvThread();

	};

	class Server {
		Server();
		~Server();
	};

}

