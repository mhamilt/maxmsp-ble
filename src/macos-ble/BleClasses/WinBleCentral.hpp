#pragma once
// Windows BLE Interface
#include "pch.h"
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

