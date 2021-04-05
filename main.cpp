

#include "WahlSoBlue.h"
#include "GuidHelper.h"
using namespace std;

DEFINE_GUID(SAMPLE_UUID, 0x31b44148, 0x041f, 0x42f5, 0x8e, 0x73, 0x18, 0x6d, 0x5a, 0x47, 0x9f, 0xc9); //31b44148-041f-42f5-8e73-186d5a479fc9


int main()
{
	WahlBlues::init();

	WahlBlues::BTDevice* devices = nullptr;
	int numDevices = WahlBlues::discoverDevices(devices);

	std::string guidstr = "{31b44148-041f-42f5-8e73-186d5a479fc9}";
	std::string addr = "";
	if (!numDevices) {
		cout << "No devices found" << endl;
	}

	for (int i = 0; i < numDevices; ++i) {
		cout << "Print device #: " << i << endl;
		WahlBlues::printDevice(devices[i]);
		cout << endl;

		if (devices[i].name == "IEEEPi") {
			cout << "Found pi" << endl;
			addr = devices[i].address;
			
		}
	}
	delete[] devices;
	numDevices = WahlBlues::discoverServices(devices, addr, guidstr);
	for (int i = 0; i < numDevices; ++i) {
		cout << "Print device #: " << i << endl;
		WahlBlues::printDevice(devices[i]);
		cout << endl;
	}

	WahlBlues::shutdown();
	delete[] devices;
	return 0;
}