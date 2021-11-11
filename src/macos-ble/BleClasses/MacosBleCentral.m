//------------------------------------------------------------------------------
#import "MacosBleCentral.h"
//------------------------------------------------------------------------------
@implementation MacosBleCentral
//------------------------------------------------------------------------------
- (instancetype)init
{
    dispatch_queue_t newQueue = dispatch_queue_create("max_masp_ble",
                                                      DISPATCH_QUEUE_SERIAL);
    return [self initWithQueue:newQueue];
}
////------------------------------------------------------------------------------
- (instancetype)initWithQueue: (dispatch_queue_t) centralDelegateQueue
{
    self = [super init];
    if (self)
    {
        discoveredPeripherals = [[NSMutableArray alloc] init];
        discoveredPeripheralsRSSIs = [[NSMutableArray alloc] init];
        blacklistPeripherals = [[NSMutableArray alloc] init];
        bleQueue = centralDelegateQueue;
        manager = [[CBCentralManager alloc] initWithDelegate: self
                                                       queue: bleQueue];
        shouldReport = YES;
        shouldConnect = NO;
        ignoreUnconnectable = NO;
        rssiSensitivity = 127;
        ignoreiPhone = NO;
        servicesToScan = [[NSMutableArray alloc] init];
        
    }
    return self;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma mark Manager Methods

- (void)centralManager:(CBCentralManager *)central
 didDiscoverPeripheral:(CBPeripheral *)aPeripheral
     advertisementData:(NSDictionary *)advertisementData
                  RSSI:(NSNumber *)RSSI
{
    if (![discoveredPeripherals containsObject: aPeripheral]
        && ! [blacklistPeripherals containsObject: aPeripheral]
        //&& !(!connectable && ignoreUnconnectable)
        && (abs(RSSI.intValue) < abs(rssiSensitivity) && RSSI.intValue < 0)
        && !(advertisementData[@"kCBAdvDataAppleMfgData"] && ignoreiPhone)
        )
    {
        NSUInteger deviceIndex = discoveredPeripherals.count;
        if(shouldReport)
        {
            NSString* deviceName = ((aPeripheral.name) ?
                                    [NSString stringWithFormat:@" <Name: %s>,",
                                     aPeripheral.name.UTF8String] :
                                    [NSString stringWithFormat:@""]);
            
            post("Index: %d,%s UUID: %s, RSSI: %d\n",
                 deviceIndex,
                 deviceName.UTF8String,
                 aPeripheral.identifier.UUIDString.UTF8String,
                 RSSI.intValue);
            post("------------------------");
        }
        [discoveredPeripherals addObject:aPeripheral];
        [discoveredPeripheralsRSSIs addObject:RSSI];
        
        outputFoundDeviceList(maxObjectRef,
                              deviceIndex,
                              aPeripheral.identifier.UUIDString.UTF8String,
                              RSSI.intValue);
        
        if (connectMode == BLE_CONNECT_WITH_DEVICE_UUID
            && ([aPeripheral.identifier isEqual:targetDeviceUUID]))
        {
            [self connectToDevice:aPeripheral
                      withOptions:nil];
            [manager stopScan];
        }
    }
}

//------------------------------------------------------------------------------
- (void) centralManager: (CBCentralManager *)central
   didConnectPeripheral: (CBPeripheral *)aPeripheral
{
    if (shouldReport)
        post("Connected to %s\n",
             ((aPeripheral.name) ?
              aPeripheral.name.UTF8String
              : aPeripheral.identifier.UUIDString.UTF8String));
    
    [aPeripheral setDelegate:self];
    
    switch (connectMode)
    {
        case BLE_CONNECT_GET_RSSI:
            [aPeripheral readRSSI]; // goto didReadRSSI
            break;
        case BLE_CONNECT_WRITE:
        case BLE_CONNECT_UNSUBSCRIBE_CHARACTERISTIC:
        case BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC:
            [aPeripheral discoverServices:@[serviceUuid]];
            break;
        default:
            [aPeripheral discoverServices:nil];
            break;
    }
}
//------------------------------------------------------------------------------
- (void)centralManagerDidUpdateState:(CBCentralManager *)manager
{
    if ([manager state] == CBManagerStatePoweredOn)
    {
    }
}
//------------------------------------------------------------------------------
// Invoked whenever an existing connection with the peripheral is torn down.
- (void) centralManager: (CBCentralManager *)central
didDisconnectPeripheral: (CBPeripheral *)aPeripheral
                  error: (NSError *)error
{
    if(error)
    {
        post("Error %d: %s Device: %s\n",
             error.code,
             error.localizedDescription.UTF8String,
             aPeripheral.identifier.UUIDString.UTF8String);
    }
}
//------------------------------------------------------------------------------
// Invoked whenever the central manager fails to create a connection with the peripheral.
- (void) centralManager: (CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)aPeripheral
                  error:(NSError *)error
{
    if(shouldReport)
    {
        post("Connection Failed\n");
        NSLog(@"Fail to connect to peripheral: %@ with error = %@", aPeripheral, [error localizedDescription]);
        post("Failed to connect to peripheral: %s Error: %s",
             ((aPeripheral.name) ?
              aPeripheral.name.UTF8String
              : aPeripheral.identifier.UUIDString.UTF8String),
             [[error localizedDescription] UTF8String]);
    }
}
//------------------------------------------------------------------------------
#pragma mark Interface Methods

- (void) scan;
{
    [servicesToScan removeAllObjects];
    [self startScan];
}

- (void) startScan
{
    
    if (![manager isScanning])
    {
        post("Start scanning\n");
        post("------------------------");
    }
    
    
    [manager scanForPeripheralsWithServices: ((servicesToScan.count) ? servicesToScan : nil)
                                    options: @{CBCentralManagerScanOptionAllowDuplicatesKey: @YES}];
}

- (void)scanForService: (t_atom*) serviceUUIDs
                 count: (long) argc
{
    if (!argc)
    {
        [servicesToScan removeAllObjects];
    }
    else
    {
        for (int i = 0; i < argc; i++)
        {
            [servicesToScan addObject:[CBUUID UUIDWithString:
                                       [[NSString alloc]
                                        initWithUTF8String:atom_getsym(serviceUUIDs + i)->s_name]]];
        }
    }
    
    [self startScan];
}

- (void) stop;
{
    post("Stop scanning\n");
    shouldConnect = NO;
    [manager stopScan];
}




- (void) connectToFoundDevice: (int) deviceIndex
{
    if (deviceIndex < discoveredPeripherals.count)
    {
        connectMode = BLE_CONNECT_EVERYTHING;
        [self  connectToDevice:discoveredPeripherals[deviceIndex] withOptions:nil];
    }
    
}

- (void)connectToDeviceWithUUID: (const char*) uuid
{
    targetDeviceUUID = [[NSUUID alloc] initWithUUIDString:[[NSString alloc]
                                                           initWithUTF8String:uuid]];
    
    BOOL isTargetDeviceFound = NO;
    for (CBPeripheral* device in discoveredPeripherals)
    {
        if ([device.identifier isEqual:targetDeviceUUID])
        {
            isTargetDeviceFound = YES;
            [self  connectToDevice:device
                       withOptions:nil];
            break;
        }
    }
    
    if (!isTargetDeviceFound)
    {
        connectMode = BLE_CONNECT_WITH_DEVICE_UUID;
        [manager scanForPeripheralsWithServices:nil
                                        options:nil];
    }
    
}

- (void)connectToDeviceWithName: (const char*) name
{
    BOOL deviceFound = NO;
    NSString* deviceName = [[NSString alloc] initWithUTF8String:name];
    
    for (CBPeripheral* device in discoveredPeripherals)
    {
        if ([device.name isEqual:deviceName])
        {
            deviceFound = YES;
            [self  connectToDevice:device
                       withOptions:nil];
            break;
        }
    }
    
    if (!deviceFound)
        object_post((t_object *)maxObjectRef, "device with name %s not found", name);
}

//------------------------------------------------------------------------------

- (void) clearDicoveredPeripherals
{
    [discoveredPeripherals removeAllObjects];
    [discoveredPeripheralsRSSIs removeAllObjects];
}

- (void)blacklistDevicesStillConnecting
{
    
}
//------------------------------------------------------------------------------
- (void) getRssiOfFoundDevice: (int) deviceIndex
{
    if (deviceIndex < discoveredPeripherals.count)
    {
        connectMode = BLE_CONNECT_GET_RSSI;
        [self  connectToDevice:discoveredPeripherals[deviceIndex] withOptions:nil];
    }
}

- (void)setRssiSensitivity:(int)newRSSISensitivity
{    
    rssiSensitivity = newRSSISensitivity;
}

- (void)setIgnoreiPhone:(BOOL)shouldIgnore
{
    ignoreiPhone = shouldIgnore;
}

//------------------------------------------------------------------------------
- (void)setMaxObjectRef: (MaxExternalObject *) extMaxObjectRef
{
    maxObjectRef = extMaxObjectRef;
}

- (void)setReporting: (BOOL) reportingMode
{
    shouldReport = reportingMode;
}

- (void)getFoundDeviceList
{
    for(CBPeripheral* device in discoveredPeripherals)
    {
        NSUInteger index = [discoveredPeripherals indexOfObject:device];
        NSNumber*  rssi  = discoveredPeripheralsRSSIs[index];
        if (shouldReport)
            post("%d,\tUUID: %s,\tname: %s,\tRSSI: %d\n",
                 index,
                 device.identifier.UUIDString.UTF8String,
                 device.name.UTF8String,
                 rssi.intValue);
        
        outputFoundDeviceList(maxObjectRef,
                              index,
                              device.identifier.UUIDString.UTF8String,
                              rssi.intValue);
    }
}

- (const char*)getDeviceUUIDatIndex: (int) deviceIndex
{
    CBPeripheral* device = discoveredPeripherals[deviceIndex];
    return device.identifier.UUIDString.UTF8String;
}


- (int)getNumberOfDevices
{
    return (int)discoveredPeripherals.count;
}


- (void) subscribeToCharacteristic: (const char *) cuuid
                         OfService: (const char*)  suuid
                     OfFoundDevice: (int) deviceIndex
                   shouldSubscribe: (BOOL) shouldSubscribe
{
    if (deviceIndex < discoveredPeripherals.count) {
        
        connectMode = ((shouldSubscribe) ?
                       BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC :
                       BLE_CONNECT_UNSUBSCRIBE_CHARACTERISTIC);
        
        characteristicUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: cuuid] ];
        serviceUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: suuid] ];
        [self connectToDevice:discoveredPeripherals[deviceIndex] withOptions:nil];
        
        post("Subscribe to %s: %s\n",
             serviceUuid.UUIDString.UTF8String,
             characteristicUuid.UUIDString.UTF8String);
    }
    else
    {
        
    }
}

