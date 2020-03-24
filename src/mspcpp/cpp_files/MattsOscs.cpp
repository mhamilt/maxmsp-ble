//
//  MattsOscs.cpp
//  mspcpp~
//
//  Created by mhamilt7 on 13/03/2020.
//

#include "MattsOscs.hpp"


float Phasor::updatePhase()
{
    phase += phaseDelta;
    
    if (phase > 1.0f)
        phase -= 1.0f;
    
    return phase;
}

double Phasor::process()
{
    return phase;
}

void Phasor::setSampleRate(float sr)
{
    sampleRate = sr;
}

void Phasor::setFrequency(float freq)
{
    frequency = freq;
    phaseDelta = frequency / sampleRate;
}

void SineOsc::setup(float sr, float freq)
{
    setSampleRate(sr);
    setFrequency(freq);
}

double SineOsc::process()
{
    return std::sin(2 * 3.14159 * updatePhase());
}

// Try adding a SinOsc and SquareOsc class

