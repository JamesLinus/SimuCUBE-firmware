/* USBJoystick.h */
/* USB device example: Joystick*/
/* Copyright (c) 2011 ARM Limited. All rights reserved. */
/* Modified Mouse code for Joystick - WH 2012 */

#ifndef USBGAMECONTROLLER_H
#define USBGAMECONTROLLER_H

#include "USBHID.h"

#define REPORT_ID_JOYSTICK  4

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
             USBHID(0, 0, vendor_id, product_id, product_release, false)
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
         bool update(int16_t brake, int16_t clutch, int16_t throttle, int16_t rudder, int16_t x, int16_t y, uint32_t button, uint8_t hat);

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

     private:
         int16_t Throttle;
         int16_t Brake;
         int16_t Clutch;
         int16_t Rudder;
         int16_t X;
         int16_t Y;
         uint32_t Buttons;
         uint8_t Hat;

         void _init();
};

#endif
