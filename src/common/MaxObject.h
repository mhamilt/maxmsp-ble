#pragma once
#include <stdint.h>
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
//------------------------------------------------------------------------------
#define MAXBLEWRITEBUFFERSIZE 512
#define MAXBLELISTSIZE 512
typedef void MaxBleCentralRef;
typedef struct MaxExternalObject
{
    t_pxobject x_obj;
    t_symbol*  x_arrayname;
    MaxBleCentralRef* bleCentral;
    void*   ble_event_outlet;
    size_t  maxListSize;
    t_atom* outputList;
    long listSize;
    char listAllocSuccess;
    char writeBuffer[MAXBLEWRITEBUFFERSIZE];
} MaxExternalObject;

///
void onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes);
///
void onCharacteristicWrite(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid);
///
void onDeviceConnectionStateChange(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, bool connected);
///
void onError(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, const char* error);
///
void onNotificationRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes);
///
void onRSSIRead(MaxExternalObject* maxObjectPtr, const char* uuid, int rssi);
///
void onSubscriptionChange(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, bool subscribed);
///
void onDeviceFound(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, int rssi);
