/*
  File:         SignalEnvelope.cpp
  Version:      0.0.5
  Date:         19-Dec-2018
  Revision:     14-Feb-2019
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

	//Auto Calibration of Baseline
	Autocal_Millis 		= INITIAL_AUTOCAL_FREQ;			

	//Set initial values	
	speed			= _speed;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay		= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for raw_mean = baseline

	thres_upper		= 0.0;				// Set to 0 :no baseline update possible
	thres_lower		= 0.0;				// Set to 0 :no baseline update possible

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	baseline		= 0.0;
	envelope_up		= 0.0;
	envelope_lo		= 0.0;

	raw_xi			= 0;
	raw_xi2			= 0;
	raw_mean		= raw_xi;
	raw_var			= raw_xi2 - (raw_xi * raw_xi);
	raw_stdev		= sqrt(raw_var);

	isMomentsUpdate		= 0;
	isBaselineUpdate 	= 0;

	lastCal 		= millis();         		// set millis for start

	// Object parameter's error handling
	ResetErrors();
}


SignalEnvelope::SignalEnvelope(uint8_t _speed, int _operation, float _baseline)
{

	//Auto Calibration of Baseline
	Autocal_Millis 		= INITIAL_AUTOCAL_FREQ;

	//Set initial values	
	speed			= _speed;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay		= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for raw_mean = baseline

	thres_upper		= 0.0;				// Set to 0 :no baseline update possible
	thres_lower		= 0.0;				// Set to 0 :no baseline update possible

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	baseline		= _baseline;
	envelope_up		= _baseline;
	envelope_lo		= _baseline;

	raw_xi			= _baseline;
	raw_xi2			= _baseline * _baseline;
	raw_mean		= raw_xi;
	raw_var			= raw_xi2 - (raw_xi * raw_xi);
	raw_stdev		= sqrt(raw_var);

	isMomentsUpdate		= 0;
	isBaselineUpdate 	= 0;

	lastCal 		= millis();         		// set millis for start

	// Object parameter's error handling
	ResetErrors();
}


SignalEnvelope::SignalEnvelope(uint8_t _speed, int _operation, float _baseline, float _thres_upper, float _thres_lower)
{

	//Auto Calibration of Baseline
	Autocal_Millis 		= INITIAL_AUTOCAL_FREQ;

	//Set initial values	
	speed			= _speed;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay		= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for raw_mean = baseline

	thres_upper		= _thres_upper;
	thres_lower		= _thres_lower;

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	baseline		= _baseline;
	envelope_up		= _baseline;
	envelope_lo		= _baseline;

	raw_xi			= _baseline;
	raw_xi2			= _baseline * _baseline;
	raw_mean		= raw_xi;
	raw_var			= raw_xi2 - (raw_xi * raw_xi);
	raw_stdev		= sqrt(raw_var);

	isMomentsUpdate		= 0;
	isBaselineUpdate 	= 0;
	
	lastCal 		= millis();         		// set millis for start

	// Object parameter's error handling
	ResetErrors();
}


// Public Methods //////////////////////////////////////////////////////////////
// Update envelope level with the Requested available envelope level
// Update envelope level with default available envelope level
float SignalEnvelope::Envelope(float rawSignal)
{

	if (error < 0) return error;            // bad constructor variables

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

	if (error < 0) return error;            // bad constructor variables

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

	if (error < 0) return error;            // bad constructor variables

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

	if (error < 0) return error;            // bad constructor variables

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


//Returns isMomentsUpdate
int SignalEnvelope::GetisMomentsUpdate(void)
{
	return isMomentsUpdate;
}


//Returns isBaselineUpdate
int SignalEnvelope::GetisBaselineUpdate(void)
{
	return isBaselineUpdate;
}


//Set the envelope speed
void SignalEnvelope::SetSpeed(uint8_t _speed)
{

	//Set initial values	
	speed				= _speed;		// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	timedecay			= (float) 1 - (1 / (speed * SPEED_ATTENFACT));// Time decay factor for raw_mean = baseline

	// Object parameter's error handling
	ResetErrors();
}


//Set Upper press threshold
void SignalEnvelope::SetThres_Upper(float _thres_upper)
{

	//set parameter
	thres_upper	= _thres_upper;

	// Object parameter's error handling
	ResetErrors();
}


//Set Lower press threshold
void SignalEnvelope::SetThres_Lower(float _thres_lower)
{

	//set parameter
	thres_lower	= _thres_lower;

	// Object parameter's error handling
	ResetErrors();
}


//Get Upper press threshold
float SignalEnvelope::GetThres_Upper(void)
{
	return thres_upper;
}


//Get Lower press threshold
float SignalEnvelope::GetThres_Lower(void)
{
	return thres_lower;
}


//Set the baseline
void SignalEnvelope::SetBaseline(float _baseline)
{
	baseline		= _baseline;

	// Object parameter's error handling
	ResetErrors();
}


//Set the envelope level to specific value
void SignalEnvelope::SetEnvelope_Upper(float _envelope)
{
	envelope_up		= _envelope;	

	// Object parameter's error handling
	ResetErrors();
}


//Set the envelope level to specific value
void SignalEnvelope::SetEnvelope_Lower(float _envelope)
{
	envelope_lo		= _envelope;	

	// Object parameter's error handling
	ResetErrors();
}


//Sets the default envelope level
void SignalEnvelope::SetEnvelope(float _envelope)
{

	//Setting upper or lower
	if (operation==0) {
		envelope_up	= _envelope;	
	} 
	else 
	if (operation==1) {
		envelope_lo	= _envelope;	
	}
 
	// Object parameter's error handling
	ResetErrors();
}


//Get the Baseline level
float SignalEnvelope::GetBaseline(void)
{
	return baseline;	
}


//Get the raw_xi level
float SignalEnvelope::GetRawSignal_xi(void)
{
	return raw_xi;	
}


//Get the raw_xi2 level
float SignalEnvelope::GetRawSignal_xi2(void)
{
	return raw_xi2;	
}


//Get the raw_mean level
float SignalEnvelope::GetRawSignal_Mean(void)
{
	return raw_mean;	
}


//Get the raw_var level
float SignalEnvelope::GetRawSignal_Variance(void)
{
	return raw_var;	
}


//Get the raw_stdev level
float SignalEnvelope::GetRawSignal_StDeviation(void)
{
	return raw_stdev;	
}


//Get the timedecay factor implied from speed
float SignalEnvelope::GetTimedecay(void)
{
	return timedecay;	
}


//Get the speed parameter
float SignalEnvelope::GetSpeed(void)
{
	return speed;	
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

	// Object parameter's error handling
	ResetErrors();
}


//Get the Autocal frequency parameter
unsigned long SignalEnvelope::Get_Autocal_Millis(void) 
{
	return Autocal_Millis;
}


// Private Methods /////////////////////////////////////////////////////////////

// Functions only available to other functions in this library
// Update Up Envelope
void SignalEnvelope::CalculateEnvelope_Up(float rawSignal)
{
float decay;

	decay  	= (envelope_up - rawSignal) / speed;

	/* 
	// ---- PREVIOUS VERSION ---------------------------------------------
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
	// -------------------------------------------------------------------
	*/

	// Update envelope_up
	if (rawSignal>envelope_up) {
		envelope_up = rawSignal;		// Attack
	} else {
		envelope_up -= decay;	 		// Decay
		if (envelope_up<baseline) 		// Boundary condition
			envelope_up = baseline;
	}

}


