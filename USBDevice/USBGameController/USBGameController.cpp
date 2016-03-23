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

bool USBGameController::update(int16_t t, int16_t r, int16_t x, int16_t y, uint8_t button, uint8_t hat) {
             HID_REPORT report;
   Throttle = t;
   Brake = r;   
   X = x;
   Y = y;
   Buttons = button;     
   Hat = hat;

   update();
}
 
bool USBGameController::update() {
   HID_REPORT report;

   // Fill the report according to the Joystick Descriptor
   report.data[0] = Throttle & 0xff;            
   report.data[1] = Brake & 0xff;               
   report.data[2] = X & 0xff;
   report.data[3] = ((X&0xff00)>>8);
   report.data[4] = Y & 0xff;
   report.data[5] = ((Y&0xff00)>>8);

   report.data[6] = ((Buttons & 0x0f) << 4) | (Hat & 0x0f) ;
   report.length = 7;

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

bool USBGameController::button(uint8_t button) {
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
               USAGE_PAGE(1), 0x02,            // Simulation Controls
               USAGE(1), 0xBB,                 // Throttle             
               USAGE(1), 0xBA,                 // Rudder               
               LOGICAL_MINIMUM(1), 0x81,       // -127
               LOGICAL_MAXIMUM(1), 0x7f,       // 127
               REPORT_SIZE(1), 0x08,
               REPORT_COUNT(1), 0x02,
               INPUT(1), 0x02,                 // Data, Variable, Absolute               
               USAGE_PAGE(1), 0x01,            // Generic Desktop
               USAGE(1), 0x01,                 // Usage (Pointer)
               COLLECTION(1), 0x00,            // Physical
                 USAGE(1), 0x30,                 // X
                 USAGE(1), 0x31,                 // Y
// 8 bit values
//                 LOGICAL_MINIMUM(1), 0x81,       // -127
//                 LOGICAL_MAXIMUM(1), 0x7f,       // 127
//                 REPORT_SIZE(1), 0x08,
//                 REPORT_COUNT(1), 0x02,
//                 INPUT(1), 0x02,                 // Data, Variable, Absolute
// 16 bit values
                 LOGICAL_MINIMUM(2), 0x01, 0x80,       // -32767
				 //LOGICAL_MINIMUM(1), 0,       // 0
                 LOGICAL_MAXIMUM(2), 0xff, 0x7f, // 32767
                 REPORT_SIZE(1), 0x10,
                 REPORT_COUNT(1), 0x02,
                 INPUT(1), 0x02,                 // Data, Variable, Absolute

               END_COLLECTION(0),               
// 4 Position Hat Switch
//               USAGE(1), 0x39,                 // Usage (Hat switch)
//               LOGICAL_MINIMUM(1), 0x00,       // 0
//               LOGICAL_MAXIMUM(1), 0x03,       // 3
//               PHYSICAL_MINIMUM(1), 0x00,      // Physical_Minimum (0)
//               PHYSICAL_MAXIMUM(2), 0x0E, 0x01, // Physical_Maximum (270)
//               UNIT(1), 0x14,                  // Unit (Eng Rot:Angular Pos)                            
//               REPORT_SIZE(1), 0x04,
//               REPORT_COUNT(1), 0x01,
//               INPUT(1), 0x02,                 // Data, Variable, Absolute               
// 8 Position Hat Switch
               USAGE(1), 0x39,                 // Usage (Hat switch)
               LOGICAL_MINIMUM(1), 0x00,       // 0
               LOGICAL_MAXIMUM(1), 0x07,       // 7
               PHYSICAL_MINIMUM(1), 0x00,      // Physical_Minimum (0)
               PHYSICAL_MAXIMUM(2), 0x3B, 0x01, // Physical_Maximum (315)
               UNIT(1), 0x14,                  // Unit (Eng Rot:Angular Pos)                            
               REPORT_SIZE(1), 0x04,
               REPORT_COUNT(1), 0x01,
               INPUT(1), 0x02,                 // Data, Variable, Absolute               
//
               USAGE_PAGE(1), 0x09,            // Buttons
               USAGE_MINIMUM(1), 0x01,         // 1
               USAGE_MAXIMUM(1), 0x04,         // 4
               LOGICAL_MINIMUM(1), 0x00,       // 0
               LOGICAL_MAXIMUM(1), 0x01,       // 1
               REPORT_SIZE(1), 0x01,
               REPORT_COUNT(1), 0x04,
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

