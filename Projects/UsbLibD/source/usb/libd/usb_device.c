// ******************************************************
// usb_device.c
// USB device interface
// teplofizik
// ******************************************************

#include "usb_device.h"
#include "drv_usb_hw.h"
#include "drv_usbd_int.h"
#include "usbd_enum.h"

#include <string.h>

#define MIN(a,b) ((a<b) ? a : b)
#define NO_CMD                           0xFFU

typedef struct
{
	uint8_t   Endpoint;
	uint16_t  Size;
	uint8_t * Data;
} TEPBuffer;

TEPBuffer EPBuffer[MAX_ENDPOINTS+1];

const char * udi_GetManufacturer(void) { return ClassConfig.Manufacturer; }
const char * udi_GetProductDescription(void)  { return ClassConfig.ProductDescription; }
const char * udi_GetSerialNumber(void) { return ClassConfig.SerialNumber; }

usb_core_driver device_driver;

extern uint32_t usbfs_prescaler;

const usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength         = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR
     },
    .wLANGID              = ENG_LANGID
};

static usb_desc_str manufacturer_string;
static usb_desc_str product_string;
static usb_desc_str serial_string;

void *const usbd_hid_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc device_desc = {
    .dev_desc    = (uint8_t *)0,
    .config_desc = (uint8_t *)0,
    .strings     = usbd_hid_strings
};

/* local function prototypes ('static') */
static uint8_t dev_init    (usb_dev *udev, uint8_t config_index);
static uint8_t dev_deinit  (usb_dev *udev, uint8_t config_index);
static uint8_t dev_req     (usb_dev *udev, usb_req *req);
static uint8_t dev_data_in (usb_dev *udev, uint8_t ep_num);
static uint8_t dev_data_out(usb_dev *udev, uint8_t ep_num);
static uint8_t dev_sof (usb_dev *udev);

usb_class_core usbd_device_cb = {
    .command         = NO_CMD,
    .alter_set       = 0U,

    .init            = dev_init,
    .deinit          = dev_deinit,
    .req_proc        = dev_req,
    .data_in         = dev_data_in,
    .data_out        = dev_data_out,
	
	.SOF             = dev_sof
};

int8_t GetEndpointIndex(uint8_t EP)
{
	int i;
	for(i = 0; i < MAX_ENDPOINTS+1; i++)
	{
		if(EPBuffer[i].Endpoint == EP) return i;
	}
	
	return -1;
}

TEPBuffer * GetRxBuffer(uint8_t EP)
{
	int EPi = GetEndpointIndex(EP);
	
	if(EPi >= 0) 
		return &EPBuffer[EPi];
	else
		return 0;
}

const usb_desc_ep * GetEndpointDescriptor(uint8_t Index) {
	if(ClassConfig.GetCfgDesc)
	{
		TDescriptor * CfgDesc = ClassConfig.GetCfgDesc(0);
		
		const uint8_t * Descriptor = CfgDesc->Descriptor;
		uint16_t ReadedLength = 0;
		
		uint8_t EpIndex = 0;
		while(ReadedLength < CfgDesc->Length) {
			uint8_t Length = Descriptor[0];
			uint8_t Type = Descriptor[1];
		
			if(Type == AUSB_ENDPOINT_DESCRIPTOR_TYPE)
			{
				if(Index == EpIndex)
					return (const usb_desc_ep *)Descriptor;
				
				EpIndex++;
			}
			
			Descriptor += Length;
			ReadedLength += Length;
		}
		
		return 0;
	}
	else
		return 0;
}

const usb_desc_ep * GetEndpointDescriptorByAddress(uint8_t Address)
{
	const usb_desc_ep * ep_desc = GetEndpointDescriptor(0);
	int Index = 0;
	
	/* initialize the data TX endpoint */
	while(ep_desc)
	{
		if(ep_desc->bEndpointAddress == Address)
			return ep_desc;
		
		Index++;
		ep_desc = GetEndpointDescriptor(Index);
	}
	
	return 0;
}

