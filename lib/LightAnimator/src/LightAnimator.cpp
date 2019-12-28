/*
  LightAnimator.h - Library for animating lights by sound
  Created by Peter Czibula, February 18, 2019.
*/
#include "LightAnimator.h"

AlaColorSeq colorSeq[] =
    {
        {"Aqua   ", AlaColor::Aqua},
        {"Blue   ", AlaColor::Blue},
        {"Cyan   ", AlaColor::Cyan},
        {"Gold   ", AlaColor::Gold},
        {"Gray   ", AlaColor::Gray},
        {"Green  ", AlaColor::Green},
        {"Lime   ", AlaColor::Lime},
        {"Magenta", AlaColor::Magenta},
        {"Navy   ", AlaColor::Navy},
        {"Olive  ", AlaColor::Olive},
        {"Purple ", AlaColor::Purple},
        {"Red    ", AlaColor::Red},
        {"Teal   ", AlaColor::Teal},
        {"White  ", AlaColor::White},
        {"Yellow ", AlaColor::Yellow}};

AlaPaletteSeq paletteSeq[] =
    {
        {"Rgb          ", alaPalRgb},
        {"Rainbow      ", alaPalRainbow},
        {"RainbowStripe", alaPalRainbowStripe},
        {"Party        ", alaPalParty},
        {"Heat         ", alaPalHeat},
        {"Fire         ", alaPalFire},
        {"Cool         ", alaPalCool}};

AlaSeq lightAnimationSeq[] =
    {
        // definition of the animations that can be used:
        // string print_text, int animation, long cyclelength, int weight, byte applyfilter, byte Col_Pal;
        {"ALA_BLINK                ", ALA_BLINK, 1000, 1, 1, COL},
        {"ALA_BLINKALT             ", ALA_BLINKALT, 1000, 1, 1, COL},
        {"ALA_STROBO               ", ALA_STROBO, 1000, 1, 1, COL},
        //  {"ALA_FADEIN               ", ALA_FADEIN, 1000, 1, 1, COL },
        {"ALA_FADEOUT              ", ALA_FADEOUT, 1000, 1, 1, COL},
        //  {"ALA_FADEINOUT            ", ALA_FADEINOUT, 1000, 1, 1, COL },
        //  {"ALA_GLOW                 ", ALA_GLOW, 1000, 1, 1, COL },

        {"ALA_SPARKLE              ", ALA_SPARKLE, 1000, 1, 1, PAL},
        {"ALA_SPARKLE2             ", ALA_SPARKLE2, 1000, 1, 1, PAL},
        {"ALA_PIXELSHIFTRIGHT      ", ALA_PIXELSHIFTRIGHT, 1000, 1, 1, PAL},
        {"ALA_PIXELSHIFTLEFT       ", ALA_PIXELSHIFTLEFT, 1000, 1, 1, PAL},
        {"ALA_PIXELBOUNCE          ", ALA_PIXELBOUNCE, 1000, 1, 1, PAL},
        {"ALA_PIXELSMOOTHSHIFTRIGHT", ALA_PIXELSMOOTHSHIFTRIGHT, 1000, 1, 1, PAL},
        {"ALA_PIXELSMOOTHSHIFTLEFT ", ALA_PIXELSMOOTHSHIFTLEFT, 1000, 1, 1, PAL},
        {"ALA_COMET                ", ALA_COMET, 1000, 1, 1, PAL},
        {"ALA_COMETCOL             ", ALA_COMETCOL, 1000, 1, 1, PAL},
        {"ALA_PIXELSMOOTHBOUNCE    ", ALA_PIXELSMOOTHBOUNCE, 1000, 1, 1, PAL},
        {"ALA_LARSONSCANNER        ", ALA_LARSONSCANNER, 1000, 1, 1, PAL},
        {"ALA_LARSONSCANNER2       ", ALA_LARSONSCANNER2, 1000, 1, 1, PAL},
        {"ALA_PLASMA               ", ALA_PLASMA, 1000, 1, 1, PAL},
        {"ALA_FADECOLORS           ", ALA_FADECOLORS, 1000, 1, 1, PAL},
        {"ALA_PIXELSFADECOLORS     ", ALA_PIXELSFADECOLORS, 1000, 1, 1, PAL},
        {"ALA_FADECOLORSLOOP       ", ALA_FADECOLORSLOOP, 1000, 1, 1, PAL},
        {"ALA_CYCLECOLORS          ", ALA_CYCLECOLORS, 1000, 1, 1, PAL},
        {"ALA_MOVINGBARS           ", ALA_MOVINGBARS, 1000, 1, 1, PAL},
        {"ALA_MOVINGGRADIENT       ", ALA_MOVINGGRADIENT, 1000, 1, 1, PAL},
        {"ALA_FIRE                 ", ALA_FIRE, 1000, 1, 1, PAL}

};

LightAnimator::LightAnimator()
{
  Entropy.initialize();
  return;
}

