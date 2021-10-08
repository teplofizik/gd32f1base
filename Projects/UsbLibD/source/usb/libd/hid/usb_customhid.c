// ******************************************************
// usb_hid.c
// HID interface
// teplofizik
// ******************************************************
 
#include "usb_customhid.h"
#include "usb_device.h"
#include "usbd_enum.h"

#define MIN(a,b) ((a<b) ? a : b)

#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03
#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02
#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01

extern TDescriptor udDeviceDescriptor;
extern TDescriptor udConfigDescriptor;
extern TDescriptor udHIDConfigDescriptor;
extern TDescriptor udReportDescriptor;

// not used
TUsbClassEpConfig EPConfig = {
	2, // Endpoint count
	{
		{ HID_IN_EP,  HID_IN_PACKET_SIZE,  EPT_Interrupt }, // In
		{ HID_OUT_EP, HID_OUT_PACKET_SIZE, EPT_Interrupt }, // Out
	}
};

// static uint32_t  USBD_HID_AltSet    = 0;
static uint32_t  USBD_HID_Protocol  = 0;
static uint32_t  USBD_HID_IdleState = 0;

THIDDataHandler HidOUTHandler = 0;

static TUsbClassEpConfig * GetConfigEP(uint8_t Config)
{
	return &EPConfig;
}

static uint8_t Setup(TUsbSetupReq * req)
{
	if((req->bmRequest & AUSB_REQ_TYPE_MASK) == AUSB_REQ_TYPE_CLASS)
	{
		switch (req->bRequest)
		{
			case HID_REQ_SET_PROTOCOL: USBD_HID_Protocol = (uint8_t)(req->wValue); break;
			case HID_REQ_GET_PROTOCOL: ud_CtlSendData((uint8_t *)&USBD_HID_Protocol, 1); break;
			case HID_REQ_SET_IDLE:     USBD_HID_IdleState = (uint8_t)(req->wValue >> 8); break;
			case HID_REQ_GET_IDLE:     ud_CtlSendData((uint8_t *)&USBD_HID_IdleState, 1); break;
			default:                   
				//ud_CtlError(req);
				return AUSB_FAIL; 
		}
	}
	return AUSB_OK;
}

// Отправка репорта
bool hid_SendReport(uint8_t * report, uint16_t len)
{
	if (ud_IsConfigured())
		return ud_SendData (HID_IN_EP, report, len);
	else
		return false;
}

// Установка обработчика
void hid_SetHandler(THIDDataHandler Handler)
{
	HidOUTHandler = Handler;
}

static TDescriptor * GetDeviceDesc(uint8_t speed) { return &udDeviceDescriptor; }
static TDescriptor * GetCfgDesc(uint8_t speed)    { return &udConfigDescriptor; }
static TDescriptor * GetDescriptor(uint8_t DescriptorType)
{
	switch(DescriptorType)
	{
		case HID_DESCRIPTOR_TYPE: return &udHIDConfigDescriptor;
		case HID_REPORT_DESC:     return &udReportDescriptor;
		default:                  return &udConfigDescriptor;
	}
}

static uint8_t DataOut (uint8_t epnum, uint8_t * data, uint16_t Length)
{
	if(HidOUTHandler) HidOUTHandler(data, Length);
	return AUSB_OK;
}

TUsbClassConfig ClassConfig = {
	USB_MANUFACTURER_STRING,
	USB_PRODUCT_STRING,
	USB_SERIALNUMBER_STRING,
	&GetConfigEP,   // GetConfigEP
	0,              // Init
	0,              // DeInit
	0,              // Main
	0,              // SOF
	&Setup,         // Setup
	&GetDeviceDesc, // GetDeviceDesc
	&GetCfgDesc,    // GetCfgDesc
	0,
	&GetDescriptor, // GetDescriptor
	&DataOut,       // DataOut
	0,              // DataIn
	0,              // EP0_RxReady
	0,              // EP0_TxSent
	0,              // IsoIncplt
};
