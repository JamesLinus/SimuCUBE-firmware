#include "mbed.h"
#include "USBHID.h"
 

DigitalOut led1(PD_15);
DigitalOut led2(PD_14);
 
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"

Serial SMSerial(PB_10, PB_11); // tx, rx
DigitalOut SMSerialTXEN(PD_8);
bool SMSerialMasterIsMe=true;

#ifdef __cplusplus
extern "C" {
#endif

//Init encoder input
//credit to David Lowe from https://developer.mbed.org/forum/platform-34-ST-Nucleo-F401RE-community/topic/4963/?page=1#comment-26870
void EncoderInitialize()
{
    // configure GPIO PA0 & PA1 aka A0 & A1 as inputs for Encoder
    // Enable clock for GPIOA
    __GPIOA_CLK_ENABLE(); //equivalent from hal_rcc.h

    //stm32f4xx.h
    GPIOA->MODER   |= GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1 ;           //PA0 & PA1 as Alternate Function   /*!< GPIO port mode register,               Address offset: 0x00      */
    GPIOA->OTYPER  |= GPIO_OTYPER_OT_0 | GPIO_OTYPER_OT_1 ;                 //PA0 & PA1 as Inputs               /*!< GPIO port output type register,        Address offset: 0x04      */
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1 ;     //Low speed                         /*!< GPIO port output speed register,       Address offset: 0x08      */
    GPIOA->PUPDR   |= GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1 ;           //Pull Down                         /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    GPIOA->AFR[0]  |= 0x00000011 ;                                          //AF01 for PA0 & PA1                /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
    GPIOA->AFR[1]  |= 0x00000000 ;                                          //nibbles here refer to gpio8..15   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */

    // configure TIM2 as Encoder input
    // Enable clock for TIM2
    __TIM2_CLK_ENABLE();

    TIM2->CR1   = 0x0001;     // CEN(Counter ENable)='1'     < TIM control register 1
    TIM2->SMCR  = TIM_ENCODERMODE_TI12;     // SMS='011' (Encoder mode 3)  < TIM slave mode control register
    TIM2->CCMR1 = 0x8181;     // CC1S='01' CC2S='01'         < TIM capture/compare mode register 1. 0x_1_1 blanks are input filter strength 0-F
    TIM2->CCMR2 = 0x0000;     //                             < TIM capture/compare mode register 2
    TIM2->CCER  = 0x0011;     // CC1P CC2P                   < TIM capture/compare enable register
    TIM2->PSC   = 0x0000;     // Prescaler = (0+1)           < TIM prescaler
    TIM2->ARR   = 0xffffffff; // reload at 0xfffffff         < TIM auto-reload register

    TIM2->CNT = 0x0000;  //reset the counter before we use it
}

int EncoderRead()
{
	return TIM2->CNT;
}


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


#include "USBGameController.h"
USBGameController joystick;

AnalogIn ADCUpperPin3(PC_5);
AnalogIn ADCUpperPin2(PB_0);
AnalogIn ADCUpperPin1(PB_1);

Serial pc(PA_9, PA_10); // tx, rx
#include "simplemotion.h"

int main() {
    int32_t i = 0;
    int16_t throttle = 0;
    int16_t rudder = 0;
    int16_t x = 0;
    int16_t y = 0;
    int32_t radius = 120;
    int32_t angle = 0;
    uint32_t button = 0;
    int8_t hat = 0;

    pc.baud(230400);
    pc.printf("Hello World!\n\r");

    EncoderInitialize();
    while (1) {
        // Basic Joystick
        throttle = (i >> 8) & 0xFF; // value -127 .. 128
        rudder = (i >> 8) & 0xFF;   // value -127 .. 128
        button=i;
        hat    = (i >> 8) & 0x07;   // value 0..7 or 8 for neutral
        i++;
        angle += 3;

        x=EncoderRead();
        y=0;
        joystick.update(throttle, throttle,throttle,rudder, x, y, button, hat);

        if(joystick.getPendingReceivedReportCount())
        {
        	HID_REPORT recv_report=joystick.getReceivedReport();
            pc.printf("recv (%d bytes): ",recv_report.length);
            for(int i = 0; i < recv_report.length; i++) {
                pc.printf("%d ", recv_report.data[i]);
            }
            pc.printf("\r\n");
        	joystick.handleReceivedHIDReport(recv_report);
            /*
             * wheelcheck gives output like:
				Hello World!

				recv (2 bytes): 13 4
				recv (2 bytes): 13 3
				recv (2 bytes): 14 255
				recv (2 bytes): 13 1
				recv (2 bytes): 13 1
				recv (2 bytes): 13 1
				recv (2 bytes): 13 1
				recv (2 bytes): 13 1

				don't know why it outputs only "disable" command after first
             */
        }

        wait(0.001);
    }

    pc.printf("Bye World!\n\r");
}

#if 0
int __main() {
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

	EncoderInitialize();
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
        pc.printf("val %d; stat %d; %f; %f; %f; %d\n",read,stat,ADCUpperPin1.read(),ADCUpperPin2.read(),ADCUpperPin3.read(), EncoderRead());

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

#endif