- (void)subscribeToCharacteristic: (const char*) cuuid
                        OfService: (const char*) suuid
                 ofDeviceWithUUID: (const char*) duuid
                  shouldSubscribe: (BOOL) shouldSubscribe;
{
    connectMode = ((shouldSubscribe) ?
                   BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC :
                   BLE_CONNECT_UNSUBSCRIBE_CHARACTERISTIC);
    
    characteristicUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: cuuid] ];
    serviceUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: suuid] ];
    
    BOOL isTargetDeviceFound = NO;
    for (CBPeripheral* device in discoveredPeripherals)
    {
        if ([device.identifier isEqual:targetDeviceUUID])
        {
            isTargetDeviceFound = YES;
            [self  connectToDevice:device withOptions:nil];
            break;
        }
    }
    if (!isTargetDeviceFound)
        post("Device with UUID %s not found", duuid);
}

//------------------------------------------------------------------------------
#pragma mark Peripheral Methods

- (void) peripheral: (CBPeripheral *)peripheral
didDiscoverIncludedServicesForService:(CBService *)service
              error:(NSError *)error
{
    post("didDiscoverIncludedServicesForService");
}
//------------------------------------------------------------------------------
// Invoked upon completion of a -[discoverServices:] request.
// Discover available characteristics on interested services
- (void) peripheral: (CBPeripheral *)aPeripheral
didDiscoverServices: (NSError *)error
{
    switch (connectMode)
    {
        case BLE_CONNECT_WRITE:
        case BLE_CONNECT_UNSUBSCRIBE_CHARACTERISTIC:
        case BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC:
            [aPeripheral discoverCharacteristics:@[characteristicUuid]
                                      forService:aPeripheral.services[0]];
            break;
            
        default:
            for (CBService *aService in aPeripheral.services)
            {
                [aPeripheral discoverCharacteristics: nil
                                          forService: aService];
            }
            break;
    }
}
//------------------------------------------------------------------------------
// Invoked upon completion of a -[discoverCharacteristics:forService:] request.
// Perform appropriate operations on interested characteristics
- (void) peripheral: (CBPeripheral *)aPeripheral didDiscoverCharacteristicsForService:(CBService *)service
              error: (NSError *)error
{
    
    switch (connectMode)
    {
        case BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC:
            [aPeripheral setNotifyValue:YES
                      forCharacteristic:service.characteristics[0]];
            break;
        case BLE_CONNECT_UNSUBSCRIBE_CHARACTERISTIC:
            [aPeripheral setNotifyValue:NO
                      forCharacteristic:service.characteristics[0]];
            break;
        case BLE_CONNECT_WRITE:
            [aPeripheral writeValue:dataToWrite
                  forCharacteristic:service.characteristics[0]
                               type:CBCharacteristicWriteWithResponse];
            break;
        default:
        {
            if(shouldReport)
            {
                //                post("Service (%s) %s\n",
                //                     service.UUID.UUIDString.UTF8String,
                //                     ((service.UUID.UUIDString.length == 4) ? service.UUID.description.UTF8String : ""));
            }
            for (CBCharacteristic *aChar in service.characteristics)
            {
                if (aChar.properties & CBCharacteristicPropertyRead)
                    [aPeripheral readValueForCharacteristic:aChar];
            }
            break;
        }
    }
}
//------------------------------------------------------------------------------
- (void) peripheral:(CBPeripheral *)peripheral
didUpdateValueForDescriptor:(CBDescriptor *)descriptor
              error:(NSError *)error
{
    NSData* value = descriptor.value;
    post("%s(%s): %s",
         descriptor.UUID.description.UTF8String,
         descriptor.UUID.UUIDString.UTF8String,
         value.description.UTF8String);
}
//------------------------------------------------------------------------------
// Invoked upon completion of a -[readValueForCharacteristic:] request or on the reception of a notification/indication.
- (void) peripheral: (CBPeripheral *)aPeripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if(!error)
    {
        if(characteristic.isNotifying)
        {
            onNotificationRead(maxObjectRef,
                               characteristic.UUID.UUIDString.UTF8String,
                               (uint8_t*)characteristic.value.bytes,
                               characteristic.value.length);
        }
        else
        {
            if(shouldReport)
                [self postCharacteristicDescription:characteristic];
            
            onCharacteristicRead(maxObjectRef,
                                 characteristic.service.UUID.UUIDString.UTF8String,
                                 characteristic.UUID.UUIDString.UTF8String,
                                 (uint8_t*)characteristic.value.bytes,
                                 characteristic.value.length);
        }
        
    }
    else
    {
        post("Error Getting Characteristic %s\n",[[error localizedDescription] UTF8String]);
    }
}

