#include "mbed.h"
#include "USBHID.h"
 
//We declare a USBHID device. Input out output reports have a length of 8 bytes
/*USBHID hid(8, 8);
 
//This report will contain data to be sent
HID_REPORT send_report;
HID_REPORT recv_report;*/
DigitalOut led1(PD_15);
DigitalOut led2(PD_14);
 
 /*
int main(void) {
    send_report.length = 8;
    led1=1;
    led2=1;
 
    while (1) {
        //Fill the report
        for (int i = 0; i < send_report.length; i++) {
            send_report.data[i] = rand() & 0xff;
        }
            
        //Send the report
        hid.send(&send_report);
        
        //try to read a msg
        if(hid.readNB(&recv_report)) {
            //pc.printf("recv: ");
            for(int i = 0; i < recv_report.length; i++) {
              //  pc.printf("%d ", recv_report.data[i]);
            }
            //pc.printf("\r\n");
        }
        
        wait(0.1);
        led1=!led1;
    }
}
*/

#include "USBMouse.h"
//#include "USBKeyboard.h"


USBMouse mouse;
//USBKeyboard keyboard;//keyb and mouse dont work simultaneously. hang in init

Serial pc(PA_9, PA_10); // tx, rx

int main() {
    while(1) {
        mouse.move(1, 1);     //moves the mouse down and to the left
        //keyboard.keyCode('s');

        led1 = !led1;     //cycles the LED on/off
        wait(0.25);        //waits 2 seconds, then repeats

        pc.printf("Hello World!\n");

        if(pc.readable()) {
            pc.putc(pc.getc());
        }
    }
}

