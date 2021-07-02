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
    ((WinBleCentral*)t)->scan();
}

void bleCentralCStopScan(MaxBleCentral* t)
{
    ((WinBleCentral*)t)->stop();
}

void bleCentralCConnectToFoundDevice(MaxBleCentral* t, int deviceIndex)
{
    ((WinBleCentral*)t)->connectToFoundDevice(deviceIndex);
}

void bleCentralCConnectToDeviceWithUUID(MaxBleCentral* t, const char* uuid)
{
    ((WinBleCentral*)t)->connectToDeviceWithUUID(uuid);
}

void bleCentralCConnectToDeviceWithName(MaxBleCentral* t, const char* name)
{
    ((WinBleCentral*)t)->connectToDeviceWithName(name);
}

void bleCentralCSetMaxObjRef(MaxBleCentral* t, MaxExternalObject* maxObjRef)
{
    ((WinBleCentral*)t)->setMaxObjectRef(maxObjRef);
}

void bleCentralCGetDeviceList(MaxBleCentral* t)
{
    ((WinBleCentral*)t)->getFoundDeviceList();
}

void bleCentralCClearDiscovered(MaxBleCentral* t)
{
    ((WinBleCentral*)t)->clearDicoveredPeripherals();
}
void bleCentralCGetRssi(MaxBleCentral* t, int deviceIndex)
{
    ((WinBleCentral*)t)->getRssiOfFoundDevice(deviceIndex);
}
void bleCentralCSetRSSISensitity(MaxBleCentral* t, int rssiLimit)
{
    ((WinBleCentral*)t)->setRssiSensitivity(rssiLimit);
}
void bleCentralCSetReporting(MaxBleCentral* t, bool shouldReport)
{
    ((WinBleCentral*)t)->setReporting(shouldReport);
}
void bleCentralCSubscribeToCharacteristic(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid)
{
    ((WinBleCentral*)t)->subscribeToCharacteristic(cuuid, suuid, deviceIndex);
}
void bleCentralCUnsubscribeToCharacteristic(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid)
{
    ((WinBleCentral*)t);
}
void bleCentralCSetRSSIScanThreshold(MaxBleCentral* t, int rssiLimit)
{
    ((WinBleCentral*)t)->setRssiSensitivity(rssiLimit);
}
void bleCentralCSetIgnoreiPhone(MaxBleCentral* t, bool shouldIgnore)
{
    ((WinBleCentral*)t)->setIgnoreiPhone(shouldIgnore);
}
void bleCentralCScanForServices(MaxBleCentral* t, t_atom* argv, long argc)
{
    ((WinBleCentral*)t)->scanForService(argv, argc);
}
