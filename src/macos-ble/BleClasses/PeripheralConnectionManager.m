//
//  PeripheralManager.m
//  max-ble
//
//  Created by mhamilt7 on 15/02/2022.
//


#import "PeripheralConnectionManager.h"


@implementation PeripheralConnectionManager
@synthesize device;
@synthesize tasks;
@synthesize connectionAttempts;
@synthesize maxConnectionAttempts;
@synthesize keepAlive;
- (instancetype)initWithDevice:(CBPeripheral*)aPeripheral;
{
    self = [super init];
    if (self)
    {
        device = aPeripheral;
        tasks = [[NSMutableArray alloc] initWithArray:@[]];
        connectionAttempts = 0;
        maxConnectionAttempts = 1;
        keepAlive = NO;
    }
    return self;
}



@end
