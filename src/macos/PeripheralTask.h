//
//  PeripheralManager.h
//  max-ble
//
//  Created by mhamilt7 on 15/02/2022.
//

#pragma once
#import <CoreBluetooth/CoreBluetooth.h>

typedef enum : NSUInteger {
    BLEPeripheralRead,
    BLEPeripheralSubscribe,
    BLEPeripheralWrite,
} PeripheralTaskType;

/// Interface to bundle tasks a connected peripheral should perform. See also PeripheralConnectionManager
@interface PeripheralTask: NSObject
/// Peripheral Connection Type: Read, Subscribe or Write
@property PeripheralTaskType type;
/// For Read and Write Task, which Service UUID
@property CBUUID* service;
/// For Read and Write Task, which Characteristic UUID
@property CBUUID* characteristic;
/// For Write Task, what data should be written
@property NSData* dataToWrite;

- (instancetype)initWithType:(PeripheralTaskType)taskType
               serviceCBUUID:(CBUUID*) serviceCBUUID
        characteristicCBUUID:(CBUUID*) characteristicCBUUID
                     andData:(NSData*) data;

@end
