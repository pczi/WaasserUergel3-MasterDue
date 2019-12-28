#include "Ala.h"
#include "AlaLedRgb.h"

AlaLedRgb::AlaLedRgb()
{
  // set default values

  maxOut = 0xFFFFFF;
  speed = 1000;
  lastRefreshTime = 0;
  refreshMillis = 1000 / 50;
  pxPos = NULL;
  pxSpeed = NULL;
}

void AlaLedRgb::initWS2812(int numLeds)
{
  this->driver = ALA_WS2812;
  this->numLeds = numLeds;
  this->pins = 0;

  // allocate and clear leds array
  leds = (AlaColor *)malloc(3 * numLeds);
  memset(leds, 0, 3 * numLeds);
}

void AlaLedRgb::setBrightness(AlaColor maxOut)
{
  this->maxOut = maxOut;
}

void AlaLedRgb::setRefreshRate(int refreshRate)
{
  this->refreshMillis = 1000 / refreshRate;
}

int AlaLedRgb::getCurrentRefreshRate()
{
  return refreshRate;
}

void AlaLedRgb::setAnimation(int animation, unsigned long speed, AlaColor color)
{
  // is there any change?
  if (this->animation == animation && this->speed == speed && this->palette.numColors == 1 && this->palette.colors[0] == color)
    return;

  // delete any previously allocated array
  if (pxPos != NULL)
  {
    delete[] pxPos;
    pxPos = NULL;
  }
  if (pxSpeed != NULL)
  {
    delete[] pxSpeed;
    pxSpeed = NULL;
  }

  this->animation = animation;
  this->speed = speed;

  this->palette.numColors = 1;
  // TODO is this a memory leak?
  this->palette.colors = (AlaColor *)malloc(3);
  this->palette.colors[0] = color;

  setAnimationFunc(animation);
  animStartTime = millis();
}

void AlaLedRgb::setAnimation(int animation, unsigned long speed, AlaPalette palette)
{
  // is there any change?
  if (this->animation == animation && this->speed == speed && this->palette == palette)
    return;

  // delete any previously allocated array
  if (pxPos != NULL)
  {
    delete[] pxPos;
    pxPos = NULL;
  }
  if (pxSpeed != NULL)
  {
    delete[] pxSpeed;
    pxSpeed = NULL;
  }

  this->animation = animation;
  this->speed = speed;
  this->palette = palette;

  setAnimationFunc(animation);
  animStartTime = millis();
}

void AlaLedRgb::setSpeed(unsigned long speed)
{
  this->speed = speed;
  animStartTime = millis();
}

void AlaLedRgb::setColor(AlaColor color)
{
  this->palette.colors[0] = color;
}

int AlaLedRgb::getAnimation()
{
  return animation;
}

bool AlaLedRgb::runAnimation()
{
  if (animation == ALA_STOPSEQ)
    return true;

  // skip the refresh if not enough time has passed since last update
  // leave this, otherwise sparke animation etc. will be too fast
  unsigned long cTime = millis();
  if ((cTime < lastRefreshTime + refreshMillis) && !beat)
  {
    return false;
  }

  // calculate real refresh rate
  refreshRate = 1000 / (cTime - lastRefreshTime);

  lastRefreshTime = cTime;

  // run the animantion calculation
  if (animFunc != NULL)
    (this->*animFunc)();

  return true;
}

///////////////////////////////////////////////////////////////////////////////