static void recv_endpoint(usb_dev *udev, const usb_desc_ep * ep_desc)
{
	TEPBuffer * Buffer = GetRxBuffer(ep_desc->bEndpointAddress);
	
	usbd_ep_recev (udev, ep_desc->bEndpointAddress, Buffer->Data, Buffer->Size);
}

static uint8_t dev_init    (usb_dev *udev, uint8_t config_index)
{
	const usb_desc_ep * ep_desc = GetEndpointDescriptor(0);
	int Index = 0;
	
	// 0x00 ep for setup
	EPBuffer[Index].Endpoint = 0;
	EPBuffer[Index].Data = malloc(AUSB_MAX_EP0_BUFFER_SIZE);
	EPBuffer[Index].Size = AUSB_MAX_EP0_BUFFER_SIZE;
	
	Index++;
	/* initialize the data TX endpoint */
	while(ep_desc)
	{
		EPBuffer[Index].Endpoint = ep_desc->bEndpointAddress;
		EPBuffer[Index].Data = malloc(ep_desc->wMaxPacketSize);
		EPBuffer[Index].Size = ep_desc->wMaxPacketSize;
		
		udev->dev.class_data[0] = (void *)0;
		
		usbd_ep_setup (udev, ep_desc);
		
		if((ep_desc->bEndpointAddress & 0x80) == 0) // OUT 
		{
			recv_endpoint(udev, ep_desc);
		}
		
		Index++;
		ep_desc = GetEndpointDescriptor(Index);
	}
	
	//if (NULL != udev->dev.user_data) {
	//	((hid_fop_handler *)udev->dev.user_data)->hid_itf_config();
	//}

	return USBD_OK;
}

static uint8_t dev_deinit  (usb_dev *udev, uint8_t config_index)
{
	const usb_desc_ep * ep_desc = GetEndpointDescriptor(0);
	int Index = 0;
	
    /* deinitialize endpoints */
	while(ep_desc)
	{
		uint8_t Address = ep_desc->bEndpointAddress;
		usbd_ep_clear(udev, Address);
		
		Index++;
		ep_desc = GetEndpointDescriptor(Index);
	}

	return USBD_OK;
}

static uint8_t dev_req (usb_dev *udev, usb_req *req)
{
	if ((req->bmRequestType & AUSB_REQ_TYPE_MASK) == AUSB_REQ_TYPE_STANDARD)
	{
		switch (req->bRequest)
		{
			case AUSB_REQ_GET_DESCRIPTOR: 
				{
					if(ClassConfig.GetDescriptor)
					{
						uint8_t Type = (req->wValue >> 8) & 0xFF;
						TDescriptor * D = ClassConfig.GetDescriptor(Type);
						
						uint16_t len = MIN(D->Length, req->wLength);
						ud_CtlSendData (D->Descriptor, len);
					}
					else
						ud_CtlSendData (0, 0);
				}
				break;
		}
	}
	else
	{
		if(ClassConfig.Setup) 
			return (TUsbStatus)ClassConfig.Setup((TUsbSetupReq*)req);
		else
			return REQ_NOTSUPP;
	}
	return REQ_NOTSUPP;
}

static uint8_t dev_data_in (usb_dev *udev, uint8_t ep_num)
{
	if(ClassConfig.DataIn)
	{
		return ClassConfig.DataIn(ep_num);
	}
	else
	{	
		return USBD_FAIL;
	}
}

static uint8_t dev_data_out(usb_dev *udev, uint8_t ep_num)
{
	if(ClassConfig.DataOut)
	{
		const usb_desc_ep * ep_desc = GetEndpointDescriptorByAddress(ep_num);
		TEPBuffer * Buffer = GetRxBuffer(ep_num);
		
		if(Buffer && ep_desc)
		{
			uint8_t Res = ClassConfig.DataOut(ep_num, Buffer->Data, Buffer->Size);
			
			recv_endpoint(udev, ep_desc);
			return Res;
		}
		else
			return USBD_FAIL;
	}
	else
		return USBD_FAIL;	
}

