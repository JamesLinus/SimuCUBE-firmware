/*
  Force Feedback Joystick

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  Copyright 2013  Saku Kekkonen
  Copyright 2016  Etienne Saint-Paul (esaintpaul [at] gameseed [dot] fr)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "stdint.h"
#include "types.h"
#include "ffb.h"
#include "ffb_pro.h"
#include "debug.h"
#include "USBGameController.h"

//--------------------------------------------------------------------------------------------------

void SetAutoCenter(uint8_t enable)
{
}

// --------------------------- effect operations ---------------------------------------------------

void InitEffect(uint8_t effectId)
{
	volatile cEffectState* effect = &gEffectStates[effectId];
	effect->counter = 0;
	effect->fcounter = 0;
	effect->fade_start_time = effect->duration - effect->fadeTime;
}

// modify operations ---------------------------------------------------------

void ModifyDuration(uint8_t effectId, uint16_t duration)
{
	volatile cEffectState* effect = &gEffectStates[effectId];
	effect->duration = duration;
	effect->counter = 0;
	effect->fade_start_time = duration - effect->fadeTime;
}

void SetEnvelope (USB_FFBReport_SetEnvelope_Output_Data_t* data,volatile cEffectState* effect)
{
	effect->attackLevel = data->attackLevel;
	effect->fadeLevel = data->fadeLevel;
	effect->attackTime = data->attackTime;
	effect->fadeTime = data->fadeTime;
}

void SetCondition (USB_FFBReport_SetCondition_Output_Data_t* data,volatile cEffectState* effect)
{
	effect->magnitude = (s16)data->positiveCoefficient;
	effect->offset = data->cpOffset;
	effect->positiveSaturation = data->positiveSaturation;
	effect->negativeSaturation = data->negativeSaturation;
}

void SetPeriodic (USB_FFBReport_SetPeriodic_Output_Data_t* data,volatile cEffectState* effect)
{
	effect->magnitude = (s16)data->magnitude;
	effect->offset = data->offset;
	effect->phase = (s16)data->phase;
	effect->period = (u16)data->period;
	effect->freq = 0.0f;
}

void SetConstantForce (USB_FFBReport_SetConstantForce_Output_Data_t* data,volatile cEffectState* effect)
{
	effect->magnitude = data->magnitude;
}

void SetRampForce (USB_FFBReport_SetRampForce_Output_Data_t* data,volatile cEffectState* effect)
{
	effect->start_mag = data->start;
	effect->end_mag= data->end;
}

void SetEffect (USB_FFBReport_SetEffect_Output_Data_t *data,volatile cEffectState* effect)
{
	uint8_t eid = data->effectBlockIndex;
	effect->type = data->effectType;
	effect->gain = data->gain;
	ModifyDuration(eid,data->duration); // 0..32767 ms

//	bool is_periodic = false;

	// Fill in the effect type specific data
	switch (data->effectType)
	{
		case USB_EFFECT_SQUARE:
		case USB_EFFECT_SINE:
		case USB_EFFECT_TRIANGLE:
		case USB_EFFECT_SAWTOOTHDOWN:
		case USB_EFFECT_SAWTOOTHUP:
//			is_periodic = true;
		case USB_EFFECT_CONSTANT:
		case USB_EFFECT_RAMP:
		break;
	
		case USB_EFFECT_SPRING:
		case USB_EFFECT_DAMPER:
		case USB_EFFECT_INERTIA:
		break;
		
		case USB_EFFECT_FRICTION:
		break;
		
		case USB_EFFECT_CUSTOM:	
			effect->period = data->samplePeriod;	// 0..32767 ms
		break;
		
		default:
		break;
	}
}

void CreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData,volatile cEffectState* effect)
{
	effect->type = inData->effectType;
	effect->gain = 0xFF;
	effect->attackLevel = 0xFF;
	effect->fadeLevel = 0xFF;

	effect->positiveSaturation = 0;
	effect->negativeSaturation = 0;
	effect->magnitude = 0;
	effect->phase = 0;
	effect->offset = 0;
	effect->period = 100;
	effect->duration = USB_DURATION_INFINITE;
	effect->fadeTime = USB_DURATION_INFINITE;
}
