/*
  File:         SignalEnvelopeSketch.ino
  Version:      0.0.8
  Date:         19-Dec-2018
  Revision:     26-Feb-2019
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
#define BAUD_RATE               115200

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
#define ENV1_ATTACK_SPEED                1     // 1 Fast, 128 Slowest 
#define ENV1_DECAY_SPEED                 8     // 1 Fast, 128 Slowest 
SignalEnvelope ENV1(ENV1_ATTACK_SPEED, ENV1_DECAY_SPEED, ENV1_OPERATION);
float Envelope1;

#define ENV2_OPERATION                   0     // 0=Rising / 1=Falling / 2=Double 
#define ENV2_ATTACK_SPEED                1     // 1 Fast, 128 Slowest 
#define ENV2_DECAY_SPEED                64     // 1 Fast, 128 Slowest 
SignalEnvelope ENV2(ENV2_ATTACK_SPEED, ENV2_DECAY_SPEED, ENV2_OPERATION);
float Envelope2;

#define ENV3_OPERATION                   1     // 0=Rising / 1=Falling / 2=Double 
#define ENV3_ATTACK_SPEED                1     // 1 Fast, 128 Slowest 
#define ENV3_DECAY_SPEED                 8     // 1 Fast, 128 Slowest 
SignalEnvelope ENV3(ENV3_ATTACK_SPEED, ENV3_DECAY_SPEED, ENV3_OPERATION);
float Envelope3;

#define ENV4_OPERATION                   1     // 0=Rising / 1=Falling / 2=Double 
#define ENV4_ATTACK_SPEED                2     // 1 Fast, 128 Slowest 
#define ENV4_DECAY_SPEED                 4     // 1 Fast, 128 Slowest 
SignalEnvelope ENV4(ENV4_ATTACK_SPEED, ENV4_DECAY_SPEED, ENV4_OPERATION);
float Envelope4;


//---- Setup ----------------------------------------------------------------
void setup() {
  Serial.begin(BAUD_RATE);
  
  #if defined(DISP_MONITOR)
  while (!Serial);
  Serial.print("\n---- Serial Started ----\n");
  Serial.print("\n---- Objects Versions ----\n");
  Serial.print("\nVersion:");  
  Serial.print(ENV1.GetVersion());      
  Serial.print("\nRelease:");  
  Serial.print(ENV1.GetReleaseDate());
  #endif

}

//---- Loop ----------------------------------------------------------------
void loop() {

  LoopCount++;

  //Generate a variable mimicking high Resonance
  Angle     = (float)(LoopCount/360.0)*pi;
  Switch    *=-1;
  Amplitude = abs(sin(Angle)*random(0,10));    
  RawSignal = CentralPoint * (1 + 0.05 * cos((float)(LoopCount/120.0)*pi)) + Amplitude*Switch;
  RawSignal_MA1 = lambda * RawSignal_MA1 + (1-lambda) * RawSignal;
  
  //Call Envelope
  Envelope1      = ENV1.Envelope(RawSignal);
  Envelope2      = ENV2.Envelope(RawSignal);
  Envelope3      = ENV3.Envelope(RawSignal);
  Envelope4      = ENV4.Envelope(RawSignal);

  #if defined(DISP_MONITOR)
    //-> Serial
    Serial.print("\n");
    Serial.print(LoopCount);
    Serial.print(")");
        
    Serial.print("\tRaw:");
    Serial.print(RawSignal,4);
    
    Serial.print("\tRawMA1:");
    Serial.print(RawSignal_MA1,4);

    Serial.print("\tEnvelope1:");
    Serial.print(Envelope1,4);
        
    Serial.print("\tEnvelope2:");
    Serial.print(Envelope2,4);
    
    Serial.print("\tEnvelope3:");
    Serial.print(Envelope3,4);

    Serial.print("\tEnvelope4:");
    Serial.print(Envelope4,4);

    delay(100);
  #endif

  #if defined(DISP_PLOTTER)
    //RAW
    Serial.print(RawSignal,4);
    Serial.print(" ");

    Serial.print(RawSignal_MA1,4);
    Serial.print(" ");

    //ENV1
    Serial.print(Envelope1,4);
    Serial.print(" ");
    
    //ENV2
    Serial.print(Envelope2,4);
    Serial.print(" ");
    
    //ENV3
    Serial.print(Envelope3,4);
    Serial.print(" ");

    //ENV4
    Serial.print(Envelope4,4);
    Serial.print(" ");

    //Plotter Boundaries
    Serial.print(CentralPoint*1.2,4);
    Serial.print(" ");
    Serial.print(CentralPoint/1.2,4);
    
    Serial.print("\n");
    delay(10);
  #endif

}
