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
        //&& !(!connectable && ignoreUnconnectable)
        && (abs([RSSI intValue]) < abs(rssiSensitivity))
        && !(advertisementData[@"kCBAdvDataAppleMfgData"] && ignoreiPhone)
        )
    {
        if(shouldReport)
        {
            NSUInteger index = discoveredPeripherals.count;
            if (@available(macOS 10.13, *))
                post("Index: %d, UUID: %s, RSSI: %d\n",
                     index,
                     aPeripheral.identifier.UUIDString.UTF8String,
                     RSSI.intValue);
            post("------------------------");
        }
        [discoveredPeripherals addObject:aPeripheral];
        [discoveredPeripheralsRSSIs addObject:RSSI];
    }
}

//------------------------------------------------------------------------------
- (void) centralManager: (CBCentralManager *)central
   didConnectPeripheral: (CBPeripheral *)aPeripheral
{
    if (shouldReport)
        post("Connected!\n");
    
    [aPeripheral setDelegate:self];
    
    switch (connectMode)
    {
        case BLE_CONNECT_GET_RSSI:
            [aPeripheral readRSSI]; // go to peripheralDidUpdateRSSI
            break;
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
    if ([manager state] == CBCentralManagerStatePoweredOn)
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
        post("Error %s\n",[[error localizedDescription] UTF8String]);
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
        post("Failed to connect to peripheral: %s", aPeripheral, [[error localizedDescription] UTF8String]);
    }
}
//------------------------------------------------------------------------------
#pragma mark Interface Methods

- (void) scan;
{
    [servicesToScan removeAllObjects];
    [self startScan];
}


