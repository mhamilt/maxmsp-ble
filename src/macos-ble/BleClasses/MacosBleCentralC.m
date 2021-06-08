/*
    C Bridge for CoreBluetooth Central on macOS
*/

#import "MacosBleCentralC.h"
//------------------------------------------------------------------------------
MacosBleCentralC* newBleCentralC(void)
{
    return CFBridgingRetain([MacosBleCentral new]);
}

void bleCentralCScan(MacosBleCentralC *t)
{
    [(__bridge MacosBleCentral *)t scan];
}

void bleCentralCStopScan(MacosBleCentralC *t)
{
    [(__bridge MacosBleCentral *)t stop];
}
//------------------------------------------------------------------------------
void bleCentralCScanForDeviceWithName(MacosBleCentralC *t, const char* name)
{
    [(__bridge MacosBleCentral *)t scanForDeviceWithName: [NSString stringWithCString:name encoding:NSASCIIStringEncoding] ];
}


void bleCentralCScanForFoundDevice (MacosBleCentralC *t, int deviceIndex)
{
    [(__bridge MacosBleCentral *)t scanForFoundDevice: deviceIndex];
}

void bleCentralCConnectToFoundDevice (MacosBleCentralC *t, int deviceIndex)
{
    [(__bridge MacosBleCentral *)t connectToFoundDevice: deviceIndex];
}

void bleCentralCClearDiscovered (MacosBleCentralC *t)
{
    [(__bridge MacosBleCentral *)t clearDicoveredPeripherals];
}
//------------------------------------------------------------------------------
void bleCentralCRelease(MacosBleCentralC *t)
{
    CFRelease(t);
}
//------------------------------------------------------------------------------

int bleCentralCGetLatestValue(MacosBleCentralRef t)
{
    return [(__bridge MacosBleCentral *)t latestValue];
}

void bleCentralCSetMaxObjRef(MacosBleCentralRef t, MaxExternalObject* maxObjRef)
{
   [(__bridge MacosBleCentral *)t setMaxObjectRef:maxObjRef];
}

void bleCentralCGetDeviceList (MacosBleCentralC *t)
{
   [(__bridge MacosBleCentral *)t getFoundDeviceList];
}

void bleCentralCSetRSSISensitity(MacosBleCentralC *t, int rssiLimit)
{
    [(__bridge MacosBleCentral *)t setRssiSensitivity:rssiLimit]  ;
}

void bleCentralCGetRssi (MacosBleCentralC *t, int deviceIndex)
{
    [(__bridge MacosBleCentral *)t getRssiOfFoundDevice:deviceIndex];
}

void bleCentralCRequestCubeData (MacosBleCentralC *t)
{
    
}

void bleCentralCSetReporting (MacosBleCentralC *t, bool shouldReport)
{
    
}
