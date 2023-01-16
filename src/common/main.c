/*
 * max-ble: A MAXMSP BLE Object
 */
//------------------------------------------------------------------------------
#define MAXMSP
#include "ext.h"
#include "ext_proto.h"
#include "MaxObject.h"
#include "MaxBleCentral.h"
#include "SwitchString.h"
//------------------------------------------------------------------------------
void* myExternClass;

//------------------------------------------------------------------------------
void* myExternalConstructor()
{
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    maxObjectPtr->bleCentral = (MaxBleCentralRef*)newBleCentralC();    
    bleCentralCSetMaxObjRef(maxObjectPtr->bleCentral, maxObjectPtr);
    maxObjectPtr->maxListSize = MAXBLELISTSIZE;
    
    maxObjectPtr->ble_event_outlet = listout(maxObjectPtr);
    
    atom_alloc_array(maxObjectPtr->maxListSize,
                     &maxObjectPtr->listSize,
                     &maxObjectPtr->outputList,
                     &maxObjectPtr->listAllocSuccess);

//    maxObjectPtr->writeBuffer = malloc(MAXBLEWRITEBUFFERSIZE);
    
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
//------------------------------------------------------------------------------
void onPrintMessage(MaxExternalObject* maxObjectPtr)
{
    object_post( (t_object*)maxObjectPtr, "Print Message");
}
//------------------------------------------------------------------------------
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
                    sprintf(destination, "list: output on device connection status change");
                    break;
                case 3:
                    sprintf(destination, "list: output when new device found or on 'found' message");
                    break;
                default:
                    sprintf(destination, "some other outlet");
            }
            break;
    }
}
//------------------------------------------------------------------------------
void onClear      (MaxExternalObject* maxObjectPtr)
{
    bleCentralCClearDiscovered(maxObjectPtr->bleCentral);
}
//------------------------------------------------------------------------------
void onConnect    (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc == 1)
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
}
//------------------------------------------------------------------------------
void onDisconnect (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc == 1)
    {
        switch (atom_gettype(argv))
        {
            case A_LONG:
                bleCentralCDisconnectFromFoundDevice(maxObjectPtr->bleCentral,
                                                     (int)atom_getlong(argv));
                break;
            case A_SYM:
                bleCentralCDisconnectFromDeviceWithUUID(maxObjectPtr->bleCentral,
                                                        atom_getsym(argv)->s_name);
                break;
        }
    }
}
//------------------------------------------------------------------------------
void onFilter     (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc >= 1)
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
}
//------------------------------------------------------------------------------
void onFound      (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    bleCentralCGetDeviceList(maxObjectPtr->bleCentral);
}
//------------------------------------------------------------------------------
void onRead       (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc == 1)
    {
        switch (atom_gettype(argv))
        {
            case A_LONG:
                bleCentralCReadAllCharacteristicWithDeviceAtIndex(maxObjectPtr->bleCentral,
                                                                  (int)atom_getlong(argv));
                break;
            case A_SYM:
                bleCentralCReadAllCharacteristicWithDeviceUUID(maxObjectPtr->bleCentral,
                                                               atom_getsym(argv)->s_name);
                break;
        }
    }
    else if (argc == 3
             && atom_gettype(argv + 0) == A_LONG
             && atom_gettype(argv + 1) == A_SYM
             && atom_gettype(argv + 2) == A_SYM)
        bleCentralCReadCharacteristicWithDeviceAtIndex(maxObjectPtr->bleCentral,
                                                       (int)atom_getlong(argv),
                                                       atom_getsym(argv + 1)->s_name,
                                                       atom_getsym(argv + 2)->s_name);
}
//------------------------------------------------------------------------------
void onReport     (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc == 1)
    {
        if (atom_gettype(argv) == A_LONG)
            bleCentralCSetReporting(maxObjectPtr->bleCentral,
                                    (bool)atom_getlong(argv));
    }
}
//------------------------------------------------------------------------------
void onScan       (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc && atom_gettype(argv) == A_LONG && !atom_getlong(argv))
        bleCentralCStopScan(maxObjectPtr->bleCentral);
    else if (argc && atom_gettype(argv) == A_SYM)
        bleCentralCScanForServices(maxObjectPtr->bleCentral,
                                   argv,
                                   argc);
    else
        bleCentralCScan(maxObjectPtr->bleCentral);
}
//------------------------------------------------------------------------------
void onStop       (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    bleCentralCStopScan(maxObjectPtr->bleCentral);
}
//------------------------------------------------------------------------------
void onSubscribe  (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc == 3
        && (atom_gettype(argv + 0) == A_LONG || atom_gettype(argv + 0) == A_SYM)
        && atom_gettype(argv + 1) == A_SYM
        && atom_gettype(argv + 2) == A_SYM)
    {
        if(atom_gettype(argv + 0) == A_LONG)
        {
            bleCentralCSubscribeToCharacteristic(maxObjectPtr->bleCentral,
                                                 (int)atom_getlong(argv),
                                                 atom_getsym(argv + 1)->s_name,
                                                 atom_getsym(argv + 2)->s_name);
        }
        else
        {
            bleCentralCSubscribeToCharacteristicWithDeviceUUID(maxObjectPtr->bleCentral,
                                                               atom_getsym(argv + 0)->s_name,
                                                               atom_getsym(argv + 1)->s_name,
                                                               atom_getsym(argv + 2)->s_name);
        }
    }
}
//------------------------------------------------------------------------------
void onWrite      (MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc >= 3)
        bleCentralCWriteToCharactaristic (maxObjectPtr->bleCentral, argv, argc, maxObjectPtr->writeBuffer);
    
}
//------------------------------------------------------------------------------
void onUnsubscribe(MaxExternalObject* maxObjectPtr, t_symbol *s, short argc, t_atom *argv)
{
    if (argc == 3
        && (atom_gettype(argv + 0) == A_LONG || atom_gettype(argv + 0) == A_SYM)
        && atom_gettype(argv + 1) == A_SYM
        && atom_gettype(argv + 2) == A_SYM)
    {
        if(atom_gettype(argv + 0) == A_LONG)
        {
            bleCentralCUnsubscribeToCharacteristic(maxObjectPtr->bleCentral,
                                                   (int)atom_getlong(argv),
                                                   atom_getsym(argv + 1)->s_name,
                                                   atom_getsym(argv + 2)->s_name);
        }
        else
        {
            bleCentralCUnsubscribeToCharacteristicWithDeviceUUID(maxObjectPtr->bleCentral,
                                                                 atom_getsym(argv + 0)->s_name,
                                                                 atom_getsym(argv + 1)->s_name,
                                                                 atom_getsym(argv + 2)->s_name);
        }
    }
}

void onQueue      (MaxExternalObject* maxObjectPtr)
{
    bleCentralCPrintTaskQueue(maxObjectPtr->bleCentral);
}

//------------------------------------------------------------------------------
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    //    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
    class_addmethod(c, (method)inletAssistant, "assist", A_CANT, 0);
    
    class_addmethod(c, (method)onClear,             "clear", A_NOTHING);
    class_addmethod(c, (method)onConnect,         "connect", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onDisconnect,   "disconnect", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onFilter,           "filter", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onFound,             "found", A_NOTHING);
    class_addmethod(c, (method)onQueue,             "queue", A_NOTHING);
    class_addmethod(c, (method)onRead,               "read", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onReport,           "report", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onScan,               "scan", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onStop,               "stop", A_NOTHING);
    class_addmethod(c, (method)onSubscribe,     "subscribe", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onWrite,             "write", A_GIMME, A_NOTHING);
    class_addmethod(c, (method)onUnsubscribe, "unsubscribe", A_GIMME, A_NOTHING);
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

