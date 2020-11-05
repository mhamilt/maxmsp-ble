#include "MaxObject.h"
//------------------------------------------------------------------------------
void onBleNotify(MaxExternalObject* maxObjectPtr, int output)
{
    outlet_bang(maxObjectPtr->bang_out[output]);
}
//------------------------------------------------------------------------------
