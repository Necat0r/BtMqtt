#pragma once

#include <string>
#include <vector>

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
