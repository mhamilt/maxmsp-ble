#include "MaxObject.h"
//------------------------------------------------------------------------------
void onBleNotify(MaxExternalObject* maxObjectPtr, int output)
{
    
}
//------------------------------------------------------------------------------

void onCharacteristicRead(MaxExternalObject* maxObjectPtr, const char* suuid, const char* cuuid, uint8_t* byteArray, size_t numBytes)
{
    atom_setsym(maxObjectPtr->myList,     gensym(suuid));
    atom_setsym(maxObjectPtr->myList + 1, gensym(cuuid));

    if (numBytes > (maxObjectPtr->maxListSize - 2))
    {
        numBytes = maxObjectPtr->maxListSize - 2;
        post("Bytes Truncated\n");
    }

    for (short i = 0; i < numBytes; i++)
        atom_setlong(maxObjectPtr->myList + 2 + i, (t_atom_long) byteArray[i]);

    outlet_list(maxObjectPtr->list_outlet1, 0L, numBytes + 2, maxObjectPtr->myList);
}
