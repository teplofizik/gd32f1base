 /**
  usb_class_conf.h
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CLASS_CONF__H__
#define __USB_CLASS_CONF__H__

	#define USBD_CFG_MAX_NUM                      1
	#define USBD_ITF_MAX_NUM                      1

	#define USB_MAX_STR_DESC_SIZ                  100 

	#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED 

	// Endpoint sizes
	#define RX_FIFO_SIZE                          128
	#define TX0_FIFO_SIZE                         64
	#define TX1_FIFO_SIZE                         128
	#define TX2_FIFO_SIZE                         64
	#define TX3_FIFO_SIZE                         0
	
#endif
