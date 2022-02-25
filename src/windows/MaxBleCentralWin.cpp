#include "../common/MaxBleCentral.h"
#include "WinBleCentral.hpp"

MaxBleCentral* newBleCentralC(void)
{
    return new WinBleCentral;
}

void bleCentralCRelease(MaxBleCentral* t)
{
    ((WinBleCentral*)t)->stop();
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

const char* bleCentralCGetDeviceUUID(MaxBleCentral* t, int deviceIndex)
{
    return "";
}
int bleCentralCGetNumberOfDevices(MaxBleCentral* t)
{
    return 0;
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
    ((WinBleCentral*)t)->setRssiSensitivity(abs(rssiLimit));
}
void bleCentralCSetReporting(MaxBleCentral* t, bool shouldReport)
{
    ((WinBleCentral*)t)->setReporting(shouldReport);
}

void bleCentralCShouldKeepDeviceAtIndexAlive(MaxBleCentral* t, int deviceIndex, bool shouldKeepAlive) 
{
    ((WinBleCentral*)t)->setDeviceShouldKeepAlive(deviceIndex, shouldKeepAlive);
}
void bleCentralCReadCharacteristicWithDeviceUUID(MaxBleCentral* t, const char* duuid, const char* suuid, const char* cuuid) 
{
}
void bleCentralCReadCharacteristicWithDeviceAtIndex(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid) 
{
    ((WinBleCentral*)t)->getValueOfCharacteristicOfServiceOfDeviceAtIndex(cuuid, suuid, deviceIndex);
}
void bleCentralCReadAllCharacteristicWithDeviceUUID(MaxBleCentral* t, const char* duuid) 
{
    ((WinBleCentral*)t)->connectToDeviceWithUUID(duuid);
}
void bleCentralCReadAllCharacteristicWithDeviceAtIndex(MaxBleCentral* t, int deviceIndex) 
{
}

void bleCentralCSubscribeToCharacteristic(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid)
{
    ((WinBleCentral*)t)->subscribeToCharacteristicOfServiceOfDeviceAtIndex(cuuid, suuid, deviceIndex);
}

void bleCentralCSubscribeToCharacteristicWithDeviceUUID(MaxBleCentral* t, const char* duuid, const char* suuid, const char* cuuid)
{

}


void bleCentralCUnsubscribeToCharacteristic(MaxBleCentral* t, int deviceIndex, const char* suuid, const char* cuuid)
{
    ((WinBleCentral*)t);
}

void bleCentralCUnsubscribeToCharacteristicWithDeviceUUID(MaxBleCentral* t, const char* duuid, const char* suuid, const char* cuuid)
{

}


void bleCentralCSetRSSIScanThreshold(MaxBleCentral* t, int rssiLimit)
{
    ((WinBleCentral*)t)->setRssiSensitivity(abs(rssiLimit));
}
void bleCentralCSetIgnoreiPhone(MaxBleCentral* t, bool shouldIgnore)
{
    ((WinBleCentral*)t)->setIgnoreiPhone(shouldIgnore);
}
void bleCentralCScanForServices(MaxBleCentral* t, t_atom* argv, long argc)
{
    ((WinBleCentral*)t)->scanForService(argv, argc);
}

void bleCentralCWriteToCharactaristic(MaxBleCentral* t, t_atom* argv, long argc)
{

}


void bleCentralCBlacklistStalledDevices(MaxBleCentral* t)
{
    post("Not yet implemented");
}
