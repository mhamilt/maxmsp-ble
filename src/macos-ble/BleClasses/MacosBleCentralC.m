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

void bleCentralCSetReporting (MacosBleCentralC *t, bool shouldReport)
{
    [(__bridge MacosBleCentral *)t setReporting:shouldReport];
}

void bleCentralCSubscribeToCharacteristic (MacosBleCentralC *t, int deviceIndex, const char* suuid, const char* cuuid)
{
        [(__bridge MacosBleCentral *)t subscribeToCharacteristic:cuuid
                                                       OfService:suuid
                                                   OfFoundDevice:deviceIndex];
}
