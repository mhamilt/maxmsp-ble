//------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <ctype.h>
//------------------------------------------------------------------------------
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
//#define VERSION "0.1 (Max API 7.1.0)"
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
        arg1 = 3;
    }
    
    post("Start\n");
    t_myExtern_tilde* x = (t_myExtern_tilde *)object_alloc(myExtern_tildeClass);
    dsp_setup((t_pxobject*)x, 1);
    //----------------------------------------------------------------------------
    // couple inlets to methods: inlets defined in reverse order
    
    //    floatin(x, 1); /* grain amplitude */
    //    floatin(x, 2); /* density of the grains in % */
    
    //----------------------------------------------------------------------------
    //    x->x_arrayname = gensym("ms1000");
    //----------------------------------------------------------------------------
    //    for (int i = 0; i < (int)numChannels; i++)
    //    {
    //
    //    }
    outlet_new((t_object*)x, "signal");    //----------------------------------------------------------------------------
    return (x);
}

//------------------------------------------------------------------------------

void myExternDestructor(t_myExtern_tilde *x)
{
    dsp_free((t_pxobject*)x);
}
//------------------------------------------------------------------------------

/** This is the function called by MAX/MSP when the cursor is over an inlet or
 *  outlet. We can ignore the box arg for now. The message is either 1
 *  (inlet) or 2 (outlet) depending on whether the cursor is over an inlet or
 *  outlet. arg is the inlet/outlet index starting from 0 for the leftmost.
 *  dstString is the placeholder for the message we want to display; this is
 *  limited to 60 chars.
 *  */

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

void onPrintMessage(t_myExtern_tilde *x)
{
    
}

void onAnyMessage(t_myExtern_tilde *x, t_symbol *s, long argc, t_atom *argv)
{
    object_post( (t_object*)x,
                 "This method was invoked by sending the ’%s’ message to this object.",
                  s->s_name);
    // argc and argv are the arguments, as described in above.
}

//------------------------------------------------------------------------------

/* This method is called first. */

int C74_EXPORT main(void)

{
    post("START!\n");
    t_class* c = class_new("mymspextern~",
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
