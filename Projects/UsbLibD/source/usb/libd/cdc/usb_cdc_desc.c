// ******************************************************
// usb_desc.c
// HID interface descriptors
// teplofizik
// ******************************************************
 
#include "usb_cdc.h"
#include "usb_class_conf.h"
#include "usb_device.h"

const int RX_FIFO_FS_SIZE = RX_FIFO_SIZE;
const int TX0_FIFO_FS_SIZE = TX0_FIFO_SIZE;
const int TX1_FIFO_FS_SIZE = TX1_FIFO_SIZE;
const int TX2_FIFO_FS_SIZE = TX2_FIFO_SIZE;
const int TX3_FIFO_FS_SIZE = TX3_FIFO_SIZE;

#define DEVICE_CLASS_CDC                        0x02
#define DEVICE_SUBCLASS_CDC                     0x00

#define USB_SIZ_DEVICE_DESC            18
#define USB_CDC_CONFIG_DESC_SIZE      (67)
#define USB_CDC_DESC_SIZE             (67-9)

/* USB Standard Device Descriptor */
uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] =
{
    0x12,                       /*bLength */
    AUSB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x0a,                       /*bcdUSB */
    0x01,
    DEVICE_CLASS_CDC,           /*bDeviceClass*/
    DEVICE_SUBCLASS_CDC,        /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    AUSB_MAX_EP0_SIZE,          /*bMaxPacketSize*/
    LOBYTE(USB_VID),            /*idVendor*/
    HIBYTE(USB_VID),            /*idVendor*/
    LOBYTE(USB_PID),            /*idVendor*/
    HIBYTE(USB_PID),            /*idVendor*/
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    AUSB_IDX_MFC_STR,           /*Index of manufacturer  string*/
    AUSB_IDX_PRODUCT_STR,       /*Index of product string*/
    AUSB_IDX_SERIAL_STR,        /*Index of serial number string*/
    USBD_CFG_MAX_NUM            /*bNumConfigurations*/
}; /* USB_DeviceDescriptor */

uint8_t USBD_CDC_CfgDesc[USB_CDC_CONFIG_DESC_SIZE] =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  AUSB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
  USB_CDC_CONFIG_DESC_SIZE,                /* wTotalLength:no of returned bytes */
  0x00,
  0x02,   /* bNumInterfaces: 2 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */
  
  /*---------------------------------------------------------------------------*/
  
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  AUSB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */
  
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,
  
  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */
  
  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  
  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */
  
  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  AUSB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),    /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
#ifdef USE_USB_OTG_HS
  0x10,                           /* bInterval: */
#else
  0xFF,                           /* bInterval: */
#endif /* USE_USB_OTG_HS */
  
  /*---------------------------------------------------------------------------*/
  
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  AUSB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  AUSB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  AUSB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00                               /* bInterval: ignore for Bulk transfer */
};

uint8_t USBD_CDC_QualifierDescriptor[10] = {
  /* Device qualifier descriptor */
  0x0a, /* bLength */
  0x06, /* bDescriptorType: USB_DEVICE_QUALIFIER_DESCRIPTOR */
  0x00,        /*bcdUSB */
  0x02,
  0x00, /* bDeviceClass */
  0x00, /* bDeviceSubClass */
  0x00, /* bDeviceProtocol */
  0x08, /* bMaxPacketSize0 */
  0x00, /* bNumConfigurations */
  0x00, /* bReserved */
};

TDescriptor udDeviceDescriptor = { USBD_DeviceDesc, sizeof(USBD_DeviceDesc) };
TDescriptor udConfigDescriptor = { USBD_CDC_CfgDesc, sizeof(USBD_CDC_CfgDesc) };
TDescriptor udCDCConfigDescriptor = { USBD_CDC_CfgDesc + 9, sizeof(USBD_CDC_CfgDesc) - 9 };

TDescriptor udQualifierDescriptor = { USBD_CDC_QualifierDescriptor, sizeof(USBD_CDC_QualifierDescriptor) };
