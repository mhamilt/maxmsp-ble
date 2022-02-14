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
        orderedDeviceDict = [[NSMutableArray alloc] init];
        devices = [[NSMutableDictionary alloc] init];
        
        bleQueue = centralDelegateQueue;
        manager = [[CBCentralManager alloc] initWithDelegate: self
                                                       queue: bleQueue];
        shouldReport = YES;
        shouldConnect = NO;
        ignoreUnconnectable = NO;
        rssiSensitivity = 127;
        ignoreiPhone = NO;
    }
    return self;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma mark Manager Methods

- (void)centralManager:(CBCentralManager *)central
 didDiscoverPeripheral:(CBPeripheral *)aPeripheral
     advertisementData:(NSDictionary<NSString *,id> *)advertisementData
                  RSSI:(NSNumber *)RSSI
{
    
    if (![discoveredPeripherals containsObject: aPeripheral]
        && ! [blacklistPeripherals containsObject: aPeripheral]
        && (abs(RSSI.intValue) < abs(rssiSensitivity) && RSSI.intValue < 0)
        && !(advertisementData[@"kCBAdvDataAppleMfgData"] && ignoreiPhone)
        )
    {
        NSUInteger deviceIndex = discoveredPeripherals.count;
        
        if(shouldReport)
        {
            NSString* deviceName = ((aPeripheral.name) ?
                                    [NSString stringWithFormat:@"<Name: %@>,",
                                     aPeripheral.name] :
                                    [NSString stringWithFormat:@""]);
            
            post("Index: %d, %s UUID: %s, RSSI: %d\n",
                 deviceIndex,
                 deviceName.UTF8String,
                 aPeripheral.identifier.UUIDString.UTF8String,
                 RSSI.intValue);
            
            if(advertisementData[CBAdvertisementDataServiceUUIDsKey])
            {
                NSArray* advertisedServices =  (advertisementData[CBAdvertisementDataServiceUUIDsKey]);
                post("Services: %s", advertisedServices.description.UTF8String);
            }
            
            post("------------------------");
            
        }
        
        [discoveredPeripherals addObject:aPeripheral];
        [discoveredPeripheralsRSSIs addObject:RSSI];
        
        [devices setObject: [[NSMutableDictionary alloc] initWithDictionary:@{
            @"CBPeripheral":aPeripheral,
            @"RSSI": RSSI,
            @"AdvertisementData":advertisementData
        }]
                    forKey: aPeripheral.identifier.UUIDString];
        
        [orderedDeviceDict addObject:devices[aPeripheral.identifier.UUIDString]];
        
        
        outputFoundDeviceList(maxObjectRef,
                              deviceIndex,
                              aPeripheral.identifier.UUIDString.UTF8String,
                              RSSI.intValue);
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
    [aPeripheral discoverServices:nil];
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
    CBPeripheral* device = [self getDeviceAtIndex:deviceIndex];
    if (device)
        [self connectToDevice:device
                  withOptions:nil];
}

- (void)connectToDeviceWithUUID: (const char*) duuid
{
    NSUUID* deviceUUID = [self getValidUUID: [[NSString alloc] initWithUTF8String:duuid]];
    if(deviceUUID)
    {
        CBPeripheral* device = [self getPeriphralWithUUID: deviceUUID];
        if (device)
            [self  connectToDevice:device
                       withOptions:nil];
        else
            post("Device %s not found", duuid);
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
    [devices removeAllObjects];
    [orderedDeviceDict removeAllObjects];
}

//------------------------------------------------------------------------------

- (void)blacklistDevicesStillConnecting
{
    
}
//------------------------------------------------------------------------------
- (void) getRssiOfFoundDevice: (int) deviceIndex
{
    CBPeripheral* device = [self getDeviceAtIndex:deviceIndex];
    if (device)
        [device readRSSI];
}

//------------------------------------------------------------------------------

- (void)setRssiSensitivity:(int)newRSSISensitivity
{    
    rssiSensitivity = newRSSISensitivity;
}

//------------------------------------------------------------------------------

- (void)setIgnoreiPhone:(BOOL)shouldIgnore
{
    ignoreiPhone = shouldIgnore;
}

//------------------------------------------------------------------------------
- (void)setMaxObjectRef: (MaxExternalObject *) extMaxObjectRef
{
    maxObjectRef = extMaxObjectRef;
}

//------------------------------------------------------------------------------

- (void)setReporting: (BOOL) reportingMode
{
    shouldReport = reportingMode;
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

- (const char*)getDeviceUUIDatIndex: (int) deviceIndex
{
    CBPeripheral* device = discoveredPeripherals[deviceIndex];
    return device.identifier.UUIDString.UTF8String;
}

//------------------------------------------------------------------------------

- (int)getNumberOfDevices
{
    return (int)discoveredPeripherals.count;
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
/// Invoked upon completion of a -[discoverServices:] request.
/// Discover available characteristics on interested services
- (void) peripheral: (CBPeripheral *)aPeripheral
didDiscoverServices: (NSError *)error
{
    if(error || (aPeripheral.services.count == 0))
    {
        post((error) ? error.localizedDescription.UTF8String : "No matching services found");
    }
    else
    {
        for (CBService *aService in aPeripheral.services)
        {
            [aPeripheral discoverCharacteristics: nil
                                      forService: aService];
        }
    }
}
//------------------------------------------------------------------------------
// Invoked upon completion of a -[discoverCharacteristics:forService:] request.
// Perform appropriate operations on interested characteristics
- (void) peripheral: (CBPeripheral *)aPeripheral didDiscoverCharacteristicsForService:(CBService *)service
              error: (NSError *)error
{
    if(error || (service.characteristics.count == 0))
    {
        post((error) ? error.localizedDescription.UTF8String : "No matching characteristics found");
    }
    else
    {
        for (CBCharacteristic *aChar in service.characteristics)
        {
            if (aChar.properties & CBCharacteristicPropertyRead)
                [aPeripheral readValueForCharacteristic:aChar];
            else
                [self postCharacteristicDescription:aChar];
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
- (void) peripheral: (CBPeripheral *)aPeripheral
didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic
              error:(NSError *)error
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
//------------------------------------------------------------------------------
/// <#Description#>
/// @param characteristic <#characteristic description#>
- (NSString*) getCharacteristicPropertiesDescription:(CBCharacteristic *)characteristic
{
    NSMutableArray* properties = [[NSMutableArray alloc] init];
    
    if (characteristic.properties & CBCharacteristicPropertyBroadcast)
        [properties addObject:@"Broadcast"];
    if (characteristic.properties & CBCharacteristicPropertyRead)
        [properties addObject:@"Read"];
    if (characteristic.properties & CBCharacteristicPropertyWriteWithoutResponse)
        [properties addObject:@"Write Without Response"];
    if (characteristic.properties & CBCharacteristicPropertyWrite)
        [properties addObject:@"Write"];
    if (characteristic.properties & CBCharacteristicPropertyNotify)
        [properties addObject:@"Notify"];
    if (characteristic.properties & CBCharacteristicPropertyIndicate)
        [properties addObject:@"Indicate"];
    if (characteristic.properties & CBCharacteristicPropertyAuthenticatedSignedWrites)
        [properties addObject:@"SignedWrites"];
    if (characteristic.properties & CBCharacteristicPropertyExtendedProperties)
        [properties addObject:@"ExtendedProperties"];
    if (characteristic.properties & CBCharacteristicPropertyNotifyEncryptionRequired)
        [properties addObject:@"NotifyEncryptionRequired"];
    if (characteristic.properties & CBCharacteristicPropertyIndicateEncryptionRequired)
        [properties addObject:@"IndicateEncryptionRequired"];
    
    NSMutableString* propertiesDescription = [[NSMutableString alloc] initWithString:@""];
    
    for (NSString* property in properties)
    {
        [propertiesDescription appendString:property];
        if(!(property == properties.lastObject))
            [propertiesDescription appendString:@", "];
    }
    
    return [NSString stringWithString: propertiesDescription];
}
//------------------------------------------------------------------------------
/// <#Description#>
/// @param characteristic <#characteristic description#>
- (void) postCharacteristicDescription: (CBCharacteristic *)characteristic
{
    NSString* valueString = nil;
    if (characteristic.value)
    {
        if([characteristic.UUID.description containsString:@"String"] || [characteristic.UUID.description containsString:@"Name"])
            valueString = [[NSString alloc] initWithData:characteristic.value
                                                encoding:NSUTF8StringEncoding];
        else if ([characteristic.UUID.UUIDString isEqual:@"2A19"])
            valueString = [NSString stringWithFormat:@"%d", *(int*)characteristic.value.bytes];
        else
            valueString = characteristic.value.description;
    }
    
    NSString* propertiesDescription = [self getCharacteristicPropertiesDescription:characteristic];
    
    CBPeripheral* device = characteristic.service.peripheral;
    CBService* service = characteristic.service;
    
    post("Device: %s Service (%s) %s Char: (%s) %s: Value: %s Properties: %s",
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
- (void) peripheral: (CBPeripheral *)peripheral
didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic
              error:(NSError *)error
{
    if (error)
        post(error.localizedDescription.UTF8String);
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
- (void)            peripheral:(CBPeripheral *)peripheral
didWriteValueForCharacteristic:(CBCharacteristic *)characteristic
                         error:(NSError *)error
{
    
    if(error)
        post("Couldn't write to %s: %s",
             characteristic.UUID.UUIDString.UTF8String,
             [[error localizedDescription] UTF8String]);
    else
        if (shouldReport)
            post("Success Writing to %s", characteristic.UUID.UUIDString.UTF8String);
    
}

//------------------------------------------------------------------------------
- (void)connectToDevice: (CBPeripheral*) device
            withOptions: (NSDictionary<NSString *,id> *) options
{
    if (device.state == CBPeripheralStateConnecting)
        [manager cancelPeripheralConnection:device];
    
    [manager connectPeripheral:device
                       options:options];
    
}
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
#pragma mark Main Peripheral Interface

- (void)forCharacteristic:(const char *)cuuid
                OfService:(const char *)suuid
          ofDeviceAtIndex:(int)i
             performBlock:(void (^)(CBUUID* charUUID, CBUUID* serviceUUID, CBPeripheral* device)) callback

{
    CBUUID* serviceCBUUID = nil;
    CBUUID* characteristicCBUUID = nil;
    
    if (strlen(cuuid) == 4)
        characteristicCBUUID =  [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String:cuuid]];
    else
    {
        NSUUID* cuuidString = [self getValidUUID: [[NSString alloc] initWithUTF8String: cuuid]];
        if (cuuid)
            characteristicCBUUID =  [CBUUID UUIDWithNSUUID: cuuidString];
    }
    
    if (strlen(suuid) == 4)
        serviceCBUUID =  [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String:suuid]];
    else
    {
        NSUUID* suuidString = [self getValidUUID: [[NSString alloc] initWithUTF8String: suuid]];
        if (suuid)
            serviceCBUUID =  [CBUUID UUIDWithNSUUID: suuidString];
    }
    
    if (serviceCBUUID && characteristicCBUUID)
    {
        CBPeripheral* device = [self getDeviceAtIndex:i];
        if (device)
            callback(characteristicCBUUID, serviceCBUUID, device);
        
    }
}

//------------------------------------------------------------------------------

- (void) forCharacteristic:(const char *)cuuid
                 ofService:(const char *)suuid
                  ofDevice:(const char *)duuid
              performBlock:(void (^)(CBUUID* charUUID, CBUUID* serviceUUID, CBPeripheral* device)) callback
{
    NSUUID* deviceUUID = [self getValidUUID: [[NSString alloc] initWithUTF8String:duuid]];
    
    if (deviceUUID)
    {
        CBUUID* serviceCBUUID = nil;
        CBUUID* characteristicCBUUID = nil;
        
        if (strlen(cuuid) == 4)
            characteristicCBUUID =  [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String:cuuid]];
        else
        {
            NSUUID* cuuidString = [self getValidUUID: [[NSString alloc] initWithUTF8String: cuuid]];
            if (cuuid)
                characteristicCBUUID =  [CBUUID UUIDWithNSUUID: cuuidString];
        }
        
        if (strlen(suuid) == 4)
            serviceCBUUID =  [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String:suuid]];
        else
        {
            NSUUID* suuidString = [self getValidUUID: [[NSString alloc] initWithUTF8String: suuid]];
            if (suuid)
                serviceCBUUID =  [CBUUID UUIDWithNSUUID: suuidString];
        }
        
        if (serviceCBUUID && characteristicCBUUID)
        {
            CBPeripheral* device = [self getPeriphralWithUUID: deviceUUID];
            if (device)
                callback(characteristicCBUUID, serviceCBUUID, device);
        }
    }
}


//------------------------------------------------------------------------------

- (void)readCharacteristic:(const char *)cuuid
                 OfService:(const char *)suuid
          ofDeviceWithUUID:(const char *)duuid
{
    [self forCharacteristic:cuuid
                  ofService:suuid
                   ofDevice:duuid
               performBlock:^(CBUUID *charUUID,
                              CBUUID *serviceUUID,
                              CBPeripheral *device) {
        CBCharacteristic* characteristic = [device getCharacteristicWithUUID:charUUID
                                                          forServiceWithUUID:serviceUUID];
        if(characteristic)
            [device readValueForCharacteristic:characteristic];
    }];
}

//------------------------------------------------------------------------------

- (void)readCharacteristic:(const char *)cuuid
                 OfService:(const char *)suuid
           ofDeviceAtIndex:(int)i
{
    [self forCharacteristic:cuuid
                  OfService:suuid
            ofDeviceAtIndex:i
               performBlock:^(CBUUID *charUUID, CBUUID *serviceUUID, CBPeripheral *device) {
        
        CBCharacteristic* characteristic = [device getCharacteristicWithUUID:charUUID
                                                          forServiceWithUUID:serviceUUID];
        if(characteristic)
            [device readValueForCharacteristic:characteristic];
    }];
}

//------------------------------------------------------------------------------

- (void)readCharacteristicsOfDeviceWithUUID:(const char*)duuid
{
    NSUUID* deviceUUID = [self getValidUUID: [[NSString alloc] initWithUTF8String:duuid]];
    if(deviceUUID)
    {
        CBPeripheral* device = [self getPeriphralWithUUID: deviceUUID];
        if (device)
        {
            for (CBService* service in [device services])
            {
                for (CBCharacteristic* characteristic in [service characteristics])
                {
                    if (characteristic.properties & CBCharacteristicPropertyRead)
                        [device readValueForCharacteristic:characteristic];
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

- (void)readAllCharacteristicOfDeviceAtIndex:(int)i
{
    CBPeripheral* device = [self getDeviceAtIndex:i];
    if (device)
    {
        for (CBService* service in [device services])
        {
            for (CBCharacteristic* characteristic in [service characteristics])
            {
                if (characteristic.properties & CBCharacteristicPropertyRead)
                    [device readValueForCharacteristic:characteristic];
            }
        }
    }
}

//------------------------------------------------------------------------------

- (void)subscribeToCharacteristic:(const char *)cuuid
                        OfService:(const char *)suuid
                 ofDeviceWithUUID:(const char *)duuid
                  shouldSubscribe:(BOOL)subscriptionStatus
{
    __block BOOL shouldSubscribe = subscriptionStatus;
    
    [self forCharacteristic:cuuid
                  ofService:suuid
                   ofDevice:duuid
               performBlock:^(CBUUID *charUUID, CBUUID *serviceUUID, CBPeripheral *device)
     {
        CBCharacteristic* characteristic = [device getCharacteristicWithUUID:charUUID
                                                          forServiceWithUUID:serviceUUID];
        if(characteristic)
        {
            post("%s %s: %s\n",
                 (shouldSubscribe) ? "Subscribe to" : "Unsubscribe from",
                 serviceUUID.UUIDString.UTF8String,
                 charUUID.UUIDString.UTF8String);
            
            [device setNotifyValue:shouldSubscribe
                 forCharacteristic:characteristic];
        }
    }];
}

- (void)subscribeToCharacteristic:(const char *)cuuid
                        OfService:(const char *)suuid
                  ofDeviceAtIndex:(int)i
                  shouldSubscribe:(BOOL)subscriptionStatus
{
    __block BOOL shouldSubscribe = subscriptionStatus;
    
    [self forCharacteristic:cuuid
                  OfService:suuid
            ofDeviceAtIndex:i
               performBlock:^(CBUUID *charUUID, CBUUID *serviceUUID, CBPeripheral *device) {
        CBCharacteristic* characteristic = [device getCharacteristicWithUUID:charUUID
                                                          forServiceWithUUID:serviceUUID];
        if(characteristic)
        {
            post("%s %s: %s\n",
                 (shouldSubscribe) ? "Subscribe to" : "Unsubscribe from",
                 serviceUUID.UUIDString.UTF8String,
                 charUUID.UUIDString.UTF8String);
            
            [device setNotifyValue:shouldSubscribe
                 forCharacteristic:characteristic];
        }
    }];
}

//------------------------------------------------------------------------------

- (void)writeToCharacteristic: (const char*) cuuid
                    OfService: (const char*) suuid
             ofDeviceWithUUID: (const char*) duuid
                    withBytes: (void*)  values
                     ofLength: (size_t) numBytes
{
    __block NSData* dataToWrite = [[NSData alloc] initWithBytes:values
                                                         length:numBytes];
    [self forCharacteristic:cuuid
                  ofService:suuid
                   ofDevice:duuid
               performBlock:^(CBUUID *charUUID, CBUUID *serviceUUID, CBPeripheral *device) {
        [self writeToCharacteristic:charUUID
                          OfService:serviceUUID
                      OfFoundDevice:device
                           withData:dataToWrite];
    }];
}

//------------------------------------------------------------------------------

- (void)writeToCharacteristic: (const char*) cuuid
                    OfService: (const char*) suuid
                OfFoundDevice: (int)    deviceIndex
                    withBytes: (void*)  values
                     ofLength: (size_t) numBytes
{
    __block NSData* dataToWrite = [[NSData alloc] initWithBytes:values
                                                         length:numBytes];
    [self forCharacteristic:cuuid
                  OfService:suuid
            ofDeviceAtIndex:deviceIndex
               performBlock:^(CBUUID *charUUID, CBUUID *serviceUUID, CBPeripheral *device) {
        
        [self writeToCharacteristic:charUUID
                          OfService:serviceUUID
                      OfFoundDevice:device
                           withData:dataToWrite];
    }];
}

//------------------------------------------------------------------------------

- (void)writeToCharacteristic: (CBUUID*) cuuid
                    OfService: (CBUUID*) suuid
                OfFoundDevice: (CBPeripheral*) device
                     withData: (NSData*)  data

{
    CBCharacteristic* characteristic = [device getCharacteristicWithUUID:cuuid
                                                      forServiceWithUUID:suuid];
    if(characteristic)
    {
        if(characteristic.properties & CBCharacteristicPropertyWriteWithoutResponse)
            [device writeValue:data
             forCharacteristic:characteristic
                          type:CBCharacteristicWriteWithoutResponse];
        else if(characteristic.properties & CBCharacteristicPropertyWrite)
            [device writeValue:data
             forCharacteristic:characteristic
                          type:CBCharacteristicWriteWithResponse];
        else
            post("Unable to Write to Chracteritic %s", cuuid);
    }
}

//------------------------------------------------------------------------------
#pragma mark Sanity Checking

-(BOOL)isValidUUID : (NSString *)UUIDString
{
    if(!([[NSUUID alloc] initWithUUIDString: UUIDString]))
    {
        post("%s is not a valid UUID", UUIDString.UTF8String);
        return NO;
    }
    return YES;
}

//------------------------------------------------------------------------------

-(NSUUID*)getValidUUID : (NSString *)UUIDString
{
    NSUUID* validUUID = [[NSUUID alloc] initWithUUIDString: UUIDString];
    if(!validUUID)
        post("%s is not a valid UUID", UUIDString.UTF8String);
    
    return validUUID;
}

//------------------------------------------------------------------------------

-(BOOL)isDeviceIndexInRange: (int) deviceIndex
{
    if (!(deviceIndex < discoveredPeripherals.count))
    {
        post("Index %d is out of range", deviceIndex);
        return NO;
    }
    return YES;
}

//------------------------------------------------------------------------------

-(CBPeripheral*)getDeviceAtIndex: (int) i
{
    if (i >= discoveredPeripherals.count)
    {
        post("Index %d is out of range", i);
        return nil;
    }
    return discoveredPeripherals[i];
}

//------------------------------------------------------------------------------

-(CBPeripheral*) getPeriphralWithUUID: (NSUUID*)peripheralUUID
{
    for (int i = 0; i < discoveredPeripherals.count; i++)
    {
        if ([discoveredPeripherals[i].identifier isEqual:peripheralUUID])
        {
            return discoveredPeripherals[i];
        }
    }
    return nil;
}
//------------------------------------------------------------------------------
@end
