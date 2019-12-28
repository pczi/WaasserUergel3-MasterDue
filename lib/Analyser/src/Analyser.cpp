/*
  Analyser.h - Library for analysing sounds with the spectrum analyser shield.
  Created by Peter Czibula, February 18, 2019.
*/

#include "../../../src/def.h"
#include "Analyser.h"

/************* constructor / Setup ****************/

Analyser::Analyser()
{
}

/************* Initialize the Spectrum Shield ****************/

void Analyser::begin(byte strobe, byte reset, byte dcl, byte dcr, byte dcampfac, byte btn1, byte btn2)
{

  _strobe = strobe;
  _reset = reset;
  _dcl = dcl;
  _dcr = dcr;
  _dcampfac = dcampfac;
  _btn1 = btn1;
  _btn2 = btn2;

#if verbose_level > -1
  // verbode_level = 0 or 1
  Serial.println((String) "Analyser strobe OUT on digital pin " + _strobe);
  Serial.println((String) "Analyser reset OUT on digital pin " + _reset);
  Serial.println((String) "Analyser dc left IN on analog pin " + _dcl);
  Serial.println((String) "Analyser dc right IN on analog pin " + _dcr);
  Serial.println((String) "Analyser dc amplification factor IN on analog pin " + _dcampfac);
  Serial.println((String) "Analyser button1 IN on digital pin " + _btn1);
  Serial.println((String) "Analyser button2 IN on digital pin " + _btn2);
#endif

  // Spectrum Shield pin setup
  pinMode(_strobe, OUTPUT);
  pinMode(_reset, OUTPUT);
  pinMode(_dcl, INPUT);
  pinMode(_dcr, INPUT);
  pinMode(_dcampfac, INPUT);
  pinMode(_btn1, INPUT_PULLUP);
  pinMode(_btn2, INPUT_PULLUP);

  // Create an initial state for our pins
  digitalWrite(_reset, LOW);
  digitalWrite(_strobe, LOW);
  delay(1);

  // Reset the MSGEQ7 as per the datasheet timing diagram
  digitalWrite(_reset, HIGH);
  delay(1);
  digitalWrite(_reset, LOW);
  digitalWrite(_strobe, HIGH);
  delay(1);
}

/************* Pull frquencies from Spectrum Shield ****************/

