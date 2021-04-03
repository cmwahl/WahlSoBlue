

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
	}

	WahlBlues::shutdown();
	return 0;
}