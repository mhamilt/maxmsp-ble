/*
    C Bridge for CoreBluetooth Central on macOS
*/

#import "MacosBleCentralC.h"
//------------------------------------------------------------------------------
MacosBleCentralC* newMacosBleCentralC(void)
{
    return CFBridgingRetain([MacosBleCentral new]);
}
//------------------------------------------------------------------------------
void MacosBleCentralC_scanFor(MacosBleCentralC *t, const char* name)
{
    [(__bridge MacosBleCentral *)t scanForDeviceWithName: [NSString stringWithCString:name encoding:NSASCIIStringEncoding] ];
}
//------------------------------------------------------------------------------
void MacosBleCentralC_release(MacosBleCentralC *t)
{
    CFRelease(t);
}
//------------------------------------------------------------------------------
MacosBleCentralRef MacosBleCentralRefCreate(void)
{
     return CFBridgingRetain([MacosBleCentral new]);
}

void MacosBleCentralRefScanFor(MacosBleCentralRef t, const char* name)
{
    [(__bridge MacosBleCentral *)t scanForDeviceWithName: [NSString stringWithCString:name encoding:NSASCIIStringEncoding] ];
}

int MacosBleCentralRefGetLatestValue(MacosBleCentralRef t)
{
    return [(__bridge MacosBleCentral *)t latestValue];
}

void MacosBleCentralRefSetMaxObjRef(MacosBleCentralRef t, MaxExternalObject* maxObjRef)
{
   [(__bridge MacosBleCentral *)t setMaxObjectRef:maxObjRef];
}