- (void) postCharacteristicDescription: (CBCharacteristic *)characteristic
{
    NSString* valueString = nil;
    
    if([characteristic.UUID.description containsString:@"String"] || [characteristic.UUID.description containsString:@"Name"])
        valueString = [[NSString alloc] initWithData:characteristic.value
                                            encoding:NSUTF8StringEncoding];
    else if ([characteristic.UUID.UUIDString isEqual:@"2A19"])
        valueString = [NSString stringWithFormat:@"%d", *(int*)characteristic.value.bytes];
    else
        valueString = characteristic.value.description;
    
    NSMutableString* propertiesDescription = [[NSMutableString alloc] initWithString:@""];
    
    if (characteristic.properties & CBCharacteristicPropertyBroadcast)
        [propertiesDescription appendString:@"Broadcast, "];
    if (characteristic.properties & CBCharacteristicPropertyRead)
        [propertiesDescription appendString:@"Read, "];
    if (characteristic.properties & CBCharacteristicPropertyWriteWithoutResponse)
        [propertiesDescription appendString:@"Write Without Response, "];
    if (characteristic.properties & CBCharacteristicPropertyWrite)
        [propertiesDescription appendString:@"Write, "];
    if (characteristic.properties & CBCharacteristicPropertyNotify)
        [propertiesDescription appendString:@"Notify, "];
    if (characteristic.properties & CBCharacteristicPropertyIndicate)
        [propertiesDescription appendString:@"Indicate, "];
    if (characteristic.properties & CBCharacteristicPropertyAuthenticatedSignedWrites)
        [propertiesDescription appendString:@"SignedWrites, "];
    if (characteristic.properties & CBCharacteristicPropertyExtendedProperties)
        [propertiesDescription appendString:@"ExtendedProperties, "];
    if (characteristic.properties & CBCharacteristicPropertyNotifyEncryptionRequired)
        [propertiesDescription appendString:@"NotifyEncryptionRequired, "];
    if (characteristic.properties & CBCharacteristicPropertyIndicateEncryptionRequired)
        [propertiesDescription appendString:@"IndicateEncryptionRequired"];
    
    //    characteristic.service.peripheral.
    CBPeripheral* device = characteristic.service.peripheral;
    CBService* service = characteristic.service;
    
    post("Device: %s Service (%s) %s Char: (%s) %s: Value: %s Porperties: %s",
         device.identifier.UUIDString.UTF8String,
         service.UUID.UUIDString.UTF8String,
         ((service.UUID.UUIDString.length == 4) ?
          service.UUID.description.UTF8String : ""),
         characteristic.UUID.UUIDString.UTF8String,
         ((characteristic.UUID.UUIDString.length == 4) ?
          characteristic.UUID.description.UTF8String : ""),
         valueString.UTF8String,
         propertiesDescription.UTF8String);
}

