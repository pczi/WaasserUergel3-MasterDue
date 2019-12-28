#ifndef AlaLedRgb_h
#define AlaLedRgb_h

#include "Ala.h"

/**
    AlaLedRgb can be used to drive a single or multiple RGB leds to perform animations.
    Available drivers are PWM pin, TLC5940, WS2811.
*/
class AlaLedRgb
{

public:
  AlaLedRgb();

  /**
      Initializes WS2812 LEDs. It be invoked in the setup() function of the main Arduino sketch.

      The type field can be used to set the RGB order and chipset frequency. Constants are ExtNeoPixel.h file.
      It is set by default to NEO_GRB + NEO_KHZ800.
    */
  void initWS2812(int numLeds);

  /**
      Sets the maximum brightness level.
    */
  void setBrightness(AlaColor maxOut);

  /**
      Sets the maximum refresh rate in Hz (default value is 50 Hz).
      May be useful to reduce flickering in some cases.
    */
  void setRefreshRate(int refreshRate);

  int getCurrentRefreshRate();

  void setAnimation(int animation, unsigned long speed, AlaColor color);
  void setAnimation(int animation, unsigned long speed, AlaPalette palette);
  void setSpeed(unsigned long speed);
  void setColor(AlaColor color);
  int getAnimation();

  bool runAnimation();

  int numLeds;    // number of leds
  AlaColor *leds; // array to store leds brightness values
  AlaColor maxOut;

  byte volume;
  byte eneryVar;
  byte beat;

private:
  void setAnimationFunc(int animation);

  void on();
  void off();
  void blink();
  void blinkAlt();
  void sparkle();
  void sparkle2();
  void strobo();
  void cycleColors();

  void pixelShiftRight();
  void pixelShiftLeft();
  void pixelBounce();
  void pixelSmoothShiftRight();
  void pixelSmoothShiftLeft();
  void comet();
  void cometCol();
  void pixelSmoothBounce();
  void larsonScanner();
  void larsonScanner2();

  void fadeIn();
  void fadeOut();
  void fadeInOut();
  void glow();
  void plasma();
  void fadeColors();
  void pixelsFadeColors();
  void fadeColorsLoop();

  void movingBars();
  void movingGradient();

  void fire();

  byte driver; // type of led driver: ALA_PWM, ALA_TLC5940
  byte *pins;  // pins where the leds are attached to

  int animation;
  unsigned long speed;
  AlaPalette palette;
  AlaSeq *animSeq;
  long animSeqDuration;
  unsigned long animStartTime;

  void (AlaLedRgb::*animFunc)();
  int refreshMillis;
  int refreshRate; // current refresh rate
  unsigned long animSeqStartTime;
  unsigned long lastRefreshTime;

  float *pxPos;
  float *pxSpeed;
};

#endif
