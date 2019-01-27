/*
  File:         SignalEnvelope.cpp
  Version:      0.0.3
  Date:         19-Dec-2018
  Revision:     25-Jan-2019
  Author:       Jerome Drouin (jerome.p.drouin@gmail.com)

  Editions:	Please go to SignalEnvelope.h for Edition Notes.
 
  SignalEnvelope.h - Library for 'duino
  https://github.com/newEndeavour/SignalEnvelope
  
  The SignalEnvelope object implements an Envelope for Touch decoding 
  routine situations in which the primary Signal cannot be filtered 
  via classic Signal Filtering Techniques (ie Butterworth, Chebychev, etc.).
  and as the Signal is parasited by a high noise component whilst the 
  Sensor is pressed. 

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

*/


#include "Arduino.h"
#include "SignalEnvelope.h"


// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances
SignalEnvelope::SignalEnvelope(uint8_t _speed, int _operation)
{
	// initialize this instance's variables
	error = 1;
	if (_speed<MINSPEEDPARAM) 	error =-1;		// incorrect speed variable
	if (_speed>MAXSPEEDPARAM) 	error =-2;		// incorrect speed variable
	if ((_operation!=0) 
		&& (_operation!=1) 
		&& (_operation!=2))	error =-4;		// incorrect operation

	//Auto Calibration of Baseline
	Autocal_Millis 		= INITIAL_AUTOCAL_FREQ;			

	//Set initial values	
	speed			= _speed;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay		= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for avgma1 = baseline

	thres_upper		= 0.0;				// Set to 0 :no baseline update possible
	thres_lower		= 0.0;				// Set to 0 :no baseline update possible

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	baseline		= 0.0;
	avgma1			= 0.0;
	envelope_up		= 0.0;
	envelope_lo		= 0.0;

	isAvgUpdate		= 0;
	isBaselineUpdate 	= 0;

	lastCal 		= millis();         		// set millis for start
}


SignalEnvelope::SignalEnvelope(uint8_t _speed, int _operation, float _baseline)
{
	// initialize this instance's variables
	error = 1;
	if (_speed<MINSPEEDPARAM) 	error =-1;		// incorrect speed variable
	if (_speed>MAXSPEEDPARAM) 	error =-2;		// incorrect speed variable
	if ((_operation!=0) 
		&& (_operation!=1) 
		&& (_operation!=2))	error =-4;		// incorrect operation

	//Auto Calibration of Baseline
	Autocal_Millis 		= INITIAL_AUTOCAL_FREQ;

	//Set initial values	
	speed			= _speed;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay		= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for avgma1 = baseline

	thres_upper		= 0.0;				// Set to 0 :no baseline update possible
	thres_lower		= 0.0;				// Set to 0 :no baseline update possible

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	baseline		= _baseline;
	avgma1			= _baseline;
	envelope_up		= _baseline;
	envelope_lo		= _baseline;

	isAvgUpdate		= 0;
	isBaselineUpdate 	= 0;

	lastCal 		= millis();         		// set millis for start
}


SignalEnvelope::SignalEnvelope(uint8_t _speed, int _operation, float _baseline, float _thres_upper, float _thres_lower)
{
	// initialize this instance's variables
	error = 1;
	if (_speed<MINSPEEDPARAM) 	error =-1;		// incorrect speed variable
	if (_speed>MAXSPEEDPARAM) 	error =-2;		// incorrect speed variable
	if (_thres_upper<0) 		error =-3;		// incorrect press threshold level
	if (_thres_lower<0) 		error =-3;		// incorrect press threshold level
	if ((_operation!=0) 
		&& (_operation!=1) 
		&& (_operation!=2))	error =-4;		// incorrect operation
	

	//Auto Calibration of Baseline
	Autocal_Millis 		= INITIAL_AUTOCAL_FREQ;

	//Set initial values	
	speed			= _speed;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay		= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for avgma1 = baseline

	thres_upper		= _thres_upper;
	thres_lower		= _thres_lower;

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	baseline		= _baseline;
	avgma1			= _baseline;
	envelope_up		= _baseline;
	envelope_lo		= _baseline;

	isAvgUpdate		= 0;
	isBaselineUpdate 	= 0;
	
	lastCal 		= millis();         		// set millis for start
}


// Public Methods //////////////////////////////////////////////////////////////
// Update envelope level with the Requested available envelope level
// Update envelope level with default available envelope level
float SignalEnvelope::Envelope(float rawSignal)
{
	//if error
	if (error<0)	return error;

	CalculateEnvelope(rawSignal);
	
	//Returning upper or lower
	if (operation==0) {
		return envelope_up;
	} 
	else 
	if (operation==1) {
		return envelope_lo;
	} 
	else return -8;

}


float SignalEnvelope::Envelope(float rawSignal, int returntype)
{

	//if error
	if (error<0)	return error;

	CalculateEnvelope(rawSignal);
	
	//Returning upper or lower
	if (returntype==0) {
		return envelope_up;
	} 
	else 
	if (returntype==1) {
		return envelope_lo;
	} 
	else return -7;

}


//Get the default envelope level
float SignalEnvelope::GetEnvelope(void)
{

	//if error
	if (error<0)	return error;

	//Returning upper or lower
	if (operation==0) {
		return envelope_up;
	} 
	else 
	if (operation==1) {
		return envelope_lo;
	}
	else return -8;
 
}


