#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <Windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Storage.Streams.h>

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


std::string winrtGuidToString(winrt::guid uuid);


class WindowsBleCentral
{
public:
    WindowsBleCentral();

    void startScanning();
    void stopScanning();

    void connectToDeviceAtIndex(int i);

    void readValueOfCharacteristic(const char* cuuid, const char* suuid, const char* duuid);
    void writeValueToCharacteristic();
    void subscribeToCharacteristic();

    void getFoundDevices();
    void clearDiscoveredDevices();
    void setMaxObjectRef();

    void didStopScanning(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementWatcherStoppedEventArgs   eventArgs);
    void didDiscoverPeripheral(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs);
    void didMakeBluetoothLEDeviceReference(IAsyncOperation<BluetoothLEDevice>, AsyncStatus);
    void didChangeConnectionStatus(BluetoothLEDevice, IInspectable);        
    void didDiscoverServices(IAsyncOperation<GattDeviceServicesResult>, AsyncStatus);
    void didDiscoverCharacteristics(IAsyncOperation<GattCharacteristicsResult>, AsyncStatus);
    void didReadValueForCharacteristic(IAsyncOperation<GattReadResult>, AsyncStatus);
    void didUpdateValueForCharacteristic(GattCharacteristic const& notifyingCharacteristic, GattValueChangedEventArgs args);
    void didWriteValueForCharacteristic();

private:
    bool isPeripheralNew(BluetoothLEAdvertisementReceivedEventArgs eventArgs);
    void printGattCommunicationStatus(GattCommunicationStatus status);
    guid constCharToGUID(const char* uuid);

private:
    BluetoothLEAdvertisementWatcher bleWatcher;
    std::vector<uint64_t> discoveredDeviceUUIDs;
    std::vector<BluetoothLEDevice>  discoveredDevices;
    std::vector<GattCharacteristic> subscribedCharacteristics;
};