void LightAnimator::begin(int lightcount, Analyser *panalyser)
{
  alaLedRgbStrip.initWS2812(lightcount);
  LightCount = lightcount;
  _analyser = panalyser;
}

AlaColor LightAnimator::getLightRgb(int Address)
{
  return alaLedRgbStrip.leds[Address];
}

void LightAnimator::computeLightAnimation()
{
  // not optimal
  // ALA_STROBO, ALA_BLINK, ALA_BLINKALT, ALA_SPARKLE, ALA_PIXELBOUNCE
  // quite good
  // ALA_SPARKLE, ALA_SPARKLE2, ALA_STROBO, ALA_PIXELSHIFTRIGHT, ALA_PIXELSHIFTLEFT, ALA_PIXELSMOOTHSHIFTRIGHT, ALA_PIXELSMOOTHSHIFTLEFT
  // ALA_COMET, ALA_COMETCOL, ALA_PIXELSMOOTHBOUNCE, ALA_LARSONSCANNER, ALA_LARSONSCANNER2
  // ALA_FADEIN, ALA_FADEOUT, ALA_FADEINOUT, ALA_GLOW, ALA_PLASMA, ALA_FADECOLORS, ALA_PIXELSFADECOLORS, ALA_FADECOLORSLOOP
  // ALA_CYCLECOLORS, ALA_MOVINGBARS, ALA_MOVINGGRADIENT
  // ALA_FIRE

  // AlaColor::Aqua, AlaColor::Black, AlaColor::Blue, AlaColor::Cyan, AlaColor::Gold, AlaColor::Gray, AlaColor::Green, AlaColor::Lime, AlaColor::Magenta,
  // AlaColor::Navy, AlaColor::Olive, AlaColor::Purple, AlaColor::Red, AlaColor::Teal, AlaColor::White, AlaColor::Yellow

  // alaPalRgb, alaPalRainbow, alaPalRainbowStripe, alaPalParty, alaPalHeat, alaPalFire, alaPalCool

  static int current_animation = -1;
  static int next_animation = 0;
  static unsigned long animation_end = 0;
  static int animationSeqCount = sizeof(lightAnimationSeq) / sizeof(lightAnimationSeq[0]);
  static int colorSeqCount = sizeof(colorSeq) / sizeof(colorSeq[0]);
  static int paletteSeqCount = sizeof(paletteSeq) / sizeof(paletteSeq[0]);

#if color_animation_mode == -1
  // automatic animation switching
  if (millis() > animation_end)
  {
    // choose the next animation
    next_animation = Entropy.random(animationSeqCount);
    animation_end = millis() + color_animation_duration;
  }
#else
  // fixed animation
  next_animation = (int)color_animation_mode;
#endif

  if (current_animation != next_animation)
  {
    if (lightAnimationSeq[next_animation].Col_Pal == COL)
    {
      // color mode, get random colors 
      int next_color = Entropy.random(colorSeqCount);
      alaLedRgbStrip.setAnimation(lightAnimationSeq[next_animation].animation, lightAnimationSeq[next_animation].cyclelength, colorSeq[next_color].color);
#if verbose_level > -1
      // verbose_level = 0 or 1
      Serial.print(next_animation);
      Serial.print("/");
      Serial.print(animationSeqCount);
      Serial.print(" ");
      Serial.print(lightAnimationSeq[next_animation].text);
      Serial.print(" + ");
      Serial.print(next_color);
      Serial.print("/");
      Serial.print(colorSeqCount);
      Serial.print(" ");
      Serial.print(colorSeq[next_color].text);
      Serial.println();
#endif
    }
    else
    {
      // palette mode, get random palette
      int next_palette = Entropy.random(paletteSeqCount);
      alaLedRgbStrip.setAnimation(lightAnimationSeq[next_animation].animation, lightAnimationSeq[next_animation].cyclelength, paletteSeq[next_palette].palette);
#if verbose_level > -1
      Serial.print(next_animation);
      Serial.print("/");
      Serial.print(animationSeqCount);
      Serial.print(" ");
      Serial.print(lightAnimationSeq[next_animation].text);
      Serial.print(" + ");
      Serial.print(next_palette);
      Serial.print("/");
      Serial.print(paletteSeqCount);
      Serial.print(" ");
      Serial.print(paletteSeq[next_palette].text);
      Serial.println();
#endif
    }
    current_animation = next_animation;
  }

  alaLedRgbStrip.volume = _analyser->Volume;
  alaLedRgbStrip.eneryVar = _analyser->EnergyVar;
  // animation based on energybeat
  alaLedRgbStrip.beat = _analyser->EnergyBeatDetected;

  // use the ALA library to compute led values
  alaLedRgbStrip.runAnimation();
  //  Brightness = map(_analyser->Volume, 0, 255, 25, 255);
  //  _matrix->setBrightness(Brightness);
  // alaLedRgbStrip.setBrightness(0x444444);
}
