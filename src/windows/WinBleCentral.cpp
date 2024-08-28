#include "pch.h"
#include "WinBleCentral.hpp"
#include "..\common\MaxObject.h"

//--------------------------------------------------------------------------------------------

std::string bytesToHexString(const uint8_t* v, const size_t s)
{
    std::stringstream ss;

    ss << std::uppercase << std::hex << std::setfill('0');

    for (int i = 0; i < s; i++)
        ss << std::uppercase << std::hex << std::setw(2) << static_cast<int>(v[i]);

    return ss.str();
}

winrt::guid charUUIDToGUID(const char* uuid)
{
    std::string uuidString = uuid;
    if (uuidString.length() == 4)
    {
        return GattCharacteristic::ConvertShortIdToUuid((uint16_t)std::stoul(uuidString, nullptr, 16));
    }

    std::wstring wuuid = std::wstring(uuidString.begin(), uuidString.end());
    return winrt::guid(wuuid);
}

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
    if (shouldReport) object_post((t_object*)maxObjectRef, "Start Scanning\n");
    bleWatcher.Start();
}
void WinBleCentral::scanForService(t_atom* serviceUUID, long argc)
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
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
    if (shouldReport) object_post((t_object*)maxObjectRef, "Connect to: %s", bluetoothAddressToString(deviceAddress).c_str());
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
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::disconnectFromFoundDevice(int deviceIndex)
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::disconnectFromDeviceWithUUID(const char* uuid)
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::clearDicoveredPeripherals()
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Device List Cleared");
    discoveredPeripherals.clear();
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::getRssiOfFoundDevice(int deviceIndex)
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::getFoundDeviceList()
{
    for (size_t i = 0; i < discoveredPeripherals.size(); i++)
    {
        if (shouldReport)
            object_post((t_object*)maxObjectRef, "%d, UUID: %s, RSSI: %d\n",
                i,
                bluetoothAddressToString(discoveredPeripherals[i].BluetoothAddress()).c_str(),
                (int)discoveredPeripherals[i].RawSignalStrengthInDBm());

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
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
}
//--------------------------------------------------------------------------------------------

void WinBleCentral::setReporting(bool reportingMode)
{
    shouldReport = reportingMode;
}
//--------------------------------------------------------------------------------------------

void WinBleCentral::subscribeToCharacteristic(const char* cuuid, const char* suuid, int deviceIndex)
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Not yet implemented on Windows");
}

//--------------------------------------------------------------------------------------------

void WinBleCentral::setDeviceShouldKeepAlive(int i, bool keepAlive)
{

}

//--------------------------------------------------------------------------------------------


void WinBleCentral::setMaxObjectRef(MaxExternalObject* extObjRef)
{
    maxObjectRef = extObjRef;
}

void WinBleCentral::getValueOfCharacteristicOfServiceOfDeviceAtIndex(const char* cuuid, const char* suuid, int deviceIndex)
{
    winrt::guid charGuid = charUUIDToGUID(cuuid);
    winrt::guid serviceGuid = charUUIDToGUID(suuid);
    auto deviceAdvert = getDeviceAtIndex(deviceIndex);
    if (deviceAdvert)
    {
        BluetoothLEDevice::FromBluetoothAddressAsync(deviceAdvert->BluetoothAddress()).Completed(
            [=](IAsyncOperation<BluetoothLEDevice> sender, AsyncStatus status)
            {

               
                if (auto device = sender.GetResults(); device && status == winrt::Windows::Foundation::AsyncStatus::Completed)
                {
                    
                    this->forCharacteristicOfServiceOfDevicePerformCallback(charGuid,
                        serviceGuid, 
                        device, 
                        [=](IAsyncOperation<GattCharacteristicsResult>sender, AsyncStatus status) 
                        {
                            if (auto result = sender.GetResults(); result)
                            {
                                this->didDiscoverCharacteristicsForService(result.Characteristics(), result.Status());
                            }
                        });
                }
                else
                {
                    this->didFailToConnectPeripheral();
                }
            });
    }
}


