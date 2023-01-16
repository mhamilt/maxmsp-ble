#include "MaxObject.h"

//------------------------------------------------------------------------------

void onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym (maxObjectPtr->outputList + 0, gensym("read"));
    atom_setsym (maxObjectPtr->outputList + 1, gensym(duuid));
    atom_setsym (maxObjectPtr->outputList + 2, gensym(suuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(cuuid));
    
    if (numBytes > (maxObjectPtr->maxListSize - 1))
    {
        numBytes = maxObjectPtr->maxListSize - 1;
        post("Bytes Truncated\n");
    }
    
    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 4 + i, (t_atom_long) byteArray[i]);
    
    outlet_list(maxObjectPtr->ble_event_outlet, 0L, numBytes + 4, maxObjectPtr->outputList);
}

void onCharacteristicWrite(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid)
{
    atom_setsym (maxObjectPtr->outputList + 0, gensym("write"));
    atom_setsym (maxObjectPtr->outputList + 1, gensym(duuid));
    atom_setsym (maxObjectPtr->outputList + 2, gensym(suuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(cuuid));
    
    outlet_list(maxObjectPtr->ble_event_outlet, 0L, 4, maxObjectPtr->outputList);
}

void onDeviceConnectionStateChange(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, bool connected)
{    
    atom_setsym (maxObjectPtr->outputList + 0, gensym(( (connected) ? "connected" : "disconnected" )) );
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long)index);
    atom_setsym (maxObjectPtr->outputList + 2, gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(name));
    outlet_list(maxObjectPtr->ble_event_outlet, 0L, 4, maxObjectPtr->outputList);
}

void onNotificationRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym (maxObjectPtr->outputList + 0, gensym("notifying"));
    atom_setsym (maxObjectPtr->outputList + 1, gensym(duuid));
    atom_setsym (maxObjectPtr->outputList + 2, gensym(suuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(cuuid));
    
    if (numBytes > (maxObjectPtr->maxListSize - 1))
    {
        numBytes = maxObjectPtr->maxListSize - 1;
        post("Bytes Truncated\n");
    }
    
    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 4 + i, (t_atom_long) byteArray[i]);
    
    outlet_list(maxObjectPtr->ble_event_outlet, 0L, numBytes + 4, maxObjectPtr->outputList);
}

void onRSSIRead(MaxExternalObject* maxObjectPtr, const char* uuid, int rssi)
{
    atom_setsym (maxObjectPtr->outputList + 0, gensym("rssi"));
    atom_setsym (maxObjectPtr->outputList + 1, gensym(uuid));
    atom_setlong(maxObjectPtr->outputList + 2, (t_atom_long) rssi);
    outlet_list (maxObjectPtr->ble_event_outlet, 0L, 3, maxObjectPtr->outputList);
}

void onDeviceFound(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, int rssi)
{
    atom_setsym (maxObjectPtr->outputList + 0, gensym("found"));
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long)index);
    atom_setsym (maxObjectPtr->outputList + 2, gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(name));
    atom_setlong(maxObjectPtr->outputList + 4, (t_atom_long) rssi);
    outlet_list(maxObjectPtr->ble_event_outlet, 0L, 5, maxObjectPtr->outputList);
}


void onSubscriptionChange(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, bool subscribed)
{
    atom_setsym (maxObjectPtr->outputList + 0, gensym(( (subscribed) ? "subscribed" : "unsubscribed" )) );
    atom_setsym (maxObjectPtr->outputList + 1, gensym(duuid));
    atom_setsym (maxObjectPtr->outputList + 2, gensym(suuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(cuuid));
    outlet_list(maxObjectPtr->ble_event_outlet, 0L, 4, maxObjectPtr->outputList);
}
