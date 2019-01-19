/*
  File:         SignalEnvelope.h
  Version:      0.0.2
  Date:         19-Dec-2018
  Revision:     19-Jan-2019
  Author:       Jerome Drouin

  Editions:
  - 0.0.1	: First version
  - 0.0.2	: Added Operation mode for dual envelope functionality (and assoc. variables. 
		  Added constructors for when thresolds and baseline are unknown at time of building.

  SignalEnvelope.h v.01 - Library for 'duino
  https://github.com/newEndeavour/SignalEnvelope

  The SignalEnvelope object implements an Envelope for Touch decoding 
  routine situations in which the primary Signal cannot be filtered 
  via classic Signal Filtering Techniques (ie Butterworth, Chebychev, etc.),
  and as the Signal is parasited by a high noise component whilst the 
  Sensor is pressed. 

  Also see: 
  - http://ww1.microchip.com/downloads/en/appnotes/00001334b.pdf,p25.
  - http://ww1.microchip.com/downloads/en/appnotes/01317a.pdf, p11.

  Copyright (c) 2019 Jerome Drouin  All rights reserved.

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


// ensure this library description is only included once
#ifndef SignalEnvelope_h
#define SignalEnvelope_h

#include "Arduino.h"


#define MINSPEEDPARAM 		2	// MinSpeed = 2	 , Beta = 1/2, decay = 0.50
#define MAXSPEEDPARAM 	      128	// MaxSpeed = 128, Beta = 1/128, decay = 0.9921
#define SPEED_ATTENFACT      16.0	// Speed = 4, SPEED_ATTENFACT=2.0, Timedecay = 1 - 1 /(2.0 x 4) = 0.8750 (fast MA1)
					// Speed = 4, SPEED_ATTENFACT=4.0, Timedecay = 1 - 1 /(4.0 x 4) = 0.9375 (medi MA1)
					// Speed = 4, SPEED_ATTENFACT=8.0, Timedecay = 1 - 1 /(8.0 x 4) = 0.9687 (slow MA1)
					// Speed = 4, SPEED_ATTENFACT=16., Timedecay = 1 - 1 /(16. x 4) = 0.9843 (slug MA1)

// library interface description
class SignalEnvelope
{
  // user-accessible "public" interface
  public:
  // methods
	SignalEnvelope(uint8_t _speed, int _operation);
	SignalEnvelope(uint8_t _speed, int _operation, float _baseline);
	SignalEnvelope(uint8_t _speed, int _operation, float _baseline, float _thres_upper, float _thres_lower);

	int 	GetisAvgUpdate(void);

	float 	Envelope(float rawSignal);			//Update and return the default envelope level 
								//if operation 0=Rising, 1=Falling
	float 	Envelope(float rawSignal, int returntype);	//Update and return the envelope level 
								//and returntype 0=Rising, 1=Falling

	float 	GetEnvelope(int returntype);			//Get the requested envelope level and return 0=Rising, 1=Falling
	float 	GetEnvelope(void);				//Get the default envelope if operation is 0=Rising, 1=Falling

	void 	SetSpeed(uint8_t _speed);			//Set speed level

	void 	SetThres_Upper(float _thres_upper);		//Set threshold
	void 	SetThres_Lower(float _thres_lower);		//Set threshold

	void 	SetBaseline(float _baseline);			//Set the baseline
	void 	SetEnvelope_Upper(float _envelope);		//Set the Upper envelope level to specific value
	void 	SetEnvelope_Lower(float _envelope);		//Set the Lower envelope level to specific value
	void 	SetEnvelope(float _envelope);			//Set the envelope level to specific value

	float 	GetBaseline(void);				//Get the Baseline level
	float 	GetTimedecay(void);				//Get the Timedecay factor 


  // library-accessible "private" interface
  private:
  // variables
	int 		error;

	int 		operation;				//0=Upper, 1=Lower, 2=Double

	int 		isAvgUpdate;

	uint8_t		speed;

	float 		thres_upper; 
	float 		thres_lower; 

	float		timedecay;
	float 		envelope_up;
	float 		envelope_lo;
	float		avgma1; 
	float 		baseline;
	
  // methods
	void 	CalculateEnvelope(float rawSignal);
	void 	CalculateEnvelope_Up(float rawSignal);
	void 	CalculateEnvelope_Lo(float rawSignal);
	void 	UpdatebaselineMA1(float rawSignal);

};

#endif