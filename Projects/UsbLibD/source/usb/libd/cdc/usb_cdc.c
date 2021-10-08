// ******************************************************
// usb_cdc.c
// Template usb interface
// teplofizik
// ******************************************************
 
#include "usb_cdc.h"
#include "usb_device.h"
#include "usb_class_conf.h"

#define QUALIFIER_DESCRIPTOR_TYPE               0x06
#define CDC_DESCRIPTOR_TYPE                     0x21

#define DEVICE_CLASS_CDC                        0x02
#define DEVICE_SUBCLASS_CDC                     0x00

/**************************************************/
/* CDC Requests                                   */
/**************************************************/
#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define NO_CMD                                  0xFF

extern TDescriptor udDeviceDescriptor;
extern TDescriptor udConfigDescriptor;
extern TDescriptor udCDCConfigDescriptor;
extern TDescriptor udQualifierDescriptor;

TUsbClassEpConfig EPConfig = {
	3, // Endpoint count
	{
		{ CDC_IN_EP,  CDC_DATA_MAX_PACKET_SIZE, EPT_Bulk }, // In
		{ CDC_OUT_EP, CDC_DATA_MAX_PACKET_SIZE, EPT_Bulk }, // Out
		{ CDC_CMD_EP, CDC_CMD_PACKET_SIZE, EPT_Interrupt}   // Cmd
	}
};

uint8_t CmdBuff[CDC_CMD_PACKET_SIZE];

static uint32_t cdcCmd = 0xFF;
static uint32_t cdcLen = 0;

uint8_t  APP_Rx_Buffer[APP_RX_DATA_SIZE]; 
uint32_t APP_Rx_Read  = 0;
uint32_t APP_Rx_Write = 0;

uint8_t  APP_Tx_Buffer[APP_TX_DATA_SIZE]; 
uint32_t APP_Tx_Read  = 0;
uint32_t APP_Tx_Write = 0;
bool     TXFree = true;

uint32_t USBD_CDC_AltSet = 0;

uint8_t  USB_Tx_Buffer[CDC_DATA_MAX_PACKET_SIZE]; 

TCharHandler CharHandler = 0;

static void SendTxData(void);

// *******************************************************
//
// CDC User Interface
// 
// *******************************************************

void cdc_AddHandler(TCharHandler Handler)
{
	CharHandler = Handler;
}

void cdc_WriteChar(char C)
{
	APP_Tx_Buffer[APP_Tx_Write] = C;
			
	if(APP_Tx_Write < APP_TX_DATA_SIZE - 1)
		APP_Tx_Write++;
	else
		APP_Tx_Write = 0;
}

// Запись символов на отправку в хост
void cdc_WriteData(uint8_t * Data, uint16_t Length)
{
	int i;
	for(i = 0; i < Length; i++) cdc_WriteChar(Data[i]);
	
	if(TXFree) SendTxData();
}

// *******************************************************
//
// CDC USB Interface
// 
// *******************************************************
static TUsbClassEpConfig * GetConfigEP(uint8_t Config)
{
	return &EPConfig;
}

static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
	switch (Cmd)
	{
	case SEND_ENCAPSULATED_COMMAND:
		/* Not  needed for this driver */
		break;

	case GET_ENCAPSULATED_RESPONSE:
		/* Not  needed for this driver */
		break;

	case SET_COMM_FEATURE:
		/* Not  needed for this driver */
		break;

	case GET_COMM_FEATURE:
		/* Not  needed for this driver */
		break;

	case CLEAR_COMM_FEATURE:
		/* Not  needed for this driver */
		break;

	case SET_LINE_CODING://команда настроить контроллер
		//эта команда вызвывается, если на компьютере меняются настройки порта
		break;

	case GET_LINE_CODING://команда считать настройки из контроллера
		break;

	case SET_CONTROL_LINE_STATE:
		/* Not  needed for this driver */
		break;

	case SEND_BREAK:
		/* Not  needed for this driver */
		break;    
    
	default:
		break;
	}

	return AUSB_OK;
}

static uint8_t Setup(TUsbSetupReq * req)
{
	if((req->bmRequest & AUSB_REQ_TYPE_MASK) == AUSB_REQ_TYPE_CLASS)
	{
		/* Check if the request is a data setup packet */
		if (req->wLength)
		{
			/* Check if the request is Device-to-Host */
			if(req->bmRequest & 0x80)
			{
				/* Get the data to be sent to Host from interface layer */
				VCP_Ctrl(req->bRequest, CmdBuff, req->wLength);
				ud_CtlSendData(CmdBuff, req->wLength);
			}
			else /* Host-to-Device request */
			{
				cdcCmd = req->bRequest;
				cdcLen = req->wLength;
						
				/* Prepare the reception of the buffer over EP0
				Next step: the received data will be managed in usbd_cdc_EP0_TxSent() 
				function. */
				//ud_CtlPrepareRx(CmdBuff, req->wLength);  
			}
		} 
		else /* No Data request */
		{
			/* Transfer the command to the interface layer */
			VCP_Ctrl(req->bRequest, 0, 0);
		}
	}
	
	return AUSB_OK;
}

