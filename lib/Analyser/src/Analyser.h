/*
  Analyser.h - Library for analysing sounds with the spectrum analyser shield.
  Created by Peter Czibula, February 18, 2019.
*/

#ifndef Analyser_h
#define Analyser_h

// define the time length of each sample block
// define the sample block history size to keep
// we should keep 1000 ms of history, 100 blocks @ 10ms or 66 blocks @15ms or 40 blocks @25ms
#define Energyblock_buffer_size 66
#define Energyblock_sample_millis 15

#define Bpm_buffer_size 64

#include "../../../include/def.h"
#include "Arduino.h"
#include <CircularBuffer.h>

class Analyser
{
  public:
    // constructor / Setup
    Analyser();
    // initialize the spectrum analyser
    void begin(byte strobe, byte reset, byte dcl, byte dcr, byte dcampfac, byte btn1, byte btn2);
    // read the 7 bands and compute all sorts of things
    void read();
    // Spectrum analyzer read values.
    byte Spectrum[7];
    CircularBuffer <byte, 5>  SpectrumBuffer[7];
    byte spectrumBufferAverage(byte Band);
    byte spectrumBufferMax(byte Band);
    // Minimum noise values, will be adjusted
    // after reading spectrum analyse values
    byte NoiseLevel = 32;
    byte AmplificationAnalogPort = A5;
    float AmplificationFactor = 1.2;
    // average BPM for the last 10 seconds
    int BPM;

    // Current variance
    byte EnergyAvg = 0;
    byte EnergyVar = 0;
    byte EnergyStdDev = 0;
    bool EnergyBeatDetected = false;

    bool ButtonPressed1 = false;
    bool ButtonPressed2 = false;

    // Current volume value (sum of all bands / 6)
    byte Volume;
    // Peak value for 1000 millis
    byte MaxVolume;
    // Peak value decay evey 1000 millis
    byte MaxVolumeDecay = 20;
    // detect beat when volumes is bigger than 75% of MaxVolume
    float MaxVolumeThreshold = 0.75;
    byte MinVolume = 40;
    bool VolumeBeatDetected = false;

    byte Bass;
    // Peak value for 1000 millis
    byte MaxBass;
    // Peak value decay evey 1000 millis
    byte MaxBassDecay = 20;
    // detect beat when volumes is bigger than 75% of MaxVolume
    float MaxBassThreshold = 0.75;
    byte MinBass = 40;
    bool BassBeatDetected = false;

  private:

    // store 1000 ms history in 40 Energyblocks à 25 ms
    CircularBuffer <byte, Energyblock_buffer_size> EnergyBuffer;
    byte energyBufferAverage();
    byte energyBufferVariance(byte average);

    CircularBuffer <unsigned long, Bpm_buffer_size> BpmBuffer;
    int bpmBufferBpmCount(unsigned long lastmillis);

    // spectrum shield: strobe pin
    byte _strobe;
    // spectrum shield: reset pin
    byte _reset;
    // spectrum shield: analog port to read left spectrum level
    byte _dcl;
    // spectrum shield: analog port to read right spectrum level
    byte _dcr;
    // analog potentiometer to adjust amplification factor
    byte _dcampfac;
    // digital port to read button 1
    byte _btn1;
    // digital port to read button 1
    byte _btn2;

};
#endif
