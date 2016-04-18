/* USBJoystick.h */
/* USB device example: Joystick*/
/* Copyright (c) 2011 ARM Limited. All rights reserved. */
/* Modified Mouse code for Joystick - WH 2012 */

#ifndef USBGAMECONTROLLER_H
#define USBGAMECONTROLLER_H

#include "USBHID.h"
#include "ffb.h"

#define REPORT_ID_JOYSTICK  4

// bmRequestType
#define REQUEST_HOSTTODEVICE	0x00
#define REQUEST_DEVICETOHOST	0x80
#define REQUEST_DIRECTION		0x80

#define REQUEST_STANDARD		0x00
#define REQUEST_CLASS			0x20
#define REQUEST_VENDOR			0x40
#define REQUEST_TYPE			0x60

#define REQUEST_DEVICE			0x00
#define REQUEST_INTERFACE		0x01
#define REQUEST_ENDPOINT		0x02
#define REQUEST_OTHER			0x03
#define REQUEST_RECIPIENT		0x03

#define REQUEST_DEVICETOHOST_CLASS_INTERFACE  (REQUEST_DEVICETOHOST + REQUEST_CLASS + REQUEST_INTERFACE)
#define REQUEST_HOSTTODEVICE_CLASS_INTERFACE  (REQUEST_HOSTTODEVICE + REQUEST_CLASS + REQUEST_INTERFACE)

//	Class requests

#define CDC_SET_LINE_CODING			0x20
#define CDC_GET_LINE_CODING			0x21
#define CDC_SET_CONTROL_LINE_STATE	0x22

#define MSC_RESET					0xFF
#define MSC_GET_MAX_LUN				0xFE

#define HID_GET_REPORT				0x01
#define HID_GET_IDLE				0x02
#define HID_GET_PROTOCOL			0x03
#define HID_SET_REPORT				0x09
#define HID_SET_IDLE				0x0A
#define HID_SET_PROTOCOL			0x0B

#define NB_AXIS			8
#define NB_FF_AXIS		1

#define X_AXIS_NB_BITS	16
#define Y_AXIS_NB_BITS	16
#define Z_AXIS_NB_BITS	16
#define RX_AXIS_NB_BITS	16
#define RY_AXIS_NB_BITS	16
#define RZ_AXIS_NB_BITS	16
#define SX_AXIS_NB_BITS	16
#define SY_AXIS_NB_BITS	16
#define NB_BUTTONS		32

#define X_AXIS_LOG_MAX		((1L<<(X_AXIS_NB_BITS))-1)
#define X_AXIS_LOG_MIN		0//(-X_AXIS_LOG_MAX)
#define X_AXIS_PHYS_MAX		((1L<<X_AXIS_NB_BITS)-1)

#define Y_AXIS_LOG_MAX		((1L<<(Y_AXIS_NB_BITS))-1)
#define Y_AXIS_LOG_MIN		0//(-Y_AXIS_LOG_MAX)
#define Y_AXIS_PHYS_MAX		((1L<<Y_AXIS_NB_BITS)-1)

#define Z_AXIS_LOG_MAX		((1L<<(Z_AXIS_NB_BITS))-1)
#define Z_AXIS_LOG_MIN		0//(-Z_AXIS_LOG_MAX)
#define Z_AXIS_PHYS_MAX		((1L<<Z_AXIS_NB_BITS)-1)

#define RX_AXIS_LOG_MAX		((1L<<(RX_AXIS_NB_BITS))-1)
#define RX_AXIS_LOG_MIN		0//(-RX_AXIS_LOG_MAX)
#define RX_AXIS_PHYS_MAX	((1L<<RX_AXIS_NB_BITS)-1)

#define RY_AXIS_LOG_MAX		((1L<<(RY_AXIS_NB_BITS))-1)
#define RY_AXIS_LOG_MIN		0//(-RY_AXIS_LOG_MAX)
#define RY_AXIS_PHYS_MAX	((1L<<RY_AXIS_NB_BITS)-1)

#define RZ_AXIS_LOG_MAX		((1L<<(RZ_AXIS_NB_BITS))-1)
#define RZ_AXIS_LOG_MIN		0//(-RZ_AXIS_LOG_MAX)
#define RZ_AXIS_PHYS_MAX	((1L<<RZ_AXIS_NB_BITS)-1)

