#include "pch.h"
#include "WinBleCentral.hpp"

//--------------------------------------------------------------------------------------------
WinBleCentral::WinBleCentral()
{
    post("Windows BLE Started\n");
    bleWatcher.Received([this](BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs) {this->didDiscoverPeripheral(watcher, eventArgs); });
    bleWatcher.Stopped([this](BluetoothLEAdvertisementWatcher w, BluetoothLEAdvertisementWatcherStoppedEventArgs b) {this->didCancelScanning(); });
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
    std::cout << std::hex << "Connect to: " << deviceAddress << std::endl;
    connectPeripheral(deviceAddress);
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::connectToDeviceWithUUID(const char* uuid)
{
    // for items in dicovered peripherals
    // if item.uuid equals uuid
    // connectToPeripheral(item.BluetoothAddress());
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::connectToDeviceWithName(const char* name) {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::clearDicoveredPeripherals()
{
    discoveredPeripherals.clear();
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::getRssiOfFoundDevice(int deviceIndex) {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::getFoundDeviceList()
{
    for (auto device : discoveredPeripherals)
    {

    }
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::setRssiSensitivity(int rssiSensitivity)
{
    this->rssiSensitivity = rssiSensitivity;
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::setIgnoreiPhone(bool shouldIgnore) {}
//--------------------------------------------------------------------------------------------

void WinBleCentral::setReporting(bool reportingMode) {}
//--------------------------------------------------------------------------------------------

void WinBleCentral::subscribeToCharacteristic(const char* cuuid, const char* suuid, int deviceIndex)
{}

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
        [this](IAsyncOperation<GattReadResult> sender, AsyncStatus status)
        {
            if (GattReadResult  result = sender.get(); result)
            {
                switch (status)
                {
                case winrt::Windows::Foundation::AsyncStatus::Completed:
                    this->didReadValueForCharacteristic(result.Value(), result.Status());
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

        //connectToFoundDevice(discoveredPeripherals.size() - 1);
    }
}

void WinBleCentral::didCancelScanning()
{
    std::cout << "stopped scanning" << std::endl;
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didConnectPeripheral(BluetoothLEDevice& device)
{
    std::wcout << "didConnectPeripheral:" << device.Name().c_str() << std::endl;
    discoverServices(device);
}


//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverIncludedServicesForService() {}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverServices(IVectorView<GattDeviceService> services, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        std::cout << "didDiscoverServices: " << services.GetAt(0).Device().Name().c_str() << std::endl;

        for (auto service : services)
        {
            std::cout << "Service: " << winrtGuidToString(service.Uuid()) << std::endl;
            discoverCharacteristicsForService(service);
        }
    }
    else
    {
        std::cout << "Error Getting Services: ";
        switch (status)
        {
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
            std::cout << "Unreachable";
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
            std::cout << "ProtocolError";
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
            std::cout << "AccessDenied";
            break;
        }
        std::cout << std::endl;
    }
}

//--------------------------------------------------------------------------------------------
void WinBleCentral::didDiscoverCharacteristicsForService(IVectorView<GattCharacteristic> characteristics, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        std::cout << "didDiscoverCharacteristicsForService: " << winrtGuidToString(characteristics.GetAt(0).Service().Uuid()) << std::endl;

        for (auto characteristic : characteristics)
        {
            std::cout << "Characteristic: " << winrtGuidToString(characteristic.Uuid()) << " : " << characteristic.UserDescription().c_str() << std::endl;
            readValueForCharacteristic(characteristic);
        }
    }
    else
    {
        std::cout << "Error Getting Characteristics: ";
        switch (status)
        {
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
            std::cout << "Unreachable";
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
            std::cout << "ProtocolError";
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
            std::cout << "AccessDenied";
            break;
        }
        std::cout << std::endl;
    }
}

void WinBleCentral::didReadValueForCharacteristic(winrt::Windows::Storage::Streams::IBuffer value, GattCommunicationStatus status)
{
    if (status == GattCommunicationStatus::Success)
    {
        std::cout << "Value: ";
        for (size_t i = 0; i < value.Length(); i++)
        {
            printf("%02x", value.data()[i]);
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Error Value For Characteristic: ";
        switch (status)
        {
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Unreachable:
            std::cout << "Unreachable";
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::ProtocolError:
            std::cout << "ProtocolError";
            break;
        case winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::AccessDenied:
            std::cout << "AccessDenied";
            break;
        }
        std::cout << std::endl;
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
    post("Index: %d, Address: %012llX, RSSI: %d\n",
        discoveredPeripherals.size() - 1,
        device.BluetoothAddress(),
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


void oldGetDeviceInfo()
{
    //auto gattServices = device.GetGattServicesAsync(BluetoothCacheMode::Uncached).get(); // https://docs.microsoft.com/en-us/cpp/parallel/concrt/reference/task-class?view=msvc-160#get

    //std::cout << gattServices.Services().Size() << std::endl;

    //std::array<uint8_t, 8> ar = { 17,17,17,17,17,17,17,17 };
    //std::array<uint8_t, 8> ar2 = { 34,34,34,34,34,34,34,34 };

    //winrt::guid Guid((uint32_t)0x111111111, (uint16_t)0x1111, (uint16_t)0x1111, ar);
    //winrt::guid charGuid((uint32_t)0x22222222, (uint16_t)0x2222, (uint16_t)0x2222, ar2);

    //auto bruh = device.GetGattServicesForUuidAsync(Guid);
    //auto d = bruh.get();
    //auto services = d.Services();
    //std::cout << "Service Size: " << services.Size() << std::endl;
    //std::cout << "Char Size: " << services.GetAt(0).GetCharacteristicsForUuidAsync(charGuid).get().Characteristics().Size() << std::endl;
    // DataWriter f;
    // f.WriteInt16(102);
    // auto b = services.GetAt(0).GetCharacteristicsForUuidAsync(charGuid).get().Characteristics().GetAt(0).WriteValueAsync(f.DetachBuffer());

    // auto re = services.GetAt(0).GetCharacteristicsForUuidAsync(charGuid).get().Characteristics().GetAt(0).CharacteristicProperties();
    // std::cout << (int)re << std::endl;
}