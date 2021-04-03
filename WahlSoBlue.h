// WahlSoBlue.h : Include file for standard system include files,
// or project specific include files.

#pragma once

// For the unique ID
#include <initguid.h>

#include <iostream>
#include <string>
#include <thread>

namespace WahlBlues {
	
	void init();
	void shutdown();

	struct BTDevice {
		std::string address = "";
		int port = 0;
		std::string name = "";
		std::string description = "";
	};

	int discoverDevices(BTDevice* &devices);

	void printDevice(BTDevice device);

	class Client {
		Client();
		~Client();
	};

	class Server {
		Server();
		~Server();
	};

}

