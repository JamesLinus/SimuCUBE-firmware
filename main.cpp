/* SimuCUBE production testing app. Usage:
 *
 * 1.
 * Straight RJ45 cables connected between X11 upper and lower port and X12 upper and lower port
 * Both USB on PC
 * Test IONI drive insterted
 * Test motor connected
 * SWD programmer connected
 *
 * 2. power on 48V
 *
 * Test starts. Mouse should move on screen, then  motor rotates, etc.
 *
 * 3. If all goes fine, one green led blinks and red is off. If no leds blink, USB HID connection is not established.
 *
 * 4. Test connection to Granity or other SMV2 app
 *
 * 5. Power off, use external resistor to bleed out capacitor charge (put on big capacitor pads)
 *
 * 6. Disconnect all
 *
 */

#include "mbed.h"
#include "USBHID.h"
 
DigitalOut led2red(PD_15);
DigitalOut led1green(PD_14);
DigitalOut led3green(PD_13);


#include "USBMouse.h"
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


USBMouse mouse;
#include "USBGameController.h"

AnalogIn ADCUpperPin3(PC_5);
AnalogIn ADCUpperPin2(PB_0);
AnalogIn ADCUpperPin1(PB_1);

Serial pc(PA_9, PA_10); // tx, rx
#include "simplemotion.h"
DigitalOut DriveEnable(PB_5);

void testResultLedLoop(int result)
{
	SMPortSetMaster(false);

	led2red=0;
	led1green=led3green=0;
	while(1)
	{
		switch(result)
		{
		case -1://ok
			led2red=0;
			led1green=0;
			led3green=!led3green;
			break;
		case 0://faults 0-n
			led2red=!led2red;
			led1green=!led1green;
			led3green=!led3green;
			break;
		case 1:
			led2red=!led2red;
			led1green=!led1green;
			led3green=1;
			break;
		case 2:
			led2red=!led2red;
			led1green=1;
			led3green=!led3green;
			break;
		case 3:
			led2red=!led2red;
			led1green=0;
			led3green=!led3green;
			break;
		case 4:
			led2red=!led2red;
			led1green=!led1green;
			led3green=0;
			break;
		default:
			led2red=!led2red;
			led1green=1;
			led3green=1;
			break;
		}

		wait(0.5);
	}
}


//check that adc readings r1-r6 match with states s1-s6
bool verifyADCreadings( bool s1, bool s2, bool s3, bool s4, bool s5, bool s6,
		float r1, float r2, float r3, float r4, float r5, float r6)
{
	float offmax=0.05;
	float onmin=0.25, onmax=0.55;

	if(!s1)
		if(r1>offmax) return false;
	if(s1)
		if(r1>onmax || r1<onmin) return false;

	if(!s2)
		if(r2>offmax) return false;
	if(s2)
		if(r2>onmax || r2<onmin) return false;

	if(!s3)
		if(r3>offmax) return false;
	if(s3)
		if(r3>onmax || r3<onmin) return false;

	if(!s4)
		if(r4>offmax) return false;
	if(s4)
		if(r4>onmax || r4<onmin) return false;

	if(!s5)
		if(r5>offmax) return false;
	if(s5)
		if(r5>onmax || r5<onmin) return false;

	if(!s6)
		if(r6>offmax) return false;
	if(s6)
		if(r6>onmax || r6<onmin) return false;


	//no errors
	return true;
}


bool testPortX12()
{
	/*connections upper->lower row, pin to pin, starting from 1
	 * 1 PC11->PE2
	 * 2 PC6->PE3
	 * 3 PC7->PE1
	 * 4 PC8->PE4
	 * 5 PC9->PC12
	 * 6 PE5->PC13
	 * 7 PC10->PC14
	 * 8 gnd->gnd
	 */

	DigitalOut O1(PC_11);
	DigitalOut O2(PC_6);
	DigitalOut O3(PC_7);
	DigitalOut O4(PC_8);
	DigitalOut O5(PC_9);
	DigitalOut O6(PE_5);
	DigitalOut O7(PC_10);

	DigitalIn I1(PE_2);
	DigitalIn I2(PE_3);
	DigitalIn I3(PE_1);
	DigitalIn I4(PE_4);
	DigitalIn I5(PC_12);
	DigitalIn I6(PC_13);
	DigitalIn I7(PC_14);

	O1=O2=O3=O4=O5=O6=O7=0;

	O1=1; wait(0.01);
	if(I1!=1) return false;
	O1=0; wait(0.01);
	if(I1!=0) return false;

	O2=1; wait(0.01);
	if(I2!=1) return false;
	O2=0; wait(0.01);
	if(I2!=0) return false;

	O3=1; wait(0.01);
	if(I3!=1) return false;
	O3=0; wait(0.01);
	if(I3!=0) return false;

	O4=1; wait(0.01);
	if(I4!=1) return false;
	O4=0; wait(0.01);
	if(I4!=0) return false;

	O5=1; wait(0.01);
	if(I5!=1) return false;
	O5=0; wait(0.01);
	if(I5!=0) return false;

	O6=1; wait(0.01);
	if(I6!=1) return false;
	O6=0; wait(0.01);
	if(I6!=0) return false;

	O7=1; wait(0.01);
	if(I7!=1) return false;
	O7=0; wait(0.01);
	if(I7!=0) return false;

	return true;//all passed
}

