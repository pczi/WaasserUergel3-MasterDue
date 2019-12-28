/*
  WaterAnimator.h - Library for animating waterjets by sound
  Created by Peter Czibula, February 18, 2019.
*/
#include "WaterAnimator.h"

AwaSeq waterAnimationSeq[] =
    {
        // definition of the animations that can be used:
        // string print_text, int animation, int weight, byte applyfilter;
        {"1_OVER_1        ", AWA_ANI_1_OVER_1, 1, 1},
        {"2_OVER_2        ", AWA_ANI_2_OVER_2, 1, 1},
        {"3_OVER_3        ", AWA_ANI_3_OVER_3, 1, 1},
        {"4_OVER_4        ", AWA_ANI_4_OVER_4, 1, 1},
        {"ROT_NEXT_1      ", AWA_ANI_ROT_NEXT_1, 1, 1},
        {"ROT_NEXT_2      ", AWA_ANI_ROT_NEXT_2, 1, 1},
        {"CROSS_OVER_1    ", AWA_ANI_CROSS_OVER_1, 1, 1}};

AwaReadyToLaunchModeSeq readyToLaunchModeSeq[] =
    {
        {"Continous      ", AWA_RTL_CONTINUOUS},
        {"BassBeat       ", AWA_RTL_BASSBEAT},
        {"VolumeBeat     ", AWA_RTL_VOLUMEBEAT},
        {"BassVolumeBeat ", AWA_RTL_BASSVOLUMEBEAT}};

WaterAnimator::WaterAnimator()
{
  // constructor
  Entropy.initialize();
  // set default values
  WaterjetAttack = 40;
  WaterjetDecay = 20;
  WaterjetcycleMillis = 50;
  return;
}

void WaterAnimator::begin(int waterjetcount, Analyser *panalyser)
{
  _analyser = panalyser;
  WaterjetCount = waterjetcount;
  avaValves.begin(waterjetcount, panalyser);
}

AwaValve WaterAnimator::getValve(int Address)
{
  return avaValves.valves[Address];
}

void WaterAnimator::computeWaterAnimation()
{
  static int current_animation = -1;
  static int next_animation = 0;
  static unsigned long animation_end = 0;
  static int current_direction = AWA_DIR_FORWARD;
  static int current_readyToLaunchMode = AWA_RTL_CONTINUOUS;
  static int animationSeqCount = sizeof(waterAnimationSeq) / sizeof(waterAnimationSeq[0]);
  static int readyToLaunchModeSeqCount = sizeof(readyToLaunchModeSeq) / sizeof(readyToLaunchModeSeq[0]);

#if water_animation_mode == -1
  // automatic animation switching
  if (millis() > animation_end)
  {
    // choose the next animation
    next_animation = Entropy.random(animationSeqCount);
    animation_end = millis() + water_animation_duration;
  }
#else
  // fixed animation
  next_animation = (int)water_animation_mode;
#endif

  if (current_animation != next_animation)
  {

    if (Entropy.randomByte() > 128)
    {
      current_direction = AWA_DIR_FORWARD;
    }
    else
    {
      current_direction = AWA_DIR_REVERSE;
    }
    current_readyToLaunchMode = Entropy.random(readyToLaunchModeSeqCount);
    avaValves.setAnimation(waterAnimationSeq[next_animation].animation,
                           current_direction,
                           readyToLaunchModeSeq[current_readyToLaunchMode].mode);

#if verbose_level > -1
    // verbose_level = 0 or 1
    Serial.print(next_animation);
    Serial.print("/");
    Serial.print(animationSeqCount);
    Serial.print(" ");
    Serial.print(waterAnimationSeq[next_animation].text);
    Serial.print(" + ");
    Serial.print(current_readyToLaunchMode);
    Serial.print("/");
    Serial.print(readyToLaunchModeSeqCount);
    Serial.print(" ");
    Serial.print(readyToLaunchModeSeq[current_readyToLaunchMode].text);
    Serial.print(" + ");
    if (current_direction == AWA_DIR_FORWARD)
    {
      Serial.print("FWD");
    }
    else
    {
      Serial.print("REV");
    }
    Serial.println();
#endif
    current_animation = next_animation;
  }

  avaValves.volume = _analyser->Volume;
  avaValves.eneryVar = _analyser->EnergyVar;
  avaValves.bassBeat = _analyser->EnergyBeatDetected;
  avaValves.volumeBeat = _analyser->VolumeBeatDetected;
  avaValves.attack = WaterjetAttack;
  avaValves.decay = WaterjetDecay;
  avaValves.cycleMillis = WaterjetcycleMillis;

  avaValves.runAnimation();
}
