
#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////
//TYPES ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

#ifndef u32
typedef unsigned long u32;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef u8
typedef unsigned char u8;
#endif

#ifndef s32
typedef signed long s32;
#endif
#ifndef s16
typedef signed short s16;
#endif
#ifndef s8
typedef signed char s8;
#endif
#ifndef b8
typedef u8 b8;
#endif
#ifndef b16
typedef u16 b16;
#endif
#ifndef b32
typedef u32 b32;
#endif
#ifndef f32
typedef float				f32;
#endif
#ifndef f64
typedef double				f64;
#endif
#ifndef s64
typedef signed long long	s64;
#endif
#ifndef u64
typedef unsigned long long	u64;
#endif
#ifndef FALSE
#define FALSE	false
#endif
#ifndef TRUE
#define TRUE	true
#endif

#ifndef NULL
#define NULL 0
#endif

	/*Defines for Bset etc directives....*/
#define Bset(data,val) data|=(val)
#define Bclr(data,val) data&=~(val)
#define Btest(data,val) ((data&(val))==(val))
#define Bchg(data,val) if (Btest(data,val)) Bclr(data,val); else Bset(data,val)
#define Bmov(data,data1,val) if (Btest(data,val)) Bset(data1,val); else Bclr(data1,val)

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
//#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

//for simple motion param ranges
#define S32MAX 2147483647L
#define S32MIN -2147483648L
#define S30MAX 536870911L
#define S30MIN -536870912L
#define S22MAX 2097151L
#define S22MIN -2097152L
#define S16MAX 32767
#define S16MIN -32768
#define S14MAX 8191L
#define S14MIN -8192L

#define U32MAX 4294967295UL
#define U32MIN 0UL
#define U16MAX 65535U
#define U16MIN 0



//C++ like type castings. ie U32(variable) = (u32)variable
#define U8(v) ((u8)(v))
#define U16(v) ((u16)(v))
#define U32(v) ((u32)(v))
#define S8(v) ((s8)(v))
#define S16(v) ((s16)(v))
#define S32(v) ((s32)(v))

typedef  u8 SMP_CMD_STATUS_TYPE;

/* Header of STM32 main app. This struct is used to read some constants from main app flash space.
 * These values are checked by bootloader before jumping to main app to ensure that wrong code wont be executed
 * This struct is always located in same mem address in main app FW so bootloader finds it */
typedef struct
{
	u32 Checksum; /*flash checksum, replaced by GD modified srec2bin with correct checksum */
	u32 BinaryLength; /*size of flashed FW in bytes */
	u32 TargetHardwareID; /*hardware id where this FW is intended */
	/*note the order of these min/max are swapped in header&binary afther HC model introduced, as comparison was done wrong in BL:*/
	u32 TargetHardwareIDMax; /*The highest HW version that has been supported by this FW. Set this as the current HW version which is used on FW development*/
	u32 TargetHardwareIDMin; /*The lowest HW version which is supported by this FW*/
	u32 FWBackwardsCompVersion; /*The lowest FW version which backwards compatible to this version*/
	u32 FWVersion; /*current FW version*/
	u32 Serial; /*ser nr programmed in fab*/
} FWHeader;

#define BLHEADER_ADDRESS 0x08000190

/* These values are stored in bootloader code as constants in fixed location 0x08000184
 * main app may read these from the location to see underlying HW version etc. */
typedef struct {
	u32 Checksum; /*BL program area flash checksum, replaced by GD modified srec2bin with correct checksum */
	u32 BinaryLength; /*size of used BL area in bytes */
	u32 HardwareID_obsolete; /*hardware id http://granitedevices.com/wiki/Hardware_version_ID *//*obsolete, read use type ID in OTP instead*/
	u32 BLFWversion; /*bootloader version*/
	u32 BLFWCompatVersion;/*bootloader version compatible with old version number*/
	u32 Reserved; /*for future use*/
} BLHeader;

/*this is a state transfer struct from BL to main app stored in dedicated space in ram.
 * BL writes this and App reads it because otherwise sampling is not simutaneous and leds interfere in mobo
 */
typedef struct {
	u32 key1; /* app verifies validity of data, this must be 0x12345678 */
	s32 addrsel1voltage; /*in mV steps*/
	s32 addrsel2voltage; /*in mV steps*/
	s32 addrsel1;/*0 to 3*/
	s32 addrsel2;/*0 to 3*/
	u32 bootloaderStartMode;/*written by main app 0x43217979 if bl should stay in BL mode, also key1&2 filled*/
	u32 miscBits;/*lowest 3 bits values mean wh type and model: 0=ioni 1=ionipro 2=ioni hc 3=ionihcpro*/
	u32 key2; /* app verifies validity of data, this must be 0x65656565 */
} BLToAppTransferStructType;


/* These values are stored in bootloader flash at address 0x08007200 and max size is 2kb. only write access is allowed here so its OTP memory
 * struct size must be in 32 bit boundary
 */
#define USAGE_TIME_COUNTER_SIZE 512

