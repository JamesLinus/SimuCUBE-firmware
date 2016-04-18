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


/* Contributors:
 * Tero Kontkanen
 * Etienne Saint-Paul
 */

#include "stdint.h"
#include "types.h"
#include "USBGameController.h"
#include "ffb.h"
#include "USBDevice_Types.h"

bool USBGameController::update(uint16_t brake, uint16_t clutch, uint16_t throttle, uint16_t rudder, uint16_t x, uint16_t y, uint32_t button, uint8_t hat)
{
	Throttle = throttle;
	Brake = brake;
	Clutch = clutch;
	Rudder = rudder;
	X = x;
	Y = y;
	Z = y;
	T = y;
	Buttons = button;     
	Hat = hat;

	return(update());
}

#define AddAxisValue(m_val)		{report.data[i++] = m_val & 0xff; report.data[i++] = ((m_val & 0xff00) >> 8);}
 
bool USBGameController::update() 
{
	HID_REPORT report;

	int i=0;

	report.data[i++] = REPORT_ID_JOYSTICK;
	AddAxisValue(X);
	AddAxisValue(Y);
	AddAxisValue(Z);
	AddAxisValue( Brake);
	AddAxisValue(Throttle);
	AddAxisValue(Clutch);
	AddAxisValue(Rudder);
	AddAxisValue(T);

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


void USBGameController::_init() 
{
	Throttle = -127;
	Brake = -127;
	Clutch=0;
	Rudder=0;
	X = 0;                       
	Y = 0;
	Buttons = 0x00;
	Hat = 0x00;
}

bool USBGameController::USBCallback_request()
{
	CONTROL_TRANSFER * transfer = getTransferPtr();
	u8 report_id = DESCRIPTOR_INDEX(transfer->setup.wValue);
	int r = transfer->setup.bRequest;
	int requestType = transfer->setup.bmRequestType.Type;
	if (requestType == REQUEST_INTERFACE)//REQUEST_HOSTTODEVICE_CLASS_INTERFACE)
	{
		switch (r)
		{
		case HID_GET_REPORT:
			if (report_id == 6)// && (gNewEffectBlockLoad.reportId==6))
			{
				USB_SendControl(0,(u8 *)&mSetReportAnwser,sizeof(USB_FFBReport_PIDBlockLoad_Feature_Data_t));
				mSetReportAnwser.reportId = 0;
				return (true);
			}
			if (report_id == 7)
			{
				mGetReportAnwser.reportId = report_id;
				mGetReportAnwser.ramPoolSize = 0xffff;
				mGetReportAnwser.maxSimultaneousEffects = MAX_EFFECTS;
				mGetReportAnwser.memoryManagement = 3;
				USB_SendControl(0,(u8 *)&mGetReportAnwser,sizeof(USB_FFBReport_PIDPool_Feature_Data_t));
				return (true);
			}
			return true;
		case HID_GET_PROTOCOL:
				//Send8(_hid_protocol);	// TODO
				return true;

		case HID_SET_PROTOCOL:
//			_hid_protocol = setup.wValueL;
			return true;

		case HID_SET_IDLE:
//			_hid_idle = setup.wValueL;
			return true;

		case HID_SET_REPORT:
			if (report_id == 5)
			{
				USB_RecvControl(0,sizeof(USB_FFBReport_CreateNewEffect_Feature_Data_t));
				return true;
			}
		}
	}
	return (USBHID::USBCallback_request());
}

void USBGameController::USBCallback_requestCompleted(uint8_t * buf, uint32_t length)
{
	CONTROL_TRANSFER * transfer = getTransferPtr();
	u8 report_id = DESCRIPTOR_INDEX(transfer->setup.wValue);
	int r = transfer->setup.bRequest;
	int requestType = transfer->setup.bmRequestType.Type;
	if ((r == HID_SET_REPORT)&&(requestType == REQUEST_INTERFACE)&&(report_id == 5))
		FfbOnCreateNewEffect((USB_FFBReport_CreateNewEffect_Feature_Data_t *)buf, &mSetReportAnwser);
}

u32 USBGameController::USB_SendControl (u8 flags, const u8 *d, u32 len)
{
	CONTROL_TRANSFER * transfer = getTransferPtr();
	getTransferPtr()->remaining = len;
	getTransferPtr()->ptr = (u8*) d;
	getTransferPtr()->direction = DEVICE_TO_HOST;
	return (len);
}

u32 USBGameController::USB_RecvControl (u8 *d, u32 len)
{
	CONTROL_TRANSFER * transfer = getTransferPtr();
	transfer->remaining = len;
	transfer->notify = true;
	transfer->direction = HOST_TO_DEVICE;
	return (len);
}

bool USBGameController::EPINT_OUT_callback()
{
	bool returnval;

    USBDevice::readEP(EPINT_OUT, receivedReports[receivedReportBufferHead].data, &receivedReports[receivedReportBufferHead].length, MAX_HID_REPORT_SIZE);

	receivedReportBufferHead=(receivedReportBufferHead+1)&(RX_REPORT_BUFFER_COUNT-1);

    // We activate the endpoint to be able to recceive data
    if (!readStart(EPINT_OUT, MAX_HID_REPORT_SIZE))
        returnval= false;
    else
    	returnval= true;

    return returnval;
}

unsigned int USBGameController::getPendingReceivedReportCount()
{
	 //calculate how many reports are pending to read
	 return (receivedReportBufferHead-receivedReportBufferTail)&(RX_REPORT_BUFFER_COUNT-1);
}

HID_REPORT USBGameController::getReceivedReport()
{
	 int takeFrom=receivedReportBufferTail;
	 receivedReportBufferTail=(receivedReportBufferTail+1)&(RX_REPORT_BUFFER_COUNT-1);
	 return receivedReports[takeFrom];
}
