#pragma once
// Windows BLE Interface
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

using winrt::Windows::Devices::Bluetooth::BluetoothConnectionStatus;
using winrt::Windows::Devices::Bluetooth::BluetoothLEDevice;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisement;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher;
using winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicsResult;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSession;
using winrt::Windows::Storage::Streams::DataWriter;
using winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteOption;
using winrt::Windows::Devices::Bluetooth::BluetoothCacheMode;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

/// @brief Bluetooth Low Energy Central Delegate Class 
///        The structure of the WinBleClass mimic that of the central manager delegate of the CoreBluetooth API
class WinBleCentral
{

public:
    WinBleCentral();
    ~WinBleCentral();
    /// @brief 
    void scan();
    //void scanForService(t_atom* serviceUUID, long argc);

    /// @brief 
    void stop();
    /// @brief 
    /// @param deviceIndex 
    void connectToFoundDevice(int deviceIndex);
    /// @brief 
    /// @param uuid 
    void connectToDeviceWithUUID(const char* uuid);
    /// @brief 
    /// @param name 
    void connectToDeviceWithName(const char* name);
    /// @brief 
    void clearDicoveredPeripherals();
    /// @brief 
    /// @param deviceIndex 
    void getRssiOfFoundDevice(int deviceIndex);
    /// @brief 
    void getFoundDeviceList();
    /// @brief 
    /// @param rssiSensitivity 
    void setRssiSensitivity(int rssiSensitivity);
    /// @brief  
    /// @param shouldIgnore 
    void setIgnoreiPhone(bool shouldIgnore);
    //void setMaxObjectRef(MaxExternalObject* extMaxObjectRef);
    void setReporting(bool reportingMode);
    void subscribeToCharacteristic(const char* cuuid,
        const char* suuid,
        int deviceIndex);
    std::string winrtGuidToString(winrt::guid);
private:
    /// @brief 
    /// @param windowsDeviceAddress 
    void connectPeripheral(uint64_t windowsDeviceAddress);
    /// @brief 
    /// @param device 
    void discoverServices(BluetoothLEDevice device);
    /// @brief 
    /// @param service 
    void discoverCharacteristicsForService(GattDeviceService service);
    /// @brief 
    /// @param characteristic 
    void readValueForCharacteristic(GattCharacteristic characteristic);
    /// @brief 
    /// @param watcher 
    /// @param eventArgs 
    void didDiscoverPeripheral(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs);
    /// @brief 
    void didCancelScanning();
    /// @brief 
    /// @param device 
    void didConnectPeripheral(BluetoothLEDevice& device);
    /// @brief 
    void didDisconnectPeripheral();
    /// @brief 
    void didFailToConnectPeripheral();

    /// @brief 
    void didFailToDiscoverCharacteristicsForService();
    /// @brief 
    void didFailToDiscoverServices();
    /// @brief 
    void didFailToReadValueForCharacteristic();
    /// @brief 
    void didDiscoverIncludedServicesForService();
    /// @brief 
    /// @param services 
    /// @param error 
    void didDiscoverServices(IVectorView<GattDeviceService> services, GattCommunicationStatus error);
    /// @brief 
    /// @param characteristics 
    /// @param error 
    void didDiscoverCharacteristicsForService(IVectorView<GattCharacteristic> characteristics, GattCommunicationStatus error);
    /// @brief 
    /// @param value 
    /// @param error 
    void didReadValueForCharacteristic(winrt::Windows::Storage::Streams::IBuffer value, GattCommunicationStatus error);
    /// @brief 
    void didUpdateValueForDescriptor();
    /// @brief 
    void didUpdateValueForCharacteristic();
    /// @brief 
    void didDiscoverDescriptorsForCharacteristic();
    /// @brief 
    void postCharacteristicDescription();
    /// @brief 
    /// @param device 
    void printDeviceDescription(BluetoothLEAdvertisementReceivedEventArgs device);
    /// @brief 
    /// @param  
    /// @return 
    bool isPeripheralNew(BluetoothLEAdvertisementReceivedEventArgs);

private:
    /// @brief 
    std::vector<uint64_t> discoveredPeripheralUUIDs;
    ///
    std::vector<BluetoothLEAdvertisementReceivedEventArgs> discoveredPeripherals;
    ///
    bool shouldReport = true;
    ///
    uint32_t connectDeviceIndex = 0;
    ///
    int rssiSensitivity = 127;
    ///
    BluetoothLEAdvertisementWatcher bleWatcher;
    /// @brief 
    bool connecting = false;
};



#include <string_view>
// https://gist.github.com/kennykerr/6c948882de395c25b3218ad8d4daf362
using namespace std::literals;

struct guid
{
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
};

constexpr bool operator==(guid const& left, guid const& right) noexcept
{
    return left.Data1 == right.Data1 &&
        left.Data2 == right.Data2 &&
        left.Data3 == right.Data3 &&
        left.Data4[0] == right.Data4[0] &&
        left.Data4[1] == right.Data4[1] &&
        left.Data4[2] == right.Data4[2] &&
        left.Data4[3] == right.Data4[3] &&
        left.Data4[4] == right.Data4[4] &&
        left.Data4[5] == right.Data4[5] &&
        left.Data4[6] == right.Data4[6] &&
        left.Data4[7] == right.Data4[7];
}


constexpr uint32_t to_uint(char const value) noexcept
{
    if (value >= '0' && value <= '9')
    {
        return value - '0';
    }

    if (value >= 'A' && value <= 'F')
    {
        return 10 + value - 'A';
    }

    if (value >= 'a' && value <= 'f')
    {
        return 10 + value - 'a';
    }

    std::terminate();
}

constexpr guid make_guid(std::string_view const& value) noexcept
{
    if (value.size() != 36 || value[8] != '-' || value[13] != '-' || value[18] != '-' || value[23] != '-')
    {
        std::terminate();
    }

    return
    {
        ((to_uint(value[0]) * 16 + to_uint(value[1])) << 24) +
        ((to_uint(value[2]) * 16 + to_uint(value[3])) << 16) +
        ((to_uint(value[4]) * 16 + to_uint(value[5])) << 8) +
         (to_uint(value[6]) * 16 + to_uint(value[7])),

        static_cast<uint16_t>(((to_uint(value[9]) * 16 + to_uint(value[10])) << 8) +
        (to_uint(value[11]) * 16 + to_uint(value[12]))),

        static_cast<uint16_t>(((to_uint(value[14]) * 16 + to_uint(value[15])) << 8) +
         (to_uint(value[16]) * 16 + to_uint(value[17]))),

        {
            static_cast<uint8_t>(to_uint(value[19]) * 16 + to_uint(value[20])),
            static_cast<uint8_t>(to_uint(value[21]) * 16 + to_uint(value[22])),

            static_cast<uint8_t>(to_uint(value[24]) * 16 + to_uint(value[25])),
            static_cast<uint8_t>(to_uint(value[26]) * 16 + to_uint(value[27])),
            static_cast<uint8_t>(to_uint(value[28]) * 16 + to_uint(value[29])),
            static_cast<uint8_t>(to_uint(value[30]) * 16 + to_uint(value[31])),
            static_cast<uint8_t>(to_uint(value[32]) * 16 + to_uint(value[33])),
            static_cast<uint8_t>(to_uint(value[34]) * 16 + to_uint(value[35])),
        }
    };
}
static constexpr guid b
{
    make_guid("8AA90CAD-fed1-4c54-89db-9B7522D8AA92"sv)
};
