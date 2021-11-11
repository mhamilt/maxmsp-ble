/*
 * Max Ble: A macOS BLE
 */
//------------------------------------------------------------------------------
#define MAXMSP
#include "ext.h"
#include "ext_proto.h"
#include "MaxObject.h"
#include "BleClasses/MaxBleCentral.h"
#include "SwitchString.h"
//------------------------------------------------------------------------------
void* myExternClass;
//------------------------------------------------------------------------------
void* myExternalConstructor()
{
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    maxObjectPtr->bleCentral = newBleCentralC();
    bleCentralCSetMaxObjRef(maxObjectPtr->bleCentral, maxObjectPtr);
    maxObjectPtr->maxListSize = 100;
    
    maxObjectPtr->list_outlet3 = listout(maxObjectPtr);
    maxObjectPtr->list_outlet2 = listout(maxObjectPtr);
    maxObjectPtr->list_outlet1 = listout(maxObjectPtr);
    
    atom_alloc_array(maxObjectPtr->maxListSize,
                     &maxObjectPtr->listSize,
                     &maxObjectPtr->outputList,
                     &maxObjectPtr->listAllocSuccess);
    
    return maxObjectPtr;
}
//------------------------------------------------------------------------------
void myExternDestructor(MaxExternalObject* maxObjectPtr)
{
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
    switchs(s)
    {
        //----------------------------------------------------------------------
        cases("blacklist")
        bleCentralCBlacklistStalledDevices(maxObjectPtr->bleCentral);
        break;
        //----------------------------------------------------------------------
        cases("clear")
        bleCentralCClearDiscovered(maxObjectPtr->bleCentral);
        break;
        //----------------------------------------------------------------------
        cases("connect")
        if(argc == 1)
        {
            switch (atom_gettype(argv))
            {
                case A_LONG:
                    bleCentralCConnectToFoundDevice(maxObjectPtr->bleCentral, (int)atom_getlong(argv));
                    break;
                case A_SYM:
                    bleCentralCConnectToDeviceWithUUID(maxObjectPtr->bleCentral,
                                                       atom_getsym(argv)->s_name);
                    break;
            }
        }
        else if (argc >= 2)
        {
            switchs(atom_getsym(argv))
            {
                cases("service")
                break;
                cases("name")
                bleCentralCConnectToDeviceWithName(maxObjectPtr->bleCentral, atom_getsym(argv + 1)->s_name);
                break;
            }switchs_end
        }
        break;
        //----------------------------------------------------------------------
        cases("device")
        if(argc >= 2)
        {
            int deviceIndex = (int)atom_getlong(argv);
            switchs(atom_getsym(argv + 1))
            {
                cases("rssi")
                bleCentralCGetRssi(maxObjectPtr->bleCentral, deviceIndex);
                break;
                cases("subscribe")
                if(argc == 4
                   && atom_gettype(argv + 2) == A_SYM
                   && atom_gettype(argv + 3) == A_SYM)
                    bleCentralCSubscribeToCharacteristic(maxObjectPtr->bleCentral,
                                                         deviceIndex,
                                                         atom_getsym(argv + 2)->s_name,
                                                         atom_getsym(argv + 3)->s_name);
                if(argc == 5
                   && atom_gettype(argv + 2) == A_SYM
                   && atom_gettype(argv + 3) == A_SYM
                   && atom_gettype(argv + 4) == A_LONG)
                {
                    if (atom_getlong(argv + 4))
                    {
                        bleCentralCSubscribeToCharacteristic(maxObjectPtr->bleCentral,
                                                             deviceIndex,
                                                             atom_getsym(argv + 2)->s_name,
                                                             atom_getsym(argv + 3)->s_name);
                    }
                    else
                    {
                        bleCentralCUnsubscribeToCharacteristic(maxObjectPtr->bleCentral,
                                                               deviceIndex,
                                                               atom_getsym(argv + 2)->s_name,
                                                               atom_getsym(argv + 3)->s_name);
                    }
                }
                break;
                cases("unsubscribe")
                if(argc == 2)
                {
                    object_post( (t_object*)maxObjectPtr,
                                "unsubscribe from all characteristics of device %d",
                                deviceIndex);
                }
                else if(argc == 4
                        && atom_gettype(argv + 2) == A_SYM
                        && atom_gettype(argv + 3) == A_SYM)
                {
                    //                    bleCentralCUnsubscribeToCharacteristic(maxObjectPtr->bleCentral,
                    //                                                           deviceIndex,
                    //                                                           atom_getsym(argv + 2)->s_name,
                    //                                                           atom_getsym(argv + 3)->s_name);
                    //
                    object_post( (t_object*)maxObjectPtr,
                                "unsubscribe from Char: %s of Service: %s of Device %s",
                                atom_getsym(argv + 2)->s_name,
                                atom_getsym(argv + 3)->s_name,
                                bleCentralCGetDeviceUUID(maxObjectPtr->bleCentral, deviceIndex));
                }
                break;
            } switchs_end
        }
        break;
        //----------------------------------------------------------------------
        cases("filter")
        
        if(argc >= 1)
        {
            switchs(atom_getsym(argv))
            {
                cases("rssi")
                bleCentralCSetRSSIScanThreshold (maxObjectPtr->bleCentral,
                                                 (int)atom_getlong(argv + 1));
                break;
                cases("iphone")
                bleCentralCSetIgnoreiPhone(maxObjectPtr->bleCentral,
                                           (bool)atom_getlong(argv + 1));
                break;
            }switchs_end;
        }
        break;
        //----------------------------------------------------------------------
        cases("found")
        bleCentralCGetDeviceList(maxObjectPtr->bleCentral);
        break;
        //----------------------------------------------------------------------
        cases("report")
        if(argc == 1)
        {
            if(atom_gettype(argv) == A_LONG)
                bleCentralCSetReporting(maxObjectPtr->bleCentral,
                                        (bool)atom_getlong(argv));
        }
        break;
        //----------------------------------------------------------------------
        cases("scan")
        if(argc && atom_gettype(argv) == A_LONG && !atom_getlong(argv))
            bleCentralCStopScan(maxObjectPtr->bleCentral);
        else if (argc && atom_gettype(argv) == A_SYM)
            bleCentralCScanForServices(maxObjectPtr->bleCentral,
                                       argv,
                                       argc);
        else
            bleCentralCScan(maxObjectPtr->bleCentral);
        break;
        //----------------------------------------------------------------------
        cases("stop")
        bleCentralCStopScan(maxObjectPtr->bleCentral);
        break;
        //----------------------------------------------------------------------
        cases("write")
        break;
        //----------------------------------------------------------------------
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

/// @brief This is the function called by MaxMSP when the cursor is over an inlet or
/// outlet.
/// @param maxObjectPtr object pointer
/// @param box still don't know what this is
/// @param message either inlet  1 or outlet 2
/// @param arg inlet / outlet index
/// @param dstString pointer to destination: limited to 60 chars.
void inletAssistant(MaxExternalObject* maxObjectPtr,
                    void *box,
                    long message,
                    long arg,
                    char *destination)
{
    switch (message)
    {
        case 1: // inletMessage
            switch (arg)
            {
                case 0:
                    sprintf(destination, "A message list scan $1, stop, found, clear, connect $1");
                    break;
                default:
                    sprintf(destination, "some other inlet");
            }
            break;
        case 2:  // outletMessage
            switch (arg)
            {
                case 0:
                    sprintf(destination, "list: output of service characteristic and raw bytes");
                    break;
                case 1:
                    sprintf(destination, "list: out on notification recieved from subscribed device");
                    break;
                case 2:
                    sprintf(destination, "list: output when new device found or on 'found' message");
                    break;
                default:
                    sprintf(destination, "some other outlet");
            }
            break;
    }
}


//------------------------------------------------------------------------------
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    //    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
    class_addmethod(c, (method)onAnyMessage, "anything", A_GIMME, 0);
    class_addmethod(c, (method)inletAssistant, "assist", A_CANT, 0);
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
