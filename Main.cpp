#include "BluetoothScanner.h"
#include "Mqtt.h"

#include <atomic>
#include <iostream>
#include <thread>

static const char* ID = "btmqtt";
static const char* ScanTopic = "btmqtt/scan";			// To trigger a scan
static const char* PresenceTopic = "btmqtt/presence";	// For reporting presence

using namespace std::chrono;
static const auto SleepDuration = seconds(1);
static const auto ScanInterval = minutes(30);
static const auto IntensiveDuration = minutes(1);
static const auto IntensiveInterval = seconds(5);

void printUsing(const char* executable)
{
	printf("Usage:\n");
	printf("%s <mqtt host> <port>", executable);
}

bool scanForInRange(BluetoothScanner& bluetooth)
{
	printf("Scanning!\n");
	for (auto&& device : bluetooth.scan())
	{
		printf("Device: %s, inRange: %s\n", device.name.c_str(), device.inRange ? "true" : "false");
		if (device.inRange)
			return true;
	}

	return false;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Too few arguments: %d\n", argc);
		printUsing(argv[0]);
		return -1;
	}

	const char* hostName = argv[1];
	const int port = atoi(argv[2]);
	if (port == 0)
	{
		printf("Invalid port specified\n");
		return -2;
	}

	printf("Starting bridge\n");

	std::atomic<bool> intensiveScan = false;

	Mqtt::Mappings mappings = {
		{ ScanTopic, [&intensiveScan](const char*, const void*, int)
			{
				printf("Triggering intensive scan\n");
				intensiveScan = true;
			}
		}
	};

	Mqtt mqtt(ID, hostName, port, std::move(mappings));
	BluetoothScanner bluetooth;

	using namespace std::this_thread;
	auto startTime = system_clock::now();
	auto intensiveScanTime = startTime;
	auto nextTime = startTime;

	bool forceSync = true;
	bool wasInRange = false;

	while (true)
	{
		sleep_for(SleepDuration);
		auto nowTime = system_clock::now();

		if (intensiveScan)
		{
			forceSync = true;
			intensiveScan = false;
			intensiveScanTime = nowTime + IntensiveDuration;
			
			// Trigger a scan directly
			nextTime = nowTime;
		}

		if (nowTime < nextTime)
			continue;

		bool inRange = scanForInRange(bluetooth);
		if (inRange != wasInRange || forceSync)
		{
			forceSync = false;
			wasInRange = inRange;

			std::string status = inRange ? "true" : "false";
			mqtt.publish(PresenceTopic, status.c_str(), status.length());
		}

		// Disable intensive scanning if we've found a device
		if (inRange)
			intensiveScanTime = nowTime;

		if (nowTime < intensiveScanTime)
			nextTime = nowTime + IntensiveInterval;
		else
			nextTime = nowTime + ScanInterval;
	}

	return 0;
}