static uint8_t dev_sof (usb_dev *udev)
{
	if(ClassConfig.SOF)
	{
		return ClassConfig.SOF();
	}
	
	return USBD_OK;
}

// Сконфигурирован ли USB?
bool ud_IsConfigured(void)
{
	return (USBD_CONFIGURED == device_driver.dev.cur_status);
}

static void FillStringDescriptor(usb_desc_str * string, const char * Text)
{
	size_t size = strlen(Text);
	size_t i = 0;
	
	string->header.bDescriptorType = USB_DESCTYPE_STR;
	string->header.bLength = size;
	for(i = 0; i < size; i++)
		string->unicode_string[i] = Text[i];
}

static void InitDescriptors()
{
	uint8_t speed = 0; // HS/FS ??
	
	FillStringDescriptor(&manufacturer_string, udi_GetManufacturer());
	FillStringDescriptor(&product_string, udi_GetProductDescription());
	FillStringDescriptor(&serial_string, udi_GetSerialNumber());
	
	device_desc.dev_desc = ClassConfig.GetDeviceDesc(speed)->Descriptor;
	device_desc.config_desc = ClassConfig.GetCfgDesc(speed)->Descriptor;
}

void ud_Init(void)
{
	memset(EPBuffer, 0, sizeof(EPBuffer));
	
	InitDescriptors();
	
    usb_rcu_config();
    usb_timer_init();
	
   //  hid_itfop_register (&hid_keyboard, &fop_handler);
	
    usbd_init (&device_driver, USB_CORE_ENUM_FS, &device_desc, &usbd_device_cb);
    usb_intr_config();
}

void ud_Main(void)
{	
	/* check if USB device is enumerated successfully */
	if(ud_IsConfigured())
	{
		//fop_handler.hid_itf_data_process(&hid_keyboard);
		
		if(ClassConfig.Main) ClassConfig.Main();
	}
}

void ud_CtlPrepareRx(uint8_t * Data, uint16_t Length)
{
	usb_dev *udev = &device_driver;
	
	TEPBuffer * Buffer = GetRxBuffer(0);
	
	usbd_ep_recev (udev, 0, Data, Length);
}


// Отправка данных через конечную точку
bool ud_SendData(uint8_t EP, uint8_t * data, uint16_t length)
{
	usb_dev *udev = &device_driver;
	
	usbd_ep_send (udev, EP, data, length);
	
	return true;
}

// Отправка данных по контрольной конечной точке
void ud_CtlSendData(uint8_t * Data, uint16_t Length)
{
	TEPBuffer * Buffer = GetRxBuffer(0);
	usb_dev *udev = &device_driver;
	
	if(Length > AUSB_MAX_EP0_BUFFER_SIZE)
	{
		// ERROR
	}
	else
	{
		usb_transc *transc = &udev->dev.transc_in[0];
		
		memcpy(Buffer->Data, Data, Length);
		
		transc->xfer_buf = Buffer->Data;
		transc->remain_len = Length;
	}
}

/*!
    \brief      this function handles USBFS interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler (void)
{
    usbd_isr(&device_driver);
}


/*!
    \brief      resume mcu clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HSE */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HSE is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)){
    }

    /* enable PLL1 */
    rcu_osci_on(RCU_PLL1_CK);

    /* wait till PLL1 is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLL1STB)){
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLL != rcu_system_clock_source_get()){
    }
}

/*!
    \brief      this function handles USBFS wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (device_driver.bp.low_power) {
        resume_mcu_clk();

        rcu_usb_clock_config(usbfs_prescaler);

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_active(&device_driver);
    }

    exti_interrupt_flag_clear(EXTI_18);
}
