/*
  C Bridge for CoreBluetooth Central on macOS
 
 Bridging techinque by Rob Napier. https://github.com/rnapier
 */
#pragma once
#import <CoreFoundation/CoreFoundation.h>
#include <objc/runtime.h>
#include "../MaxObject.h"
#ifdef __OBJC__
#import "MacosBleCentral.h"
#endif

//typedef CFTypeRef MacosBleCentralRef;
//MacosBleCentralRef MacosBleCentralRefCreate(void);
typedef const void MacosBleCentralC; // 'const void *' is more CF-like, but either is fine
MacosBleCentralC* newBleCentralC(void);
void bleCentralCScan            (MacosBleCentralC *t);
void bleCentralCStopScan        (MacosBleCentralC *t);
void bleCentralCConnectToFoundDevice (MacosBleCentralC *t, int deviceIndex);
void bleCentralCRelease         (MacosBleCentralC *t);
void bleCentralCSetMaxObjRef    (MacosBleCentralC *t, MaxExternalObject* maxObjRef);
void bleCentralCGetDeviceList   (MacosBleCentralC *t);
void bleCentralCClearDiscovered (MacosBleCentralC *t);
void bleCentralCGetRssi         (MacosBleCentralC *t, int deviceIndex);
void bleCentralCSetRSSISensitity(MacosBleCentralC *t, int rssiLimit);
void bleCentralCSetReporting    (MacosBleCentralC *t, bool shouldReport);
void bleCentralCSubscribeToCharacteristic   (MacosBleCentralC *t, int deviceIndex, const char* suuid ,const char* cuuid);
void bleCentralCUnsubscribeToCharacteristic (MacosBleCentralC *t, const char* charUuid);
void bleCentralCSetRSSIScanThreshold (MacosBleCentralC *t, int rssi);
void bleCentralCSetIgnoreiPhone (MacosBleCentralC *t, bool shouldIgnore);
void bleCentralCScanForServices (MacosBleCentralC *t, t_atom* argv, long argc);

