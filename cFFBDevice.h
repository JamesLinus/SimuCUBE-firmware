#ifndef _CFFB_DEVICE_H_
#define _CFFB_DEVICE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "debug.h"
#include "Effects.h"
#include "Command.h"
#include "SimpleMotion/simplemotion.h"

_Pragma("pack(1)")
class cProfileConfig
{
public:
	s32 mMaxAngle;

	u8 mFlags;
	u8 mNbTaps;

	s8 mMainGain;
	s8 mSpringGain;
	s8 mFrictionGain;
	s8 mDamperGain;
	s8 mInertiaGain;
	s8 mConstantGain;
	s8 mPeriodicGain;
	s8 mForceTableGain;
};

class cHardwareConfig
{
public:
	s32 mOffset;

	s8 mDesktopSpringGain;
	s8 mDesktopDamperGain;

	s8 mStopsSpringGain;
	s8 mStopsFrictionGain;
};

class cDeviceConfig
{
public:
	cDeviceConfig()	{ SetDefault(); }
	
	void SetDefault();

	u8 *GetProfileConfigAddr()			{ return ((u8*)&mMaxAngle); }
	u8 *GetHardwareConfigAddr()			{ return ((u8*)&mOffset); }

	void CopyProfileFrom(u8 *conf)		{ memcpy(GetProfileConfigAddr(), conf, sizeof(cProfileConfig)); }
	void CopyProfileTo(u8 *conf)		{ memcpy(conf, GetProfileConfigAddr(), sizeof(cProfileConfig)); }
	void CopyHardwareFrom(u8 *conf)		{ memcpy(GetHardwareConfigAddr(), conf, sizeof(cHardwareConfig)); }
	void CopyHardwareTo(u8 *conf)		{ memcpy(conf, GetHardwareConfigAddr(), sizeof(cHardwareConfig)); }
	
	s32 mMaxAngle;							// Profile config

	u8 mFlags;
	u8 mNbTaps;

	s8 mMainGain;
	s8 mSpringGain;
	s8 mFrictionGain;
	s8 mDamperGain;
	s8 mInertiaGain;
	s8 mConstantGain;
	s8 mPeriodicGain;
	s8 mForceTableGain;

	s32 mOffset;							// Hardware config

	s8 mDesktopSpringGain;
	s8 mDesktopDamperGain;

	s8 mStopsSpringGain;
	s8 mStopsFrictionGain;

	u32 mVersion;
};
_Pragma("pack()")

	
class cFFBDevice
{
public:
	cFFBDevice()		{ SetDefault();	}

	void SetDefault()	{ mConfig.SetDefault(); }

	s32 CalcTorqueCommand(s32 pos);
	
	cDeviceConfig mConfig;

	b8 mAutoCenter;
	smbus mSMBusHandle;
};

extern cFFBDevice gFFBDevice;

#endif	//_CFFB_DEVICE_H_