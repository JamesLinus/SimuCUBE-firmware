#ifndef _COMMAND_H_
#define _COMMAND_H_

#define MAX_NORM_TORQUE		0x10000

void InitializeTorqueCommand ();
void SetTorque (s32 normalized_torque);

#endif	//_COMMAND_H_