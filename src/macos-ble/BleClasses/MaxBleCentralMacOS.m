/*
 C Bridge for CoreBluetooth Central on macOS
 */

#import "MaxBleCentral.h"
//------------------------------------------------------------------------------
MaxBleCentral* newBleCentralC(void)
{
    return CFBridgingRetain([MacosBleCentral new]);
}

void bleCentralCScan(MaxBleCentral *t)
{
    [(__bridge MacosBleCentral *)t scan];
}

void bleCentralCStopScan(MaxBleCentral *t)
{
    [(__bridge MacosBleCentral *)t stop];
}
//------------------------------------------------------------------------------
void bleCentralCConnectToFoundDevice (MaxBleCentral *t, int deviceIndex)
{
    [(__bridge MacosBleCentral *)t connectToFoundDevice: deviceIndex];
}

void bleCentralCConnectToDeviceWithUUID (MaxBleCentral *t, const char *uuid)
{
    [(__bridge MacosBleCentral *)t connectToDeviceWithUUID: uuid];
}

void bleCentralCConnectToDeviceWithName (MaxBleCentral *t, const char *name)
{
    [(__bridge MacosBleCentral *)t connectToDeviceWithName:name];
}

void bleCentralCClearDiscovered (MaxBleCentral *t)
{
    [(__bridge MacosBleCentral *)t clearDicoveredPeripherals];
}
//------------------------------------------------------------------------------
void bleCentralCRelease(MaxBleCentral *t)
{
    CFRelease(t);
}
//------------------------------------------------------------------------------

void bleCentralCSetMaxObjRef(MaxBleCentral* t, MaxExternalObject* maxObjRef)
{
    [(__bridge MacosBleCentral *)t setMaxObjectRef:maxObjRef];
}

void bleCentralCGetDeviceList (MaxBleCentral *t)
{
    [(__bridge MacosBleCentral *)t getFoundDeviceList];
}

const char* bleCentralCGetDeviceUUID   (MaxBleCentral *t, int deviceIndex)
{
    return [(__bridge MacosBleCentral *)t getDeviceUUIDatIndex:deviceIndex];
}

int bleCentralCGetNumberOfDevices(MaxBleCentral *t)
{
    return [(__bridge MacosBleCentral *)t getNumberOfDevices];
}

void bleCentralCSetRSSISensitity(MaxBleCentral *t, int rssiLimit)
{
    [(__bridge MacosBleCentral *)t setRssiSensitivity:rssiLimit]  ;
}

void bleCentralCGetRssi (MaxBleCentral *t, int deviceIndex)
{
    [(__bridge MacosBleCentral *)t getRssiOfFoundDevice:deviceIndex];
}

void bleCentralCSetReporting (MaxBleCentral *t, bool shouldReport)
{
    [(__bridge MacosBleCentral *)t setReporting:shouldReport];
}

void bleCentralCSubscribeToCharacteristic (MaxBleCentral *t, int deviceIndex, const char* suuid, const char* cuuid)
{
    [(__bridge MacosBleCentral *)t subscribeToCharacteristic:cuuid
                                                   OfService:suuid
                                               OfFoundDevice:deviceIndex
                                             shouldSubscribe:YES];
}

void bleCentralCSubscribeToCharacteristicWithDeviceUUID (MaxBleCentral *t, const char* duuid, const char* suuid, const char* cuuid)
{
    [(__bridge MacosBleCentral *)t subscribeToCharacteristic:cuuid
                                                   OfService:suuid
                                            ofDeviceWithUUID:duuid
                                             shouldSubscribe:YES];
}

void bleCentralCUnsubscribeToCharacteristic (MaxBleCentral *t, int deviceIndex, const char *suuid, const char *cuuid)
{
    [(__bridge MacosBleCentral *)t subscribeToCharacteristic:cuuid
                                                   OfService:suuid
                                               OfFoundDevice:deviceIndex
                                             shouldSubscribe:NO];
}

void bleCentralCUnsubscribeToCharacteristicWithDeviceUUID (MaxBleCentral *t, const char* duuid, const char* suuid, const char* cuuid)
{
    [(__bridge MacosBleCentral *)t subscribeToCharacteristic:cuuid
                                                   OfService:suuid
                                            ofDeviceWithUUID:duuid
                                             shouldSubscribe:NO];
}


void bleCentralCSetRSSIScanThreshold (MaxBleCentral *t, int rssi)
{
    
    [(__bridge MacosBleCentral *)t setRssiSensitivity: abs(rssi)];
}

void bleCentralCSetIgnoreiPhone (MaxBleCentral *t, bool shouldIgnore)
{
    [(__bridge MacosBleCentral *)t setIgnoreiPhone: shouldIgnore];
}

void bleCentralCScanForServices (MaxBleCentral *t, t_atom* argv, long argc)
{
    [(__bridge MacosBleCentral *)t scanForService:argv count:argc];
}

void bleCentralCWriteToCharactaristic (MaxBleCentral *t, t_atom* argv, long argc)
{
        const char* suuid = atom_getsym(argv + 1)->s_name;
        const char* cuuid = atom_getsym(argv + 2)->s_name;
        
        size_t numBytes = 0;
        
        for (int i = 3; i < argc; i++)
        {
            if(atom_gettype(argv + i) == A_SYM)
                numBytes += strlen(atom_getsym(argv + i)->s_name);
            else
                numBytes += 4;
        }
        
        void* bytes = malloc(numBytes);
        numBytes = 0;
        
        for (int i = 3; i < argc; i++)
        {
            if(atom_gettype(argv + i) == A_SYM)
            {
                const char* value = atom_getsym(argv + i)->s_name;
                memcpy((void*)(bytes + numBytes), value, strlen(value));
            }
            else if(atom_gettype(argv + i) == A_FLOAT)
            {
                float value = (float)atom_getfloat(argv + i);
                memcpy((void*)(bytes + numBytes), &value, 4);
                numBytes += 4;
            }
            else if(atom_gettype(argv + i) == A_LONG)
            {
                int value = (int)atom_getlong(argv + i);
                memcpy((void*)(bytes + numBytes), &value, 4);
                numBytes += 4;
            }
        }
        
    if(atom_gettype(argv) == A_SYM)
    {
        t_symbol* duuid = atom_getsym(argv);
        
        [(__bridge  MacosBleCentral *)t writeToCharacteristic: cuuid
                                                    OfService: suuid
                                             ofDeviceWithUUID: duuid->s_name
                                                    withBytes: bytes
                                                     ofLength: numBytes];
    }
    else if (atom_gettype(argv) == A_LONG)
    {
        int deviceIndex = (int)atom_getlong(argv);
        
        [(__bridge  MacosBleCentral *)t writeToCharacteristic: cuuid
                                                    OfService: suuid
                                                OfFoundDevice: deviceIndex
                                                    withBytes: bytes
                                                     ofLength: numBytes];
    }
    
}

void bleCentralCBlacklistStalledDevices (MaxBleCentral *t)
{
    [(__bridge MacosBleCentral *)t blacklistDevicesStillConnecting];
}
