//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <ctype.h>
//------------------------------------------------------------------------------
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
#include "switch_string.h"
//------------------------------------------------------------------------------

void* myExtern_tildeClass;

//------------------------------------------------------------------------------
typedef struct myObj
{
    float a;
} t_myObj;

/** @struct
 The MaxMSP object
 */
typedef struct _myExtern_tilde
{
    t_pxobject x_obj;
    t_symbol* x_arrayname;
    t_myObj a;
} t_myExtern_tilde;
//------------------------------------------------------------------------------
/// Called After Main
void* myExternalConstructor(long arg1)
{
    if (!arg1)
    {
        post("no arguement\n");
    }
    
    post("Start\n");
    t_myExtern_tilde* x = (t_myExtern_tilde *)object_alloc(myExtern_tildeClass);
    dsp_setup((t_pxobject*)x, 1);
    
    outlet_new((t_object*)x, "signal");    //----------------------------------------------------------------------------
    return (x);
}

//------------------------------------------------------------------------------

void myExternDestructor(t_myExtern_tilde *x)
{
    dsp_free((t_pxobject*)x);
}
//------------------------------------------------------------------------------

/// @brief This is the function called by MAX/MSP when the cursor is over an inlet or
/// outlet.
/// @param x object pointer
/// @param box <#box description#>
/// @param message either inlet  1 or outlet 2
/// @param arg inlet / outlet index
/// @param dstString pointer to destination: limited to 60 chars.
void inletAssistant(t_myExtern_tilde *x, void *box, long message,
                    long arg, char *dstString)
{
    //    if (message == 2)
    //        sprintf(dstString, "(signal) granulated output");
    //    else
    //    {
    //        switch (arg) {
    //            case 0:
    //                sprintf(dstString, "(bang/list/message) bang starts granulation...");
    //                break;
    //            case 1:
    //                sprintf(dstString, "(float) Transposition offset in semitones");
    //                break;
    //        }
    //    }
}

//------------------------------------------------------------------------------
void mspExternalProcessBlock(t_myExtern_tilde* x, t_object* dsp64, double** ins,
                             long numins, double** outs, long numouts,
                             long sampleframes, long flags, void* userparam)

{
    double* in = ins[0];
    
    for (int i = 0; i < numouts; ++i)
    {
        //        g->channelBuffers[i] = (mdefloat*)outs[i];
    }
}
//------------------------------------------------------------------------------

/** This gets called third, when the audio engine starts (after _new!). */

/// <#Description#>
/// @param x <#x description#>
/// @param dsp64 <#dsp64 description#>
/// @param count <#count description#>
/// @param samplerate <#samplerate description#>
/// @param vectorsize <#vectorsize description#>
/// @param flags <#flags description#>
void prepareToPlay(t_myExtern_tilde* x, t_object* dsp64, short* count,
                   double samplerate, long vectorsize, long flags)
{
    object_method(dsp64,
                  gensym("dsp_add64"),
                  x,
                  mspExternalProcessBlock,
                  0,
                  NULL);
}
//------------------------------------------------------------------------------

/** This gets called when we receive a bang */

void onBang(t_myExtern_tilde *x)
{
    post("I got a bang!\n");
}
//------------------------------------------------------------------------------

/** this gets called when a list is sent to the object */

void onList(t_myExtern_tilde *x, t_symbol *s,
            short argc, t_atom *argv)
{
    
}
//------------------------------------------------------------------------------
void onPrintMessage(t_myExtern_tilde *x)
{
    
}
//------------------------------------------------------------------------------
/// <#Description#>
/// @param x <#x description#>
/// @param s <#s description#>
/// @param argc <#argc description#>
/// @param argv <#argv description#>
void onAnyMessage(t_myExtern_tilde *x, t_symbol *s, long argc, t_atom *argv)
{
    object_post( (t_object*)x,
                "This method was invoked by sending the ’%s’ message to this object.",
                s->s_name);
    
    switchs(s->s_name)
    {
        cases("help")
            post("you asked for help");
            break;
        defaults
            post("No match\n");
            break;
    } switchs_end;
}

//------------------------------------------------------------------------------

/* This method is called first. */

int C74_EXPORT main(void)

{
    post("START!\n");
    t_class* c = class_new("mspcpp~",
                           (method)myExternalConstructor,
                           (method)myExternDestructor,
                           (short)sizeof(t_myExtern_tilde),
                           0L,
                           A_DEFLONG,
                           0);
    /* to couple an inlet to a method */
    class_addmethod(c, (method)onBang, "bang", 0);
    //    class_addmethod(c, (method)onList, "list", A_GIMME, 0);
    //    class_addmethod(c, (method)inletAssistant,"assist", A_CANT,0);
    //    class_addmethod(c, (method)onPrintMessage, "print", 0);
    class_addmethod(c, (method)onAnyMessage, "anything", A_GIMME, 0);
    class_addmethod(c, (method)prepareToPlay, "dsp64", A_CANT, 0);
    class_dspinit(c);
    class_register(CLASS_BOX, c);
    myExtern_tildeClass = c;
    return 0;
}
//------------------------------------------------------------------------------

/* EOF mdeGranular~maxmsp.c */

