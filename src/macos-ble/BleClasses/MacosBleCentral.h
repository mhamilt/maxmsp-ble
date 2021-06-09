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
    BLE_CONNECT_WITH_MANU_DATA,
    BLE_CONNECT_WITH_DEVICE_NAME,
    BLE_CONNECT_GET_RSSI,
    BLE_CONNECT_EVERYTHING,
    BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC
} BleConnectMode;

//------------------------------------------------------------------------------

/// <#Description#>
@interface MacosBleCentral: NSObject
<CBCentralManagerDelegate, CBPeripheralDelegate>
{
    CBUUID *serviceUuid;
    CBUUID *characteristicUuid;
    NSMutableArray *servicesToScan;
    MaxExternalObject* maxObjectRef;
    NSMutableArray *discoveredPeripheralsRSSIs;
    NSMutableArray *discoveredPeripherals;
    dispatch_queue_t bleQueue;
    CBCentralManager * manager;
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
- (void)clearDicoveredPeripherals;
- (void)getRssiOfFoundDevice: (int) deviceIndex;
- (void)setRssiSensitivity:(int)rssiSensitivity;
- (void)setIgnoreiPhone:(BOOL)shouldIgnore;
- (void)setMaxObjectRef: (MaxExternalObject *) extMaxObjectRef;
- (void)setReporting: (BOOL) reportingMode;
- (void)getFoundDeviceList;
- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                    OfFoundDevice: (int) deviceIndex;
//------------------------------------------------------------------------------
@end