#define SX_AXIS_LOG_MAX		((1L<<(SX_AXIS_NB_BITS))-1)
#define SX_AXIS_LOG_MIN		0//(-SX_AXIS_LOG_MAX)
#define SX_AXIS_PHYS_MAX	((1L<<SX_AXIS_NB_BITS)-1)

#define SY_AXIS_LOG_MAX		((1L<<(SY_AXIS_NB_BITS))-1)
#define SY_AXIS_LOG_MIN		0//(-SY_AXIS_LOG_MAX)
#define SY_AXIS_PHYS_MAX	((1L<<SY_AXIS_NB_BITS)-1)

#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

/* Common usage */
enum JOY_BUTTON {
     JOY_B0 = 1,
     JOY_B1 = 2,
     JOY_B2 = 4,
     JOY_B3 = 8,
};

#if(0)
enum JOY_HAT {
     JOY_HAT_UP      = 0,
     JOY_HAT_RIGHT   = 1,
     JOY_HAT_DOWN    = 2,
     JOY_HAT_LEFT    = 3,
     JOY_HAT_NEUTRAL = 4,
};
#else
enum JOY_HAT {
     JOY_HAT_UP         = 0,
     JOY_HAT_UP_RIGHT   = 1,
     JOY_HAT_RIGHT      = 2,
     JOY_HAT_RIGHT_DOWN = 3,
     JOY_HAT_DOWN       = 4,
     JOY_HAT_DOWN_LEFT  = 5,
     JOY_HAT_LEFT       = 6,
     JOY_HAT_LEFT_UP    = 7,
     JOY_HAT_NEUTRAL    = 8,
};
#endif

/* X, Y and T limits */
/* These values do not directly map to screen pixels */
/* Zero may be interpreted as meaning 'no movement' */
#define JX_MIN_ABS    (-127)     /*!< The maximum value that we can move to the left on the x-axis */
#define JY_MIN_ABS    (-127)     /*!< The maximum value that we can move up on the y-axis */
#define JT_MIN_ABS    (-127)     /*!< The minimum value for the throttle */
#define JX_MAX_ABS    (127)      /*!< The maximum value that we can move to the right on the x-axis */
#define JY_MAX_ABS    (127)      /*!< The maximum value that we can move down on the y-axis */
#define JT_MAX_ABS    (127)      /*!< The maximum value for the throttle */

/**
 *
 * USBJoystick example
 * @code
 * #include "mbed.h"
 * #include "USBJoystick.h"
 *
 * USBJoystick joystick;
 *
 * int main(void)
 * {
 *   while (1)
 *   {
 *      joystick.move(20, 0);
 *      wait(0.5);
 *   }
 * }
 *
 * @endcode
 *
 *
 * @code
 * #include "mbed.h"
 * #include "USBJoystick.h"
 * #include <math.h>
 *
 * USBJoystick joystick;
 *
 * int main(void)
 * {
 *   int16_t i = 0;
 *   int16_t throttle = 0;
 *   int16_t rudder = 0;
 *   int16_t x = 0;
 *   int16_t y = 0;
 *   int32_t radius = 120;
 *   int32_t angle = 0;
 *   int8_t button = 0;
 *   int8_t hat = 0;
 *
 *   while (1) {
 *       // Basic Joystick
 *       throttle = (i >> 8) & 0xFF; // value -127 .. 128
 *       rudder = (i >> 8) & 0xFF;   // value -127 .. 128
 *       button = (i >> 8) & 0x0F;   // value    0 .. 15, one bit per button
 *        hat    = (i >> 8) & 0x07;   // value 0..7 or 8 for neutral
 *       i++;
 *
 *       x = cos((double)angle*3.14/180.0)*radius;  // value -127 .. 128
 *       y = sin((double)angle*3.14/180.0)*radius;  // value -127 .. 128
 *       angle += 3;
 *
 *       joystick.update(throttle, rudder, x, y, button, hat);
 *
 *       wait(0.001);
 *   }
 * }
 * @endcode
 */


#define RX_REPORT_BUFFER_COUNT 32//this must be power of 2

class USBGameController: public USBHID {
   public:

