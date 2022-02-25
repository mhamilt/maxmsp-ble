#pragma once

#include <iostream>
#include <vector>
#include <Windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

using winrt::Windows::Devices::Bluetooth::BluetoothConnectionStatus;
using winrt::Windows::Devices::Bluetooth::BluetoothLEDevice;
using winrt::Windows::Devices::Bluetooth::BluetoothUuidHelper;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace winrt;

class DeviceTask
{
    enum DeviceTaskType
    {
        BleDeviceRead,
        BleDeviceWrite,
        BleDeviceSubscribe
    };

    DeviceTaskType type;
    winrt::guid service;
    winrt::guid characteristic;
    void* dataToWrite;
};

class DeviceConnectionManager
{
public:
    BluetoothLEDevice* device;
    std::vector<int> tasks;
    int connectionAttempts;
    int maxConnectionAttempts;
    bool keepAlive;
};

