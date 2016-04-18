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

#include "..\..\cFFBDevice.h"
#include "ffb.h"
#include "ffb_pro.h"
#include "USBGameController.h"

//------------------------------------- Defines ----------------------------------------------------------

#define LEDs_SetAllLEDs(l)

//--------------------------------------- Globals --------------------------------------------------------

// Effect management
volatile USB_FFBReport_PIDStatus_Input_Data_t pidState;	// For holding device status flags

void SendPidStateForEffect(uint8_t eid, uint8_t effectState);
void SendPidStateForEffect(uint8_t eid, uint8_t effectState)
{
	pidState.effectBlockIndex = effectState;
	pidState.effectBlockIndex = 0;
}

volatile cEffectState gEffectStates[MAX_EFFECTS+1];	// one for each effect (array index 0 is unused to simplify things)

USB_FFBReport_PIDBlockLoad_Feature_Data_t gNewEffectBlockLoad;

uint8_t GetNextFreeEffect(void);
void StartEffect(uint8_t id);
void StopEffect(uint8_t id);
void StopAllEffects(void);
void FreeEffect(uint8_t id);
void FreeAllEffects(void);

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

uint8_t GetNextFreeEffect(void)
{
	// Find the next free effect ID for next time
	for (u8 i = FIRST_EID; i <= MAX_EFFECTS; i++)
	{
		if (gEffectStates[i].state == 0)
		{
			gEffectStates[i].state = MEffectState_Allocated;
			return(i);
		}
	}
	return 0;
 }

void StopAllEffects(void)
{
	for (uint8_t id = FIRST_EID; id <= MAX_EFFECTS; id++)
		StopEffect(id);
}

void StartEffect(uint8_t id)
{
	if ((id > MAX_EFFECTS) || (gEffectStates[id].state==0))
		return;
	InitEffect(id);
	gEffectStates[id].state |= MEffectState_Playing;
}

void StopEffect(uint8_t id)
{
	if ((id > MAX_EFFECTS) || (gEffectStates[id].state == 0))
		return;
	gEffectStates[id].state &= ~MEffectState_Playing;
}

void FreeEffect(uint8_t id)
{
	if (id > MAX_EFFECTS)
		return;

	LogText("Free effect ");
	LogBinaryLf(&id,1);
	gEffectStates[id].state = 0;
}

void FreeAllEffects(void)
{
	LogTextLf("Free All effects");
 	memset((void*) gEffectStates, 0, sizeof(gEffectStates));
}

// Lengths of each report type
const uint16_t OutReportSize[] = 
{
	sizeof(USB_FFBReport_SetEffect_Output_Data_t),		// 1
	sizeof(USB_FFBReport_SetEnvelope_Output_Data_t),	// 2
	sizeof(USB_FFBReport_SetCondition_Output_Data_t),	// 3
	sizeof(USB_FFBReport_SetPeriodic_Output_Data_t),	// 4
	sizeof(USB_FFBReport_SetConstantForce_Output_Data_t),	// 5
	sizeof(USB_FFBReport_SetRampForce_Output_Data_t),	// 6
	sizeof(USB_FFBReport_SetCustomForceData_Output_Data_t),	// 7
	sizeof(USB_FFBReport_SetDownloadForceSample_Output_Data_t),	// 8
	0,	// 9
	sizeof(USB_FFBReport_EffectOperation_Output_Data_t),	// 10
	sizeof(USB_FFBReport_BlockFree_Output_Data_t),	// 11
	sizeof(USB_FFBReport_DeviceControl_Output_Data_t),	// 12
	sizeof(USB_FFBReport_DeviceGain_Output_Data_t),	// 13
	sizeof(USB_FFBReport_SetCustomForce_Output_Data_t),	// 14
};

void FfbHandle_EffectOperation(USB_FFBReport_EffectOperation_Output_Data_t *data);
void FfbHandle_BlockFree(USB_FFBReport_BlockFree_Output_Data_t *data);
void FfbHandle_DeviceControl(USB_FFBReport_DeviceControl_Output_Data_t *data);
void FfbHandle_DeviceGain(USB_FFBReport_DeviceGain_Output_Data_t *data);
void FfbHandle_SetCustomForceData(USB_FFBReport_SetCustomForceData_Output_Data_t* data);
void FfbHandle_SetDownloadForceSample(USB_FFBReport_SetDownloadForceSample_Output_Data_t* data);
void FfbHandle_SetCustomForce(USB_FFBReport_SetCustomForce_Output_Data_t* data);
void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data);