typedef struct {
	u32 deviceModel;/*see IONI_xxx_MODEL defines*/
	u32 serial;/*starting from 110000000*/
	u32 mcu_uid_check;/*mcu uid is compared to this*/
	u32 testing_date;/*unix timestamp*/
	u32 fw_uid[2];/*unique randon numbers generated by testing app, stored in test db*/
	u32 reserved[8];
	u16 lock_otp;/*if !=ffff then writing to this struct via BL is no nonger possible*/
	u16 permanent_log[256]; /*such as fatal faults info*/
	u16 usage_time_counter[USAGE_TIME_COUNTER_SIZE]; /*write usage time in minutes in these registers on each reset or power down*/
} FirmwareOTP;

#define FIRMWARE_OTP_ADDRESS (0x08005800)
#define FIRMWARE_OTP ((FirmwareOTP *) FIRMWARE_OTP_ADDRESS)

#define IONI_STD_MODEL 11000
#define IONI_PRO_MODEL 11001
#define IONI_STD_HC_MODEL 11200
#define IONI_PRO_HC_MODEL 11201
//FW will compare underlying HWID and decide whether it runs on it or not:
#define FW_SUPPORT_HW_MODEL_MIN 11000
#define FW_SUPPORT_HW_MODEL_MAX 11202


/*
 * The struct is located just behind vector table at flash.
 *
 * The Struct start is 0x184 from beginning of program flash (+start offset, i.e. 0x08000184 for zero offset [or 0x08008184 for 32kb offset = WTF?])
 */
#if 0
extern const volatile BLHeader VSDR_BL_Header __attribute__((section(".firmware_consts"))) = {
		0xc7ecc7ec, /*bootloader checksum, TODO actually use this!*/
		0x0, /*bootloader length in bytes */
		DEVICE_TYPE_HW_ID,
		90,/*hw version*/
		1250,/*bl fw version*/
		1200,/*bl fw compat version*/
		0xFFFFFFFF/*reserved*/
};
#endif


//SM payload command structure
typedef struct {
	/* ID=0 param size 30 bits (cmd total 4 bytes)
	 * ID=1 param size 22 bits (cmd total 3 bytes)
	 * ID=2 set parameter store address, param: 14 bits=register address  (cmd total 2 bytes)
	 * ID=3 reserved
	 */
	u8 ID :2;
	s32 param :30;
	bool discardRetData; //if true, return data will not be inserted in ret queue
} __attribute__ ((packed)) SMPayloadCommandForQueue;


typedef struct {
	/* ID=0 param size 30 bits (cmd total 4 bytes)
	 * ID=1 param size 22 bits (cmd total 3 bytes)
	 * ID=2 set parameter store address, param: 14 bits=register address  (cmd total 2 bytes)
	 * ID=3 reserved
	 */
	s32 param :30; //LSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommand32;

typedef struct {
	/* ID=0 param size 30 bits (cmd total 4 bytes)
	 * ID=1 param size 22 bits (cmd total 3 bytes)
	 * ID=2 set parameter store address, param: 14 bits=register address  (cmd total 2 bytes)
	 * ID=3 reserved
	 */
	s32 param :14; //LSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommand16;

typedef struct {
	/* ID=0 param size 30 bits (cmd total 4 bytes)
	 * ID=1 param size 22 bits (cmd total 3 bytes)
	 * ID=2 set parameter store address, param: 14 bits=register address  (cmd total 2 bytes)
	 * ID=3 reserved
	 */
	s32 param :22; //MSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommand24;

//SM payload command return data structure
typedef struct {
	/* ID=0 ret data 30 bits (tot 4 bytes)
	 * ID=1 ret data 22 bits (tot 3 bytes)
	 * ID=2 ret data 16 bits (tot 2 bytes), i.e. ACK/NACK.
	 * ID=3 reserved
	 */
	s32 retData: 30; //LSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommandRet32;
//SM payload command return data structure
typedef struct {
	/* ID=0 ret data 30 bits (tot 4 bytes)
	 * ID=1 ret data 22 bits (tot 3 bytes)
	 * ID=2 ret data 16 bits (tot 2 bytes), i.e. ACK/NACK.
	 * ID=3 reserved
	 */
	s32 retData: 22; //LSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommandRet24;
//SM payload command return data structure
typedef struct {
	/* ID=0 ret data 30 bits (tot 4 bytes)
	 * ID=1 ret data 22 bits (tot 3 bytes)
	 * ID=2 ret data 16 bits (tot 2 bytes), i.e. ACK/NACK.
	 * ID=3 reserved
	 */
	s32 retData: 14; //LSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommandRet16;
//SM payload command return data structure
typedef struct {
	/* ID=0 ret data 30 bits (tot 4 bytes)
	 * ID=1 ret data 22 bits (tot 3 bytes)
	 * ID=2 ret data 16 bits (tot 2 bytes), i.e. ACK/NACK.
	 * ID=3 reserved
	 */
	s32 retData: 6; //LSB 30 bits
	u8 ID:2; //MSB 2 bits. when serailzied to bytestream byte4 must be transmitted first to contain ID
} __attribute__ ((packed)) SMPayloadCommandRet8;



#ifdef __cplusplus
}
#endif

#endif