static TDescriptor * GetDeviceDesc(uint8_t speed) { return &udDeviceDescriptor; }
static TDescriptor * GetCfgDesc(uint8_t speed)    { return &udConfigDescriptor; }
static TDescriptor * GetDescriptor(uint8_t DescriptorType)
{
	if(DescriptorType == CDC_DESCRIPTOR_TYPE)
		return &udCDCConfigDescriptor;
	if(DescriptorType == QUALIFIER_DESCRIPTOR_TYPE)
		return &udQualifierDescriptor;
	else	
		return &udConfigDescriptor;
}

static uint8_t EP0_RxReady(void)
{
	if (cdcCmd != NO_CMD)
	{
		/* Process the data */
		VCP_Ctrl(cdcCmd, CmdBuff, cdcLen);
    
		/* Reset the command variable to default value */
		cdcCmd = NO_CMD;
	}
  
	return AUSB_OK;
}

static uint8_t DataOut (uint8_t epnum, uint8_t * data, uint16_t Length)
{
	int i;
	for(i = 0; i < Length; i++)
	{
		APP_Rx_Buffer[APP_Rx_Write] = data[i];
		
		if(APP_Rx_Write < APP_RX_DATA_SIZE - 1)
			APP_Rx_Write++;
		else
			APP_Rx_Write = 0;
	}
	
	return AUSB_OK;
}

static void SendTxData(void)
{
	if (APP_Tx_Read != APP_Tx_Write) 
	{
		uint16_t USB_Tx_length = 0;
		int i;
		for(i = 0; (i < CDC_DATA_MAX_PACKET_SIZE) && (APP_Tx_Read != APP_Tx_Write); i++)
		{
			USB_Tx_Buffer[i] = APP_Tx_Buffer[APP_Tx_Read];
			
			if(APP_Tx_Read < APP_TX_DATA_SIZE - 1)
				APP_Tx_Read++;
			else
				APP_Tx_Read = 0;
			
			USB_Tx_length++;
		}
		
		TXFree = false;
		/* Prepare the available data buffer to be sent on IN endpoint */
		ud_SendData(CDC_IN_EP,
					(uint8_t*)&USB_Tx_Buffer[0],
					USB_Tx_length);
	}
}

static uint8_t DataIn(uint8_t epnum)
{
	if(epnum == CDC_IN_EP)
	{
		TXFree = true;
		SendTxData();
	}
		
	return AUSB_OK;
}

static uint8_t SOF(void)
{
	static uint32_t FrameCount = 0;
  
	if (FrameCount++ == CDC_IN_FRAME_INTERVAL)
	{
		/* Reset the frame counter */
		FrameCount = 0;
    
		/* Check the data to be sent through IN pipe */
		SendTxData();
	}
  
	return AUSB_OK;
}

static void Main(void)
{
	const int MaxProcessedBytes = 32;
	int Processed = 0;
	
	if(CharHandler) 
	{
		while ((APP_Rx_Read != APP_Rx_Write) && (Processed++ < MaxProcessedBytes))
		{
			CharHandler(APP_Rx_Buffer[APP_Rx_Read]);
			if(APP_Rx_Read < APP_RX_DATA_SIZE - 1)
				APP_Rx_Read++;
			else
				APP_Rx_Read = 0;
		}
	}
}

TUsbClassConfig ClassConfig = {
	USB_MANUFACTURER_STRING,
	USB_PRODUCT_STRING,
	USB_SERIALNUMBER_STRING,
	&GetConfigEP,   // GetConfigEP
	0,              // Init
	0,              // DeInit
	&Main,          // Main
	&SOF,           // SOF
	&Setup,         // Setup
	&GetDeviceDesc, // GetDeviceDesc
	&GetCfgDesc,    // GetCfgDesc
	0,              // SetInterface
	&GetDescriptor, // GetDescriptor
	&DataOut,       // DataOut
	&DataIn,        // DataIn
	&EP0_RxReady,   // EP0_RxReady
	0,              // EP0_TxSent
	0,              // IsoIncplt
};
