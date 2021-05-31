/*
 Integrate Objective C Corebluetooth class
 */

#define MAXMSP
//#include <objc/runtime.h>
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
#include "MaxObject.h"
#include "MacosBleCentralC.h"
#include "SwitchString.h"
//------------------------------------------------------------------------------
void* myExternClass;
//------------------------------------------------------------------------------
void* myExternalConstructor()
{
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    maxObjectPtr->bleCentral = newBleCentralC();
    
    bleCentralCSetMaxObjRef(maxObjectPtr->bleCentral, maxObjectPtr);
    //    bleCentralCScanFor(maxObjectPtr->bleCentral, "DrumStick");
    
    //    maxObjectPtr->int_out = outlet_new((t_object*)maxObjectPtr, "float");
    for (int i = 0; i < 6; ++i)
    {
        maxObjectPtr->bang_out[i] = outlet_new((t_object*)maxObjectPtr, "bang");
    }
    
    //    outlet_new((t_object*)maxObjectPtr, "int");
    return maxObjectPtr;
}
//------------------------------------------------------------------------------
void myExternDestructor(MaxExternalObject* maxObjectPtr)
{
    post("END!?");
    bleCentralCRelease(maxObjectPtr->bleCentral);
    
}
//------------------------------------------------------------------------------
void onBang(MaxExternalObject* maxObjectPtr)
{
    
    //    float latestVal = MacosBleCentralRefGetLatestValue(maxObjectPtr->bleCentral);
    //    outlet_float(maxObjectPtr->int_out, latestVal);
}


/// If any message is recieved an we have not already dealt with it
/// @param maxObjectPtr object pointer
/// @param s message selector contains the text of a message and a pointer to the message object
/// @param argc number of atoms in the argv array
/// @param argv array of atoms holding the arguments.
void onAnyMessage(MaxExternalObject* maxObjectPtr, t_symbol *s, long argc, t_atom *argv)
{
    switchs(s->s_name)
    {
        cases("scan")
        bleCentralCScan(maxObjectPtr->bleCentral);
        break;
        cases("stop")
        bleCentralCStopScan(maxObjectPtr->bleCentral);
        break;
        cases("found")
        bleCentralCGetDeviceList(maxObjectPtr->bleCentral);
        break;
        defaults
        object_post( (t_object*)maxObjectPtr,
                    "This method was invoked by sending the ’%s’ message to this object.",
                    s->s_name);
        break;
    } switchs_end;
}

void onPrintMessage(MaxExternalObject* x)
{
    
}


//------------------------------------------------------------------------------
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    class_addmethod(c, (method)onAnyMessage, "anything", A_GIMME, 0);
}


int C74_EXPORT main(void)
{
    t_class* c = class_new("max-ble",
                           (method)myExternalConstructor,
                           (method)myExternDestructor,
                           (short)sizeof(MaxExternalObject),
                           0L,
                           0);
    coupleMethodsToExternal(c);
    class_register(CLASS_BOX, c);
    myExternClass = c;
    
    
    return 0;
}
