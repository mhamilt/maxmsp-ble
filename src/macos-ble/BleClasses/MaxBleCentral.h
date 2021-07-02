/*
  C Bridge for CoreBluetooth Central on macOS
 
 Bridging techinque by Rob Napier. https://github.com/rnapier
 */
#pragma once
#include "../MaxObject.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#ifdef TARGET_OS_MAC
#import <CoreFoundation/CoreFoundation.h>
#include <objc/runtime.h>
#ifdef __OBJC__
#import "MacosBleCentral.h"
#endif
#endif
#elif defined _WIN32 || defined _WIN64

#endif

//typedef CFTypeRef MacosBleCentralRef;
//MacosBleCentralRef MacosBleCentralRefCreate(void);
typedef const void MaxBleCentral; // 'const void *' is more CF-like, but either is fine
MaxBleCentral* newBleCentralC(void);
void bleCentralCScan            (MaxBleCentral *t);
void bleCentralCStopScan        (MaxBleCentral *t);
void bleCentralCConnectToFoundDevice (MaxBleCentral *t, int deviceIndex);
void bleCentralCConnectToDeviceWithUUID (MaxBleCentral *t, const char *uuid);
void bleCentralCConnectToDeviceWithName (MaxBleCentral *t, const char *name);
void bleCentralCRelease         (MaxBleCentral *t);
void bleCentralCSetMaxObjRef    (MaxBleCentral *t, MaxExternalObject *maxObjRef);
void bleCentralCGetDeviceList   (MaxBleCentral *t);
void bleCentralCClearDiscovered (MaxBleCentral *t);
void bleCentralCGetRssi         (MaxBleCentral *t, int deviceIndex);
void bleCentralCSetRSSISensitity(MaxBleCentral *t, int rssiLimit);
void bleCentralCSetReporting    (MaxBleCentral *t, bool shouldReport);
void bleCentralCSubscribeToCharacteristic   (MaxBleCentral *t, int deviceIndex, const char *suuid ,const char *cuuid);
void bleCentralCUnsubscribeToCharacteristic (MaxBleCentral *t, int deviceIndex, const char *suuid, const char *cuuid);
void bleCentralCSetRSSIScanThreshold (MaxBleCentral *t, int rssi);
void bleCentralCSetIgnoreiPhone (MaxBleCentral *t, bool shouldIgnore);
void bleCentralCScanForServices (MaxBleCentral *t, t_atom* argv, long argc);

