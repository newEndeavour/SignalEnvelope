/*
  File:         SignalEnvelopeSketch.ino
  Version:      0.0.1
  Date:         19-Dec-2018
  Revision:     14-Feb-2019
  Author:       Jerome Drouin
  
  https://github.com/newEndeavour/SignalEnvelope

  Copyright (c) 2018-2019 Jerome Drouin  All rights reserved.  

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <SignalEnvelope.h>
#include <math.h>

//#define DISP_MONITOR                 1
#define DISP_PLOTTER                 1

static long double const   pi  =  3.14159265358979323846264338L;

//---- General Variables ----------------------------------------------------
float Amplitude;
float MaxAmpl = 10;
float CentralPoint = 100;
float Angle;
float lambda = 0.90;
float RawSignal;
float RawSignal_MA1 = 0;

int Switch = 1;
int LoopCount = 0;

//---- Envelope objects ----------------------------------------------------
#define ENV1_OPERATION                   0     // 0=Rising / 1=Falling / 2=Double 
#define ENV1_DECAY_SPEED                 8     // 1 Fast, 128 Slowest 
#define ENV1_MA1_DECAY_SPEED          0.95     // 
SignalEnvelope ENV1(ENV1_DECAY_SPEED, ENV1_MA1_DECAY_SPEED, ENV1_OPERATION);
float Envelope1;

#define ENV2_OPERATION                   0     // 0=Rising / 1=Falling / 2=Double 
#define ENV2_DECAY_SPEED                64     // 1 Fast, 128 Slowest 
#define ENV2_MA1_DECAY_SPEED          0.97     // 
SignalEnvelope ENV2(ENV2_DECAY_SPEED, ENV2_MA1_DECAY_SPEED, ENV2_OPERATION, CentralPoint);
float Envelope2;

//---- Setup ----------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.print("\n---- Serial Started ----\n");

  ENV1.SetThres_Upper(CentralPoint + MaxAmpl);
  ENV1.SetBaseline(CentralPoint,0);
  ENV1.Set_Autocal_Millis(100);         // Fast Baseline update Speed

  ENV2.SetThres_Upper(CentralPoint + MaxAmpl);
  //ENV2.SetBaseline(CentralPoint);  // Already Set in Constructor
  ENV2.Set_Autocal_Millis(2000);      // Slow Baseline update Speed
}

//---- Loop ----------------------------------------------------------------
void loop() {

  LoopCount++;

  //Generate a variable mimicking high Resonance
  Angle     = (float)(LoopCount/360.0)*pi;
  Switch    *=-1;
  Amplitude = abs(sin(Angle)*random(0,10));    
  RawSignal = CentralPoint*(1 + 0.05*cos((float)(LoopCount/120.0)*pi)) + Amplitude*Switch;
  RawSignal_MA1 = lambda * RawSignal_MA1 + (1-lambda) * RawSignal;
  
  //Call Envelope
  Envelope1      = ENV1.Envelope(RawSignal);
  Envelope2      = ENV2.Envelope(RawSignal);

  #if defined(DISP_MONITOR)
    //-> Serial
    Serial.print("\n");
    Serial.print(LoopCount);
    Serial.print(")");
        
    Serial.print("\tRaw:");
    Serial.print(RawSignal,4);
    
    Serial.print("\tRawMA1:");
    Serial.print(RawSignal_MA1,4);

    if (LoopCount % 250==0) {
      ENV1.SetBaseline(RawSignal_MA1,0);
    }

    Serial.print("\tENV1.Threshold:");
    Serial.print(ENV1.GetThres_Upper(),4);
    
    Serial.print("\tENV1.Baseline:");
    Serial.print(ENV1.GetBaseline(),4);

    Serial.print("\tEnvelope1:");
    Serial.print(Envelope1,4);
        
    Serial.print("\tENV2.GetMA1Mean:");
    Serial.print(ENV2.GetMA1Mean(),4);

    Serial.print("\tENV2.GetMA1Variance:");
    Serial.print(ENV2.GetMA1Variance(),4);

    Serial.print("\tENV2.GetMA1StdDeviation:");
    Serial.print(ENV2.GetMA1StdDeviation(),4);

    Serial.print("\tEnvelope2:");
    Serial.print(Envelope2,4);
    delay(100);
  #endif

  #if defined(DISP_PLOTTER)
    //RAW
    Serial.print(RawSignal,4);
    Serial.print(" ");

    Serial.print(RawSignal_MA1,4);
    Serial.print(" ");

    if (LoopCount % 250==0) {
      ENV1.SetBaseline(RawSignal_MA1,1);
    }

    //ENV1
    Serial.print(Envelope1,4);
    Serial.print(" ");
    Serial.print(ENV1.GetThres_Upper(),4);
    Serial.print(" ");
    Serial.print(ENV1.GetBaseline(),4);
    Serial.print(" ");
    
    //ENV2
    Serial.print(Envelope2,4);
    Serial.print(" ");
    Serial.print(ENV2.GetBaseline(),4);
    Serial.print(" ");
    
    //Plotter Boundaries
    Serial.print(CentralPoint*1.2,4);
    Serial.print(" ");
    Serial.print(CentralPoint/1.2,4);
    Serial.print("\n");
    delay(10);
  #endif

}
