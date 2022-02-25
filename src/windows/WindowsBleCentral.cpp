#include "pch.h"
#include "WindowsBleCentral.h"

WindowsBleCentral::WindowsBleCentral()
{
    bleWatcher.Received({ this, &WindowsBleCentral::didDiscoverPeripheral });
    bleWatcher.Stopped({ this, &WindowsBleCentral::didStopScanning });
}

void WindowsBleCentral::startScanning()
{
    bleWatcher.Start();
}

void WindowsBleCentral::stopScanning()
{
    bleWatcher.Stop();
}

void WindowsBleCentral::connectToDeviceAtIndex(int i)
{
    //BluetoothLEDevice::FromBluetoothAddressAsync(discoveredDevices[i].BluetoothAddress()).Completed({ this, &WindowsBleCentral::didConnectPeripheral });
}

void WindowsBleCentral::readValueOfCharacteristic(const char* cuuid, const char* suuid, const char* duuid)
{
}

void WindowsBleCentral::writeValueToCharacteristic()
{
}

void WindowsBleCentral::subscribeToCharacteristic()
{
}

void WindowsBleCentral::getFoundDevices()
{
}

void WindowsBleCentral::clearDiscoveredDevices()
{
}

void WindowsBleCentral::didStopScanning(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementWatcherStoppedEventArgs eventArgs)
{
}

void WindowsBleCentral::didDiscoverPeripheral(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs)
{
    if (isPeripheralNew(eventArgs) && eventArgs.IsConnectable())
    {
        discoveredDeviceUUIDs.push_back(eventArgs.BluetoothAddress());
        auto device = BluetoothLEDevice::FromBluetoothAddressAsync(eventArgs.BluetoothAddress()).get();
        discoveredDevices.push_back(device);
        std::wcout << std::wstring{ device.DeviceId() } << L"\n";

        device.ConnectionStatusChanged({ this, &WindowsBleCentral::didChangeConnectionStatus });
        device.GetGattServicesAsync().Completed({ this, &WindowsBleCentral::didDiscoverServices });
        /*if (shouldReport)
                printDeviceDescription(eventArgs);
            outputFoundDevice(maxObjectRef, discoveredPeripherals.size() - 1, bluetoothAddressToString(eventArgs.BluetoothAddress()).c_str(), eventArgs.RawSignalStrengthInDBm())*/;
    }
}

void WindowsBleCentral::didMakeBluetoothLEDeviceReference(IAsyncOperation<BluetoothLEDevice>sender, AsyncStatus status)
{
    auto device = sender.GetResults();

    if (device && status == AsyncStatus::Completed)
    {
        discoveredDevices.push_back(device);
    }
    else
    {
        std::cout << "Some Kind of Error" << '\n';
    }
}

void WindowsBleCentral::didChangeConnectionStatus(BluetoothLEDevice device, IInspectable)
{
    //   std::wcout << std::wstring{ device.DeviceId() };
    switch (device.ConnectionStatus())
    {
    case BluetoothConnectionStatus::Connected:
        //     std::wcout << L"Connected\n";
        break;
    case BluetoothConnectionStatus::Disconnected:
        //   std::wcout << L"Disconnected\n";
        break;
    }
}


void WindowsBleCentral::didDiscoverServices(IAsyncOperation<GattDeviceServicesResult> sender, AsyncStatus status)
{
    GattDeviceServicesResult result = sender.get();
    if (result && 
        status == AsyncStatus::Completed &&
        result.Status() == GattCommunicationStatus::Success)
    {
        for (auto service : result.Services())
        {
            service.GetCharacteristicsAsync().Completed({ this, &WindowsBleCentral::didDiscoverCharacteristics });
        }
    }
    else
    {
        std::cout << "Failed to Get Service ";
        printGattCommunicationStatus(result.Status());
        std::cout << "\n";

    }
}

void WindowsBleCentral::didDiscoverCharacteristics(IAsyncOperation<GattCharacteristicsResult> sender, AsyncStatus status)
{
    GattCharacteristicsResult result = sender.get();
    if (result && 
        status == AsyncStatus::Completed &&
        result.Status() == GattCommunicationStatus::Success)
    {
        if (result.Characteristics().Size())
        {
            for (auto characteristic : result.Characteristics())
            {
                if (static_cast<bool>((characteristic.CharacteristicProperties() & GattCharacteristicProperties::Read)))
                {
                    auto readValueOp = characteristic.ReadValueAsync();

                    readValueOp.Completed
                    (
                        [this, characteristic](IAsyncOperation<GattReadResult>sender, AsyncStatus status)
                        {
                            std::cout << winrtGuidToString(characteristic.Service().Uuid()) << " " << winrtGuidToString(characteristic.Uuid()) << " ";
                            auto result = sender.get();
                            if (result &&
                                status == AsyncStatus::Completed &&
                                result.Status() == GattCommunicationStatus::Success)
                            {
                                auto value = result.Value();
                                if (value)
                                {
                                    auto data = value.data();
                                    auto length = value.Length();
                                    for (size_t i = 0; i < length; i++)
                                        std::cout << std::hex << data[i];
                                    std::cout << '\n';
                                }
                                else
                                {
                                    std::cout << "no Data\n";
                                }
                            }
                            else
                            {
                                printGattCommunicationStatus(result.Status());
                                std::cout << "\n";
                            }

                        }
                    );
                }
                else
                {
                    std::cout << winrtGuidToString(characteristic.Service().Uuid()) << " " << winrtGuidToString(characteristic.Uuid()) << "\n";
                }
            }
        }
    }
    else
    {
        std::cout << "Failed to Get Characteristic ";
        printGattCommunicationStatus(result.Status());
        std::cout << "\n";
    }
}

void WindowsBleCentral::didReadValueForCharacteristic(IAsyncOperation<GattReadResult>sender, AsyncStatus status)
{
    auto result = sender.get();
    if (result && status == AsyncStatus::Completed)
    {

    }
}

void WindowsBleCentral::didUpdateValueForCharacteristic(GattCharacteristic const& notifyingCharacteristic, GattValueChangedEventArgs args)
{
}

void WindowsBleCentral::didWriteValueForCharacteristic()
{
}

bool WindowsBleCentral::isPeripheralNew(BluetoothLEAdvertisementReceivedEventArgs eventArgs)
{
    return  (discoveredDeviceUUIDs.empty() || !(std::find(discoveredDeviceUUIDs.begin(), discoveredDeviceUUIDs.end(), eventArgs.BluetoothAddress()) != discoveredDeviceUUIDs.end()));
}

void WindowsBleCentral::printGattCommunicationStatus(GattCommunicationStatus status)
{
    switch (status)
    {
    case GattCommunicationStatus::AccessDenied:
        std::cout << "AccessDenied";
        break;
    case GattCommunicationStatus::ProtocolError:
        std::cout << "ProtocolError";
        break;
    case GattCommunicationStatus::Success:
        std::cout << "Success";
        break;
    case GattCommunicationStatus::Unreachable:
        std::cout << "Unreachable";
        break;
    }
}

std::string winrtGuidToString(winrt::guid uuid)
{
    char uuidCStr[34];
    uint64_t d6 = *((uint64_t*)&uuid.Data4[2]);
    if (uuid.Data2 == 0)
        sprintf(uuidCStr, "%04X", uuid.Data1);
    else
        sprintf(uuidCStr, "%08X-%04X-%04X-%02X%02X-%06X", uuid.Data1, uuid.Data2, uuid.Data3, uuid.Data4[0], uuid.Data4[1], d6);
    std::string guid = std::string(uuidCStr);
    return guid;
}

