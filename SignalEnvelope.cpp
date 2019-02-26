/*
  File:         SignalEnvelope.cpp
  Version:      0.0.8
  Date:         19-Dec-2018
  Revision:     26-Feb-2019
  Author:       Jerome Drouin (jerome.p.drouin@gmail.com)

  Editions:	Please go to SignalEnvelope.h for Edition Notes.
 
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

*/


#include "Arduino.h"
#include "SignalEnvelope.h"


// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances
SignalEnvelope::SignalEnvelope(uint8_t _speed_attack, uint8_t _speed_decay, int _operation)
{

	//Set initial values	
	speed_decay		= _speed_decay;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	speed_attack		= _speed_attack;		// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	envelope_up		= 0.0;
	envelope_lo		= 0.0;

	// Object parameter's error handling
	ResetErrors();
}



SignalEnvelope::SignalEnvelope(uint8_t _speed_attack, uint8_t _speed_decay, int _operation, float _thres_upper, float _thres_lower)
{

	//Set initial values	
	speed_decay		= _speed_decay;			// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128
	speed_attack		= _speed_attack;		// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128

	operation		= _operation;			//0=Upper, 1=Lower, 2=Double

	envelope_up		= _thres_upper;
	envelope_lo		= _thres_lower;

	// Object parameter's error handling
	ResetErrors();
}


// Public Methods //////////////////////////////////////////////////////////////
// Returns the Version
String SignalEnvelope::GetVersion()
{
	return VER_SignalEnvelope;
}


// Returns the Release Date
String SignalEnvelope::GetReleaseDate()
{
	return REL_SignalEnvelope;
}


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

// Update envelope level with the Requested available envelope level
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



//Set the envelope speed_decay
void SignalEnvelope::SetSpeedDecay(uint8_t _speed_decay)
{

	//Set initial values	
	speed_decay		= _speed_decay;		// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128

	// Object parameter's error handling
	ResetErrors();
}


//Set the envelope speed_attack
void SignalEnvelope::SetSpeedAttack(uint8_t _speed_attack)
{

	//Set initial values	
	speed_attack		= _speed_attack; 	// Speeds from Fastest-Slowest: // 2, 4, 8, 16, 32, 64, 128

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



//Get the speed_decay parameter
float SignalEnvelope::GetSpeedDecay(void)
{
	return speed_decay;	
}

//Get the speed_attack parameter
float SignalEnvelope::GetSpeedAttack(void)
{
	return speed_attack;	
}



// Private Methods /////////////////////////////////////////////////////////////

// Functions only available to other functions in this library
// Update Up Envelope
void SignalEnvelope::CalculateEnvelope_Up(float rawSignal)
{
float decay;
float attack;

	// Update envelope_up
	if (rawSignal>envelope_up) {
		
		attack	     = (rawSignal - envelope_up) / speed_attack; 	
		envelope_up += attack;					// Attack
		
	} else {
		decay  	     = (envelope_up - rawSignal) / speed_decay;
		envelope_up -= decay;	 				// Decay
	}

}


// Update Lo Envelope
void SignalEnvelope::CalculateEnvelope_Lo(float rawSignal)
{
float decay;
float attack;

	// Update envelope_lo
	if (rawSignal<envelope_lo) {
		attack	     = (rawSignal - envelope_lo) / speed_attack; 	
		envelope_lo += attack;					// Attack

	} else {		
		decay  	     = (envelope_lo - rawSignal) / speed_decay;
		envelope_lo -= decay;	 				// Decay
	}

}


// Calculates Envelope
void SignalEnvelope::CalculateEnvelope(float rawSignal)
{

	//Envelope_up
	if (operation==0) {
		CalculateEnvelope_Up(rawSignal);

	} else 


	//Envelope_lo
	if (operation==1) {
		CalculateEnvelope_Lo(rawSignal);

	} else 

	if (operation==2) {
		CalculateEnvelope_Up(rawSignal);
		CalculateEnvelope_Lo(rawSignal);

	}

}


//Reset error flag following importance hierarchy
//by increasing error importance:
//	- Operation	: -5
//	- Speed		: -4
//	Note: Object instanciated with incorrect Operation mode and incorrect speed value
//	will return -4 (first), and then -5 only after speed conflict has been resolved.
//	
void SignalEnvelope::ResetErrors(void) 
{

	// Object parameter's error handling
	error = 1;

	if ((operation!=0) 
	 && (operation!=1)
	 && (operation!=2)) 			error =-5;	// incorrect _operation mode

	if (speed_decay<MINSPEEDPARAM) 		error =-4;	// incorrect speed_decay variable
	if (speed_decay>MAXSPEEDPARAM) 		error =-4;	// incorrect speed_decay variable

	if (speed_attack<MINSPEEDPARAM) 	error =-3;	// incorrect speed_attack variable
	if (speed_attack>MAXSPEEDPARAM) 	error =-3;	// incorrect speed_attack variable
	

}



// /////////////////////////////////////////////////////////////////////////////

