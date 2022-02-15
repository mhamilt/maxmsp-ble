//------------------------------------------------------------------------------
#pragma once
#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#include <stdio.h>
#include <objc/runtime.h>
#include "../MaxObject.h"
#import "CBPeripheral+PeripheralUtility.h"
#import "PeripheralConnectionManager.h"

#ifdef MAXMSP
#include "ext.h"
#else
#define post printf
#endif

//------------------------------------------------------------------------------

/// <#Description#>
@interface MacosBleCentral: NSObject
<CBCentralManagerDelegate, CBPeripheralDelegate>
{    
    NSMutableArray *servicesToScan;
    MaxExternalObject* maxObjectRef;
    NSMutableArray *discoveredPeripheralsRSSIs;
    NSMutableArray<CBPeripheral*> *discoveredPeripherals;
    NSMutableArray<PeripheralConnectionManager*> *orderedDeviceDict;
    NSMutableDictionary <NSString*, PeripheralConnectionManager*> *devices;
    NSMutableArray *blacklistPeripherals;
    dispatch_queue_t bleQueue;
    CBCentralManager *manager;
    BOOL shouldReport;
    BOOL shouldConnect;
    BOOL ignoreUnconnectable;
    BOOL ignoreiPhone;
    int rssiSensitivity;
    NSDictionary* avertisementDataFunctions;
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
- (int) getNumberOfDevices;
- (void)shouldKeepDeviceAtIndex:(int)i connectionAlive:(BOOL)shouldKeepAlive;
- (const char*)getDeviceUUIDatIndex: (int) deviceIndex;

- (void)readCharacteristic: (const char*) cuuid
                 OfService: (const char*) suuid
          ofDeviceWithUUID: (const char*) duuid;

- (void)readCharacteristic:(const char *)cuuid
                 OfService:(const char *)suuid
           ofDeviceAtIndex:(int)i;

- (void)readCharacteristicsOfDeviceWithUUID: (const char*) duuid;
- (void)readAllCharacteristicOfDeviceAtIndex:(int)i;

- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                  ofDeviceAtIndex: (int)  deviceIndex
                  shouldSubscribe: (BOOL) shouldSubscribe;

- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                 ofDeviceWithUUID: (const char*) duuid
                  shouldSubscribe: (BOOL) shouldSubscribe;


- (void)writeToCharacteristic: (const char*) cuuid
                    OfService: (const char*) suuid
             ofDeviceWithUUID: (const char*) duuid
                    withBytes: (void*)  values
                     ofLength: (size_t) numBytes;

- (void)writeToCharacteristic: (const char*) cuuid
                    OfService: (const char*) suuid
                OfFoundDevice: (int)    deviceIndex
                    withBytes: (void*)  values
                     ofLength: (size_t) numBytes;
//------------------------------------------------------------------------------
@end
