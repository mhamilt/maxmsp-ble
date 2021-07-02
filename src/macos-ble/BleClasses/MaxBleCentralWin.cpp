#include "MaxBleCentral.h"
#include "WinBleCentral.hpp"

MaxBleCentral* newBleCentralC(void)
{
    return new WinBleCentral;
}

void bleCentralCRelease(MaxBleCentral* t)
{
    delete ((WinBleCentral*)t);
}

void bleCentralCScan(MaxBleCentral* t)
{
    ((WinBleCentral*)t);
}
void bleCentralCStopScan(MaxBleCentral* t)
{
    ((WinBleCentral*)t);
}
void bleCentralCConnectToFoundDevice(MaxBleCentral* t, int deviceIndex)
{
    ((WinBleCentral*)t);
}
void bleCentralCConnectToDeviceWithUUID(MaxBleCentral* t, const char* uuid)
{
    ((WinBleCentral*)t);
}
void bleCentralCConnectToDeviceWithName(MaxBleCentral* t, const char* name)
{
    ((WinBleCentral*)t);
}
void bleCentralCSetMaxObjRef(MaxBleCentral* t, MaxExternalObject* maxObjRef)
{
    ((WinBleCentral*)t);
}
void bleCentralCGetDeviceList(MaxBleCentral* t)
{
    ((WinBleCentral*)t);
}
void bleCentralCClearDiscovered(MaxBleCentral* t)
{
    ((WinBleCentral*)t);
}
void bleCentralCGetRssi(MaxBleCentral* t, int deviceIndex)
{
    ((WinBleCentral*)t);
}
void bleCentralCSetRSSISensitity(MaxBleCentral* t, int rssiLimit)
{
    ((WinBleCentral*)t);
}
void bleCentralCSetReporting(MaxBleCentral* t, bool shouldReport)
{
    ((WinBleCentral*)t);
}
void bleCentralCSubscribeToCharacteristic(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid)
{
    ((WinBleCentral*)t);
}
void bleCentralCUnsubscribeToCharacteristic(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid)
{
    ((WinBleCentral*)t);
}
void bleCentralCSetRSSIScanThreshold(MaxBleCentral* t, int rssi)
{
    ((WinBleCentral*)t);
}
void bleCentralCSetIgnoreiPhone(MaxBleCentral* t, bool shouldIgnore)
{
    ((WinBleCentral*)t);
}
void bleCentralCScanForServices(MaxBleCentral* t, t_atom* argv, long argc)
{
    ((WinBleCentral*)t);
}
