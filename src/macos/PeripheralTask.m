//
//  PeripheralManager.m
//  max-ble
//
//  Created by mhamilt7 on 15/02/2022.
//


#import "PeripheralTask.h"



@implementation PeripheralTask

@synthesize type;
@synthesize service;
@synthesize characteristic;
@synthesize dataToWrite;

- (instancetype)initWithType:(PeripheralTaskType)taskType
               serviceCBUUID:(CBUUID*) serviceCBUUID
        characteristicCBUUID:(CBUUID*) characteristicCBUUID
                     andData:(NSData*) data
{
    self = [super init];
    if (self)
    {        
        type = taskType;
        service = serviceCBUUID;
        characteristic = characteristicCBUUID;
        dataToWrite = data;
    }
    return self;
}

@end
