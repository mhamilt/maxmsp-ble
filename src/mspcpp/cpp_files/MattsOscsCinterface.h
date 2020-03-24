//
//  MattsOscsCinterface.hpp
//  mspcpp~
//
//  Created by mhamilt7 on 13/03/2020.
//

#ifndef MattsOscsCinterface_h
#define MattsOscsCinterface_h

#ifdef __cplusplus
extern "C"{
#endif

typedef struct SineOsc SineOsc;
SineOsc* newSineOsc();

void SineOsc_setup(SineOsc* v, float sr, float freq);
void SineOsc_setFrequency(SineOsc* v, float freq);
double SineOsc_process(SineOsc* v);

void deleteSineOsc(SineOsc* v);

#ifdef __cplusplus
}
#endif

#endif /* MattsOscsCinterface_hpp */
