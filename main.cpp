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
#include "stm32f4xx_hal_gpio.h"

Serial SMSerial(PB_10, PB_11); // tx, rx
DigitalOut SMSerialTXEN(PD_8);
bool SMSerialMasterIsMe=true;

#ifdef __cplusplus
extern "C" {
#endif

int SMPortWrite(const char *data, int len)
{
	int i;

	//if we are not in control of SM bus, return
	if(SMSerialMasterIsMe==false)
		return 0;

	//write
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

	//if we are not in control of SM bus, return
	if(SMSerialMasterIsMe==false)
		return 0;

	//try reading a byte
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

//takes control of SM bus for IONI if parameter is true, if false, lets second USB port (dedicated Granity port) to control it
//in practice it turns TX & TXEN to high impedance state (inputs) when me=false and outputs when true
void SMPortSetMaster(bool me)
{
	GPIO_InitTypeDef pin;

	//TX
	pin.Pin=GPIO_PIN_10;
	pin.Speed=GPIO_SPEED_FREQ_LOW;
	pin.Pull=GPIO_NOPULL;
	pin.Alternate=7;//USART3
	if(me)
		pin.Mode=GPIO_MODE_AF_PP;
	else
		pin.Mode=GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB,&pin);

	//TXEN
	pin.Pin=GPIO_PIN_8;
	if(me)
		pin.Mode=GPIO_MODE_OUTPUT_PP;
	else
		pin.Mode=GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOD,&pin);

	SMSerialMasterIsMe=me;
}


USBMouse mouse;
AnalogIn ADCUpperPin3(PC_5);
AnalogIn ADCUpperPin2(PB_0);
AnalogIn ADCUpperPin1(PB_1);
//USBKeyboard keyboard;//keyb and mouse dont work simultaneously. hang in init

Serial pc(PA_9, PA_10); // tx, rx
#include "simplemotion.h"

int main() {
	smbus h;
	h=smOpenBus("MBEDSERIAL");
	SMSerial.baud(460800);
	pc.baud(230400);

	Timer t;
	t.start();
	int triggered=0;
	int cnt=0;
DigitalOut test(PB_5);
test=1;

    while(1) {
        //mouse.move(1, 1);     //moves the mouse down and to the left
        //keyboard.keyCode('s');

        led1 = !led1;     //cycles the LED on/off
        wait(0.05);        //waits 2 seconds, then repeats

        if(cnt&32)
        	test=1;
        else
        	test=0;

        SM_STATUS stat=smSetParameter(h,1,SMP_ABSOLUTE_SETPOINT,1111);
        smint32 read;
        stat|=smRead1Parameter(h,1,SMP_ACTUAL_BUS_VOLTAGE,&read);
        pc.printf("val %d; stat %d; %f; %f; %f\n",read,stat,ADCUpperPin1.read(),ADCUpperPin2.read(),ADCUpperPin3.read());
        stat=0;


        /*if(t.read()>20 && triggered==0)
        {
        	SMPortSetMaster(false);
        	triggered=1;
        }

        if(t.read()>60 && triggered==1)
        {
        	SMPortSetMaster(true);
        	triggered=2;
        	pc.printf("MASTER\n");
        }*/
    }
}