        /**
         *   Constructor
         *
         * @param vendor_id Your vendor_id (default: 0x1234)
         * @param product_id Your product_id (default: 0x0002)
         * @param product_release Your product_release (default: 0x0001)
         */
         USBGameController(uint16_t vendor_id = 0x7c5a, uint16_t product_id = 0xb101, uint16_t product_release = 0x0001):
             USBHID(0, 0, vendor_id, product_id, product_release, false),receivedReportBufferHead(0),FFBEnabled(false),receivedReportBufferTail(0)
             {
                 _init();
                 connect();
             };

         /**
         * Write a state of the game controller
         *
         * @param 4 pedals first
         * @param x x-axis position
         * @param y y-axis position
         * @param buttons buttons state
         * @param hat hat state 0 (up), 1 (right, 2 (down), 3 (left) or 4 (neutral)
         * @returns true if there is no error, false otherwise
         */
         bool update(uint16_t brake, uint16_t clutch, uint16_t throttle, uint16_t rudder, uint16_t x, uint16_t y, uint32_t button, uint8_t hat);

         /**
         * Write a state of the mouse
         *
         * @returns true if there is no error, false otherwise
         */
         bool update();

         /**
         * Move the throttle position
         *
         * @param t throttle position
         * @returns true if there is no error, false otherwise
         */
         bool throttle(int16_t t);

         /**
         * Move the rudder position
         *
         * @param r rudder position
         * @returns true if there is no error, false otherwise
         */
         bool rudder(int16_t r);

         /**
         * Move the cursor to (x, y)
         *
         * @param x-axis position
         * @param y-axis position
         * @returns true if there is no error, false otherwise
         */
         bool move(int16_t x, int16_t y);

         /**
         * Press one or several buttons
         *
         * @param button button state
         * @returns true if there is no error, false otherwise
         */
         bool button(uint32_t button);

         /**
         * Press hat
         *
         * @param hat hat state
         * @returns true if there is no error, false otherwise
         */
         bool hat(uint8_t hat);

         /*
         * To define the report descriptor. Warning: this method has to store the length of the report descriptor in reportLength.
         *
         * @returns pointer to the report descriptor
         */
         virtual uint8_t * reportDesc();

         /*
         * Get string product descriptor
         *
         * @returns pointer to the string product descriptor
         */
         virtual uint8_t * stringIproductDesc();


         /*
         * Called when a data is received on the OUT endpoint.
         *
         * @returns if handle by subclass, return true
         */
         virtual bool EPINT_OUT_callback();
	

		/*
		* Called by USBDevice on Endpoint0 request. Warning: Called in ISR context
		* This is used to handle extensions to standard requests
		* and class specific requests
		*
		* @returns true if class handles this request
		*/
		virtual bool USBCallback_request();

		/*
		* Called by USBDevice on Endpoint0 request completion
		* if the 'notify' flag has been set to true. Warning: Called in ISR context
		*
		* In this case it is used to indicate that a HID report has
		* been received from the host on endpoint 0
		*
		* @param buf buffer received on endpoint 0
		* @param length length of this buffer
		*/
		virtual void USBCallback_requestCompleted(uint8_t * buf, uint32_t length);
	
	     bool handleReceivedHIDReport(HID_REPORT report);

         unsigned int getPendingReceivedReportCount();

         HID_REPORT getReceivedReport();

	u32 USB_SendControl (u8 flags, const u8 *d, u32 len);
	u32 USB_RecvControl (u8 *d, u32 len);

     private:
         bool FFBEnabled;
         int16_t Throttle;
         int16_t Brake;
         int16_t Clutch;
         int16_t Rudder;
         int16_t X;
         int16_t Y;
         int16_t Z;
         int16_t T;
         uint32_t Buttons;
         uint8_t Hat;

         //storage for SET_REPORTs from host
         unsigned int receivedReportBufferHead, receivedReportBufferTail;//head is index where new arrived report is stored, tail is the index where is last unhandled report
		 HID_REPORT receivedReports[RX_REPORT_BUFFER_COUNT];

			USB_FFBReport_PIDPool_Feature_Data_t mGetReportAnwser;
			USB_FFBReport_PIDBlockLoad_Feature_Data_t mSetReportAnwser;
	
         void _init();
};

#endif