void WinBleCentral::subscribeToCharacteristicOfServiceOfDeviceAtIndex(const char* cuuid, const char* suuid, int deviceIndex)
{
    
    post("subscribe to %s", cuuid);
    winrt::guid charGuid = charUUIDToGUID(cuuid);
    winrt::guid serviceGuid = charUUIDToGUID(suuid);
    auto deviceAdvert = getDeviceAtIndex(deviceIndex);

    if (deviceAdvert)
    {
        post("device exists");
        BluetoothLEDevice::FromBluetoothAddressAsync(deviceAdvert->BluetoothAddress()).Completed(
            [=](IAsyncOperation<BluetoothLEDevice> sender, AsyncStatus status)
            {
                if (auto device = sender.GetResults(); device && status == winrt::Windows::Foundation::AsyncStatus::Completed)
                {
                    post("device connected");
                    this->forCharacteristicOfServiceOfDevicePerformCallback(charGuid,
                        serviceGuid,
                        device,
                        [=](IAsyncOperation<GattCharacteristicsResult>sender, AsyncStatus status)
                        {
                            
                            if (auto result = sender.GetResults(); result)
                            {
                                post("got characteristics");
                                for (auto& character : result.Characteristics())
                                {
                                    post("%s", winrtGuidToString(character.Uuid()).c_str() );
                                    this->subscriptionHandler = TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs>([this](GattCharacteristic characteristic, GattValueChangedEventArgs args)
                                        {
                                            post("subscribe");
                                        });
                                    character.ValueChanged(this->subscriptionHandler);
                                    character.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify).Completed(
                                        [this, &character](IAsyncOperation<GattCommunicationStatus>sender, AsyncStatus status)
                                        {
                                            post("write config sync status %d: %d", (int)status, (int)sender.GetResults());
                                            
                                        });                                    
                                }                                
                            }
                        });
                }
                else
                {
                    this->didFailToConnectPeripheral();
                }
            });
    }
}
//--------------------------------------------------------------------------------------------

BluetoothLEAdvertisementReceivedEventArgs* WinBleCentral::getDeviceAtIndex(int i)
{
    if (i < discoveredPeripherals.size())
        return &discoveredPeripherals[i];
    
    post("Device Index %d is out of range", i);
    return nullptr;
}

std::string WinBleCentral::winrtGuidToString(winrt::guid uuid)
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
                this->didFailToConnectPeripheral();
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
                this->didFailToDiscoverServices();
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
                this->didFailToDiscoverCharacteristicsForService();
            }
        });
}

void WinBleCentral::readValueForCharacteristic(GattCharacteristic& characteristic)
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
                this->didFailToReadValueForCharacteristic();
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
    if (shouldReport) object_post((t_object*)maxObjectRef, "Stopped Scanning");
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didConnectPeripheral(BluetoothLEDevice& device)
{    
    device.ConnectionStatusChanged([this](BluetoothLEDevice device, IInspectable const&sender) 
        {
            switch (device.ConnectionStatus())
            {
            case BluetoothConnectionStatus::Connected:
                break;
            case BluetoothConnectionStatus::Disconnected:
                // attempt reconnect if keep alive
                break;
            }
        });

    if (shouldReport) object_post((t_object*)maxObjectRef, "Connected to: %s <%ls>", bluetoothAddressToString(device.BluetoothAddress()).c_str(), device.Name().c_str());
    discoverServices(device);
}


//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverIncludedServicesForService() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverServices(IVectorView<GattDeviceService> services, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        for (auto service : services)
        {
            discoverCharacteristicsForService(service);
        }
    }
    else
    {
        std::stringstream ss;
        ss << "Device: "
            << bluetoothAddressToString(services.GetAt(0).Device().BluetoothAddress())
            << " Services: ";
        appendGattCommunicationStatus(ss, status);

        if (shouldReport) object_post((t_object*)maxObjectRef, "%s", ss.str().c_str());
    }
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverCharacteristicsForService(IVectorView<GattCharacteristic> characteristics, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        for (auto characteristic : characteristics)
        {
            readValueForCharacteristic(characteristic);
        }
    }
    else
    {
        std::stringstream ss;
        ss << "Device: "
            << bluetoothAddressToString(characteristics.GetAt(0).Service().Device().BluetoothAddress())
            << " Service: "
            << winrtGuidToString(characteristics.GetAt(0).Service().Uuid())
            << " Characteristics: ";
        appendGattCommunicationStatus(ss, status);

        if (shouldReport) object_post((t_object*)maxObjectRef, "%s", ss.str().c_str());
    }
}