//------------------------------------------------------------------------------
- (void) peripheral: (CBPeripheral *)peripheral
didDiscoverDescriptorsForCharacteristic:(CBDescriptor *)descriptor
              error:(NSError *)error
{
    [peripheral readValueForDescriptor:descriptor];
}
//------------------------------------------------------------------------------
- (void) peripheral: (CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    
}
//------------------------------------------------------------------------------
- (void) peripheral: (CBPeripheral *)peripheral
  didModifyServices: (NSArray<CBService *> *)invalidatedServices
{
    post("Service Modified\n");
    [manager cancelPeripheralConnection:peripheral];
}
- (void) peripheral:(CBPeripheral *)peripheral
        didReadRSSI:(NSNumber *)RSSI
              error:(NSError *)error
{
    NSUInteger deviceIndex = [discoveredPeripherals indexOfObject:peripheral];
    if(deviceIndex != NSNotFound)
        discoveredPeripheralsRSSIs[deviceIndex] = RSSI;
    
    onRSSIRead(maxObjectRef,
               peripheral.identifier.UUIDString.UTF8String,
               RSSI.intValue);
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
- (void)connectToDevice: (CBPeripheral*) device
            withOptions: (NSDictionary<NSString *,id> *) options
{
    if (device.state == CBPeripheralStateConnecting)
        [manager cancelPeripheralConnection:device];
    
    [manager connectPeripheral:device
                       options:options];
    
}



/// blacklist a specific device so that it is no longer discovered and no longer possible to to attekpt connection
/// this method will update the found device list.
/// @param device device to blacklist
- (void)blacklistDevice: (CBPeripheral*) device
{
    [manager cancelPeripheralConnection:device];
    NSUInteger index = [discoveredPeripherals indexOfObject:device];
    [discoveredPeripheralsRSSIs removeObjectAtIndex:index];
    [discoveredPeripherals removeObject:device];
    [blacklistPeripherals addObject:device];
    
    for(CBPeripheral* device in discoveredPeripherals)
    {
        NSUInteger index = [discoveredPeripherals indexOfObject:device];
        NSNumber*  rssi  = discoveredPeripheralsRSSIs[index];
        
        outputFoundDeviceList(maxObjectRef,
                              index,
                              device.identifier.UUIDString.UTF8String,
                              rssi.intValue);
    }
}

- (void)writeToToCharacteristic: (const char*) cuuid
                      OfService: (const char*) suuid
                  OfFoundDevice: (int)   deviceIndex
                      withBytes: (void*) values
                       ofLength: (int)   numBytes
                    
{
    if (deviceIndex < discoveredPeripherals.count)
    {
        characteristicUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: cuuid] ];
        serviceUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: suuid] ];
        dataToWrite = [[NSData alloc] initWithBytes:values
                                             length:numBytes];
        connectMode = BLE_CONNECT_WRITE;
        [self
         connectToDevice:discoveredPeripherals[deviceIndex]   withOptions:nil];
    }
}

@end
