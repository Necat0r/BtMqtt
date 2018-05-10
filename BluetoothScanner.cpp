#define _CRT_SECURE_NO_WARNINGS
#include "BluetoothScanner.h"

#include <bluetoothapis.h>

#pragma comment(lib, "bluetoothapis.lib")
#pragma comment(lib, "ws2_32.lib")

namespace bluetoothScanner
{
	template <typename Lambda>
	void queryDevices(Lambda lambda)
	{
		HBLUETOOTH_DEVICE_FIND hFind;
		BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = { sizeof(searchParams) };
		BLUETOOTH_DEVICE_INFO deviceInfo = { sizeof(deviceInfo) };
		
		searchParams.fReturnAuthenticated = TRUE;
		searchParams.fReturnRemembered = TRUE;
		searchParams.fReturnUnknown = FALSE;
		
		hFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
		if (!hFind)
			return;

		do
		{
			char name[BLUETOOTH_MAX_NAME_SIZE * 2 + 1];
			wcstombs(name, deviceInfo.szName, sizeof(name));

			lambda(name, deviceInfo.Address.ullLong);
		}
		while (BluetoothFindNextDevice(hFind, &deviceInfo));
		
		BluetoothFindDeviceClose(hFind);
	}

	bool isInRange(const BTH_ADDR& address)
	{
		SOCKET socketPtr = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (socketPtr == INVALID_SOCKET)
		{
			int lastError = WSAGetLastError();
			printf("socket failed %d\n", lastError);
			return false;
		}

		SOCKADDR_BTH socketAddress;
		socketAddress.addressFamily = AF_BTH;
		socketAddress.btAddr = address;
		socketAddress.serviceClassId = OBEX_PROTOCOL_UUID;
		socketAddress.port = 0;

		int connectResult = connect(socketPtr, (SOCKADDR*)&socketAddress, sizeof(socketAddress));
		if (connectResult == SOCKET_ERROR)
		{
			int lastError = WSAGetLastError();
			if (lastError != WSAETIMEDOUT)
				printf("connect failed %d\n", lastError);

			closesocket(socketPtr);
			return false;
		}

		closesocket(socketPtr);
		return true;
	}
}

BluetoothScanner::DeviceInfo::DeviceInfo(const char* name, bool inRange)
: name(name)
, inRange(inRange)
{}

BluetoothScanner::BluetoothScanner()
{
	WSADATA wsaData;
	DWORD result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("Cannot startup Winsock, error code %d\n", result);
		return;
	}
}

BluetoothScanner::~BluetoothScanner()
{
	WSACleanup();
}

BluetoothScanner::DeviceInfos BluetoothScanner::scan()
{
	DeviceInfos devices;
	devices.reserve(5);

	bluetoothScanner::queryDevices([&devices](const char* name, const BTH_ADDR& address)
	{
		// TODO - Look into keeping the connection in place rather than closing everything. If the power usage is low enough, it would give much quicker disconnect detection
		bool inRange = bluetoothScanner::isInRange(address);

		devices.emplace_back(name, inRange);
	});

	return devices;
}
