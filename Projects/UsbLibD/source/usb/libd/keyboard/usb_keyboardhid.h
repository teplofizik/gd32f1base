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

	#define USB_VID                     0x28E9U
	#define USB_PID                     0x0380U

	#define USB_MANUFACTURER_STRING     "GigaDevice"
	#define USB_PRODUCT_STRING          "Keyboard!"
	#define USB_SERIALNUMBER_STRING     "00000000123D"

	#define HID_IN_EP                     0x81

	#define HID_IN_PACKET_SIZE               8

	#define HID_DESCRIPTOR_TYPE           0x21
	#define HID_REPORT_DESC               0x22

	// Отправка
	void hid_SendKey(uint8_t Char);

#endif
