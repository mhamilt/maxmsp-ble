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

typedef enum : NSUInteger {
    BLE_CONNECT_WITH_MANU_DATA,
    BLE_CONNECT_WITH_DEVICE_NAME,
    BLE_CONNECT_GET_RSSI,
    BLE_CONNECT_EVERYTHING,
    BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC
} BleConnectMode;


/// <#Description#>
@interface MacosBleCentral: NSObject
<CBCentralManagerDelegate, CBPeripheralDelegate>
{
    CBUUID *serviceUuid;
    CBUUID *characteristicUuid;
    MaxExternalObject* maxObjectRef;
    NSData *charDataCopy;
    BOOL shouldReport;
    NSMutableArray *discoveredPeripheralsRSSIs;
}
//------------------------------------------------------------------------------
@property (retain) NSMutableArray *discoveredPeripherals;
@property (strong, nonatomic) NSString *deviceName;
@property (strong, nonatomic) CBCentralManager * manager;
@property (strong, nonatomic) CBPeripheral *peripheral;
@property (strong, atomic) CBService *currentService;
@property (strong, nonatomic) dispatch_queue_t bleQueue;
@property (nonatomic) NSUInteger connectDeviceIndex;
@property (nonatomic) BleConnectMode connectMode;
@property (nonatomic) BOOL shouldConnect;
@property (nonatomic) BOOL ignoreUnconnectable;
@property (nonatomic) int rssiSensitivity;
@property (copy) NSString *manufacturer;
@property (atomic) int latestValue;
//------------------------------------------------------------------------------
- (instancetype)init;
- (instancetype)initWithQueue: (dispatch_queue_t) centralDelegateQueue;
- (instancetype)initWithQueue: (dispatch_queue_t) centralDelegateQueue
                serviceToScan: (CBUUID *) scanServiceId
         characteristicToRead: (CBUUID *) characteristicId;
- (void)scan;
- (void)stop;
- (void)connectToFoundDevice: (int) deviceIndex;
- (void)clearDicoveredPeripherals;
- (void)getRssiOfFoundDevice: (int) deviceIndex;
- (void)setRssiSensitivity:(int)rssiSensitivity;
- (void)setMaxObjectRef: (MaxExternalObject *) extMaxObjectRef;
- (void)setReporting: (BOOL) reportingMode;
- (void)getFoundDeviceList;
- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                    OfFoundDevice: (int) deviceIndex;
//------------------------------------------------------------------------------
@end