bool testPortX11()
{
	/*connections upper->lower row, pin to pin, starting from 1
	 * 1 PB1 (adc)->PC15 (digital)
	 * 2 PB0 (adc) -> PC3 (adac)
	 * 3 PC5 (adc) -> PA3 (adc)
	 * 4 5v->5v
	 * 5 PC4 (adc)-> PA4 (adc)
	 * 6 PA7 (adc) -> PA5 (adc)
	 * 7 PE12 (clkout) -> PA6 (adc)
	 * 8 gnd->gnd
	 */

	DigitalOut O1(PC_15);
	DigitalOut O2(PB_0);
	DigitalOut O3(PC_5);
	DigitalOut O4(PC_4);
	DigitalOut O5(PA_7);
	DigitalOut O6(PE_12);

	AnalogIn I1(PB_1);
	AnalogIn I2(PC_3);
	AnalogIn I3(PA_3);
	AnalogIn I4(PA_4);
	AnalogIn I5(PA_5);
	AnalogIn I6(PA_6);

	/*
	O1=O2=O3=O4=O5=O6=0;
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());
	O1=1;
	wait(0.01);
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());
	O2=1;
	wait(0.01);
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());
	O3=1;
	wait(0.01);
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());
	O4=1;
	wait(0.01);
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());
	O5=1;
	wait(0.01);
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());
	O6=1;
	wait(0.01);
	pc.printf("%f %f %f %f %f %f\n",I1.read(),I2.read(),I3.read(),I4.read(),I5.read(),I6.read());

	above will output when X11 upper & lower connected with straight RJ45:
	0.003663 0.000488 0.000733 0.000733 0.000733 0.000488
	0.479609 0.001954 0.000733 0.000488 0.000733 0.000733
	0.479609 0.330159 0.001465 0.000733 0.000733 0.000488
	0.479609 0.330403 0.329915 0.001709 0.000733 0.000733
	0.479609 0.330403 0.329426 0.331136 0.001709 0.000488
	0.480830 0.330159 0.329670 0.330891 0.330647 0.001465
	0.479365 0.330403 0.329426 0.331136 0.330647 0.274969
	*/


	if(verifyADCreadings(false,false,false,false,false,false, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;
	O1=1;
	wait(0.01);
	if(verifyADCreadings(true,false,false,false,false,false, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;
	O2=1;
	wait(0.01);
	if(verifyADCreadings(true,true,false,false,false,false, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;
	O3=1;
	wait(0.01);
	if(verifyADCreadings(true,true,true,false,false,false, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;
	O4=1;
	wait(0.01);
	if(verifyADCreadings(true,true,true,true,false,false, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;
	O5=1;
	wait(0.01);
	if(verifyADCreadings(true,true,true,true,true,false, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;
	O6=1;
	wait(0.01);
	if(verifyADCreadings(true,true,true,true,true,true, I1.read(), I2.read(), I3.read(), I4.read(), I5.read(), I6.read())==false) return false;

	return true;//all passed
}

int main() {
	smbus h;
	h=smOpenBus("MBEDSERIAL");
	SMSerial.baud(460800);
	pc.baud(230400);
    SM_STATUS stat;

    led1green=1;
    led3green=1;
    led2red=1;

	Timer t;
	t.start();
	EncoderInitialize();

	//move mouse, tester need watch if cursor moves
    mouse.move(10, 10);
    wait(0.2);
    mouse.move(-10, -10);
    wait(0.2);
    mouse.move(10, 10);
    wait(0.2);
    mouse.move(-10, -10);
    wait(0.2);
    mouse.move(10, 10);
    wait(0.2);
    for(int i=0;i<50;i++)//to top corner
    {
    	mouse.move(-100, -100);
    	wait(0.01);
    }


	//test ports. straight wired RJ45s must be connected from upper->lower on X11 & X12
	if(testPortX11()==false)
		testResultLedLoop(3);

	if(testPortX12()==false)
		testResultLedLoop(4);

	//init drive
	DriveEnable=1;
	bool done=false;
	while(!done)
	{
        smint32 read;
        stat=smRead1Parameter(h,1,SMP_STATUS,&read);
        if(stat!=SM_OK)
        {
        	testResultLedLoop(0);
        }
        if(t.read()>15)//timeout
        {
        	testResultLedLoop(1);
        }
        if(read&STAT_SERVO_READY)
        	done=true;
	}
	TIM2->CNT = 0x0000;

	//verify that its pos mode
    smint32 read;
    stat|=smRead1Parameter(h,1,SMP_CONTROL_MODE,&read);
    if(read!=SM_OK)
    {
    	testResultLedLoop(0);
    }
    if(read!=CM_POSITION)
    	testResultLedLoop(5);


	//test driving
	smSetParameter(h,1,SMP_ABSOLUTE_SETPOINT,0);
	wait(2);
	TIM2->CNT = 0x0000;
	smSetParameter(h,1,SMP_ABSOLUTE_SETPOINT,-5000);
	wait(2);
	pc.printf("a %d\n",EncoderRead());
	if(EncoderRead()<4000||EncoderRead()>6000)
		testResultLedLoop(2);
	smSetParameter(h,1,SMP_ABSOLUTE_SETPOINT,5000);
	wait(2);
	if(EncoderRead()<-6000||EncoderRead()>-4000)
		testResultLedLoop(2);
	pc.printf("b %d\n",EncoderRead());

	//all pass
	testResultLedLoop(-1);

    while(1) {}
}