void AlaLedRgb::setAnimationFunc(int animation)
{

  switch (animation)
  {

  case ALA_ON:
    animFunc = &AlaLedRgb::on;
    break;
  case ALA_OFF:
    animFunc = &AlaLedRgb::off;
    break;
  case ALA_BLINK:
    animFunc = &AlaLedRgb::blink;
    break;
  case ALA_BLINKALT:
    animFunc = &AlaLedRgb::blinkAlt;
    break;
  case ALA_SPARKLE:
    animFunc = &AlaLedRgb::sparkle;
    break;
  case ALA_SPARKLE2:
    animFunc = &AlaLedRgb::sparkle2;
    break;
  case ALA_STROBO:
    animFunc = &AlaLedRgb::strobo;
    break;
  case ALA_CYCLECOLORS:
    animFunc = &AlaLedRgb::cycleColors;
    break;

  case ALA_PIXELSHIFTRIGHT:
    animFunc = &AlaLedRgb::pixelShiftRight;
    break;
  case ALA_PIXELSHIFTLEFT:
    animFunc = &AlaLedRgb::pixelShiftLeft;
    break;
  case ALA_PIXELBOUNCE:
    animFunc = &AlaLedRgb::pixelBounce;
    break;
  case ALA_PIXELSMOOTHSHIFTRIGHT:
    animFunc = &AlaLedRgb::pixelSmoothShiftRight;
    break;
  case ALA_PIXELSMOOTHSHIFTLEFT:
    animFunc = &AlaLedRgb::pixelSmoothShiftLeft;
    break;
  case ALA_PIXELSMOOTHBOUNCE:
    animFunc = &AlaLedRgb::pixelSmoothBounce;
    break;
  case ALA_COMET:
    animFunc = &AlaLedRgb::comet;
    break;
  case ALA_COMETCOL:
    animFunc = &AlaLedRgb::cometCol;
    break;
  case ALA_MOVINGBARS:
    animFunc = &AlaLedRgb::movingBars;
    break;
  case ALA_MOVINGGRADIENT:
    animFunc = &AlaLedRgb::movingGradient;
    break;
  case ALA_LARSONSCANNER:
    animFunc = &AlaLedRgb::larsonScanner;
    break;
  case ALA_LARSONSCANNER2:
    animFunc = &AlaLedRgb::larsonScanner2;
    break;

  case ALA_FADEIN:
    animFunc = &AlaLedRgb::fadeIn;
    break;
  case ALA_FADEOUT:
    animFunc = &AlaLedRgb::fadeOut;
    break;
  case ALA_FADEINOUT:
    animFunc = &AlaLedRgb::fadeInOut;
    break;
  case ALA_GLOW:
    animFunc = &AlaLedRgb::glow;
    break;
  case ALA_PLASMA:
    animFunc = &AlaLedRgb::plasma;
    break;
  case ALA_PIXELSFADECOLORS:
    animFunc = &AlaLedRgb::pixelsFadeColors;
    break;
  case ALA_FADECOLORS:
    animFunc = &AlaLedRgb::fadeColors;
    break;
  case ALA_FADECOLORSLOOP:
    animFunc = &AlaLedRgb::fadeColorsLoop;
    break;

  case ALA_FIRE:
    animFunc = &AlaLedRgb::fire;
    break;

  default:
    animFunc = &AlaLedRgb::off;
  }
}

void AlaLedRgb::on()
{
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = palette.colors[0];
  }
}

void AlaLedRgb::off()
{
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = 0x000000;
  }
}

void AlaLedRgb::blink()
{
  int t = getNextStepOnBeat(animStartTime, speed, 2, beat);
  int k = (t + 1) % 2;
  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = palette.colors[0].scale(k);
  }
}

void AlaLedRgb::blinkAlt()
{
  int t = getNextStepOnBeat(animStartTime, speed, 2, beat);

  for (int x = 0; x < numLeds; x++)
  {
    int k = (t + x) % 2;
    leds[x] = palette.colors[0].scale(k);
  }
}

void AlaLedRgb::sparkle()
{
  int p;
  if (this->beat)
  {
    p = 0;
  }
  else
  {
    p = speed / 100;
  }

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = palette.colors[random(palette.numColors)].scale(random(p) == 0);
  }
}

