#include "mbed.h"
#include "cFFBDevice.h"

//---------------------------------------------------------------------------------------------

void InitializeTorqueCommand()
{
}

void SetTorque (s32 normalized_torque)		// between -0x10000 and 0x10000
{
	SM_STATUS stat = smSetParameter(gFFBDevice.mSMBusHandle, 1, SMP_ABSOLUTE_SETPOINT, normalized_torque);
}
