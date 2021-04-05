

#include "WahlSoBlue.h"

using namespace std;

int main()
{
	WahlBlues::init();

	WahlBlues::BTDevice* devices = nullptr;
	int numDevices = WahlBlues::discoverDevices(devices);
	
	if (!numDevices) {
		cout << "No devices found" << endl;
	}

	for (int i = 0; i < numDevices; ++i) {
		cout << "Print device #: " << i << endl;
		WahlBlues::printDevice(devices[i]);
		cout << endl;

		if (devices[i].name == "IEEEPi") {
			cout << "Found pi" << endl;
			devices[i].port = 1;
			WahlBlues::Client myClient = WahlBlues::Client(devices[i]);
			if (myClient.connectToServer()) {
				cout << "We connected to the server" << endl;
				myClient.sendMessage("Whats up");
				cout << "Sent message" << endl;
				while (!myClient.getNumberOfMessages()) {

				}
				cout << "Message received: " << myClient.getNextMessage() << endl;
				myClient.disconnect();
				break;
			}
		}
	}

	WahlBlues::shutdown();
	delete[] devices;
	return 0;
}