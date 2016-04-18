/*
  Force Feedback Joystick
  Basic debugging utilities.

  This code is for Microsoft Sidewinder Force Feedback Pro joystick
  with some room for additional extra controls.

  Copyright 2012  Tero Loimuneva (tloimu [at] gmail [dot] com)
  MIT License.

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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#define PSTR(m_str)		m_str

/*// Method of debugging
extern const u8 DEBUG_TO_NONE;
extern const u8 DEBUG_TO_UART;
extern const u8 DEBUG_TO_USB;
extern const u8 DEBUG_DETAIL;
*/
extern volatile u8 gDebugMode;

//#define _DEBUG_LOG

// Returns true if debug settings contain all of the given attributes
// (see above constants DEBUG_xxx).
b8 DoDebug(const u8 type);

// If below are defined, code for respective debug target is included into build
//#define DEBUG_ENABLE_UART
//#define DEBUG_ENABLE_USB

#define DEBUG_BUFFER_SIZE 512

// Debugging utilities

// The basic debug data sending method used by all other methods.
// Implement this to send debug data to desired destination.
void LogSendByte(u8 data);

// Send out the given null terminated text
void LogText(const char *text);
void LogTextLf(const char *text);	// Adds linefeed
void LogTextP(const char *text);	// From program memory
void LogTextLfP(const char *text);	// From program memory, adds linefeed

// Send out the given binary data
void LogBinary(const void *data,u16 len);
void LogBinaryLf(const void *data,u16 len);	// Adds linefeed

// Send out data with a prefix of text and an integer
void LogData(const char *text,u8 reportId,const void *data,u16 len);
void LogDataLf(const char *text,u8 reportId,const void *data,u16 len);	// Adds linefeed

// Log all reports found in the given data (may have one or more)
// The <text> must point to string in program memory.
void LogReport(const char *text,const u16 *reportSizeArray,u8 *data,u16 len);

// Debugging utils for USB-serial debugging
void FlushDebugBuffer(void);

#ifdef USE_LCD
#define DEBUG_PIPE		lcd
#else
#define DEBUG_PIPE		Serial
#endif

//#define _DEBUG

#ifdef _DEBUG
#define DEBUG_SETUP
//#define DEBUG_LCD
//#define DEBUG_FFB
//#define DEBUG_CALIBRATION
#define DEBUG_INPUTS
//#define DEBUG_SIMPLE_MOTION
//#define DEBUG_COMMAND
//#define DEBUG_FFB
//#define DEBUG_PARAMS
#endif

#ifdef DEBUG_CALIBRATION
#define cal_print(str)			DEBUG_PIPE.print(str)
#define cal_println(str)		DEBUG_PIPE.println(str)
#else
#define cal_print(str)
#define cal_println(str)
#endif

#ifdef DEBUG_SETUP
#define setup_print(str)		DEBUG_PIPE.print(str)
#define setup_println(str)		DEBUG_PIPE.println(str)
#else
#define setup_print(str)
#define setup_println(str)
#endif

#ifdef DEBUG_INPUTS
#define inputs_print(str)		DEBUG_PIPE.print(str)
#define inputs_print_hex(str)	DEBUG_PIPE.print(str,HEX)
#define inputs_println(str)		DEBUG_PIPE.println(str)
#else
#define inputs_print(str)
#define inputs_print_hex(str)
#define inputs_println(str)
#endif

#ifdef DEBUG_SIMPLE_MOTION
#define sm_print(str)			DEBUG_PIPE.print(str)
#define sm_println(str)			DEBUG_PIPE.println(str)
#else
#define sm_print(str)
#define sm_println(str)
#endif

#ifdef DEBUG_COMMAND
#define cmd_print(str)			DEBUG_PIPE.print(str)
#define cmd_println(str)		DEBUG_PIPE.println(str)
#else
#define cmd_print(str)
#define cmd_println(str)
#endif

#ifdef DEBUG_FFB
#define ffb_print(str)			DEBUG_PIPE.print(str)
#define ffb_println(str)		DEBUG_PIPE.println(str)
#else
#define ffb_print(str)
#define ffb_println(str)
#endif

#ifdef DEBUG_PARAMS
#define param_print(str)		DebugParamPrint(str)
#else
#define param_print(str)
#endif

//------------------------------------- LEDs/DebugSignals -----------------------------------------------

#define LED_SIGNAL			0
#define	LCSYNC_SIGNAL		1
#define	CTRL_SIGNAL			2
#define	SEND_SIGNAL			3

#ifdef LED_SIGNAL
#define	SET_LED(m_val)		SetStatusSignal(LED_SIGNAL,m_val)
#else
#define	SET_LED(m_val)
#endif
#define	LED_HIGH()			SET_LED(HIGH)
#define	LED_LOW()			SET_LED(LOW)

#ifdef LCSYNC_SIGNAL
#define	SET_LCSYNC_SIGNAL(m_val)	SetStatusSignal(LCSYNC_SIGNAL,m_val)
#else
#define	SET_LCSYNC_SIGNAL(m_val)
#endif
#define	LCSYNC_LED_HIGH()		SET_LCSYNC_SIGNAL(HIGH)
#define	LCSYNC_LED_LOW()		SET_LCSYNC_SIGNAL(LOW)


#ifdef CTRL_SIGNAL
#define	SET_CTRL_SIGNAL(m_val)		SetStatusSignal(CTRL_SIGNAL,m_val)
#else
#define	SET_CTRL_SIGNAL(m_val)
#endif
#define	CTRL_LED_HIGH()			SET_CTRL_SIGNAL(HIGH)
#define	CTRL_LED_LOW()			SET_CTRL_SIGNAL(LOW)

#ifdef SEND_SIGNAL
#define	SET_SEND_SIGNAL(m_val)		SetStatusSignal(SEND_SIGNAL,m_val)
#else
#define	SET_SEND_SIGNAL(m_val)
#endif
#define	SEND_LED_HIGH()			SET_SEND_SIGNAL(HIGH)
#define	SEND_LED_LOW()			SET_SEND_SIGNAL(LOW)

#endif // _DEBUG_H_
