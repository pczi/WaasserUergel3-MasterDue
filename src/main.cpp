
#include "def.h"
#include <Arduino.h>

#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include "Analyser.h"
#include "LightAnimator.h"
#include "WaterAnimator.h"
#include "DisplayNeoMatrix.h"

// interesting related documentation and projects:
// rlogiacco/CircularBuffer
// https://www.parallelcube.com/2018/03/30/beat-detection-algorithm/
// https://github.com/mastayb/arduino-beat-detection
// http://mziccard.me/2015/05/28/beats-detection-algorithms-1/

Analyser analyser;
DisplayNeoMatrix neoMatrix;
LightAnimator lights;
WaterAnimator waters;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 1, 2, 6,
                                               NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                                   NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE +
                                                   NEO_TILE_PROGRESSIVE,
                                               NEO_GRB + NEO_KHZ800);

void setup()
{

  // serial communication initialize
  Serial.begin(115200);

#if verbose_level == -1
  Serial.println("verbose_level = -1, use the plotter");
#else
  Serial.println("verbose_level > -1, use the monitor");
#endif

  // sound analyser setup
  analyser.begin(4, 5, A0, A1, A5, 10, 11);
  analyser.AmplificationAnalogPort = A5;
  analyser.AmplificationFactor = 1.6;
  analyser.NoiseLevel = 64;
  analyser.MaxVolumeDecay = 20;
  analyser.MaxVolumeThreshold = 0.85;
  analyser.MinVolume = 10;
  analyser.MaxBassDecay = 20;
  analyser.MaxBassThreshold = 0.75;
  analyser.MinBass = 30;

  // light animation setup with 13 led rings
  lights.begin(13, &analyser);

  // waterjet animation setup with 13 waterjets
  waters.begin(13, &analyser);
  // attack, how fast will the waterjet go up
  waters.WaterjetAttack = 40;
  // decay, how fast will the waterjet go down
  waters.WaterjetDecay = 20;

  // neopixel matrix setup
  neoMatrix.begin(&matrix, &analyser, &lights, &waters);
}

void loop()
{

  // take the current cycle start time
  unsigned long CycleStart = millis();
  int HZcount = 0;
  while (millis() - CycleStart < 1000)
  {
    // read values from our spectrum analyser
    analyser.read();

    lights.computeLightAnimation();

    waters.computeWaterAnimation();

    neoMatrix.clearScreen();
#if display_mode == 0
    neoMatrix.drawVUmeter();
#else
    neoMatrix.drawWaterjets();
#endif
    neoMatrix.drawVolume();
    neoMatrix.drawEnergyVar();
    neoMatrix.drawVolumeBeatDetected();
    neoMatrix.drawEnergyBeatDetected();
    neoMatrix.showScreen();
    HZcount++;
  }

#if verbose_level > -1
  // verbose_level 0 or 1
  Serial.print(HZcount);
  Serial.print(" hz, ");

  Serial.print(analyser.Volume);
  Serial.print(" vol, ");

  Serial.print(analyser.EnergyVar);
  Serial.print(" var, ");

  //  Serial.print(analyser.BPM);
  //  Serial.print(" bpm, ");

  Serial.print(analyser.NoiseLevel);
  Serial.print(" noise, ");

  Serial.print(neoMatrix.Brightness);
  Serial.print(" bright ");

  Serial.println();
#endif
}
