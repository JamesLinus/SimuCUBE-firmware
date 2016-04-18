#ifndef _FFB_PRO_
#define _FFB_PRO_

#include <stdint.h>
#include "ffb.h"

void SetAutoCenter(uint8_t enable);

void InitEffect(uint8_t id);

void ModifyDuration(uint8_t effectId, uint16_t duration);

void SetEnvelope(USB_FFBReport_SetEnvelope_Output_Data_t* data, volatile cEffectState* effect);
void SetCondition(USB_FFBReport_SetCondition_Output_Data_t* data, volatile cEffectState* effect);
void SetPeriodic(USB_FFBReport_SetPeriodic_Output_Data_t* data, volatile cEffectState* effect);
void SetConstantForce(USB_FFBReport_SetConstantForce_Output_Data_t* data, volatile cEffectState* effect);
void SetRampForce(USB_FFBReport_SetRampForce_Output_Data_t* data, volatile cEffectState* effect);
void SetEffect(USB_FFBReport_SetEffect_Output_Data_t *data, volatile cEffectState* effect);
void CreateNewEffect(USB_FFBReport_CreateNewEffect_Feature_Data_t* inData, volatile cEffectState* effect);

#endif // _FFB_PRO_