void Analyser::read()
{
  // --------------------------
  // read amplification factor
  // --------------------------

  AmplificationFactor = float(analogRead(AmplificationAnalogPort) * 2) / 1024;

  // --------------------------
  // read button 1
  // --------------------------

  static bool _btnPressed1 = false;
  static unsigned long _btnPressedMillis1 = millis();
  if (digitalRead(_btn1) == HIGH && _btnPressed1 == LOW &&
      (millis() - _btnPressedMillis1) > 10)
  {
    ButtonPressed1 = true;
    _btnPressed1 = true;
    _btnPressedMillis1 = millis();
  }
  else
  {
    ButtonPressed1 = false;
    _btnPressed1 = false;
  }

  // --------------------------
  // read button 2
  // --------------------------

  static bool _btnPressed2 = false;
  static unsigned long _btnPressedMillis2 = millis();
  if (digitalRead(_btn2) == HIGH && _btnPressed2 == LOW &&
      (millis() - _btnPressedMillis2) > 10)
  {
    ButtonPressed2 = true;
    _btnPressed2 = true;
    _btnPressedMillis2 = millis();
  }
  else
  {
    ButtonPressed2 = false;
    _btnPressed2 = false;
  }

  // --------------------------
  // read the spectrum frequencies
  // adjust noise level
  // --------------------------

  // Set reset pin low to enable strobe
  // digitalWrite(_reset, HIGH);
  // digitalWrite(_reset, LOW);

  unsigned long SpectrumSum = 0;
  unsigned long NoiseLevelSum = 0;
  // Band 0 = Lowest Frequency
  for (byte Band = 0; Band < 7; Band++)
  {
    digitalWrite(_strobe, LOW);
    // Delay necessary due to timing diagram
    delayMicroseconds(20);

    // Read L & R minus noiselevel and divide by 2, then by 2 and again by 2
    long level = constrain((analogRead(_dcl) + analogRead(_dcr) - NoiseLevel - NoiseLevel) * AmplificationFactor, 0, 2048);
    // reduce values between 0 and 256
    Spectrum[Band] = byte(level >> 3);
    // push value into the circular buffer to compute average
    SpectrumBuffer[Band].push(level);
    SpectrumSum = SpectrumSum + Spectrum[Band];
    NoiseLevelSum += analogRead(_dcl) + analogRead(_dcr);

    digitalWrite(_strobe, HIGH);
    // Delay necessary due to timing diagram
    delayMicroseconds(1);
  }
  // recompute NoiseLevel
  NoiseLevel = min(NoiseLevel, NoiseLevelSum / 14);

  // --------------------------
  // compute energy values based on the 2 lowest bands
  // --------------------------
  static unsigned long EnergyCycleStart = millis();
  static unsigned long TempBlockEnergySum = 0;
  static byte TempBlockEnergyCount = 0;
  if (millis() - EnergyCycleStart > Energyblock_sample_millis)
  {
    // measure the energy E contained in every 25 ms block
    // compute energy of the temp block
    byte e = TempBlockEnergySum / TempBlockEnergyCount;
    // reset temp block
    TempBlockEnergySum = 0;
    TempBlockEnergyCount = 0;

    // assign energy to the circular buffer
    EnergyBuffer.push(e);

    // compute average and variance of the circular buffer
    EnergyAvg = this->energyBufferAverage();
    EnergyVar = this->energyBufferVariance(EnergyAvg);
    EnergyStdDev = sqrt(EnergyVar);

    // compute threshold
    // byte threshold = ((float)(-0.008 * EnergyStdDev) + 1.5f) * EnergyAvg;
    byte threshold = EnergyAvg + EnergyStdDev;

#if verbose_level == -1
    Serial.print((int)(e - EnergyAvg));
    Serial.print("egy ");
    Serial.print((int)(EnergyAvg - EnergyAvg));
    Serial.print("avg ");
    Serial.print((int)(threshold - EnergyAvg));
    Serial.print("thh ");
    Serial.print((int)(EnergyStdDev));
    Serial.print("stdev ");
    Serial.println();
#endif

    // --------------------------
    // recompute Bass, MaxBass and BassBeat (2 lower bands + 1 oldbass)
    // --------------------------
    static bool _EnergyOverThreshold = false;
    EnergyBeatDetected = false;
    if (e > threshold && EnergyAvg > 10)
    {
      if (!_EnergyOverThreshold)
      {
        // we have a rising edge
        EnergyBeatDetected = true;
      }
      _EnergyOverThreshold = true;
    }
    else
    {
      _EnergyOverThreshold = false;
    }

    // --------------------------
    // compute BPM based on EnergyBeat
    // --------------------------
    if (EnergyBeatDetected)
    {
      BpmBuffer.push(millis());
    }
    BPM = bpmBufferBpmCount(5000);

    EnergyCycleStart = millis();
  }
  // add Band 0+1 energy to temp block
  TempBlockEnergySum += (Spectrum[0] + Spectrum[1]);
  TempBlockEnergyCount++;

  // --------------------------
  // recompute Volume, MaxVolume and VolumeBeat (7 bands + 1 oldvolume)
  // --------------------------
  static bool _VolumeOverThreshold = false;
  // divide by 8 because band 0 and band 7 are mostly close to 0
  Volume = constrain((SpectrumSum + Volume) >> 3, 0, 255);
  MaxVolume = max(MaxVolume, Volume);
  VolumeBeatDetected = false;
  if ((Volume > (MaxVolume * MaxVolumeThreshold)) && (Volume > MinVolume))
  {
    if (!_VolumeOverThreshold)
    {
      // we have a rising edge
      VolumeBeatDetected = true;
    }
    _VolumeOverThreshold = true;
  }
  else
  {
    _VolumeOverThreshold = false;
  }

  // --------------------------
  // recompute Bass, MaxBass and BassBeat (2 lower bands + 1 oldbass)
  // --------------------------
  static bool _BassOverThreshold = false;
  Bass = constrain((Spectrum[0] + Spectrum[0] + Spectrum[1] + Bass) >> 2, 0, 255);
  MaxBass = max(MaxBass, Bass);
  // detect rising edge of the bass beat
  BassBeatDetected = false;
  if ((Bass > (MaxBass * MaxBassThreshold)) && (Bass > MinBass))
  {
    if (!_BassOverThreshold)
    {
      // we have a rising edge
      BassBeatDetected = true;
    }
    _BassOverThreshold = true;
  }
  else
  {
    _BassOverThreshold = false;
  }

  // --------------------------
  // recompute MaxVolumes, MaxBass based on their decays
  // --------------------------
  static unsigned long MaxCycleStart = millis();
  if (millis() - MaxCycleStart > 1000)
  {
    // reduce the peak volume every 1000 millis
    MaxVolume = constrain(MaxVolume - MaxVolumeDecay, 0, 255);
    MaxBass = constrain(MaxBass - MaxBassDecay, 0, 255);
    MaxCycleStart = millis();
  }
}

/************* Compute BPM's ****************/

int Analyser::bpmBufferBpmCount(unsigned long lastmillis)
{
  int BPMcount = 0;
  unsigned long lasttensecs = millis() - lastmillis;
  for (byte i = 0; i < BpmBuffer.size(); i++)
  {
    if (BpmBuffer[i] > lasttensecs)
    {
      BPMcount++;
    }
  }
  return BPMcount * (60000 / lastmillis);
}

byte Analyser::energyBufferAverage()
{
  // compute energy average
  unsigned long sum = 0;
  // the following ensures using the right type for the index variable
  // using index_t = decltype(this->SpectrumBuffer[Band])::index_t;
  for (byte i = 0; i < EnergyBuffer.size(); i++)
  {
    sum += EnergyBuffer[i];
  }
  return (byte)(sum / EnergyBuffer.size());
}

byte Analyser::energyBufferVariance(byte average)
{
  // compute energy variance
  unsigned long diffsum = 0;
  // the following ensures using the right type for the index variable
  // using index_t = decltype(this->SpectrumBuffer[Band])::index_t;
  for (byte i = 0; i < EnergyBuffer.size(); i++)
  {
    diffsum += ((EnergyBuffer[i] - average) * (EnergyBuffer[i] - average));
  }
  return (byte)(diffsum / EnergyBuffer.size());
}

byte Analyser::spectrumBufferAverage(byte Band)
{
  unsigned long sum = 0;
  // the following ensures using the right type for the index variable
  // using index_t = decltype(this->SpectrumBuffer[Band])::index_t;
  for (byte i = 0; i < SpectrumBuffer[Band].size(); i++)
  {
    sum += SpectrumBuffer[Band][i];
  }
  return (byte)sum / SpectrumBuffer[Band].size();
}

byte Analyser::spectrumBufferMax(byte Band)
{
  byte maximum = 0;
  // the following ensures using the right type for the index variable
  // using index_t = decltype(this->SpectrumBuffer[Band])::index_t;
  for (byte i = 0; i < SpectrumBuffer[Band].size(); i++)
  {
    if (maximum < SpectrumBuffer[Band][i])
    {
      maximum = SpectrumBuffer[Band][i];
    }
  }
  return (byte)maximum;
}
