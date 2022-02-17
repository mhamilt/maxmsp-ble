//
//  PeripheralManager.h
//  max-ble
//
//  Created by mhamilt7 on 15/02/2022.
//

#pragma once
#import <CoreBluetooth/CoreBluetooth.h>
#import "PeripheralTask.h"

/// @brief PeripheralConnectionManager provides an interface for tracking what tasks a CBPeripheral* should perform
///  particularly if the device is disconnected.
@interface PeripheralConnectionManager: NSObject
/// Reference to CBPeripheral
@property CBPeripheral* device;
/// Array of tasks peripheral is scheduled to perform
@property NSMutableArray<PeripheralTask*>* tasks;
/// how attempts have been made to connect to the peripheral
@property int connectionAttempts;
/// 
@property int maxConnectionAttempts;
/// should the peripheral connection be kept alive (i.e. on disconnect should there be an attempt to automatically reconnect)
@property BOOL keepAlive;

- (instancetype)initWithDevice:(CBPeripheral*)aPeripheral;
@end
