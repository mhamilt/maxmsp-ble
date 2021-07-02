#include "pch.h"
#include "WinBleCentral.hpp"
#include "..\MaxObject.h"

//--------------------------------------------------------------------------------------------
WinBleCentral::WinBleCentral()
{
    bleWatcher.Received([this](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs) {this->didDiscoverPeripheral(watcher, eventArgs); });
    bleWatcher.Stopped([this](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementWatcherStoppedEventArgs eventArgs) {this->didCancelScanning(); });
}
//--------------------------------------------------------------------------------------------
WinBleCentral::~WinBleCentral()
{
}
//--------------------------------------------------------------------------------------------
void WinBleCentral::scan()
{
    post("Start Scanning\n");
    bleWatcher.Start();
}
void WinBleCentral::scanForService(t_atom* serviceUUID, long argc)
{
    post("Not yet implemented on Windows");
}
//--------------------------------------------------------------------------------------------
void WinBleCentral::stop()
{
    bleWatcher.Stop();
}
//--------------------------------------------------------------------------------------------
void WinBleCentral::connectToFoundDevice(int deviceIndex)
{
    uint64_t deviceAddress = discoveredPeripherals[deviceIndex].BluetoothAddress();
    post("Connect to: %s", bluetoothAddressToString(deviceAddress).c_str());
    connectPeripheral(deviceAddress);
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::connectToDeviceWithUUID(const char* uuid)
{
    std::string uuidString = std::string(uuid);

    for (auto& device : discoveredPeripherals)
    {
        if (uuidString.compare(bluetoothAddressToString(device.BluetoothAddress())))
        {
            connectPeripheral(device.BluetoothAddress());
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::connectToDeviceWithName(const char* name)
{
    post("Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::clearDicoveredPeripherals()
{
    object_post((t_object*)maxObjectRef, "Device List Cleared");
    discoveredPeripherals.clear();
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::getRssiOfFoundDevice(int deviceIndex)
{
    post("Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::getFoundDeviceList()
{
    for (size_t i = 0; i < discoveredPeripherals.size(); i++)
    {
        outputFoundDevice(
            maxObjectRef,
            i,
            bluetoothAddressToString(discoveredPeripherals[i].BluetoothAddress()).c_str(),
            discoveredPeripherals[i].RawSignalStrengthInDBm());
    }

}

//--------------------------------------------------------------------------------------------
void WinBleCentral::setRssiSensitivity(int rssiSensitivity)
{
    this->rssiSensitivity = rssiSensitivity;
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::setIgnoreiPhone(bool shouldIgnore)
{
    post("Not yet implemented on Windows");
}
//--------------------------------------------------------------------------------------------

void WinBleCentral::setReporting(bool reportingMode)
{
    post("Not yet implemented on Windows");
}
//--------------------------------------------------------------------------------------------

void WinBleCentral::subscribeToCharacteristic(const char* cuuid, const char* suuid, int deviceIndex)
{
    post("Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------    

void WinBleCentral::setMaxObjectRef(MaxExternalObject* extObjRef)
{
    maxObjectRef = extObjRef;
}

//--------------------------------------------------------------------------------------------
std::string WinBleCentral::winrtGuidToString(winrt::guid uuid)
{
    char uuidCStr[34];
    uint64_t d6 = *((uint64_t*)&uuid.Data4[2]);
    if (uuid.Data2 == 0)
        sprintf(uuidCStr, "%04x", uuid.Data1);
    else
        sprintf(uuidCStr, "%08x-%04x-%04x-%02x%02x-%06x", uuid.Data1, uuid.Data2, uuid.Data3, uuid.Data4[0], uuid.Data4[1], d6);
    std::string guid = std::string(uuidCStr);
    return guid;
}

std::string WinBleCentral::bluetoothAddressToString(uint64_t address)
{
    char addressStr[13];
    sprintf(addressStr, "%012llX", address);
    return std::string(addressStr);
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::connectPeripheral(uint64_t windowsDeviceAddress)
{
    BluetoothLEDevice::FromBluetoothAddressAsync(windowsDeviceAddress).Completed(
        [this](IAsyncOperation<BluetoothLEDevice> sender, AsyncStatus status)
        {
            if (auto device = sender.GetResults(); device)
            {
                switch (status)
                {
                case winrt::Windows::Foundation::AsyncStatus::Completed:
                    this->didConnectPeripheral(device);
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Canceled:
                case winrt::Windows::Foundation::AsyncStatus::Error:
                case winrt::Windows::Foundation::AsyncStatus::Started:
                    this->didFailToConnectPeripheral();
                }
            }
            else
            {
                std::cout << "Device is Null: " << sender.ErrorCode() << std::endl;
            }
        });
}

void WinBleCentral::discoverServices(BluetoothLEDevice device)
{
    device.GetGattServicesAsync().Completed(
        [this](IAsyncOperation<GattDeviceServicesResult>sender, AsyncStatus status)
        {
            if (GattDeviceServicesResult result = sender.get(); result)
            {
                switch (status)
                {
                case winrt::Windows::Foundation::AsyncStatus::Completed:
                    this->didDiscoverServices(result.Services(), result.Status());
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Canceled:
                case winrt::Windows::Foundation::AsyncStatus::Error:
                case winrt::Windows::Foundation::AsyncStatus::Started:
                    this->didFailToDiscoverServices();
                }
            }
            else
            {
                std::cout << "Services are Null" << std::endl;
            }
        });
}

void WinBleCentral::discoverCharacteristicsForService(GattDeviceService service)
{
    service.GetCharacteristicsAsync().Completed(
        [this](IAsyncOperation<GattCharacteristicsResult>sender, AsyncStatus status)
        {
            if (GattCharacteristicsResult result = sender.get(); result)
            {
                switch (status)
                {
                case winrt::Windows::Foundation::AsyncStatus::Completed:
                    this->didDiscoverCharacteristicsForService(result.Characteristics(), result.Status());
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Canceled:
                case winrt::Windows::Foundation::AsyncStatus::Error:
                case winrt::Windows::Foundation::AsyncStatus::Started:
                    this->didFailToDiscoverCharacteristicsForService();
                }
            }
            else
            {
                std::cout << "Characteristics are Null" << std::endl;
            }
        });
}

void WinBleCentral::readValueForCharacteristic(GattCharacteristic characteristic)
{
    characteristic.ReadValueAsync().Completed(
        [this, characteristic](IAsyncOperation<GattReadResult> sender, AsyncStatus status)
        {
            if (GattReadResult  result = sender.get(); result)
            {
                switch (status)
                {
                case winrt::Windows::Foundation::AsyncStatus::Completed:
                    this->didReadValueForCharacteristic(characteristic, result.Value(), result.Status());
                    break;
                case winrt::Windows::Foundation::AsyncStatus::Canceled:
                case winrt::Windows::Foundation::AsyncStatus::Error:
                case winrt::Windows::Foundation::AsyncStatus::Started:
                    this->didFailToReadValueForCharacteristic();
                }
            }
            else
            {
                std::cout << "Value is Null" << std::endl;
            }
        });
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverPeripheral(BluetoothLEAdvertisementWatcher watcher,
    BluetoothLEAdvertisementReceivedEventArgs eventArgs)
{
    if (isPeripheralNew(eventArgs) && abs(eventArgs.RawSignalStrengthInDBm()) < rssiSensitivity)
    {
        discoveredPeripheralUUIDs.push_back(eventArgs.BluetoothAddress());
        discoveredPeripherals.push_back(eventArgs);
        if (shouldReport)
            printDeviceDescription(eventArgs);
        outputFoundDevice(maxObjectRef, discoveredPeripherals.size() - 1, bluetoothAddressToString(eventArgs.BluetoothAddress()).c_str(), eventArgs.RawSignalStrengthInDBm());
    }
}

void WinBleCentral::didCancelScanning()
{
    post("Stopped Scanning");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didConnectPeripheral(BluetoothLEDevice& device)
{
    post("didConnectPeripheral: %s", device.Name().data());
    discoverServices(device);
}


//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverIncludedServicesForService() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverServices(IVectorView<GattDeviceService> services, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        //        std::cout << "didDiscoverServices: " << services.GetAt(0).Device().Name().c_str() << std::endl;

        for (auto service : services)
        {
            //std::cout << "Service: " << winrtGuidToString(service.Uuid()) << std::endl;
            discoverCharacteristicsForService(service);
        }
    }
    else
    {
        switch (status)
        {
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
            post("Error Getting Services: Unreachable");
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
            post("Error Getting Services: ProtocolError");
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
            post("Error Getting Services: AccessDenied");
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverCharacteristicsForService(IVectorView<GattCharacteristic> characteristics, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        //std::cout << "didDiscoverCharacteristicsForService: " << winrtGuidToString(characteristics.GetAt(0).Service().Uuid()) << std::endl;

        for (auto characteristic : characteristics)
        {
            //std::cout << "Characteristic: " << winrtGuidToString(characteristic.Uuid()) << " : " << characteristic.UserDescription().c_str() << std::endl;
            readValueForCharacteristic(characteristic);
        }
    }
    else
    {
        switch (status)
        {
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
            post("Error Getting Services: Unreachable");
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
            post("Error Getting Services: ProtocolError");
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
            post("Error Getting Services: AccessDenied");
            break;
        }
    }
}

std::string uint8_to_hex_string(const uint8_t* v, const size_t s) {
    std::stringstream ss;

    ss << std::hex << std::setfill('0');

    for (int i = 0; i < s; i++) {
        ss << std::hex << std::setw(2) << static_cast<int>(v[i]);
    }

    return ss.str();
}

void WinBleCentral::didReadValueForCharacteristic(GattCharacteristic characteristic, winrt::Windows::Storage::Streams::IBuffer value, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        std::string dataHex = uint8_to_hex_string(value.data(), value.Length());

        post("D: %s, S: %s, C: %s, Value: %s",
            bluetoothAddressToString(characteristic.Service().Device().BluetoothAddress()).c_str(),
            winrtGuidToString(characteristic.Service().Uuid()).c_str(),
            winrtGuidToString(characteristic.Uuid()).c_str(),
            dataHex.c_str()
        );

        onCharacteristicRead(maxObjectRef,
            winrtGuidToString(characteristic.Service().Uuid()).c_str(),
            winrtGuidToString(characteristic.Uuid()).c_str(),
            value.data(),
            value.Length());
    }
    else
    {
        switch (status)
        {
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
            post("Error Getting Services: Unreachable");
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
            post("Error Getting Services: ProtocolError");
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
            post("Error Getting Services: AccessDenied");
            break;
        }
    }
}


//--------------------------------------------------------------------------------------------
void WinBleCentral::didUpdateValueForDescriptor() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didUpdateValueForCharacteristic() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverDescriptorsForCharacteristic() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::postCharacteristicDescription() {}
//--------------------------------------------------------------------------------------------
void WinBleCentral::printDeviceDescription(BluetoothLEAdvertisementReceivedEventArgs device)
{
    post("Index: %d, Address: %s, RSSI: %d\n",
        discoveredPeripherals.size() - 1,
        bluetoothAddressToString(device.BluetoothAddress()).c_str(),
        device.RawSignalStrengthInDBm());
    post("------------------------");
}
//--------------------------------------------------------------------------------------------
bool WinBleCentral::isPeripheralNew(BluetoothLEAdvertisementReceivedEventArgs eventArgs)
{
    return (discoveredPeripheralUUIDs.empty() || !(std::find(discoveredPeripheralUUIDs.begin(), discoveredPeripheralUUIDs.end(), eventArgs.BluetoothAddress()) != discoveredPeripheralUUIDs.end()));
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDisconnectPeripheral() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didFailToConnectPeripheral()
{
    std::cout << "didFailToConnectPeripheral" << std::endl;
}
void WinBleCentral::didFailToDiscoverServices()
{
    std::cout << "didFailToDiscoverServices" << std::endl;
}

void WinBleCentral::didFailToDiscoverCharacteristicsForService()
{
    std::cout << "didFailToDiscoverCharacteristicsForService" << std::endl;
}

void WinBleCentral::didFailToReadValueForCharacteristic()
{
    std::cout << "didFailToReadValueForCharacteristic" << std::endl;

}

void WinBleCentral::outputFoundDevice(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, int rssi)
{
    atom_setsym(maxObjectPtr->outputList + 0, gensym(uuid));
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long)rssi);
    outlet_list(maxObjectPtr->list_outlet3, 0L, 2, maxObjectPtr->outputList);
}

void WinBleCentral::onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->outputList, gensym(suuid));
    atom_setsym(maxObjectPtr->outputList + 1, gensym(cuuid));

    if (numBytes > (maxObjectPtr->maxListSize - 2))
    {
        numBytes = maxObjectPtr->maxListSize - 2;
        post("Bytes Truncated\n");
    }

    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 2 + i, (t_atom_long)byteArray[i]);

    outlet_list(maxObjectPtr->list_outlet1, 0L, numBytes + 2, maxObjectPtr->outputList);
}