void AlaLedRgb::sparkle2()
{
  int p;
  if (this->beat)
  {
    p = 0;
  }
  else
  {
    p = speed / 10;
  }

  for (int x = 0; x < numLeds; x++)
  {
    if (random(p) == 0)
      leds[x] = palette.colors[random(palette.numColors)];
    else
      leds[x] = leds[x].scale(0.88);
  }
}

void AlaLedRgb::strobo()
{
  byte step_count = speed / 10;
  int t = getFirstStepOnBeat(animStartTime, speed, step_count, beat);

  AlaColor c = palette.colors[0].scale(t == 0);
  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = c;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Shifting effects
////////////////////////////////////////////////////////////////////////////////////////////

void AlaLedRgb::pixelShiftRight()
{
  // adjust speed
  int halfNumLeds = numLeds / 2;
  int t = getFirstStepSetSpeedOnBeat(animStartTime, speed, halfNumLeds, beat, numLeds * 2, numLeds * 50);

  float tx = getStepFloat(animStartTime, speed, palette.numColors);
  AlaColor c = palette.getPalColor(tx);

  for (int x = 0; x <= halfNumLeds; x++)
  {
    int k = (x == (t + 1) ? 1 : 0);
    leds[x - 1] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::pixelShiftLeft()
{
  // adjust speed
  int halfNumLeds = (numLeds / 2) + 1;
  int t = getFirstStepSetSpeedOnBeat(animStartTime, speed, halfNumLeds, beat, numLeds * 2, numLeds * 50);

  float tx = getStepFloat(animStartTime, speed, palette.numColors);
  AlaColor c = palette.getPalColor(tx);

  for (int x = 0; x <= halfNumLeds; x++)
  {
    int k = ((x == (halfNumLeds - t) ? 1 : 0));
    leds[x - 1] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

// Bounce back and forth
void AlaLedRgb::pixelBounce()
{
  // adjust speed
  int halfNumLeds = (numLeds / 2) + 1;
  int t = getFirstStepSetSpeedOnBeat(animStartTime, speed, 2 * halfNumLeds - 2, beat, numLeds * 10, numLeds * 50);

  float tx = getStepFloat(animStartTime, speed, palette.numColors);
  AlaColor c = palette.getPalColor(tx);

  for (int x = 0; x <= halfNumLeds; x++)
  {
    int k = x == (-abs(t - halfNumLeds + 2) + halfNumLeds) ? 1 : 0;
    leds[x - 1] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::pixelSmoothShiftRight()
{
  // adjust speed
  int halfNumLeds = numLeds / 2;
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, halfNumLeds + 1, beat, numLeds * 10, numLeds * 50);

  float tx = getStepFloat(animStartTime, speed, palette.numColors);
  AlaColor c = palette.getPalColor(tx);

  for (int x = 0; x <= halfNumLeds; x++)
  {
    float k = max(0, (-abs(t - x) + 1));
    leds[x] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::pixelSmoothShiftLeft()
{
  // adjust speed
  int halfNumLeds = numLeds / 2;
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, halfNumLeds + 1, beat, numLeds * 10, numLeds * 50);
  float tx = getStepFloat(animStartTime, speed, palette.numColors);
  AlaColor c = palette.getPalColor(tx);

  for (int x = 0; x <= halfNumLeds; x++)
  {
    float k = max(0, (-abs(halfNumLeds - t - x) + 1));
    leds[x] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::comet()
{
  // adjust speed
  int halfNumLeds = (numLeds / 2) + 1;
  float l = halfNumLeds / 2; // length of the tail
  // float t = getStepFloat(animStartTime, speed, 2 * halfNumLeds - l);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 2 * halfNumLeds - 1, beat, numLeds * 10, numLeds * 50);
  float tx = getStepFloat(animStartTime, speed, palette.numColors);
  AlaColor c = palette.getPalColor(tx);

  for (int x = 0; x <= halfNumLeds; x++)
  {
    float k = constrain((((x - t) / l + 1.2f)) * (((x - t) < 0) ? 1 : 0), 0, 1);
    leds[x - 1] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::cometCol()
{
  // adjust speed
  int halfNumLeds = numLeds / 2;
  float l = halfNumLeds / 2; // length of the tail
  // float t = getStepFloat(animStartTime, speed, 2 * numLeds - l);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 2 * halfNumLeds - 1, beat, numLeds * 10, numLeds * 50);

  AlaColor c;
  for (int x = 0; x <= halfNumLeds; x++)
  {
    float tx = mapfloat(max(t - x, 0), 0, halfNumLeds / 1.7, 0, palette.numColors - 1);
    c = palette.getPalColor(tx);
    float k = constrain((((x - t) / l + 1.2f)) * (((x - t) < 0) ? 1 : 0), 0, 1);
    leds[x] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::pixelSmoothBounce()
{
  // adjust speed
  int halfNumLeds = (numLeds / 2) + 1;
  // see larsonScanner
  // float t = getStepFloat(animStartTime, speed, 2 * numLeds - 2);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 2 * halfNumLeds - 1, beat, numLeds * 10, numLeds * 50);

  AlaColor c = palette.getPalColor(getStepFloat(animStartTime, speed, palette.numColors));

  for (int x = 0; x <= halfNumLeds; x++)
  {
    float k = constrain((-abs(abs(t - halfNumLeds + 2) - x) + 1), 0, 1);
    leds[x] = c.scale(k);
    leds[numLeds - x] = c.scale(k);
  }
}

void AlaLedRgb::larsonScanner()
{
  // adjust speed
  float l = numLeds / 4;
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 2 * numLeds - 2, beat, numLeds * 10, numLeds * 100);
  AlaColor c = palette.getPalColor(getStepFloat(animStartTime, speed, palette.numColors));

  for (int x = 0; x < numLeds; x++)
  {
    float k = constrain((-abs(abs(t - numLeds + 1) - x) + l), 0, 1);
    leds[x] = c.scale(k);
  }
}

void AlaLedRgb::larsonScanner2()
{
  // adjust speed
  float l = numLeds / 4; // 2>7, 3-11, 4-14
  // float t = getStepFloat(animStartTime, speed, 2 * numLeds + (l * 4 - 1));
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 2 * numLeds + (l * 4 - 1), beat, numLeds * 10, numLeds * 100);
  AlaColor c = palette.getPalColor(getStepFloat(animStartTime, speed, palette.numColors));

  for (int x = 0; x < numLeds; x++)
  {

    float k = constrain((-abs(abs(t - numLeds - 2 * l) - x - l) + l), 0, 1);
    leds[x] = c.scale(k);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Fading effects
////////////////////////////////////////////////////////////////////////////////////////////

void AlaLedRgb::fadeIn()
{
  // adjust speed
  // float s = getStepFloat(animStartTime, speed, 1);
  float s = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 1, beat, 10, 1000);
  AlaColor c = palette.colors[0].scale(s);

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = c;
  }
}

void AlaLedRgb::fadeOut()
{
  // adjust speed
  // float s = getStepFloat(animStartTime, speed, 1);
  float s = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 1, beat, 10, 1000);
  AlaColor c = palette.colors[0].scale(1 - s);

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = c;
  }
}

void AlaLedRgb::fadeInOut()
{
  // adjust speed
  // float s = getStepFloat(animStartTime, speed, 2) - 1;
  float s = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, 1, beat, 10, 1000) - 1;
  AlaColor c = palette.colors[0].scale(abs(1 - abs(s)));

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = c;
  }
}

void AlaLedRgb::glow()
{
  // adjust speed
  // float s = getStepFloat(animStartTime, speed, TWO_PI);
  float s = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, TWO_PI, beat, 10, 1000);
  float k = (-cos(s) + 1) / 2;

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = palette.colors[0].scale(k);
  }
}

void AlaLedRgb::plasma()
{
  // adjust speed
  // float t = getStepFloat(animStartTime, speed, numLeds);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, numLeds, beat, 10, 1000);

  for (int x = 0; x < numLeds; x++)
  {
    AlaColor c1 = palette.getPalColor((float)((x + t) * palette.numColors) / numLeds);
    AlaColor c2 = palette.getPalColor((float)((2 * x - t + numLeds) * palette.numColors) / numLeds);
    leds[x] = c1.interpolate(c2, 0.5);
  }
}

void AlaLedRgb::fadeColors()
{
  // adjust speed
  // float t = getStepFloat(animStartTime, speed, palette.numColors - 1);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, palette.numColors - 1, beat, 10, 1000);
  AlaColor c = palette.getPalColor(t);
  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = c;
  }
}

void AlaLedRgb::pixelsFadeColors()
{
  // adjust speed
  // float t = getStepFloat(animStartTime, speed, palette.numColors);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, palette.numColors, beat, 10, 1000);

  for (int x = 0; x < numLeds; x++)
  {
    AlaColor c = palette.getPalColor(t + 7 * x);
    leds[x] = c;
  }
}

void AlaLedRgb::fadeColorsLoop()
{
  // adjust speed
  // float t = getStepFloat(animStartTime, speed, palette.numColors);
  float t = getFirstStepSetSpeedOnBeatFloat(animStartTime, speed, palette.numColors, beat, 10, 1000);
  AlaColor c = palette.getPalColor(t);
  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = c;
  }
}

void AlaLedRgb::cycleColors()
{
  // adjust speed
  // int t = getStep(animStartTime, speed, palette.numColors);
  int t = getNextStepOnBeat(animStartTime, speed, palette.numColors, beat);

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = palette.colors[t];
  }
}

void AlaLedRgb::movingBars()
{
  // adjust speed
  // int t = getStep(animStartTime, speed, numLeds);
  int t = getNextStepOnBeat(animStartTime, speed, numLeds, beat);

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = palette.colors[(((t + x) * palette.numColors) / numLeds) % palette.numColors];
  }
}

void AlaLedRgb::movingGradient()
{
  // adjust speed
  float t = getNextStepOnBeatFloat(animStartTime, speed, numLeds, beat, 2);

  for (int x = 0; x < numLeds; x++)
  {
    leds[x] = palette.getPalColor((float)((x + t) * palette.numColors) / numLeds);
  }
}

/*******************************************************************************
  FIRE
  Porting of the famous Fire2012 effect by Mark Kriegsman.
  Actually works at 50 Hz frame rate with up to 50 pixels.
*******************************************************************************/
void AlaLedRgb::fire()
{
  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 550
#define COOLING 600

  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  // Default 120, suggested range 50-200.
#define SPARKING 120

  // Array of temperature readings at each simulation cell
  static byte *heat = NULL;

  static int halfNumLeds = (numLeds / 2) + 1;

  // Initialize array if necessary
  if (heat == NULL)
    heat = new byte[halfNumLeds];

  // Step 1.  Cool down every cell a little
  int rMax = (COOLING / halfNumLeds) + 2;
  for (int i = 0; i < halfNumLeds; i++)
  {
    heat[i] = max(((int)heat[i]) - random(0, rMax), 0);
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = halfNumLeds - 1; k >= 3; k--)
  {
    heat[k] = ((int)heat[k - 1] + (int)heat[k - 2] + (int)heat[k - 3]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (beat)
  //if (random(255) < SPARKING)
  {
    int y = random(7);
    heat[y] = min(heat[y] + random(160, 255), 255);
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < halfNumLeds; j++)
  {
    float colorindex = (float)(heat[j] * (palette.numColors - 1)) / 256;
    leds[j] = palette.getPalColor(colorindex);
    leds[numLeds - j] = palette.getPalColor(colorindex);
  }
}
