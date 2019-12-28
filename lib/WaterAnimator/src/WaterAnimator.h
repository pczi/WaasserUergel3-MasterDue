/*
  WaterAnimator.h - Library for animating waterjets by sound
  Created by Peter Czibula, February 18, 2019.
*/
#ifndef WaterAnimator_h
#define WaterAnimator_h

#include "../../../include/def.h"
#include "Arduino.h"
#include "Analyser.h"
#include "Awa.h"
#include <Entropy.h>

class WaterAnimator
{

public:
  // constructor / Setup
  WaterAnimator();
  void begin(int waterjetcount, Analyser *panalyser);
  AwaValve getValve(int Address);
  void computeWaterAnimation();
  int WaterjetCount;

  // Waterjet attack every 40 millis
  byte WaterjetAttack;
  // Waterjet decay every 20 millis
  byte WaterjetDecay;
  // Waterjet cycle every 50 millis
  byte WaterjetcycleMillis;

private:
  Awa avaValves;
  Analyser *_analyser;
};
#endif
