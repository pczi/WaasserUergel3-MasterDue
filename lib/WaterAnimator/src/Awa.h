/*
  Arduino Water Animator - Library for animating waterjets by sound
  Created by Peter Czibula, February 18, 2019.
*/

#ifndef Awa_h
#define Awa_h

#include <stdint.h>
#include "Arduino.h"
#include "Analyser.h"
#include <CircularBuffer.h>

////////////////////////////////////////////////////////////////////////////////
// when to launch next Animation step

#define AWA_RTL_CONTINUOUS 1
#define AWA_RTL_BASSBEAT 2
#define AWA_RTL_VOLUMEBEAT 4
#define AWA_RTL_BASSVOLUMEBEAT 8

////////////////////////////////////////////////////////////////////////////////
// Animation direction

#define AWA_DIR_FORWARD 101
#define AWA_DIR_REVERSE 102

////////////////////////////////////////////////////////////////////////////////
// Animations

#define AWA_ANI_1_OVER_1 101
#define AWA_ANI_2_OVER_2 102
#define AWA_ANI_3_OVER_3 103
#define AWA_ANI_4_OVER_4 104
#define AWA_ANI_ROT_NEXT_1 105
#define AWA_ANI_ROT_NEXT_2 106
#define AWA_ANI_CROSS_OVER_1 107

////////////////////////////////////////////////////////////////////////////////

struct AwaValve
{
  // array to store valve status, 1 = open, 0 = close
  byte Open;
  // array to store the max height of the water jet
  byte MaxWaterHeight;
  // array to store the current height of the water jet
  byte CurrentWaterHeight;
};

struct AwaSeq
{
  const char *text;
  int animation;
  int weight;
  byte applyfilter;
};

struct AwaReadyToLaunchModeSeq
{
  const char *text;
  int mode;
};

class Awa
{

public:
  Awa();
  void begin(int numValves, Analyser *panalyser);

  void setAnimation(int animation, bool direction, int readyToLaunchMode);

  void runAnimation();
  boolean readyToLaunchNextPattern();
  void launchNextPattern();
  byte getSpectrumBufferMax(int valveno);
  void loadNextPattern();
  void advancePatternStep(uint8_t steps);
  void OneOverOne();
  void TwoOverTwo();
  void ThreeOverThree();
  void FourOverFour();
  void RotNextOne();
  void RotNextTwo();
  void CrossOverOne();

  // number of valves
  int numValves;
  // array to store valve values
  AwaValve *valves;

  byte volume;
  byte eneryVar;
  byte bassBeat;
  byte volumeBeat;
  // Waterjet attack every x millis
  byte attack;
  // Waterjet decay every x millis
  byte decay;
  byte cycleMillis;

private:
  Analyser *_analyser;
  uint8_t patternStep = 0;
  unsigned long cycleStartTime;
  int _animation;
  uint8_t _direction;
  int _readyToLaunchMode;

  CircularBuffer<byte, 13> nextPattern;
};

#endif
