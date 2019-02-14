/*
  File:         SignalEnvelope.h
  Version:      0.0.5
  Date:         19-Dec-2018
  Revision:     14-Feb-2019
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
  - 0.0.5	: Additional Moments : mean, variance, stddev for baseline and supporting methods.
		  Better Error handling methodology
  

*/


// ensure this library description is only included once
#ifndef SignalEnvelope_h
#define SignalEnvelope_h

#include "Arduino.h"

#define INITIAL_AUTOCAL_FREQ	     1000	// Autocalibration Frequency 5 secs
#define MINSPEEDPARAM 			2	// MinSpeed = 2	 , Beta = 1/2, decay = 0.50
#define MAXSPEEDPARAM 	      	      128	// MaxSpeed = 128, Beta = 1/128, decay = 0.9921
#define SPEED_ATTENFACT      	      8.0	// Speed = 4, SPEED_ATTENFACT=2.0, Timedecay = 1 - 1 /(2.0 x 4) = 0.8750 (fast MA1)
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

	int 	GetisMomentsUpdate(void);
	int 	GetisBaselineUpdate(void);

	float 	Envelope(float rawSignal);			//Update and return the default envelope level 
								//if operation 0=Rising, 1=Falling
	float 	Envelope(float rawSignal, int returntype);	//Update and return the envelope level 
								//and returntype 0=Rising, 1=Falling

	float 	GetEnvelope(int returntype);			//Get the requested envelope level and return 0=Rising, 1=Falling
	float 	GetEnvelope(void);				//Get the default envelope if operation is 0=Rising, 1=Falling

	void 	SetSpeed(uint8_t _speed);			//Set speed level

	void 	SetThres_Upper(float _thres_upper);		//Set threshold
	void 	SetThres_Lower(float _thres_lower);		//Set threshold
	float 	GetThres_Upper(void);				//Get the thres_upper level
	float 	GetThres_Lower(void);				//Get the thres_lower level

	void 	SetEnvelope(float _envelope);			//Set the envelope level to specific value
	void 	SetEnvelope_Upper(float _envelope);		//Set the Upper envelope level to specific value
	void 	SetEnvelope_Lower(float _envelope);		//Set the Lower envelope level to specific value

	void 	SetBaseline(float _baseline);			//Set the baseline
	float 	GetBaseline(void);				//Get the Baseline level

	float 	GetTimedecay(void);				//Get the Timedecay factor
	float 	GetSpeed(void);					//Get the Speed factor

	float 	GetRawSignal_xi(void);				//Get the GetRawSignal_xi level
	float 	GetRawSignal_xi2(void);				//Get the GetRawSignal_xi2 level
	float 	GetRawSignal_Mean(void);			//Get the GetRawSignal_mean level
	float 	GetRawSignal_Variance(void);			//Get the GetRawSignal_var level
	float 	GetRawSignal_StDeviation(void);			//Get the GetRawSignal_stdev level

	void 	Reset_AutoCal();				// Reset Autocal_Millis
	void 	Disable_AutoCal();				// Stops Baseline update. To Return to baseline update use 
								// Set_Autocal_Millis(x) where x>0

	void 	Set_Autocal_Millis(unsigned long autoCal_millis);
	unsigned long Get_Autocal_Millis(void);

  // library-accessible "private" interface
  private:
  // variables

	int 		error;
	int 		isMomentsUpdate;
	int		isBaselineUpdate;

	unsigned long  	Autocal_Millis;
	unsigned long  	lastCal;

	int 		operation;				//0=Upper, 1=Lower, 2=Double
	uint8_t		speed;

	float 		thres_upper; 
	float 		thres_lower; 

	float		timedecay;
	float 		envelope_up;
	float 		envelope_lo;
	float 		baseline;
	
	float		raw_xi; 
	float		raw_xi2; 
	float		raw_mean; 
	float		raw_var; 
	float		raw_stdev; 

  // methods
	void 	CalculateEnvelope(float rawSignal);
	void 	CalculateEnvelope_Up(float rawSignal);
	void 	CalculateEnvelope_Lo(float rawSignal);

	void 	Update_RawSignal_Moments(float rawSignal);
	void 	UpdateBaseline(float rawSignal);

	void 	ResetErrors(void);	// Error flag handling

};

#endif