// Handle incoming data from USB
bool USBGameController::handleReceivedHIDReport(HID_REPORT report)
{
	u8 *data = report.data;
	uint8_t effectId = data[1]; // effectBlockIndex is always the second byte.
		
	switch (data[0])	// reportID
	{
	case 1:
		FfbHandle_SetEffect((USB_FFBReport_SetEffect_Output_Data_t *) data);
		break;
	case 2:
		SetEnvelope((USB_FFBReport_SetEnvelope_Output_Data_t*) data, &gEffectStates[effectId]);
		LogTextLf("Set Envelope");
		break;
	case 3:
		SetCondition((USB_FFBReport_SetCondition_Output_Data_t*) data, &gEffectStates[effectId]);
		/*
					LogText("Set Condition - offset : ");
					LogBinary(&((USB_FFBReport_SetCondition_Output_Data_t*)data)->cpOffset,1);
					LogText(" ,PositiveCoef : ");
					LogBinaryLf(&((USB_FFBReport_SetCondition_Output_Data_t*)data)->positiveCoefficient,1);
					*/
		break;
	case 4:
		SetPeriodic((USB_FFBReport_SetPeriodic_Output_Data_t*) data, &gEffectStates[effectId]);
		//			LogTextLf("Set Periodic");
		break;
	case 5:
		SetConstantForce((USB_FFBReport_SetConstantForce_Output_Data_t*) data, &gEffectStates[effectId]);
		//			LogTextLf("Set Constant Force");
		break;
	case 6:
		SetRampForce((USB_FFBReport_SetRampForce_Output_Data_t*)data, &gEffectStates[effectId]);
		LogTextLf("Set Ramp Force");
		break;
	case 7:
		FfbHandle_SetCustomForceData((USB_FFBReport_SetCustomForceData_Output_Data_t*) data);
		LogTextLf("Set Custom Force Table");
		break;
	case 8:
		FfbHandle_SetDownloadForceSample((USB_FFBReport_SetDownloadForceSample_Output_Data_t*) data);
		break;
	case 9:
		break;
	case 10:
		FfbHandle_EffectOperation((USB_FFBReport_EffectOperation_Output_Data_t*) data);
		break;
	case 11:
		FfbHandle_BlockFree((USB_FFBReport_BlockFree_Output_Data_t *) data);
		break;
	case 12:
		FfbHandle_DeviceControl((USB_FFBReport_DeviceControl_Output_Data_t*) data);
		break;
	case 13:
		FfbHandle_DeviceGain((USB_FFBReport_DeviceGain_Output_Data_t*) data);
		break;
	case 14:
		FfbHandle_SetCustomForce((USB_FFBReport_SetCustomForce_Output_Data_t*) data);
		break;
	case 129:
		gFFBDevice.mConfig.CopyProfileFrom(&data[1]);
		break;
	case 130:
		gFFBDevice.mConfig.CopyHardwareFrom(&data[1]);
		break;
	default:
		break;
	}
	return (TRUE);
}

void FfbOnCreateNewEffect (USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, USB_FFBReport_PIDBlockLoad_Feature_Data_t *outData)
{
	outData->reportId = 6;
	outData->effectBlockIndex = GetNextFreeEffect();

	if (outData->effectBlockIndex == 0)
	{
		outData->loadStatus = 2;	// 1=Success,2=Full,3=Error
		LogText("Could not create effect");
	}
	else
	{
		outData->loadStatus = 1;	// 1=Success,2=Full,3=Error
		
		volatile cEffectState* effect = &gEffectStates[outData->effectBlockIndex];
		
		CreateNewEffect(inData, effect);

		LogText("Created effect ");
		LogBinary(&outData->effectBlockIndex,1);
		LogText(", type ");
		LogBinaryLf(&inData->effectType,1);
	}
	outData->ramPoolAvailable = 0xFFFF;	// =0 or 0xFFFF - don't really know what this is used for?
//	WaitMs(5);
}

void FfbHandle_SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data)
{
	volatile cEffectState* effect = &gEffectStates[data->effectBlockIndex];
	SetEffect(data,effect);
	LogTextLf("Set Effect");
}

void FfbOnPIDPool(USB_FFBReport_PIDPool_Feature_Data_t *data)
{
	FreeAllEffects();

	data->reportId = 7;
	data->ramPoolSize = 0xFFFF;
	data->maxSimultaneousEffects = MAX_EFFECTS;
	data->memoryManagement = 3;
}

void FfbHandle_SetCustomForceData(USB_FFBReport_SetCustomForceData_Output_Data_t *data)
{
}

void FfbHandle_SetDownloadForceSample(USB_FFBReport_SetDownloadForceSample_Output_Data_t *data)
{
}

