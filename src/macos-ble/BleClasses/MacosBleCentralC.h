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
void bleCentralCScanForDeviceWithName (MacosBleCentralC *t, const char* name);
void bleCentralCScanForFoundDevice   (MacosBleCentralC *t, int deviceIndex);
void bleCentralCConnectToFoundDevice (MacosBleCentralC *t, int deviceIndex);
void bleCentralCRelease         (MacosBleCentralC *t);
int  bleCentralCGetLatestValue  (MacosBleCentralC *t);
void bleCentralCSetMaxObjRef    (MacosBleCentralC *t, MaxExternalObject* maxObjRef);
void bleCentralCGetDeviceList   (MacosBleCentralC *t);
void bleCentralCClearDiscovered (MacosBleCentralC *t);
void bleCentralCSetRSSISensitity(MacosBleCentralC *t, int rssiLimit);
void bleCentralCRequestCubeData (MacosBleCentralC *t);
void bleCentralCSetReporting    (MacosBleCentralC *t, bool shouldReport);

