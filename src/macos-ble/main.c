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
//------------------------------------------------------------------------------
void* myExternClass;
//------------------------------------------------------------------------------
void* myExternalConstructor()
{
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    maxObjectPtr->bleCentral = MacosBleCentralRefCreate();
    
    MacosBleCentralRefSetMaxObjRef(maxObjectPtr->bleCentral, maxObjectPtr);
    MacosBleCentralRefScanFor(maxObjectPtr->bleCentral, "DrumStick");
    
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
    post("END");
}
//------------------------------------------------------------------------------
void onBang(MaxExternalObject* maxObjectPtr)
{
//    float latestVal = MacosBleCentralRefGetLatestValue(maxObjectPtr->bleCentral);
//    outlet_float(maxObjectPtr->int_out, latestVal);
}
//------------------------------------------------------------------------------
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
}

int C74_EXPORT main(void)
{
    post("hello");
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