void FfbHandle_EffectOperation(USB_FFBReport_EffectOperation_Output_Data_t *data)
{
	uint8_t eid = data->effectBlockIndex;

	if (eid == 0xFF)
		eid = 0x7F;	// All effects

	if (data->operation == 1)
	{	// Start
		LogTextLf("Start Effect");
		StartEffect(eid);
	}
	else if (data->operation == 2)
	{	// StartSolo
		// Stop all first
		LogTextLf("Start Solo Effect");
		StopAllEffects();
		// Then start the given effect
		StartEffect(eid);
	}
	else if (data->operation == 3)
	{	// Stop
		LogTextLf("Stop Effect");
		StopEffect(eid);
	}
}


void FfbHandle_BlockFree (USB_FFBReport_BlockFree_Output_Data_t *data)
{
	uint8_t eid = data->effectBlockIndex;

	if (eid == 0xFF)
	{	// all effects
		FreeAllEffects();
// 		FreeEffect(0x7f); // TODO: does this work with the wheel?
	}
	else
	{
		FreeEffect(eid);
	}
}

#define DEVICE_PAUSED			0x01
#define ACTUATORS_ENABLED		0x02
#define SAFETY_SWITCH			0x04
#define ACTUATOR_OVERRIDE		0x08
#define ACTUATOR_POWER			0x10

void FfbHandle_DeviceControl(USB_FFBReport_DeviceControl_Output_Data_t *data)
{
	//	LogTextP(PSTR("Device Control: "));

	uint8_t control = data->control;
	// 1=Enable Actuators, 2=Disable Actuators, 3=Stop All Effects, 4=Reset, 5=Pause, 6=Continue

	// PID State Report:
	//	uint8_t	reportId;	// =2
	//	uint8_t	status;	// Bits: 0=Device Paused,1=Actuators Enabled,2=Safety Switch,3=Actuator Override Switch,4=Actuator Power
	//	uint8_t	effectBlockIndex;	// Bit7=Effect Playing, Bit0..7=EffectId (1..40)

	pidState.reportId = 2;
	Bset(pidState.status,SAFETY_SWITCH);
	Bset(pidState.status,ACTUATOR_POWER);
	pidState.effectBlockIndex = 0;

	switch (control)
	{
	case 0x01:
		LogTextLf("Disable Actuators");
		Bclr(pidState.status,ACTUATORS_ENABLED);
		break;
	case 0x02:
		LogTextLf("Enable Actuators");
		Bset(pidState.status,ACTUATORS_ENABLED);
		break;
	case 0x03:
		LogTextLf("Stop All Effects");		// Disable auto-center spring and stop all effects
		SetAutoCenter(0);
		pidState.effectBlockIndex = 0;
		break;
	case 0x04:
		LogTextLf("Reset");			// Reset (e.g. FFB-application out of focus)
		SetAutoCenter(1);		// Enable auto-center spring and stop all effects
//		WaitMs(75);
		FreeAllEffects();
		break;
	case 0x05:
		LogTextLf("Pause");
		Bset(pidState.status,DEVICE_PAUSED);
		break;
	case 0x06:
		LogTextLf("Continue");
		Bclr(pidState.status,DEVICE_PAUSED);
		break;
	default:
		LogTextP(PSTR("Other "));
		LogBinaryLf(&data->control,1);
	}
}

void FfbHandle_DeviceGain(USB_FFBReport_DeviceGain_Output_Data_t *data)
{
	LogTextP(PSTR("Device Gain: "));
	LogBinaryLf(&data->gain, 1);
}

void FfbHandle_SetCustomForce(USB_FFBReport_SetCustomForce_Output_Data_t *data)
{
	LogTextLf("Set Custom Force");
//	LogBinary(&data, sizeof(USB_FFBReport_SetCustomForce_Output_Data_t));
}

// ----------------------------------------------
// Debug and other settings
// ----------------------------------------------

#ifdef _DEBUG

uint8_t FfbDebugListEffects(uint8_t *index)
	{
	if (*index == 0)
		*index = 2;

//	if (*index >= nextEID)
	if (*index >= MAX_EFFECTS)
		return 0;

	cEffectState *e = (cEffectState*) &gEffectStates[*index];

	LogBinary(index, 1);
	if (e->state == MEffectState_Allocated)
		LogTextP(PSTR(" Allocated"));
	else if (e->state == MEffectState_Playing)
		LogTextP(PSTR(" Playing\n"));
	else
		LogTextP(PSTR(" Free"));

	if (e->state)
		{
		LogTextP(PSTR("  duration="));
		LogBinary(&e->duration, 2);
		LogTextP(PSTR("\n  fadeTime="));
		LogBinary(&e->fadeTime, 2);
		LogTextP(PSTR("\n  gain="));
		LogBinary(&e->gain, 1);
		}

	*index = *index + 1;

	return 1;
}
#endif
