/*
  LightAnimator.h - Library for animating lights by sound
  Created by Peter Czibula, February 18, 2019.
*/
#ifndef LightAnimator_h
#define LightAnimator_h

#include "../../../src/def.h"
#include "Arduino.h"
#include "Analyser.h"
#include "AlaLedRgb.h"
#include <Entropy.h>

class LightAnimator
{

public:
  // constructor / Setup
  LightAnimator();
  void begin(int lightcount, Analyser *panalyser);
  AlaColor getLightRgb(int Address);
  void computeLightAnimation();
  int LightCount;

private:
  AlaLedRgb alaLedRgbStrip;
  Analyser *_analyser;
};
#endif
