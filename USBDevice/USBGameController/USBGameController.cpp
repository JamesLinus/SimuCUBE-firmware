/* Copyright (c) 2010-2011 mbed.org, MIT License
* Modified Mouse code for Joystick - WH 2012
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdint.h"
#include "USBGameController.h"

bool USBGameController::update(int16_t brake, int16_t clutch, int16_t throttle, int16_t rudder, int16_t x, int16_t y, uint32_t button, uint8_t hat)
{
             HID_REPORT report;
   Throttle = throttle;
   Brake = rudder;
   X = x;
   Y = y;
   Buttons = button;     
   Hat = hat;

   update();
}
 
bool USBGameController::update() {
   HID_REPORT report;

   int i=0;
   // Fill the report according to the Joystick Descriptor
   //report.data[0] = Brake & 0xff;
   //report.data[1] = Clutch & 0xff;
   //report.data[2] = Throttle & 0xff;
   //report.data[3] = Rudder & 0xff;
   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   report.data[i++] = Y & 0xff;
   report.data[i++] = ((Y&0xff00)>>8);

   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   report.data[i++] = X & 0xff;
   report.data[i++] = ((X&0xff00)>>8);

   //report.data[8] = ((Buttons & 0x0f) << 4) | (Hat & 0x0f) ;
   report.data[i++] = ((Buttons & 0xff) >>0);
   report.data[i++] = ((Buttons & 0xff00) >>8);
   report.data[i++] = ((Buttons & 0xff0000) >>16);
   report.data[i++] = ((Buttons & 0xff000000) >>24);
   report.length = i;

   return send(&report);
}

bool USBGameController::throttle(int16_t t) {
     Throttle = t;
     return update();
}

bool USBGameController::rudder(int16_t r) {
     Brake = r;
     return update();
}

bool USBGameController::move(int16_t x, int16_t y) {
     X = x;
     Y = y;
     return update();
}

bool USBGameController::button(uint32_t button) {
     Buttons = button;
     return update();
}

bool USBGameController::hat(uint8_t hat) {
     Hat = hat;
     return update();
}


void USBGameController::_init() {

   Throttle = -127;
   Brake = -127;
   Clutch=0;
   Rudder=0;
   X = 0;                       
   Y = 0;     
   Buttons = 0x00;
   Hat = 0x00;              
}


uint8_t * USBGameController::reportDesc() {    
         static uint8_t reportDescriptor[] = {

             USAGE_PAGE(1), 0x01,           // Generic Desktop           
             LOGICAL_MINIMUM(1), 0x00,      // Logical_Minimum (0)             
             USAGE(1), 0x04,                // Usage (Joystick)
             COLLECTION(1), 0x01,           // Application

               USAGE_PAGE(1), 0x01,            // Generic Desktop
               USAGE(1), 0x01,                 // Usage (Pointer)
               COLLECTION(1), 0x00,            // Physical
                 USAGE(1), 0x30,                 // X, wheel angle
                 USAGE(1), 0x31,                 // Y, always 0

				 //pedals etc
				 USAGE(1), 0x32,                 // Z
                 USAGE(1), 0x33,                 // Rx
                 USAGE(1), 0x34,                 // Ry
                 USAGE(1), 0x35,                 // Rz
			     USAGE(1), 0x36,                 // Rz
			     USAGE(1), 0x37,                 // Rz

				 // 16 bit values
                 LOGICAL_MINIMUM(2), 0x01, 0x80,       // -32767
                 LOGICAL_MAXIMUM(2), 0xff, 0x7f, // 32767
                 REPORT_SIZE(1), 0x10,
                 REPORT_COUNT(1), 0x08,
                 INPUT(1), 0x02,                 // Data, Variable, Absolute

               END_COLLECTION(0),               

               USAGE_PAGE(1), 0x09,            // Buttons
               USAGE_MINIMUM(1), 0x01,         // 1
               USAGE_MAXIMUM(1), 0x20,         // 32
               LOGICAL_MINIMUM(1), 0x00,       // 0
               LOGICAL_MAXIMUM(1), 0x01,       // 1
               REPORT_SIZE(1), 0x01,
               REPORT_COUNT(1), 0x20,
               UNIT_EXPONENT(1), 0x00,         // Unit_Exponent (0)
               UNIT(1), 0x00,                  // Unit (None)                                           
               INPUT(1), 0x02,                 // Data, Variable, Absolute
             END_COLLECTION(0)

        };

      reportLength = sizeof(reportDescriptor);
      return reportDescriptor;
}

uint8_t * USBGameController::stringIproductDesc() {
    static uint8_t stringIproductDescriptor[] = {
        0x12,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'S',0,'i',0,'m',0,'u',0,'C',0,'U',0,'B',0,'E',0 //bString iProduct - HID device
    };
    return stringIproductDescriptor;
}

