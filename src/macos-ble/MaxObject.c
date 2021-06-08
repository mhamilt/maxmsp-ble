#include "MaxObject.h"
//------------------------------------------------------------------------------
void onBleNotify(MaxExternalObject* maxObjectPtr, int output)
{
    
}
//------------------------------------------------------------------------------

void onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->outputList,     gensym(suuid));
    atom_setsym(maxObjectPtr->outputList + 1, gensym(cuuid));

    if (numBytes > (maxObjectPtr->maxListSize - 2))
    {
        numBytes = maxObjectPtr->maxListSize - 2;
        post("Bytes Truncated\n");
    }

    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 2 + i, (t_atom_long) byteArray[i]);

    outlet_list(maxObjectPtr->list_outlet1, 0L, numBytes + 2, maxObjectPtr->outputList);
}

void onNotificationRead(MaxExternalObject* maxObjectPtr, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->outputList, gensym(cuuid));
    
    if (numBytes > (maxObjectPtr->maxListSize - 1))
    {
        numBytes = maxObjectPtr->maxListSize - 1;
        post("Bytes Truncated\n");
    }

    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->outputList + 1 + i, (t_atom_long) byteArray[i]);

    outlet_list(maxObjectPtr->list_outlet2, 0L, numBytes + 1, maxObjectPtr->outputList);
}

void outputFoundDeviceList(MaxExternalObject* maxObjectPtr, unsigned long index, const char* uuid, int rssi)
{
    //atom_setlong(maxObjectPtr->outputList + 0, (t_atom_long) index);
    atom_setsym (maxObjectPtr->outputList + 0, gensym(uuid));
//    atom_setsym (maxObjectPtr->outputList + 2, gensym(name));
    atom_setlong(maxObjectPtr->outputList + 1, (t_atom_long) rssi);
    
    outlet_list(maxObjectPtr->list_outlet1, 0L, 2, maxObjectPtr->outputList);
}

void onRSSIRead(MaxExternalObject* maxObjectPtr, const char* uuid, int rssi)
{
    atom_setsym (maxObjectPtr->outputList,     gensym(uuid));
    atom_setsym (maxObjectPtr->outputList + 1, gensym("rssi"));
    atom_setlong(maxObjectPtr->outputList + 2, (t_atom_long) rssi);
    outlet_list (maxObjectPtr->list_outlet1, 0L, 3, maxObjectPtr->outputList);
}