//Get the requested envelope level
float SignalEnvelope::GetEnvelope(int returntype)
{

	//if error
	if (error<0)	return error;

	//Returning upper or lower
	if (returntype==0) {
		return envelope_up;
	} 
	else 
	if (returntype==1) {
		return envelope_lo;
	}
	else return -7;
}


//Returns isAvgUpdate
int SignalEnvelope::GetisAvgUpdate(void)
{
	return isAvgUpdate;
}


//Returns isBaselineUpdate
int SignalEnvelope::GetisBaselineUpdate(void)
{
	return isBaselineUpdate;
}


//Set the envelope speed
void SignalEnvelope::SetSpeed(uint8_t _speed)
{
	if (_speed<MINSPEEDPARAM) 	error =-1;		// incorrect speed variable
	if (_speed>MAXSPEEDPARAM) 	error =-2;		// incorrect speed variable

	//Set initial values	
	speed				= _speed;		// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay			= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for avgma1 = baseline
}


//Set Upper press threshold
void SignalEnvelope::SetThres_Upper(float _thres_upper)
{
	//error
	if (_thres_upper<0) error =-3;

	//set parameter
	thres_upper	= _thres_upper;
}


//Set Lower press threshold
void SignalEnvelope::SetThres_Lower(float _thres_lower)
{
	//error
	if (_thres_lower<0) error =-3;

	//set parameter
	thres_lower	= _thres_lower;
}


//Set the baseline
void SignalEnvelope::SetBaseline(float _baseline)
{
	baseline		= _baseline;
	avgma1			= _baseline;	
}


//Set the envelope level to specific value
void SignalEnvelope::SetEnvelope_Upper(float _envelope)
{
	envelope_up		= _envelope;	
}


//Set the envelope level to specific value
void SignalEnvelope::SetEnvelope_Lower(float _envelope)
{
	envelope_lo		= _envelope;	
}


//Get the Baseline level
float SignalEnvelope::GetBaseline(void)
{
	return baseline;	
}


//Get the timedecay factor implied from speed
float SignalEnvelope::GetTimedecay(void)
{
	return timedecay;	
}


//Disable Auto Calibration
void SignalEnvelope::Disable_AutoCal(void)
{
	Autocal_Millis = 0x0FFFFFFFL;
}

//Reset Auto Calibration
void SignalEnvelope::Reset_AutoCal(void)
{
	Autocal_Millis = INITIAL_AUTOCAL_FREQ;
}


//Set the Autocal frequency parameter
void SignalEnvelope::Set_Autocal_Millis(unsigned long _autocal_Millis)
{
	Autocal_Millis = _autocal_Millis;
}


// Private Methods /////////////////////////////////////////////////////////////
// Functions only available to other functions in this library
// Update Up Envelope
void SignalEnvelope::CalculateEnvelope_Up(float rawSignal)
{
float decay;

	decay  	= (envelope_up - rawSignal) / speed;

	// Update envelope_up
	if (rawSignal>baseline) {
		if (rawSignal>envelope_up) {
			envelope_up = rawSignal;		// Attack
		} else {
			envelope_up -= decay;	 		// Decay
		}
	} else {
		//temporary
		envelope_up = baseline;
	} 
}


// Update Lo Envelope
void SignalEnvelope::CalculateEnvelope_Lo(float rawSignal)
{
float decay;

	decay  	= (rawSignal - envelope_lo) / speed;

	// Update envelope_lo
	if (rawSignal<baseline) {
		if (rawSignal<envelope_lo) {
			envelope_lo = rawSignal;		// Attack
		} else {
			envelope_lo += decay;		 	// Decay
		}
	} else {
		//temporary
		envelope_lo = baseline;
	} 
}


// Update Average MA1
void SignalEnvelope::UpdateAvgMA1(float rawSignal)
{
	//avgma1 = MA(1) with timedecay factor = f(speed)
	avgma1		= timedecay * avgma1 + (1 - timedecay) * rawSignal;
	isAvgUpdate	= 1;
}


// Update Baseline 
// Note: 
//	Baseline Calibration
// 	only calibrate if time since last calibration is greater than Autocal_Millis 
// 	and rawSignal is inside thres range. 
// 	This is an attempt to keep from calibrating when the envelope threshold is being crossed.
void SignalEnvelope::UpdateBaseline()
{
long now = millis();

	if (now - lastCal > Autocal_Millis) {
		baseline = avgma1;
		lastCal  = now;
		isBaselineUpdate = 1;
	} else {
		isBaselineUpdate = 0;
	}
}


// Calculates Envelope
void SignalEnvelope::CalculateEnvelope(float rawSignal)
{
	isAvgUpdate	= 0;

	//Envelope_up
	if (operation==0) {
		CalculateEnvelope_Up(rawSignal);

		//update average, baseline
		if (rawSignal<thres_upper) {
			UpdateAvgMA1(rawSignal);
			UpdateBaseline();
		}
	} else 

	//Envelope_lo
	if (operation==1) {
		CalculateEnvelope_Lo(rawSignal);

		//update average, baseline
		if (rawSignal>thres_lower) {
			UpdateAvgMA1(rawSignal);
			UpdateBaseline();
		}
	} else 

	if (operation==2) {
		CalculateEnvelope_Up(rawSignal);
		CalculateEnvelope_Lo(rawSignal);

		//update average, baseline
		if ((rawSignal<thres_upper) && (rawSignal>thres_lower)){
			UpdateAvgMA1(rawSignal);
			UpdateBaseline();
		}
	}

}


// /////////////////////////////////////////////////////////////////////////////

