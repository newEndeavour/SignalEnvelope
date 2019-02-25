/*
  File:         SignalEnvelope.h
  Version:      0.0.7
  Date:         19-Dec-2018
  Revision:     18-Feb-2019
  Author:       Jerome Drouin (jerome.p.drouin@gmail.com)

  SignalEnvelope.h - Library for 'duino
  https://github.com/newEndeavour/SignalEnvelope

  The SignalEnvelope object implements an Envelope for Touch Sensing routine 
  decoding situations in which the primary Signal cannot be filtered via 
  classic Signal Filtering Techniques (ie Butterworth, Chebychev, etc.), 
  whilst the Sensor is pressed as the Signal is parasited by a high noise 
  component. 

  Also see: 
  - http://ww1.microchip.com/downloads/en/appnotes/00001334b.pdf, p25.
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

  Editions:
  - 0.0.1	: First version
  - 0.0.2	: Added Operation mode for dual envelope functionality (and assoc. variables). 
		  Added constructors for when thresholds and baseline are unknown at time of building.
  - 0.0.3	: Implemented Auto Calibration methods
		  Minor housekeeping.
  - 0.0.4	: Changed the way the Envelope signal reverts back to Baseline. Previously, Envelope was set
		  equal to the baseline when the RawSignal was below the threshold. In this new version, the 
		  Envelope keeps decaying back at its set speed (decay) and is only set = to baseline 
	  	  when the calculation falls under the baseline. this is to avoid a situation in Operation mode
		  "double" (=2), when the envelope could potentially cross. (Code of the previous version is still 
		  in place for those who liked it better).
  - 0.0.5	: Better Error handling methodology
  - 0.0.6	: decouplong between Speed Factor and Moment MA1 TimeDecay factor. 
		  Updating Constructors with float parameter.
  		  Added supporting methods.
  - 0.0.7	: Simplification of object:
		  removing baseline, thesholds, etc. 

*/


// ensure this library description is only included once
#ifndef SignalEnvelope_h
#define SignalEnvelope_h

#include "Arduino.h"

// DEFINES /////////////////////////////////////////////////////////////
#define VER_SignalEnvelope	       	"0.0.7"		//
#define REL_SignalEnvelope		"18Feb2019"	//


#define MINSPEEDPARAM 			2	// MinSpeed = 2	 , Beta = 1/2, decay = 0.50
#define MAXSPEEDPARAM 	      	      128	// MaxSpeed = 128, Beta = 1/128, decay = 0.9921


// library interface description
class SignalEnvelope
{
  // user-accessible "public" interface
  public:
  // methods

	SignalEnvelope(uint8_t _speed, int _operation);
	SignalEnvelope(uint8_t _speed, int _operation, float _thres_upper, float _thres_lower);

	float 	Envelope(float rawSignal);			//Update and return the default envelope level 
								//if operation 0=Rising, 1=Falling
	float 	Envelope(float rawSignal, int returntype);	//Update and return the envelope level 
								//and returntype 0=Rising, 1=Falling

	float 	GetEnvelope(int returntype);			//Get the requested envelope level and return 0=Rising, 1=Falling
	float 	GetEnvelope(void);				//Get the default envelope if operation is 0=Rising, 1=Falling

	void 	SetSpeed(uint8_t _speed);			//Set speed level

	void 	SetEnvelope(float _envelope);			//Set the envelope level to specific value
	void 	SetEnvelope_Upper(float _envelope);		//Set the Upper envelope level to specific value
	void 	SetEnvelope_Lower(float _envelope);		//Set the Lower envelope level to specific value

	float 	GetSpeed(void);					//Get the Speed factor
	String 	GetVersion();					//Returns version number	
	String 	GetReleaseDate();				//Returns Release Date

  // library-accessible "private" interface
  private:
  // variables

	int 		error;

	int 		operation;				// 0=Upper, 1=Lower, 2=Double
	uint8_t		speed;					// 	

	float 		envelope_up;
	float 		envelope_lo;
	
  // methods
	void 	CalculateEnvelope(float rawSignal);
	void 	CalculateEnvelope_Up(float rawSignal);
	void 	CalculateEnvelope_Lo(float rawSignal);

	void 	ResetErrors(void);	// Error flag handling

};

#endif