- (void) stop;
{
    post("Stop scanning\n");
    shouldConnect = NO;
    [manager stopScan];
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

- (void) startScan
{
    if (@available(macOS 10.13, *))
    {
        if (![manager isScanning])
        {
            post("Start scanning\n");
            post("------------------------");
        }
    }
    
    [manager scanForPeripheralsWithServices: ((servicesToScan.count) ? servicesToScan : nil)
                                    options: @{CBCentralManagerScanOptionAllowDuplicatesKey: @YES}];
}

//------------------------------------------------------------------------------

- (void) connectToFoundDevice: (int) deviceIndex
{
    if (deviceIndex < discoveredPeripherals.count)
    {
        connectMode = BLE_CONNECT_EVERYTHING;
        [manager connectPeripheral:discoveredPeripherals[deviceIndex] options:nil];
    }
    
}

//------------------------------------------------------------------------------

- (void) clearDicoveredPeripherals
{
    [discoveredPeripherals removeAllObjects];
}

//------------------------------------------------------------------------------

- (void)setRssiSensitivity:(int)newRSSISensitivity
{    
    rssiSensitivity = newRSSISensitivity;
}

- (void)setIgnoreiPhone:(BOOL)shouldIgnore
{
    ignoreiPhone = shouldIgnore;
}

//------------------------------------------------------------------------------

- (void) getRssiOfFoundDevice: (int) deviceIndex
{
    if (deviceIndex < discoveredPeripherals.count)
    {
        connectMode = BLE_CONNECT_GET_RSSI;
        [manager connectPeripheral:discoveredPeripherals[deviceIndex] options:nil];
    }
}


- (void) subscribeToCharacteristic: (const char *) cuuid
                         OfService: (const char*)  suuid
                     OfFoundDevice: (int) deviceIndex
{
    connectMode = BLE_CONNECT_SUBSCRIBE_CHARACTERISTIC;
    characteristicUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: cuuid] ];
    serviceUuid = [CBUUID UUIDWithString: [[NSString alloc] initWithUTF8String: suuid] ];
    [manager connectPeripheral:discoveredPeripherals[deviceIndex] options:nil];
    
    post("Subscribe to %s: %s\n",
         serviceUuid.UUIDString.UTF8String,
         characteristicUuid.UUIDString.UTF8String);
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
    switch (connectMode) {
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
            
        default:
        {
            const char *serviceUUID = [[[service UUID] UUIDString] UTF8String];
            NSString* sericeDescription = nil;
            if (@available(macOS 10.13, *))
            {
                sericeDescription = [MacosBleCentral getCBUUIDDescription:service];
            }
            
            if(shouldReport)
                post("Service %s: %s\n", serviceUUID, ((sericeDescription)?sericeDescription.UTF8String:""));
            
            for (CBCharacteristic *aChar in service.characteristics)
            {
                if (aChar.properties & CBCharacteristicPropertyRead)
                {
                    [aPeripheral readValueForCharacteristic:aChar];
                }
                
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
    post("didUpdateValueForDescriptor\n");
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
        //        if(shouldReport)
        post("Error %s\n",[[error localizedDescription] UTF8String]);
        
    }
}

- (void) postCharacteristicDescription: (CBCharacteristic *)characteristic
{
    NSString* charDescription = nil;
    
    if (@available(macOS 10.13, *))
        charDescription = [MacosBleCentral getCBUUIDDescription:characteristic];
    
    if(!charDescription)
        charDescription = [NSString stringWithFormat:@"Characteristic %@", characteristic.UUID.UUIDString];
    
    NSString* valueString = nil;
    
    if([charDescription containsString:@"String"] || [charDescription containsString:@"Name"])
        valueString = [[NSString alloc] initWithData:characteristic.value encoding:NSUTF8StringEncoding];
    else if ([characteristic.UUID.UUIDString isEqual:@"2A19"])
        valueString = [NSString stringWithFormat:@"%d", *(int*)characteristic.value.bytes];
    else
        valueString = characteristic.value.description;
    
    NSMutableString* propertiesDescription = [[NSMutableString alloc] initWithString:@"Properties: "];
    
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
    
    post("%s: %s : %s", charDescription.UTF8String, valueString.UTF8String, propertiesDescription.UTF8String);
}

//------------------------------------------------------------------------------
- (void) peripheral: (CBPeripheral *)peripheral
didDiscoverDescriptorsForCharacteristic:(CBDescriptor *)descriptor
              error:(NSError *)error
{
    post("didDiscoverDescriptorsForCharacteristic");
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
    if (@available(macOS 10.13, *))
    {
        onRSSIRead(maxObjectRef,
                   peripheral.identifier.UUIDString.UTF8String,
                   RSSI.intValue);
    }
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
        if (@available(macOS 10.13, *))
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
}

+ (void) printCharacteristicProperties:(CBCharacteristicProperties*) properties
{
    
}


+ (NSString*)getCBUUIDDescription: (CBAttribute*) attribute
API_AVAILABLE(macos(10.13))
{
    NSString* attributeType = nil;
    if ([attribute isKindOfClass:[CBService class]])
        attributeType = @"service";
    else if ([attribute isKindOfClass:[CBCharacteristic class]])
        attributeType = @"characteristic";
    
    NSDictionary* uuidDescriptions =
    @{
        @"service":
            @{
                @"1800": @"Generic Access",
                @"1801": @"Generic Attribute",
                @"1802": @"Immediate Alert",
                @"1803": @"Link Loss",
                @"1804": @"Tx Power",
                @"1805": @"Current Time",
                @"1806": @"Reference Time Update",
                @"1807": @"Next DST Change",
                @"1808": @"Glucose",
                @"1809": @"Health Thermometer",
                @"180A": @"Device Information",
                @"180D": @"Heart Rate",
                @"180E": @"Phone Alert Status",
                @"180F": @"Battery",
                @"1810": @"Blood Pressure",
                @"1811": @"Alert Notification",
                @"1812": @"Human Interface Device",
                @"1813": @"Scan Parameters",
                @"1814": @"Running Speed and Cadence",
                @"1815": @"Automation IO",
                @"1816": @"Cycling Speed and Cadence",
                @"1818": @"Cycling Power",
                @"1819": @"Location and Navigation",
                @"181A": @"Environmental Sensing",
                @"181B": @"Body Composition",
                @"181C": @"User Data",
                @"181D": @"Weight Scale",
                @"181E": @"Bond Management",
                @"181F": @"Continuous Glucose Monitoring",
                @"1820": @"Internet Protocol Support",
                @"1821": @"Indoor Positioning",
                @"1822": @"Pulse Oximeter",
                @"1823": @"HTTP Proxy",
                @"1824": @"Transport Discovery",
                @"1825": @"Object Transfer",
                @"1826": @"Fitness Machine",
                @"1827": @"Mesh Provisioning",
                @"1828": @"Mesh Proxy",
                @"1829": @"Reconnection Configuration",
                @"183A": @"Insulin Delivery",
                @"183B": @"Binary Sensor",
                @"183C": @"Emergency Configuration",
                @"183E": @"Physical Activity Monitor",
                @"1843": @"Audio Input Control",
                @"1844": @"Volume Control",
                @"1845": @"Volume Offset Control",
                @"1846": @"Coordinated Set Identification Service",
                @"1847": @"Device Time",
                @"1848": @"Media Control Service",
                @"1849": @"Generic Media Control Service",
                @"184A": @"Constant Tone Extension",
                @"184B": @"Telephone Bearer Service",
                @"184C": @"Generic Telephone Bearer Service 184D Microphone Control"
            },
        @"characteristic": @{
                @"2A00": @"Device Name",
                @"2A01": @"Appearance",
                @"2A02": @"Peripheral Privacy Flag",
                @"2A03": @"Reconnection Address",
                @"2A04": @"Peripheral Preferred Connection Parameters",
                @"2A05": @"Service Changed",
                @"2A06": @"Alert Level",
                @"2A07": @"Tx Power Level",
                @"2A08": @"Date Time",
                @"2A09": @"Day of Week",
                @"2A0A": @"Day Date Time",
                @"2A0C": @"Exact Time 256",
                @"2A0D": @"DST Offset",
                @"2A0E": @"Time Zone",
                @"2A0F": @"Local Time Information",
                @"2A11": @"Time with DST",
                @"2A12": @"Time Accuracy",
                @"2A13": @"Time Source",
                @"2A14": @"Reference Time Information",
                @"2A16": @"Time Update Control Point",
                @"2A17": @"Time Update State",
                @"2A18": @"Glucose Measurement",
                @"2A19": @"Battery Level",
                @"2A1C": @"Temperature Measurement",
                @"2A1D": @"Temperature Type",
                @"2A1E": @"Intermediate Temperature",
                @"2A21": @"Measurement Interval",
                @"2A22": @"Boot Keyboard Input Report",
                @"2A23": @"System ID",
                @"2A24": @"Model Number String",
                @"2A25": @"Serial Number String",
                @"2A26": @"Firmware Revision String",
                @"2A27": @"Hardware Revision String",
                @"2A28": @"Software Revision String",
                @"2A29": @"Manufacturer Name String",
                @"2A2A": @"IEEE 11073-20601 Regulatory Certification Data List",
                @"2A2B": @"Current Time",
                @"2A31": @"Scan Refresh",
                @"2A32": @"Boot Keyboard Output Report",
                @"2A33": @"Boot Mouse Input Report",
                @"2A34": @"Glucose Measurement Context",
                @"2A35": @"Blood Pressure Measurement",
                @"2A36": @"Intermediate Cuff Pressure",
                @"2A37": @"Heart Rate Measurement",
                @"2A38": @"Body Sensor Location",
                @"2A39": @"Heart Rate Control Point",
                @"2A3F": @"Alert Status",
                @"2A40": @"Ringer Control Point",
                @"2A41": @"Ringer Setting",
                @"2A42": @"Alert Category ID Bit Mask",
                @"2A43": @"Alert Category ID",
                @"2A44": @"Alert Notification Control Point",
                @"2A45": @"Unread Alert Status",
                @"2A46": @"New Alert",
                @"2A47": @"Supported New Alert Category",
                @"2A48": @"Supported Unread Alert Category",
                @"2A49": @"Blood Pressure Feature",
                @"2A4A": @"HID Information Allocated for",
                @"2A4C": @"HID Control Point",
                @"2A4D": @"Report",
                @"2A4E": @"Protocol Mode",
                @"2A4F": @"Scan Interval Window",
                @"2A50": @"PnP ID",
                @"2A51": @"Glucose Feature",
                @"2A52": @"Record Access Control Point",
                @"2A53": @"RSC Measurement",
                @"2A54": @"RSC Feature",
                @"2A55": @"SC Control Point",
                @"2A5A": @"Aggregate",
                @"2A5B": @"CSC Measurement",
                @"2A5C": @"CSC Feature",
                @"2A5D": @"Sensor Location",
                @"2A5E": @"PLX Spot-Check Measurement",
                @"2A5F": @"PLX Continuous Measurement",
                @"2A60": @"PLX Features",
                @"2A63": @"Cycling Power Measurement",
                @"2A64": @"Cycling Power Vector",
                @"2A65": @"Cycling Power Feature",
                @"2A66": @"Cycling Power Control Point",
                @"2A67": @"Location and Speed",
                @"2A68": @"Navigation",
                @"2A69": @"Position Quality",
                @"2A6A": @"LN Feature",
                @"2A6B": @"LN Control Point",
                @"2A6C": @"Elevation",
                @"2A6D": @"Pressure",
                @"2A6E": @"Temperature",
                @"2A6F": @"Humidity",
                @"2A70": @"True Wind Speed",
                @"2A71": @"True Wind Direction",
                @"2A72": @"Apparent Wind Speed",
                @"2A73": @"Apparent Wind Direction",
                @"2A74": @"Gust Factor",
                @"2A75": @"Pollen Concentration",
                @"2A76": @"UV Index",
                @"2A77": @"Irradiance",
                @"2A78": @"Rainfall",
                @"2A79": @"Wind Chill",
                @"2A7A": @"Heat Index",
                @"2A7B": @"Dew Point",
                @"2A7D": @"Descriptor Value Changed",
                @"2A7E": @"Aerobic Heart Rate Lower Limit",
                @"2A7F": @"Aerobic Threshold",
                @"2A80": @"Age",
                @"2A81": @"Anaerobic Heart Rate Lower Limit",
                @"2A82": @"Anaerobic Heart Rate Upper Limit",
                @"2A83": @"Anaerobic Threshold",
                @"2A84": @"Aerobic Heart Rate Upper Limit",
                @"2A85": @"Date of Birth",
                @"2A86": @"Date of Threshold Assessment",
                @"2A87": @"Email Address",
                @"2A88": @"Fat Burn Heart Rate Lower Limit",
                @"2A89": @"Fat Burn Heart Rate Upper Limit",
                @"2A8A": @"First Name",
                @"2A8B": @"Five Zone Heart Rate Limits",
                @"2A8C": @"Gender",
                @"2A8D": @"Heart Rate Max",
                @"2A8E": @"Height",
                @"2A8F": @"Hip Circumference",
                @"2A90": @"Last Name",
                @"2A91": @"Maximum Recommended Heart Rate",
                @"2A92": @"Resting Heart Rate",
                @"2A93": @"Sport Type for Aerobic and Anaerobic Thresholds",
                @"2A94": @"Three Zone Heart Rate Limits",
                @"2A95": @"Two Zone Heart Rate Limits",
                @"2A96": @"VO2 Max",
                @"2A97": @"Waist Circumference",
                @"2A98": @"Weight",
                @"2A99": @"Database Change Increment",
                @"2A9A": @"User Index",
                @"2A9B": @"Body Composition Feature",
                @"2A9C": @"Body Composition Measurement",
                @"2A9D": @"Weight Measurement",
                @"2A9E": @"Weight Scale Feature",
                @"2A9F": @"User Control Point",
                @"2AA0": @"Magnetic Flux Density - 2D",
                @"2AA1": @"Magnetic Flux Density - 3D",
                @"2AA2": @"Language",
                @"2AA3": @"Barometric Pressure Trend",
                @"2AA4": @"Bond Management Control Point",
                @"2AA5": @"Bond Management Feature",
                @"2AA6": @"Central Address Resolution",
                @"2AA7": @"CGM Measurement",
                @"2AA8": @"CGM Feature",
                @"2AA9": @"CGM Status",
                @"2AAA": @"CGM Session Start Time",
                @"2AAB": @"CGM Session Run Time",
                @"2AAC": @"CGM Specific Ops Control Point",
                @"2AAD": @"Indoor Positioning Configuration",
                @"2AAE": @"Latitude",
                @"2AAF": @"Longitude",
                @"2AB0": @"Local North Coordinate",
                @"2AB1": @"Local East Coordinate",
                @"2AB2": @"Floor Number Allocated for",
                @"2AB3": @"Altitude Uncertainty",
                @"2AB5": @"Location Name",
                @"2AB6": @"URI",
                @"2AB7": @"HTTP Headers",
                @"2AB8": @"HTTP Status Code",
                @"2AB9": @"HTTP Entity Body",
                @"2ABA": @"HTTP Control Point",
                @"2ABB": @"HTTPS Security",
                @"2ABC": @"TDS Control Point",
                @"2ABD": @"OTS Feature",
                @"2ABE": @"object name",
                @"2ABF": @"object type",
                @"2AC0": @"object size",
                @"2AC1": @"object first created",
                @"2AC2": @"object last modified",
                @"2AC3": @"object ID",
                @"2AC4": @"object properties",
                @"2AC5": @"object actioncontrol point",
                @"2AC6": @"object list control point",
                @"2AC7": @"object list filter",
                @"2AC8": @"object changed",
                @"2AC9": @"Resolvable Private Address Only",
                @"2ACA": @"Unspecified",
                @"2ACB": @"Directory Listing",
                @"2ACC": @"Fitness Machine Feature",
                @"2ACD": @"Treadmill Data",
                @"2ACE": @"Cross Trainer Data",
                @"2ACF": @"Step Climber Data",
                @"2AD0": @"Stair Climber Data",
                @"2AD1": @"Rower Data",
                @"2AD2": @"Indoor Bike Data",
                @"2AD3": @"Training Status",
                @"2AD4": @"Supported Speed Range",
                @"2AD5": @"Supported Inclination Range",
                @"2AD6": @"Supported Resistance Level Range",
                @"2AD7": @"Supported Heart Rate Range",
                @"2AD8": @"Supported Power Range",
                @"2AD9": @"Fitness Machine Control Point",
                @"2ADA": @"Fitness Machine Status",
                @"2ADB": @"Mesh Provisioning Data In",
                @"2ADC": @"Mesh Provisioning Data Out",
                @"2ADD": @"Mesh Proxy Data In",
                @"2ADE": @"Mesh Proxy Data Out",
                @"2AE0": @"Average Current",
                @"2AE1": @"Average Voltage",
                @"2AE2": @"Boolean",
                @"2AE3": @"Chromatic Distance From Planckian",
                @"2AE4": @"Chromaticity Coordinates",
                @"2AE5": @"Chromaticity in CCT And Duv Values",
                @"2AE6": @"Chromaticity Tolerance",
                @"2AE7": @"CIE 13.3-1995 Color Rendering Index",
                @"2AE8": @"Coefficient",
                @"2AE9": @"Correlated Color Temperature",
                @"2AEA": @"Count 16",
                @"2AEB": @"Count 24",
                @"2AEC": @"Country Code",
                @"2AED": @"Date UTC",
                @"2AEE": @"Electric Current",
                @"2AEF": @"Electric Current Range",
                @"2AF0": @"Electric Current Specification",
                @"2AF1": @"Electric Current Statistics",
                @"2AF2": @"Energy",
                @"2AF3": @"Energy In A Period Of Day",
                @"2AF4": @"Event Statistics",
                @"2AF5": @"Fixed String 16",
                @"2AF6": @"Fixed String 24",
                @"2AF7": @"Fixed String 36",
                @"2AF8": @"Fixed String 8",
                @"2AF9": @"Generic Level",
                @"2AFA": @"Global Trade Item Number",
                @"2AFB": @"Illuminance",
                @"2AFC": @"Luminous Efficacy",
                @"2AFD": @"Luminous Energy",
                @"2AFE": @"Luminous Exposure",
                @"2AFF": @"Luminous Flux",
                @"2B00": @"Luminous Flux Range",
                @"2B01": @"Luminous Intensity",
                @"2B02": @"Mass Flow",
                @"2B03": @"Perceived Lightness",
                @"2B04": @"Percentage 8",
                @"2B05": @"Power",
                @"2B06": @"Power Specification",
                @"2B07": @"Relative Runtime In A Current Range",
                @"2B08": @"Relative Runtime In A Generic Level Range",
                @"2B09": @"Relative Value In A Voltage Range",
                @"2B0A": @"Relative Value In An Illuminance Range",
                @"2B0B": @"Relative Value In A Period Of Day",
                @"2B0C": @"Relative Value In A Temperature Range",
                @"2B0D": @"Temperature 8",
                @"2B0E": @"Temperature 8 In A Period Of Day",
                @"2B0F": @"Temperature 8 Statistics",
                @"2B10": @"Temperature Range",
                @"2B11": @"Temperature Statistics",
                @"2B12": @"Time Decihour 8",
                @"2B13": @"Time Exponential 8",
                @"2B14": @"Time Hour 24",
                @"2B15": @"Time Millisecond 24",
                @"2B16": @"Time Second 16",
                @"2B17": @"Time Second 8",
                @"2B18": @"Voltage",
                @"2B19": @"Voltage Specification",
                @"2B1A": @"Voltage Statistics",
                @"2B1B": @"Volume Flow",
                @"2B1C": @"Chromaticity Coordinate",
                @"2B1D": @"RC Feature",
                @"2B1E": @"RC Settings",
                @"2B1F": @"Reconnection Configuration Control Point",
                @"2B20": @"IDD Status Changed",
                @"2B21": @"IDD Status",
                @"2B22": @"IDD Annunciation Status",
                @"2B23": @"IDD Features",
                @"2B24": @"IDD Status Reader Control Point",
                @"2B25": @"IDD Command Control Point",
                @"2B26": @"IDD Command Data",
                @"2B27": @"IDD Record Access Control Point",
                @"2B28": @"IDD History Data",
                @"2B29": @"Client Supported Features",
                @"2B2A": @"Database Hash",
                @"2B2B": @"BSS Control Point",
                @"2B2C": @"BSS Response",
                @"2B2D": @"Emergency ID",
                @"2B2E": @"Emergency Text",
                @"2B34": @"Enhanced Blood Pressure Measurement",
                @"2B35": @"Enhanced Intermediate Cuff Pressure",
                @"2B36": @"Blood Pressure Record",
                @"2B38": @"BR-EDR Handover Data",
                @"2B39": @"Bluetooth SIG Data",
                @"2B3A": @"Server Supported Features",
                @"2B3B": @"Physical Activity Monitor Features",
                @"2B3C": @"General Activity Instantaneous Data",
                @"2B3D": @"General Activity Summary Data",
                @"2B3E": @"CardioRespiratory Activity Instantaneous Data",
                @"2B3F": @"CardioRespiratory Activity Summary Data",
                @"2B40": @"Step Counter Activity Summary Data",
                @"2B41": @"Sleep Activity Instantaneous Data",
                @"2B42": @"Sleep Activity Summary Data",
                @"2B43": @"Physical Activity Monitor Control Point",
                @"2B44": @"Current Session",
                @"2B45": @"Session",
                @"2B46": @"Preferred Units",
                @"2B47": @"High Resolution Height",
                @"2B48": @"Middle Name",
                @"2B49": @"Stride Length",
                @"2B4A": @"Handedness",
                @"2B4B": @"Device Wearing Position",
                @"2B4C": @"Four Zone Heart Rate Limits",
                @"2B4D": @"High Intensity Exercise Threshold",
                @"2B4E": @"Activity Goal",
                @"2B4F": @"Sedentary Interval Notification",
                @"2B50": @"Caloric Intake",
                @"2B77": @"Audio Input State",
                @"2B78": @"Gain Settings Attribute",
                @"2B79": @"Audio Input Type",
                @"2B7A": @"Audio Input Status",
                @"2B7B": @"Audio Input Control Point",
                @"2B7C": @"Audio Input Description",
                @"2B7D": @"Volume State",
                @"2B7E": @"Volume Control Point",
                @"2B7F": @"Volume Flags",
                @"2B80": @"Offset State",
                @"2B81": @"Audio Location",
                @"2B82": @"Volume Offset Control Point",
                @"2B83": @"Audio Output Description",
                @"2B84": @"Set Identity Resolving Key Characteristic",
                @"2B85": @"Size Characteristic",
                @"2B86": @"Lock Characteristic",
                @"2B87": @"Rank Characteristic",
                @"2B8E": @"Device Time Feature 2B8F Device Time Parameters 2B90 Device Time",
                @"2B91": @"Device Time Control Point 2B92 Time Change Log Data 2B93 Media Player Name",
                @"2B94": @"Media Player Icon Object ID 2B95 Media Player Icon URL 2B96 Track Changed",
                @"2B97": @"Track Title",
                @"2B98": @"Track Duration",
                @"2B99": @"Track Position",
                @"2B9A": @"Playback Speed",
                @"2B9B": @"Seeking Speed",
                @"2B9C": @"Current Track Segments Object ID",
                @"2B9D": @"Current Track Object ID",
                @"2B9E": @"Next Track Object ID",
                @"2B9F": @"Parent Group Object ID",
                @"2BA0": @"Current Group Object ID",
                @"2BA1": @"Playing Order",
                @"2BA2": @"Playing Orders Supported",
                @"2BA3": @"Media State",
                @"2BA4": @"Media Control Point",
                @"2BA5": @"Media Control Point Opcodes Supported",
                @"2BA6": @"Search Results Object ID",
                @"2BA7": @"Search Control Point",
                @"2BA9": @"Media Player Icon Object Type",
                @"2BAA": @"Track Segments Object Type 2BAB Track Object Type",
                @"2BAC": @"Group Object Type",
                @"2BAD": @"Constant Tone Extension Enable",
                @"2BAE": @"Advertising Constant Tone Extension Minimum Length",
                @"2BAF": @"Advertising Constant Tone Extension Minimum Transmit Count",
                @"2BB0": @"Advertising Constant Tone Extension Transmit Duration",
                @"2BB1": @"Advertising Constant Tone Extension Interval",
                @"2BB2": @"Advertising Constant Tone Extension PHY",
                @"2BB3": @"Bearer Provider Name",
                @"2BB4": @"Bearer UCI",
                @"2BB5": @"Bearer Technology",
                @"2BB6": @"Bearer URI Schemes Supported List",
                @"2BB7": @"Bearer Signal Strength",
                @"2BB8": @"Bearer Signal Strength Reporting Interval",
                @"2BB9": @"Bearer List Current Calls",
                @"2BBA": @"Content Control ID",
                @"2BBB": @"Status Flags",
                @"2BBC": @"Incoming Call Target Bearer URI",
                @"2BBD": @"Call State",
                @"2BBE": @"Call Control Point",
                @"2BBF": @"Call Control Point Optional Opcodes",
                @"2BC0": @"Termination Reason",
                @"2BC1": @"Incoming Call",
                @"2BC2": @"Call Friendly Name",
                @"2BC3": @"Mute"
        }
    };
    return uuidDescriptions[attributeType][attribute.UUID.UUIDString];
}


@end
