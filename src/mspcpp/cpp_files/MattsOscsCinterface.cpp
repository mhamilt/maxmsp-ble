//
//  MattsOscsCinterface.cpp
//  mspcpp~
//
//  Created by mhamilt7 on 13/03/2020.
//

#include "MattsOscs.hpp"
#include "MattsOscsCinterface.h"

extern "C"
{
    SineOsc* newSineOsc()
    {
        return new SineOsc();
    }
    
    void SineOsc_setup(SineOsc* v, float sr, float freq)
    {
        v->setup(sr, freq);
    }
    
    void SineOsc_setFrequency(SineOsc* v, float freq)
    {
        v->setFrequency(freq);
    }
    
    double SineOsc_process(SineOsc* v)
    {
        return v->process();
    }
    
    void deleteSineOsc(SineOsc* v)
    {
        delete v;
    }
}
