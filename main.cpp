

#include "WahlSoBlue.h"
#include "GuidHelper.h"
#include <thread>
#include <chrono>
using namespace std;

DEFINE_GUID(SAMPLE_UUID, 0x31b44148, 0x041f, 0x42f5, 0x8e, 0x73, 0x18, 0x6d, 0x5a, 0x47, 0x9f, 0xc9); //31b44148-041f-42f5-8e73-186d5a479fc9


int main()
{
	WahlBlues::init();
	
	std::string guidstr = "{31b44148-041f-42f5-8e73-186d5a479fc9}";
	std::string addr = "";
	
	//cout << "Defined: " << GuidHelper::GuidToString(SAMPLE_UUID) << endl;
	//cout << "String: " << guidstr << endl;
	//cout << "Equivalent: " << (guidstr == GuidHelper::GuidToString(SAMPLE_UUID)) << endl;
	cout << "Equivalent 2: " << (GuidHelper::StringToGuid(guidstr) == SAMPLE_UUID) << endl;

	WahlBlues::Server server("WahlBluesTest", guidstr, "Please show this descr", 1, 4);
	
	server.startListening();
	server.startAdvertising();
	while (server.getNumberOfConnections() == 0) {

	}

	server.sendMessage("Thanks for joining!", 0);
	int count = 0;
	while (count < 10) {
		while (server.getNumberOfMessages(0) == 0 || server.getNumberOfMessages(0) == -1) {

		}

		cout << server.getNextMessage(0) << endl;
		cout << "Connection count: " << server.getNumberOfConnections() << endl;
		server.sendMessage("Now go away", 0);
		int* noob = nullptr;
		server.getConnections(noob);
		cout << "Connections: " << endl;
		for (int i = 0; i < server.getNumberOfConnections(); ++i) {
			cout << noob[i] << " ";
		}
		cout << endl;
		delete[] noob;
		++count;
	}
	

	server.stopListening();
	server.stopAdvertising(); 
	WahlBlues::shutdown();

	return 0;
}