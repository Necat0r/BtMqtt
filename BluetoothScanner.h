#pragma once

#include <string>
#include <vector>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <ws2bth.h>

class BluetoothScanner
{
public:
	struct DeviceInfo
	{
		DeviceInfo(const char* name, bool inRange);

		std::string name;
		bool inRange;
	};
	using DeviceInfos = std::vector<DeviceInfo>;

	BluetoothScanner();
	~BluetoothScanner();

	DeviceInfos scan();
};
