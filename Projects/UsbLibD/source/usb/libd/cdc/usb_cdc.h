// ******************************************************
// usb_hid.h
// HID interface
// teplofizik
// ******************************************************
 
#include <stdint.h>
#include <stdbool.h>
#include "../../../types.h"

#ifndef USB_HID_H_
#define USB_HID_H_

	#define USB_VID                         0x0483
	#define USB_PID                         0x5740

	#define USB_MANUFACTURER_STRING         "GigaDevice"
	#define USB_PRODUCT_STRING              "STM32 Virtual ComPort"
	#define USB_SERIALNUMBER_STRING         "00000000050C"

	#define CDC_IN_EP                       0x81  /* EP1 for data IN */
	#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
	#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */

	#define CDC_DATA_MAX_PACKET_SIZE        64
	#define CDC_CMD_PACKET_SIZE              8

	#define CDC_IN_FRAME_INTERVAL           5
	#define APP_RX_DATA_SIZE                1024
	#define APP_TX_DATA_SIZE                1024
	
	// Добавить обработчик принятых данных
	void cdc_AddHandler(TCharHandler Handler);
	
	// Запись символа на отправку в хост
	void cdc_WriteChar(char C);
	
	// Запись символов на отправку в хост
	void cdc_WriteData(uint8_t * Data, uint16_t Length);
	
#endif
