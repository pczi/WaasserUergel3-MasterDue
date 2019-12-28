/*
  Arduino Water Animator - Library for animating waterjets by sound
  Created by Peter Czibula, February 18, 2019.
*/

#include "Awa.h"
#include "Analyser.h"

Awa::Awa()
{
  // set default values
  // Waterjet attack every x millis
  attack = 40;
  // Waterjet decay every x millis
  decay = 20;
  cycleMillis = 100;
}

void Awa::begin(int numValves, Analyser *panalyser)
{
  this->numValves = numValves;
  // allocate and clear valve array
  valves = (AwaValve *)malloc(sizeof(AwaValve) * numValves);
  memset(valves, 0, sizeof(AwaValve) * numValves);

  _analyser = panalyser;
  patternStep = 0;
  nextPattern.clear();
}

void Awa::setAnimation(int animation, bool direction, int readyToLaunchMode)
{
  _animation = animation;
  _direction = direction;
  _readyToLaunchMode = readyToLaunchMode;
}

void Awa::runAnimation()
{
  if (millis() > cycleStartTime + cycleMillis || bassBeat || volumeBeat)
  {
    // - continous mode, evaluate at least every cycleMillis
    // - other modes, evaluate at least if there is a beat
    if (readyToLaunchNextPattern())
    {
      launchNextPattern();
      loadNextPattern();
    }
  }

  if (millis() < cycleStartTime + cycleMillis)
  {
    // early return because we did not reach cycle delay time
    return;
  }

#if verbose_level == 1
  Serial.print("C   ");
#endif
  for (byte i = 0; i < numValves; i++)
  {
    // v is only a pointer to the array element
    AwaValve &v = valves[i];

    if (v.Open == 1)
    {
      // the valve is open, increase waterjet height by attack value
      v.CurrentWaterHeight = min(v.CurrentWaterHeight + attack, 255);
      if (v.CurrentWaterHeight >= v.MaxWaterHeight)
      {
        // we are higher than the max to reach, close valve
        v.Open = 0;
      }
    }
    else
    {
      // the valve is closed, decrease waterjet height by decay value
      v.CurrentWaterHeight = max(v.CurrentWaterHeight - decay, 0);
    }
#if verbose_level == 1
    Serial.print("v");
    Serial.print(i);
    Serial.print("=");
    Serial.print(v.Open);
    Serial.print(".");
    Serial.print(v.CurrentWaterHeight);
    Serial.print("/");
    Serial.print(v.MaxWaterHeight);
    Serial.print(" ");
#endif
  }
#if verbose_level == 1
  Serial.println();
#endif
  cycleStartTime = millis();
}

boolean Awa::readyToLaunchNextPattern()
{
  if (_readyToLaunchMode == AWA_RTL_BASSBEAT && !bassBeat)
  {
    // we have to wait for the bassbeat to launch next pattern
    return false;
  }
  if (_readyToLaunchMode == AWA_RTL_VOLUMEBEAT && !volumeBeat)
  {
    // we have to wait for the volumebeat to launch next pattern
    return false;
  }
  if (_readyToLaunchMode == AWA_RTL_BASSVOLUMEBEAT && !(bassBeat || volumeBeat))
  {
    // we have to wait for the bassbeat or volumebeat to launch next pattern
    return false;
  }
  // we are ready to launch next pattern if for all the next pattern valves:
  // - valve is closed
  // - current waterjet height is less than half max waterjet height
  boolean readyToLaunch = true;
#if verbose_level == 1
  Serial.print("RTL ");
#endif
  for (byte i = 0; i < nextPattern.size(); i++)
  {
    // v is only a pointer to the array element
    AwaValve &v = valves[nextPattern[i]];
#if verbose_level == 1
    Serial.print("v");
    Serial.print(nextPattern[i]);
    Serial.print("=");
    Serial.print(v.Open);
    Serial.print(".");
    Serial.print(v.CurrentWaterHeight);
    Serial.print(" ");
#endif
    if (v.Open == 1 ||
        v.CurrentWaterHeight > (v.MaxWaterHeight / 2))
    {
      // valve still open or currentHeight > 50% of maxHeight
      readyToLaunch = false;
      break;
    }
  }
#if verbose_level == 1
  Serial.print(" -> ");
  Serial.print(readyToLaunch);
  Serial.println();
#endif
  return readyToLaunch;
}

