//
//  MattsOscs.hpp
//  mspcpp~
//
//  Created by mhamilt7 on 13/03/2020.
//

#ifndef MattsOscs_hpp
#define MattsOscs_hpp

#include <cmath>

class Phasor
{
public:
    float updatePhase();
    virtual double process();
    void setSampleRate(float sr);
    void setFrequency(float freq);
    
private:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
};

/// <#Description#>
class SineOsc : public Phasor
{
public:
    void setup(float sr, float freq);
    double process() override;
};

#endif /* MattsOscs_hpp */
