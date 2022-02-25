#pragma once
#include <stdint.h>
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
//------------------------------------------------------------------------------
typedef void MacosBleCentralRef;
typedef struct _MaxExternalObject
{
    t_pxobject x_obj;
    t_symbol*  x_arrayname;
    MacosBleCentralRef* bleCentral;
    void*   read_event_outlet1;
    void*   notify_event_outlet2;
    void*   device_discovery_outlet4;
    void*   device_status_outlet3;
    size_t  maxListSize;
    t_atom* outputList;
    long listSize;
    char listAllocSuccess;
} MaxExternalObject;

///
void onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes);
///
void onNotificationRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes);
///
void outputFoundDeviceList(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, int rssi);
///
void onDeviceConnectionStateChange(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, bool connected);
///
void onRSSIRead(MaxExternalObject* maxObjectPtr, const char* uuid, int rssi);
