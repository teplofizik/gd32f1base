// ******************************************************
// usb_hid.h
// HID interface
// teplofizik
// ******************************************************
 
#include <stdint.h>
#include <stdbool.h>

#ifndef USB_CUSTOMHID_H_
#define USB_CUSTOMHID_H_

	typedef void (* THIDDataHandler)(uint8_t * Data, uint16_t Length);

	#define USB_VID                     0x0483U
	#define USB_PID                     0x5711U

	#define USB_MANUFACTURER_STRING     "GigaDevice"
	#define USB_PRODUCT_STRING          "HID I/O"
	#define USB_SERIALNUMBER_STRING     "00000000011D"

	#define HID_IN_EP                     0x81
	#define HID_OUT_EP                    0x01

	#define HID_IN_PACKET_SIZE            0x40
	#define HID_OUT_PACKET_SIZE           0x40

	#define HID_DESCRIPTOR_TYPE           0x21
	#define HID_REPORT_DESC               0x22

	#define RPT3_COUNT 0x01 //PC->STM32
	#define RPT4_COUNT 0x04 //STM32->PC

	// Отправка репорта
	bool hid_SendReport(uint8_t * report, uint16_t len);

	// Установка обработчика
	void hid_SetHandler(THIDDataHandler Handler);

#endif
