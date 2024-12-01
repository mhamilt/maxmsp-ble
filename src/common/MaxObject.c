#include "MaxObject.h"

//------------------------------------------------------------------------------

void onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->outputList + 0, gensym(duuid));
    atom_setsym(maxObjectPtr->outputList + 1, gensym(suuid));
    atom_setsym(maxObjectPtr->outputList + 2, gensym(cuuid));
    
    if (numBytes > (maxObjectPtr->maxListSize - 2))
    {
        numBytes = maxObjectPtr->maxListSize - 2;
        post("Bytes Truncated\n");
    }
    
    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 3 + i, (t_atom_long) byteArray[i]);
    
    outlet_list(maxObjectPtr->read_event_outlet1, 0L, numBytes + 3, maxObjectPtr->outputList);
}

void onNotificationRead(MaxExternalObject* maxObjectPtr, const char* duuid, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->outputList + 0, gensym(duuid));
    atom_setsym(maxObjectPtr->outputList + 1, gensym(suuid));
    atom_setsym(maxObjectPtr->outputList + 2, gensym(cuuid));
    
    if (numBytes > (maxObjectPtr->maxListSize - 1))
    {
        numBytes = maxObjectPtr->maxListSize - 1;
        post("Bytes Truncated\n");
    }
    
    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 3 + i, (t_atom_long) byteArray[i]);
    
    outlet_list(maxObjectPtr->notify_event_outlet2, 0L, numBytes + 3, maxObjectPtr->outputList);
}

void outputFoundDeviceList(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, int rssi)
{
    atom_setlong(maxObjectPtr->outputList + 0, (t_atom_long)index);
    atom_setsym (maxObjectPtr->outputList + 1, gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 2, gensym(name));
    atom_setlong(maxObjectPtr->outputList + 3, (t_atom_long) rssi);
    outlet_list(maxObjectPtr->device_discovery_outlet4, 0L, 4, maxObjectPtr->outputList);
}

void onDeviceConnectionStateChange(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, bool connected)
{    
    atom_setsym (maxObjectPtr->outputList + 0, gensym(( (connected) ? "connected" : "disconnected" )) );
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long)index);
    atom_setsym (maxObjectPtr->outputList + 2, gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(name));
    outlet_list(maxObjectPtr->device_status_outlet3, 0L, 4, maxObjectPtr->outputList);
}

void onCharacteristicDiscovery(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, const char* name, const char* suuid, const char* cuuid)
{
    atom_setsym(maxObjectPtr->outputList + 0, gensym("discovered"));
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long)index);
    atom_setsym (maxObjectPtr->outputList + 2, gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 3, gensym(name));
    atom_setsym (maxObjectPtr->outputList + 4, gensym(suuid));
    atom_setsym (maxObjectPtr->outputList + 5, gensym(cuuid));
    outlet_list(maxObjectPtr->device_status_outlet3, 0L, 6, maxObjectPtr->outputList);
}

void onRSSIRead(MaxExternalObject* maxObjectPtr, const char* uuid, int rssi)
{
    atom_setsym (maxObjectPtr->outputList,     gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 1, gensym("rssi"));
    atom_setlong(maxObjectPtr->outputList + 2, (t_atom_long) rssi);
    outlet_list (maxObjectPtr->read_event_outlet1, 0L, 3, maxObjectPtr->outputList);
}
