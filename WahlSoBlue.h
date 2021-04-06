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
	int discoverServices(BTDevice*& devices, std::string addrIn, std::string guidstr);

	void printDevice(BTDevice& device);

	class Client {
	public:
		Client();
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
		char messageBuffer[MAX_MESSAGE_LENGTH_CLIENT + 1];

		void recvThread();

	};

	class Server {
	public:
		Server(std::string _name, std::string _uuid, std::string _description = "", int numberOfConnections = 1, int _port = BT_PORT_ANY);
		~Server();

		bool startAdvertising();
		bool stopAdvertising();
		bool startListening();
		void stopListening();

		int getNumberOfConnections();
		int getConnections(int*& targets);

		bool sendMessage(std::string message, int target);

		std::string getNextMessage(int target);
		int getNumberOfMessages(int target);

		bool disconnect(int target);

		BTDevice getClientInfo(int target);

	private:

		std::string name = "";
		std::string uuidstr = "";
		std::string description;
		GUID uuid;
		int port = 0;
		bool readyForConnections;
		bool listening;
		int maxConnections;

		SOCKET listeningSocket;
		SOCKADDR_BTH listeningInfo;
		CSADDR_INFO sockInfo;
		WSAQUERYSET svcInfo = { 0 };
		void setCSADDR_INFOAdvert();
		void setWSAQUERYSETAdvert();
		
		bool advertising;

		int clientCount = 0;
		SOCKET* clientSockets = nullptr;
		SOCKADDR_BTH* clientInfo = nullptr;
		char** messageBuffers = nullptr;
		char* messageBufferChars = nullptr;
		bool* connected = nullptr;
		std::queue<std::string>* messages = nullptr;

		void recvThread(int target);
		void listenThread();

		bool connectionExists(int target);

		bool setUpListeningSocket();
		bool establishConnection(SOCKET& client, SOCKADDR_BTH& ca);

		


	};

}

