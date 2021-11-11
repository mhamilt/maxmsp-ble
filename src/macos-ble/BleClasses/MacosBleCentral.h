//------------------------------------------------------------------------------
#pragma once
#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#include <stdio.h>
#include <objc/runtime.h>
#include "../MaxObject.h"
#ifdef MAXMSP
#include "ext.h"
#else
#define post printf
#endif
//------------------------------------------------------------------------------

typedef enum : NSUInteger {
//    BLE_CONNECT_WITH_MANU_DATA,
    BLE_CONNECT_WITH_DEVICE_UUID,
    BLE_CONNECT_GET_RSSI,
    BLE_CONNECT_EVERYTHING,
    BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC,
    BLE_CONNECT_UNSUBSCRIBE_CHARACTERISTIC,
    BLE_CONNECT_WRITE,
} BleConnectMode;

//------------------------------------------------------------------------------

/// <#Description#>
@interface MacosBleCentral: NSObject
<CBCentralManagerDelegate, CBPeripheralDelegate>
{
    NSUUID *targetDeviceUUID;
    NSData *dataToWrite;
    CBUUID *serviceUuid;
    CBUUID *characteristicUuid;
    NSMutableArray *servicesToScan;
    MaxExternalObject* maxObjectRef;
    NSMutableArray *discoveredPeripheralsRSSIs;
    NSMutableArray<CBPeripheral*> *discoveredPeripherals;
    NSMutableArray *blacklistPeripherals;
    dispatch_queue_t bleQueue;
    CBCentralManager *manager;
    NSUInteger connectDeviceIndex;
    BleConnectMode connectMode;
    BOOL shouldReport;
    BOOL shouldConnect;
    BOOL ignoreUnconnectable;
    BOOL ignoreiPhone;
    int rssiSensitivity;
}
//------------------------------------------------------------------------------
- (instancetype)init;
- (instancetype)initWithQueue: (dispatch_queue_t) centralDelegateQueue;
- (void)scan;
- (void)scanForService: (t_atom*) serviceUUID count: (long) argc;
- (void)stop;
- (void)connectToFoundDevice: (int) deviceIndex;
- (void)connectToDeviceWithUUID: (const char*) uuid;
- (void)connectToDeviceWithName: (const char*) name;
- (void)clearDicoveredPeripherals;
- (void)blacklistDevicesStillConnecting;
- (void)getRssiOfFoundDevice: (int) deviceIndex;
- (void)setRssiSensitivity:(int)rssiSensitivity;
- (void)setIgnoreiPhone:(BOOL)shouldIgnore;
- (void)setMaxObjectRef: (MaxExternalObject *) extMaxObjectRef;
- (void)setReporting: (BOOL) reportingMode;
- (void)getFoundDeviceList;
- (const char*)getDeviceUUIDatIndex: (int) deviceIndex;
- (int)getNumberOfDevices;
- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                    OfFoundDevice: (int)  deviceIndex
                  shouldSubscribe: (BOOL) shouldSubscribe;

- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                 ofDeviceWithUUID: (const char*) duuid
                  shouldSubscribe: (BOOL) shouldSubscribe;

- (void)writeToToCharacteristic: (const char*) cuuid
                      OfService: (const char*) suuid
                  OfFoundDevice: (int)   deviceIndex
                      withBytes: (void*) values
                       ofLength: (int)   numBytes;
//------------------------------------------------------------------------------
@end
