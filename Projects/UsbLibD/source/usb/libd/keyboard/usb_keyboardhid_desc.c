// ******************************************************
// usb_hid_desc.c
// HID interface descriptors
// teplofizik
// ******************************************************
 
#include "usb_keyboardhid.h"
#include "usb_class_conf.h"
#include "usb_device.h"

const int RX_FIFO_FS_SIZE = RX_FIFO_SIZE;
const int TX0_FIFO_FS_SIZE = TX0_FIFO_SIZE;
const int TX1_FIFO_FS_SIZE = TX1_FIFO_SIZE;
const int TX2_FIFO_FS_SIZE = TX2_FIFO_SIZE;
const int TX3_FIFO_FS_SIZE = TX3_FIFO_SIZE;

#define USB_SIZ_DEVICE_DESC            18
#define USB_HID_CONFIG_DESC_SIZ        34
#define HID_MOUSE_REPORT_DESC_SIZE     74
#define USB_HID_DESC_SIZ                9	//CUSTOMHID_SIZ_HID_DESC

/* USB Standard Device Descriptor */
uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    AUSB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    AUSB_MAX_EP0_SIZE,          /*bMaxPacketSize*/
    LOBYTE(USB_VID),           /*idVendor*/
    HIBYTE(USB_VID),           /*idVendor*/
    LOBYTE(USB_PID),           /*idVendor*/
    HIBYTE(USB_PID),           /*idVendor*/
    0x00,                       /*bcdDevice rel. 1.00*/
    0x01,
    AUSB_IDX_MFC_STR,           /*Index of manufacturer  string*/
    AUSB_IDX_PRODUCT_STR,       /*Index of product string*/
    AUSB_IDX_SERIAL_STR,        /*Index of serial number string*/
    USBD_CFG_MAX_NUM            /*bNumConfigurations*/
}; /* USB_DeviceDescriptor */

uint8_t CustomHID_ReportDescriptor[] =
{
    0x05, 0x01,  /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x06,  /* USAGE (Keyboard) */
    0xa1, 0x01,  /* COLLECTION (Application) */

    0x05, 0x07,  /* USAGE_PAGE (Keyboard/Keypad) */
    0x19, 0xe0,  /* USAGE_MINIMUM (Keyboard LeftControl) */
    0x29, 0xe7,  /* USAGE_MAXIMUM (Keyboard Right GUI) */
    0x15, 0x00,  /* LOGICAL_MINIMUM (0) */
    0x25, 0x01,  /* LOGICAL_MAXIMUM (1) */
    0x95, 0x08,  /* REPORT_COUNT (8) */
    0x75, 0x01,  /* REPORT_SIZE (1) */
    0x81, 0x02,  /* INPUT (Data,Var,Abs) */

    0x95, 0x01,  /* REPORT_COUNT (1) */
    0x75, 0x08,  /* REPORT_SIZE (8) */
    0x81, 0x03,  /* INPUT (Cnst,Var,Abs) */

    0x95, 0x06,  /* REPORT_COUNT (6) */
    0x75, 0x08,  /* REPORT_SIZE (8) */
    0x15, 0x00,  /* LOGICAL_MINIMUM (0) */
    0x26, 0xFF, 0x00,  /* LOGICAL_MAXIMUM (255) */
    0x05, 0x07,  /* USAGE_PAGE (Keyboard/Keypad) */
    0x19, 0x00,  /* USAGE_MINIMUM (Reserved (no event indicated)) */
    0x29, 0x65,  /* USAGE_MAXIMUM (Keyboard Application) */
    0x81, 0x00,  /* INPUT (Data,Ary,Abs) */

    0xc0         /* END_COLLECTION */
}; /* CustomHID_ReportDescriptor */
  
uint8_t USBD_HID_CfgDesc[USB_HID_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuration Descriptor size */
  AUSB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  // 0xC0,         /*bmAttributes: bus powered */
  0xA0,         /*bmAttributes: ... */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of Custom HID interface ****************/
  /* 09 */
  0x09,         /* bLength: Interface Descriptor size */
  AUSB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface descriptor type */
  0x00,         /* bInterfaceNumber: Number of Interface */
  0x00,         /* bAlternateSetting: Alternate setting */
  0x01,         /* bNumEndpoints */
  0x03,         /* bInterfaceClass: HID */
  0x01,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x01,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0,            /* iInterface: Index of string descriptor */
  /******************** Descriptor of Custom HID HID ********************/
  /* 18 */
  0x09,         /* bLength: HID Descriptor size */
  HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
  0x11,         /* bcdHID: HID Class Spec release number ?? 0x11 ??*/
  0x01,
  0x00,         /* bCountryCode: Hardware target country */
  0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
  0x22,         /* bDescriptorType */
  sizeof(CustomHID_ReportDescriptor),/* wItemLength: Total length of Report descriptor */
  0x00,
  /******************** Descriptor of Custom HID endpoints ******************/
  /* 27 */
  0x07,          /* bLength: Endpoint Descriptor size */
  AUSB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

  HID_IN_EP,     /* bEndpointAddress: Endpoint Address (IN) */
  0x03,          /* bmAttributes: Interrupt endpoint */
  HID_IN_PACKET_SIZE, /* wMaxPacketSize */
  0x00,
  0x40,          /* bInterval: Polling Interval (10 ms) */
  /* 34 */
}; /* CustomHID_ConfigDescriptor */

TDescriptor udDeviceDescriptor = {USBD_DeviceDesc, sizeof(USBD_DeviceDesc)};
TDescriptor udConfigDescriptor = {USBD_HID_CfgDesc, sizeof(USBD_HID_CfgDesc)};
TDescriptor udHIDConfigDescriptor = {USBD_HID_CfgDesc + 9, sizeof(USBD_HID_CfgDesc) - 9};
TDescriptor udReportDescriptor = {CustomHID_ReportDescriptor, sizeof(CustomHID_ReportDescriptor)};
