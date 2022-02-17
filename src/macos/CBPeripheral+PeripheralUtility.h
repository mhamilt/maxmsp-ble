//
//  CBPeripheral+utility.h
//  max-ble
//
//  Created by mhamilt7 on 12/02/2022.
//

#import <CoreBluetooth/CoreBluetooth.h>
#ifdef MAXMSP
#include "ext.h"
#else
#define post printf
#endif

NS_ASSUME_NONNULL_BEGIN

@interface CBPeripheral (PeripheralUtility)
-(CBService*) getServiceWithUUID:(CBUUID*) serviceUUID;

-(CBCharacteristic*) getCharacteristicWithUUID:(CBUUID*) characteristicUUID
                                    forService:(CBService *)service;

-(CBCharacteristic*) getCharacteristicWithUUID:(CBUUID*)characteristicUUID
                            forServiceWithUUID:(CBUUID*)serviceUUID;
@end

NS_ASSUME_NONNULL_END