void WinBleCentral::didReadValueForCharacteristic(GattCharacteristic characteristic, winrt::Windows::Storage::Streams::IBuffer value, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        std::string dataHex = bytesToHexString(value.data(), value.Length());

        if (shouldReport) object_post((t_object*)maxObjectRef, "Device: %s Service: %s Char: %s Value: %s",
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
        std::stringstream ss;
        ss << "Device: "
            << bluetoothAddressToString(characteristic.Service().Device().BluetoothAddress())
            << " Service: "
            << winrtGuidToString(characteristic.Service().Uuid())
            << " Characteristic: "
            << winrtGuidToString(characteristic.Uuid())
            << " Value: ";

        appendGattCommunicationStatus(ss, status);

        if (shouldReport) object_post((t_object*)maxObjectRef, "%s", ss.str().c_str());
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
    if (shouldReport) object_post((t_object*)maxObjectRef, "Index: %d, Address: %s, RSSI: %d\n",
        discoveredPeripherals.size() - 1,
        bluetoothAddressToString(device.BluetoothAddress()).c_str(),
        device.RawSignalStrengthInDBm());
    if (shouldReport) object_post((t_object*)maxObjectRef, "------------------------");
}
//--------------------------------------------------------------------------------------------
bool WinBleCentral::isPeripheralNew(BluetoothLEAdvertisementReceivedEventArgs eventArgs)
{
    return (discoveredPeripheralUUIDs.empty() || !(std::find(discoveredPeripheralUUIDs.begin(), discoveredPeripheralUUIDs.end(), eventArgs.BluetoothAddress()) != discoveredPeripheralUUIDs.end()));
}


void WinBleCentral::forCharacteristicOfServiceOfDevicePerformCallback(winrt::guid charGUID, winrt::guid serviceGUID, BluetoothLEDevice& device, std::function<void(IAsyncOperation<GattCharacteristicsResult>, AsyncStatus)>callback)
{
    device.GetGattServicesForUuidAsync(serviceGUID).Completed(
        [&](IAsyncOperation<GattDeviceServicesResult>sender, AsyncStatus status)
        {
            if (GattDeviceServicesResult result = sender.get(); result && status == winrt::Windows::Foundation::AsyncStatus::Completed)
            {
                post("got services");
                for (auto& service : result.Services())
                {
                    service.GetCharacteristicsForUuidAsync(charGUID).Completed(
                        callback
                    );                    
                }
            }
            else
            {
                this->didFailToDiscoverServices();
            }
        }
    );
}
//--------------------------------------------------------------------------------------------
void WinBleCentral::didDisconnectPeripheral() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didFailToConnectPeripheral()
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Failed to Connect");
}
void WinBleCentral::didFailToDiscoverServices()
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Failed to Discover Services");
}

void WinBleCentral::didFailToDiscoverCharacteristicsForService()
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Failed to Discover Characteristics");
}

void WinBleCentral::didFailToReadValueForCharacteristic()
{
    if (shouldReport) object_post((t_object*)maxObjectRef, "Failed to Read Value");
}

void WinBleCentral::outputFoundDevice(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, int rssi)
{
    atom_setsym(maxObjectPtr->outputList + 0, gensym(uuid));
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long)rssi);
    outlet_list(maxObjectPtr->device_status_outlet3, 0L, 2, maxObjectPtr->outputList);
}

void WinBleCentral::onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->outputList, gensym(suuid));
    atom_setsym(maxObjectPtr->outputList + 1, gensym(cuuid));

    if (numBytes > (maxObjectPtr->maxListSize - 2))
    {
        numBytes = maxObjectPtr->maxListSize - 2;
        if (shouldReport) object_post((t_object*)maxObjectRef, "Bytes Truncated\n");
    }

    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 2 + i, (t_atom_long)byteArray[i]);

    outlet_list(maxObjectPtr->read_event_outlet1, 0L, numBytes + 2, maxObjectPtr->outputList);
}


void WinBleCentral::appendGattCommunicationStatus(std::stringstream& ss, GattCommunicationStatus status)
{
    switch (status)
    {
    case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
        ss << "Unreachable";
        break;
    case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
        ss << "ProtocolError";
        break;
    case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
        ss << "AccessDenied";
        break;
    }
}