void Awa::launchNextPattern()
{
#if verbose_level == 1
  Serial.print("LNP ");
#endif
  for (byte i = 0; i < nextPattern.size(); i++)
  {
    // v is only a pointer to the array element
    AwaValve &v = valves[nextPattern[i]];
    v.Open = 1;
    v.MaxWaterHeight = getSpectrumBufferMax(nextPattern[i]);
#if verbose_level == 1
    Serial.print("v");
    Serial.print(nextPattern[i]);
    Serial.print("=");
    Serial.print(v.Open);
    Serial.print(".");
    Serial.print(v.MaxWaterHeight);
    Serial.print(" ");
#endif
  }
#if verbose_level == 1
  Serial.println();
#endif
}

byte Awa::getSpectrumBufferMax(int valveno)
{
  byte band = valveno >> 1;
  if ((valveno % 2) == 0)
  {
    return _analyser->spectrumBufferMax(band);
  }
  else
  {
    return (_analyser->spectrumBufferMax(band) + _analyser->spectrumBufferMax(band + 1)) >> 1;
  }
}

void Awa::loadNextPattern()
{
  switch (_animation)
  {
  case AWA_ANI_1_OVER_1:
    OneOverOne();
    break;
  case AWA_ANI_2_OVER_2:
    TwoOverTwo();
    break;
  case AWA_ANI_3_OVER_3:
    ThreeOverThree();
    break;
  case AWA_ANI_4_OVER_4:
    FourOverFour();
    break;
  case AWA_ANI_ROT_NEXT_1:
    RotNextOne();
    break;
  case AWA_ANI_ROT_NEXT_2:
    RotNextTwo();
    break;
  case AWA_ANI_CROSS_OVER_1:
    CrossOverOne();
    break;
  }
}

void Awa::advancePatternStep(uint8_t stepsCount)
{
  // advance to next step
  if (_direction == AWA_DIR_FORWARD)
  {
    patternStep++;
    if (patternStep >= stepsCount)
    {
      // overflow, we go to the start
      patternStep = 0;
    }
  }
  else
  {
    patternStep--;
    if (patternStep >= stepsCount)
    {
      // overflow, we go to the end
      patternStep = stepsCount - 1;
    }
  }
}

void Awa::OneOverOne()
{
  const uint8_t steps = 2;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, -1, 2, -1, 4, -1, 6, -1, 8, -1, 10, -1, 12},
      {-1, 1, -1, 3, -1, 5, -1, 7, -1, 9, -1, 11, -1}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}

void Awa::TwoOverTwo()
{
  const uint8_t steps = 2;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, 1, -1, -1, 4, 5, -1, 7, 8, -1, -1, 11, 12},
      {-1, -1, 2, 3, -1, -1, 6, -1, -1, 9, 10, -1, -1}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}

void Awa::ThreeOverThree()
{
  const uint8_t steps = 2;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, 1, 2, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12},
      {-1, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}

void Awa::FourOverFour()
{
  const uint8_t steps = 2;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, 1, 2, 3, -1, -1, -1, -1, -1, 9, 10, 11, 12},
      {-1, -1, -1, -1, 4, 5, 6, 7, 8, -1, -1, -1, -1}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}

void Awa::RotNextOne()
{
  const uint8_t steps = 13;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, 5, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, 7, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, 8, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, 9, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}

void Awa::RotNextTwo()
{
  const uint8_t steps = 13;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, 3, -1, -1, 6, 7, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, 8, 9, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, -1},
      {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12},
      {-1, 1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, 5, 6, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, 7, 8, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, 9, 10, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, 12}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}

void Awa::CrossOverOne()
{
  const uint8_t steps = 13;
  const uint8_t steppos = 13;
  const int jetPattern[steps][steppos] = {
      {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12},
      {-1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1},
      {-1, -1, 2, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1},
      {-1, -1, -1, 3, -1, -1, -1, -1, -1, 9, -1, -1, -1},
      {-1, -1, -1, -1, 4, -1, -1, -1, 8, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, 5, -1, 7, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, 5, -1, 7, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, 4, -1, -1, -1, 8, -1, -1, -1, -1},
      {-1, -1, -1, 3, -1, -1, -1, -1, -1, 9, -1, -1, -1},
      {-1, -1, 2, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1},
      {-1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1},
      {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12}};
  // copy the valves to the next pattern buffer
  nextPattern.clear();
  advancePatternStep(steps);
  for (uint8_t i = 0; i < steppos; i++)
  {
    if (jetPattern[patternStep][i] != -1)
    {
      nextPattern.push(jetPattern[patternStep][i]);
    }
  }
}
