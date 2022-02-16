//
//  CBPeripheral+utility.m
//  max-ble
//
//  Created by mhamilt7 on 12/02/2022.
//

#import "CBPeripheral+PeripheralUtility.h"

@implementation CBPeripheral (PeripheralUtility)
- (CBService *)getServiceWithUUID:(CBUUID *)serviceUUID
{
    for (CBService* service in [self services])
    {
        if ([service.UUID isEqual:serviceUUID])
            return service;
    }
    post("Could not find service %s: try connecting to the device to dicover services", serviceUUID.UUIDString.UTF8String);
    return nil;
}

-(CBCharacteristic*) getCharacteristicWithUUID:(CBUUID*) characteristicUUID
                                    forService:(CBService *)service
                                  
{
    for (CBCharacteristic* characteristic in [service characteristics])
    {
        if ([characteristic.UUID isEqual:characteristicUUID])
            return characteristic;
    }
    post("Could not find characteristic %s: try connecting to the device to dicover services", characteristicUUID.UUIDString.UTF8String);
    return nil;
}

-(CBCharacteristic*) getCharacteristicWithUUID:(CBUUID*)characteristicUUID
                           forServiceWithUUID:(CBUUID*)serviceUUID
                                  
{        
    CBService* service = [self getServiceWithUUID:serviceUUID];

    if(!service)
        return nil;
    
    CBCharacteristic* characteristic = [self getCharacteristicWithUUID:characteristicUUID
                                                            forService:service];
    if(!characteristic)
        return nil;
    
    return characteristic;
}

@end
