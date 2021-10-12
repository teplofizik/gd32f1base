/*!
    \file    main.c
    \brief   GPIO running led
    
    \version 2014-12-26, V1.0.0, demo for GD32F10x
    \version 2017-06-30, V2.0.0, demo for GD32F10x
    \version 2021-04-30, V2.1.0, demo for GD32F10x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f10x.h"

#include "drivers/activity.h"
#include "drivers/gpio.h"
#include "drivers/systick.h"
#include "drivers/spi.h"

#include "usb/libd/usb_device.h"
#include "usb/libd/hid/usb_customhid.h"

const TPin Led2 = {PC, 0};
const TPin Led3 = {PC, 2};
const TPin Led4 = {PE, 0};
const TPin Led5 = {PE, 1};

const TPin USB_VBUS = {PA, 9};
const TPin USB_ID = {PA, 10};
const TPin USB_DM = {PA, 11};
const TPin USB_DP = {PA, 12};

bool L1Value = false;
bool L2Value = false;
uint8_t StoredValue = 0;
	
void led_Blink() {
	static int Step = 0;
	
	switch(Step) {
		case 0:
			if(L1Value) gp_High(&Led2);
			break;
		case 1:
			if(L2Value) gp_High(&Led3);
			break;
		case 2:
			gp_High(&Led4);
			break;
		case 3:
			gp_Low(&Led2);
			gp_Low(&Led3);
			gp_Low(&Led4);
			break;
	}
	
	Step = (Step + 1) % 4;
	
	gp_Set(&Led5, ud_IsConfigured());
}

uint8_t SendBuffer[5];
static void sendReport()
{
	uint8_t L1 = L1Value ? 1 : 0;
	uint8_t L2 = L2Value ? 1 : 0;
	
	SendBuffer[0] = 4;
	SendBuffer[1] = 0; // btn 1
	SendBuffer[2] = 0; // btn 2
	SendBuffer[3] = L1 | (L2 << 1); // Led mask
	SendBuffer[4] = StoredValue;
	
	hid_SendReport(SendBuffer, sizeof(SendBuffer));
}

void sendChar() {
	if(ud_IsConfigured())
	{
		sendReport();
	}
}

void onHidData(uint8_t * Data, uint16_t Length)
{
	if(Data[0] == 0x00) // Report 0
	{
		
	}
		
	if(Data[0] == 0x01) // Report 1: led 1
	{
		L1Value = !L1Value;
	}
		
	if(Data[0] == 0x02) // Report 2: led 2
	{
		L2Value = !L2Value;
	}
		
	if(Data[0] == 0x03) // Report 3: stored trackbar value
	{
		StoredValue = Data[1];
	}
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	gp_Init();
	timer_Init(1000);
	act_Init();
	spi_Init();
	
	ud_Init();
	
	gp_Output(&Led2);
	gp_Output(&Led3);
	gp_Output(&Led4);
	gp_Output(&Led5);
	
	timer_AddFunction(10, led_Blink);
	timer_AddFunction(1000, sendChar);
	
	hid_SetHandler(onHidData);
	
    while(1){
		timer_Main();
		
		ud_Main();
    }
}
