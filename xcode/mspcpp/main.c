/*
 A template for a basic max external object with more explicit documentation
 
 See Readme of repository for build instructions.
 Create an Issue on the repository if anything is amiss or you have any suggestion
 - mhamilt Mar 2020
 */

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
#include "MattsOscsCinterface.h"
//------------------------------------------------------------------------------

/// void* to the complete new Max External class so that it can be used in the class methods
/// This will be set to t_class* in the main function
/// @code t_class* c = class_new(...);
/// myExternClass = c;
void* myExternClass;

//------------------------------------------------------------------------------
/// DSP object properties
typedef struct _MyDspStruct
{
    float a;
} MyDspStruct;

/** @struct
 The MaxMSP object
 @field t_pxobject x_obj
 @field t_symbol* x_arrayname
 @field SineOsc* x_osc pointer to our DSP object. use this with the interfacing C functions
 @field short inletConnection number of connections
 */
typedef struct _MaxExternalObject
{
    ///
    t_pxobject x_obj;
    ///
    t_symbol* x_arrayname;
    /// pointer to our DSP object. use this with the interfacing C functions
    SineOsc* x_osc;
    ///
    short inletConnection;
} MaxExternalObject;
//------------------------------------------------------------------------------
/// External Object Constructor: use this to setup any variables / properties of your DSP Struct or MaxExternalObject
/// Arguement list should be as long as the list of type arguments passed in the class_new call below.
/// @param arg1 first argument to object: should match type given in class_new(...)
/// @returns a void* to an instance of the MaxExternalObject
void* myExternalConstructor(long arg1)
{
    //--------------------------------------------------------------------------
    if (!arg1)
    {
        post("no arguement\n");
    }
    
    //--------------------------------------------------------------------------
    MaxExternalObject* maxObjectPtr = (MaxExternalObject*)object_alloc(myExternClass);
    maxObjectPtr->x_osc = newSineOsc();
    
    dsp_setup((t_pxobject*)maxObjectPtr, 1);
    //--------------------------------------------------------------------------
    // inlet_new((t_object*)maxObjectPtr, "signal");
    outlet_new((t_object*)maxObjectPtr, "signal");
    //--------------------------------------------------------------------------
    return maxObjectPtr;
}

//------------------------------------------------------------------------------
///
void myExternDestructor(MaxExternalObject* maxObjectPtr)
{
    dsp_free((t_pxobject*)maxObjectPtr);
}
//------------------------------------------------------------------------------

/// @brief This is the function called by MAX/MSP when the cursor is over an inlet or
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
                    char *dstString)
{
    const long  inletMessage = 1;
    const long outletMessage = 2;
    
    switch (message)
    {
        case inletMessage:
            switch (arg)
            {
                case 0:
                    sprintf(dstString, "inlet 1");
                    break;
                case 1:
                    sprintf(dstString, "inlet 2");
                    break;
                default:
                    sprintf(dstString, "some other inlet");
            }
            break;
        case outletMessage:
            switch (arg)
            {
                case 0:
                    sprintf(dstString, "outlet 1");
                    break;
                case 1:
                    sprintf(dstString, "outlet 2");
                    break;
                default:
                    sprintf(dstString, "some other outlet");
            }
            break;
    }
}

//------------------------------------------------------------------------------
#pragma mark DSP Loop
/// Main DSP process block, do your DSP here
/// @param maxObjectPtr
/// @param dsp64
/// @param ins double pointer array to sample inlets
/// @param numins
/// @param outs double pointer array to sample outlets
/// @param numouts
/// @param sampleframes samples per channel
/// @param flags
/// @param userparam no idea
void mspExternalProcessBlock(MaxExternalObject* maxObjectPtr, t_object* dsp64,
                             double** ins, long numins, double** outs, long numouts,
                             long sampleframes, long flags, void* userparam)

{
    //--------------------------------------------------------------------------
    // DSP loop
    for (int i = 0; i < numouts; ++i)
    {
        for (int s = 0; s < sampleframes; ++s)
        {
            outs[0][s] = SineOsc_process(maxObjectPtr->x_osc) * 0.1;
        }
    }
}

//------------------------------------------------------------------------------

/// Audio DSP setup
/// @param maxObjectPtr object pointer
/// @param dsp64
/// @param count array containing number of connections to an inlet with index [i]
/// @param samplerate
/// @param vectorsize
/// @param flags
void prepareToPlay(MaxExternalObject* maxObjectPtr, t_object* dsp64, short* count,
                   double samplerate, long vectorsize, long flags)
{
    maxObjectPtr->inletConnection = count[0];
    SineOsc_setup(maxObjectPtr->x_osc, samplerate, 440.0);
    object_method(dsp64,
                  gensym("dsp_add64"),
                  maxObjectPtr,
                  mspExternalProcessBlock,
                  0,
                  NULL);
}
//------------------------------------------------------------------------------

/// This gets called when we receive a bang
/// @param maxObjectPtr object pointer
void onBang(MaxExternalObject* maxObjectPtr)
{
    post("I got a bang!\n");
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
    post("I got a list!\n");
}

//------------------------------------------------------------------------------

void onPrintMessage(MaxExternalObject* x)
{
    post("Print some info about the object\n");
}

//------------------------------------------------------------------------------

/// If any message is recieved an we have not already dealt with it
/// @param maxObjectPtr object pointer
/// @param s message selector contains the text of a message and a pointer to the message object
/// @param argc number of atoms in the argv array
/// @param argv array of atoms holding the arguments.
void onAnyMessage(MaxExternalObject* maxObjectPtr, t_symbol *s, long argc, t_atom *argv)
{
    object_post( (t_object*)maxObjectPtr,
                "This method was invoked by sending the ’%s’ message to this object.",
                s->s_name);
}

//------------------------------------------------------------------------------
/// Bundle all class_addmethod calls into one function.
/// @param c max external class pointer
void coupleMethodsToExternal( t_class* c)
{
    class_addmethod(c, (method)onBang, "bang", 0);
    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
    class_addmethod(c, (method)inletAssistant,"assist", A_CANT,0);
    class_addmethod(c, (method)onPrintMessage, "print", 0);
    class_addmethod(c, (method)onAnyMessage, "anything", A_GIMME, 0);
    class_addmethod(c, (method)prepareToPlay, "dsp64", A_CANT, 0);
}
//------------------------------------------------------------------------------
int C74_EXPORT main(void)
{
    t_class* c = class_new("mspcpp~",
                           (method)myExternalConstructor,
                           (method)myExternDestructor,
                           (short)sizeof(MaxExternalObject),
                           0L,
                           A_DEFLONG,
                           0);
    
    coupleMethodsToExternal(c);
    
    class_dspinit(c);
    class_register(CLASS_BOX, c);
    
    myExternClass = c;
    
    return 0;
}
//------------------------------------------------------------------------------

/* EOF mdeGranular~maxmsp.c */

