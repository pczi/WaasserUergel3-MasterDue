/*
  DisplayNeoMatrix.h - Library for animating sounds/lights on a NeoMatrix
  Created by Peter Czibula, February 18, 2019.
*/

#include "DisplayNeoMatrix.h"

DisplayNeoMatrix::DisplayNeoMatrix()
{
}

/************* Initialize the Spectrum Shield ****************/

void DisplayNeoMatrix::begin(Adafruit_NeoMatrix *pmatrix, Analyser *panalyser, LightAnimator *plight, WaterAnimator *pwater)
{
  _matrix = pmatrix;
  _analyser = panalyser;
  _light = plight;
  _water = pwater;
  // Easier to define all colors needed here.
  RED = _matrix->Color(GammaColor(255, true), 0, 0);
  ORANGE = _matrix->Color(GammaColor(241, true), GammaColor(90, true), GammaColor(41, true));
  GREEN = _matrix->Color(0, GammaColor(255, true), 0);
  YELLOW = _matrix->Color(GammaColor(255, true), GammaColor(242, true), 0);
  BLACK = _matrix->Color(0, 0, 0);
  WHITE = _matrix->Color(GammaColor(255, true), GammaColor(255, true), GammaColor(255, true));

  _matrix->begin();
  // set overall brightness
  _matrix->setBrightness(100);
}

/*********** clear the matrix ***********************/

void DisplayNeoMatrix::clearScreen()
{
  _matrix->fillScreen(BLACK);
}

/*********** draw the VU meter on the matrix ***********************/

void DisplayNeoMatrix::drawVUmeter()
{
  // draw lights as ledpixels
  // this is not really so smart...
  for (int i = 0; i < _light->LightCount; i++)
  {
    _matrix->drawPixel(7, i, _matrix->Color(GammaColor(_light->getLightRgb(i).r, false), GammaColor(_light->getLightRgb(i).g, false), GammaColor(_light->getLightRgb(i).b, false)));
  }
  // draw waterjets as vumeter
  for (int i = 0; i < _water->WaterjetCount; i++)
  {
    byte band = i >> 1;
    if ((i % 2) == 0)
    {
      this->drawVUmeterBar(i, (_analyser->Spectrum[band]) >> 3);
    }
    else
    {
      this->drawVUmeterBar(i, (_analyser->Spectrum[band] + _analyser->Spectrum[band + 1]) >> 4);
    }
  }
}

void DisplayNeoMatrix::drawVUmeterBar(byte Band, byte BandLevel)
{
  uint32_t LevelColor;
  // draw the bar
  if (BandLevel > 6)
  {
    BandLevel = 6;
  }
  for (byte i = 0; i < BandLevel; i++)
  {
    if (i > 4)
    {
      LevelColor = RED;
    }
    else
    {
      if (i > 2)
      {
        LevelColor = ORANGE;
      }
      else
      {
        LevelColor = GREEN;
      }
    }
    _matrix->drawPixel(i, Band, LevelColor);
  }
}

/*********** draw the Waterjets on the matrix ***********************/

void DisplayNeoMatrix::drawWaterjets()
{
  // draw lights as ledpixels
  // this is not really so smart...
  for (int x = 0; x < _light->LightCount; x++)
  {
    uint16_t brightPixelColor = _matrix->Color(GammaColor(_light->getLightRgb(x).r, false), GammaColor(_light->getLightRgb(x).g, false), GammaColor(_light->getLightRgb(x).b, false));
    _matrix->drawPixel(0, x, brightPixelColor);

    uint16_t dimPixelColor = _matrix->Color(GammaColor(_light->getLightRgb(x).r, true), GammaColor(_light->getLightRgb(x).g, true), GammaColor(_light->getLightRgb(x).b, true));
    for (int y = 1; y <= 7; y++)
    {
      _matrix->drawPixel(y, x, dimPixelColor);
    }
  }
  // draw waterjets as waterjet
  for (int i = 0; i < _water->WaterjetCount; i++)
  {
    this->drawWaterjetBar(i, _water->getValve(i).CurrentWaterHeight >> 5);
  }
}

void DisplayNeoMatrix::drawWaterjetBar(byte Band, byte BandLevel)
{
  // draw the bar
  if (BandLevel > 6)
  {
    BandLevel = 6;
  }
  if (BandLevel != 0)
  {
    _matrix->drawPixel(BandLevel, Band, WHITE);
  }
  _matrix->drawPixel(min(BandLevel + 1, 7), Band, WHITE);
}

void DisplayNeoMatrix::drawVolume()
{
  this->drawBar(15, _analyser->Volume >> 4);
  _matrix->drawPixel(_analyser->MaxVolume >> 4, 15, WHITE);
}

void DisplayNeoMatrix::drawEnergyVar()
{
  this->drawBar(14, _analyser->EnergyVar >> 5);
}

void DisplayNeoMatrix::drawBar(byte Band, byte BandLevel)
{
  if (BandLevel > 5)
  {
    BandLevel = 5;
  }
  for (byte i = 0; i < BandLevel; i++)
  {
    _matrix->drawPixel(i, Band, WHITE);
  }
}

void DisplayNeoMatrix::drawVolumeBeatDetected()
{
  const int period = 10;
  static unsigned long time_start = 0;
  static bool status_on = false;

  if (_analyser->VolumeBeatDetected)
  {
    // start cycle
    time_start = millis();
    status_on = true;
  }

  if (millis() > time_start + period)
  {
    status_on = false;
  }

  if (status_on)
  {
    _matrix->drawPixel(6, 15, YELLOW);
    _matrix->drawPixel(7, 15, YELLOW);
  }
}

void DisplayNeoMatrix::drawEnergyBeatDetected()
{
  const int period = 10;
  static unsigned long time_start = 0;
  static bool status_on = false;

  if (_analyser->EnergyBeatDetected)
  {
    // start cycle
    time_start = millis();
    status_on = true;
  }

  if (millis() > time_start + period)
  {
    status_on = false;
  }

  if (status_on)
  {
    _matrix->drawPixel(6, 14, YELLOW);
    _matrix->drawPixel(7, 14, YELLOW);
  }
}

void DisplayNeoMatrix::showScreen()
{
  _matrix->show();
}

uint8_t DisplayNeoMatrix::GammaColor(uint8_t color, bool dimmed)
{
  if (dimmed)
  {
    return pgm_read_byte(&gamma8[byte(color * 0.60)]);
  }
  return pgm_read_byte(&gamma8[color]);
}