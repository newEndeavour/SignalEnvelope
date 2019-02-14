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

//---- Envelope object ----------------------------------------------------
#define ENV_OPERATION                   0     // 0=Rising / 1=Falling / 2=Double 
#define ENV_DECAY_SPEED                16     // 1 Fast, 128 Slowest
 
SignalEnvelope ENV(ENV_DECAY_SPEED, ENV_OPERATION);
float Envelope;
float Baseline;
float Thres_up;
int   isAvgMA1New;
int   isBaselineNew;

//---- General Variables ----------------------------------------------------
float Amplitude;
float Angle;
float RawSignal;
int Switch = 1;
int Base = 5;
int LoopCount = 0;

//---- Setup ----------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.print("\n---- Serial Started ----\n");

  Thres_up = Base*1.5;
  Baseline = 0;
  ENV.SetThres_Upper(Thres_up);
  ENV.SetBaseline(Baseline);

}

//---- Loop ----------------------------------------------------------------
void loop() {

  LoopCount++;

  //Generate a variable mimicking high Resonance
  Angle     = (float)(LoopCount/360.0)*pi;
  Switch    *=-1;
  Amplitude = abs(sin(Angle)*random(0,10));    
  RawSignal = Amplitude*Switch+Base;
  
  //Call Envelope
  Envelope      = ENV.Envelope(RawSignal);

  #if defined(DISP_MONITOR)
    //-> Serial
    Serial.print("\n");
    Serial.print(LoopCount);
    Serial.print(")");
        
    Serial.print("\tRaw:");
    Serial.print(RawSignal,4);
    
    Serial.print("\tENV.Threshold:");
    Serial.print(ENV.GetThres_Upper(),4);
    
    Serial.print("\tENV.Baseline:");
    Serial.print(ENV.GetBaseline(),4);

    Serial.print("\tEnvelope:");
    Serial.print(Envelope,4);
    
    delay(100);
  #endif

  #if defined(DISP_PLOTTER)
    Serial.print(RawSignal,4);
    Serial.print(" ");
    Serial.print(ENV.GetThres_Upper(),4);
    Serial.print(" ");
    Serial.print(ENV.GetBaseline(),4);
    Serial.print(" ");
    Serial.print(Envelope,4);
    Serial.print("\n");
    delay(10);
  #endif

}