// Update Lo Envelope
void SignalEnvelope::CalculateEnvelope_Lo(float rawSignal)
{
float decay;

	decay  	= (rawSignal - envelope_lo) / speed;

	/*
	// ---- PREVIOUS VERSION ---------------------------------------------
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
	// -------------------------------------------------------------------
	*/

	// Update envelope_lo
	if (rawSignal<envelope_lo) {
		envelope_lo = rawSignal;		// Attack
	} else {
		envelope_lo += decay;	 		// Decay
		if (envelope_lo>baseline) 		// Boundary condition
			envelope_lo = baseline;
	}

}



// Update MA1 Moments
void SignalEnvelope::Update_RawSignal_Moments(float rawSignal)
{
	//raw_mean = MA(1) with timedecay factor = f(speed)

	raw_xi		= timedecay * raw_xi + (1 - timedecay) * rawSignal;
	raw_xi2		= timedecay * raw_xi2 + (1 - timedecay) * (rawSignal*rawSignal);
	
	raw_mean	= raw_xi;			// mean
	raw_var		= raw_xi2 - (raw_xi * raw_xi);	// variance
	raw_stdev	= sqrt(raw_var);		// std deviation
	
	isMomentsUpdate	= 1;

}



// Update Baseline 
// Note: 
//	Baseline Calibration
// 	only calibrate if time since last calibration is greater than Autocal_Millis 
// 	and rawSignal is inside thres range. 
// 	This is an attempt to keep from calibrating when the envelope threshold is being crossed.
void SignalEnvelope::UpdateBaseline(float rawSignal)
{
long now = millis();

	isBaselineUpdate = 0;
	Update_RawSignal_Moments(rawSignal);

	if (now - lastCal > Autocal_Millis) {
		baseline = raw_mean;
		lastCal  = now;
		isBaselineUpdate = 1;
	} 
}


// Calculates Envelope
void SignalEnvelope::CalculateEnvelope(float rawSignal)
{
	isMomentsUpdate	 = 0;
	isBaselineUpdate = 0;

	//Envelope_up
	if (operation==0) {
		CalculateEnvelope_Up(rawSignal);

		//update average, baseline
		if (rawSignal<thres_upper)
			UpdateBaseline(rawSignal);
	} else 


	//Envelope_lo
	if (operation==1) {
		CalculateEnvelope_Lo(rawSignal);

		//update average, baseline
		if (rawSignal>thres_lower)
			UpdateBaseline(rawSignal);
	} else 

	if (operation==2) {
		CalculateEnvelope_Up(rawSignal);
		CalculateEnvelope_Lo(rawSignal);

		//update average, baseline
		if ((rawSignal<thres_upper) && (rawSignal>thres_lower))
			UpdateBaseline(rawSignal);
	}

}


//Reset error flag following importance hierarchy
//by increasing error importance:
//	- Operation	: -3
//	- Speed		: -2
//	- Thres values	: -1	(most important)
//	Note: Object instanciated with incorrect Operation mode and incorrect threshold value
//	will return -2 (first), and then -4 only after theshold conflict has been resolved.
//	
void SignalEnvelope::ResetErrors(void) 
{

	// Object parameter's error handling
	error = 1;

	if ((operation!=0) 
	 && (operation!=1)
	 && (operation!=2)) 			error =-4;	// incorrect _operation mode

	if (speed<MINSPEEDPARAM) 		error =-3;	// incorrect speed variable
	if (speed>MAXSPEEDPARAM) 		error =-3;	// incorrect speed variable

	if (thres_upper<0) 			error =-2;	// incorrect threshold level
	if (thres_lower<0) 			error =-2;	// incorrect threshold level

	if (baseline<0) 			error =-1;	// incorrect baseline level
	

}



// /////////////////////////////////////////////////////////////////////////////

