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

Serial SMSerial(PB_10, PB_11); // tx, rx
DigitalOut SMSerialTXEN(PD_8);

#ifdef __cplusplus
extern "C" {
#endif

int SMPortWrite(const char *data, int len)
{
	int i;
	SMSerialTXEN=1;
	for(i=0;i<len;i++)
		SMSerial.putc(data[i]);
	wait(43e-6);//keep TXEN up for last 2 bytes because putc returns before data is physically out
	SMSerialTXEN=0;

	return len;
}

int SMPortReadByte( char *byte )
{
	Timer timeout;
	timeout.start();
	timeout.reset();

	bool done=false;
	do
	{
		if(SMSerial.readable())
		{
			*byte=SMSerial.getc();
			return 1;
		}
	} while(timeout.read()<0.2);//loop until timeout or data received

	//timeouted
	return 0;
}

#ifdef __cplusplus
}
#endif

USBMouse mouse;
//USBKeyboard keyboard;//keyb and mouse dont work simultaneously. hang in init

Serial pc(PA_9, PA_10); // tx, rx
#include "simplemotion.h"

int main() {
	smbus h;
	h=smOpenBus("MBEDSERIAL");
	SMSerial.baud(460800);
	pc.baud(230400);

    while(1) {
        //mouse.move(1, 1);     //moves the mouse down and to the left
        //keyboard.keyCode('s');

        led1 = !led1;     //cycles the LED on/off
        //wait(0.15);        //waits 2 seconds, then repeats

        //simplemotion.printf("fdasfsadfasdfasdfsadfas");
        //SMPortWrite("fdsafdsafdsafdsafdsa",5);

        SM_STATUS stat=smSetParameter(h,1,SMP_ABSOLUTE_SETPOINT,1111);
//        pc.printf("write stat %d\n",stat);
        smint32 read;
        stat|=smRead1Parameter(h,1,SMP_ACTUAL_BUS_VOLTAGE,&read);
  //      pc.printf("read stat %d\n",stat);
        pc.printf("val %d, stat %d\n",read,stat);
        stat=0;

        /*if(pc.readable()) {
            pc.putc(pc.getc());
        }*/

    }
}

