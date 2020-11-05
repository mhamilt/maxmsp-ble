#pragma once
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "buffer.h"
//------------------------------------------------------------------------------
typedef CFTypeRef MacosBleCentralRef;
typedef struct _MaxExternalObject
{
    t_pxobject x_obj;
    t_symbol* x_arrayname;
    MacosBleCentralRef bleCentral;
    void* int_out;
    void* bang_out[6];
} MaxExternalObject;

void onBleNotify(MaxExternalObject* maxObjectPtr, int output);
