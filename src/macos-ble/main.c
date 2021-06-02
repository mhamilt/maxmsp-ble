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
    
    return maxObjectPtr;
}
//------------------------------------------------------------------------------
void myExternDestructor(MaxExternalObject* maxObjectPtr)
{
    post("Max-BLE END");
    bleCentralCRelease(maxObjectPtr->bleCentral);
    
}
//------------------------------------------------------------------------------
void onBang(MaxExternalObject* maxObjectPtr)
{
    object_post( (t_object*)maxObjectPtr, "Bang");
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
        cases("clear")
        bleCentralCClearDiscovered(maxObjectPtr->bleCentral);
        break;
        cases("connect")
        if(argc == 1)
        {
            switch (atom_gettype(argv))
            {
                case A_LONG:
                    bleCentralCConnectToFoundDevice(maxObjectPtr->bleCentral, (int)atom_getlong(argv));
                    break;
                default:
                    break;
            }
        }
        break;
        defaults
        object_post( (t_object*)maxObjectPtr,
                    "This method was invoked by sending the ’%s’ message to this object.",
                    s->s_name);
        break;
    } switchs_end;
}

void onPrintMessage(MaxExternalObject* maxObjectPtr)
{
    object_post( (t_object*)maxObjectPtr, "Print Message");
}

/// This gets called when a list is sent to the object
/// @param maxObjectPtr object pointer
/// @param s message selector contains the text of a message and a pointer to the message object
/// @param argc number of atoms in the argv array
/// @param argv array of atoms holding the arguments.
void onList(MaxExternalObject* maxObjectPtr,
            t_symbol *s,
            short argc,
            t_atom *argv)
{
    post("%s\n", s->s_name);
    
    for (int i = 0; i < argc; i++)
    {
        t_atom* ap = &argv[i];
        switch (atom_gettype(ap))
        {
            case A_LONG:
                post("%ld: %ld", i + 1, atom_getlong (ap));
                break;
            case A_FLOAT:
                post("%ld: %.2f",i + 1, atom_getfloat(ap));
                break;
            case A_SYM:
                post("%ld: %s",  i + 1, atom_getsym(ap)->s_name);
                break;
            default:
                post("%ld: unknown atom type (%ld)", i + 1, atom_gettype(ap));
                break;
        }
    }
}


//------------------------------------------------------------------------------
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
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
