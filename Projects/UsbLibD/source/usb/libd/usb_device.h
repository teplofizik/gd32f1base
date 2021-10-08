// ******************************************************
// usb_device.h
// USB device interface
// USB abstraction layer
// teplofizik
// ******************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _USB_DEVICE_H
#define _USB_DEVICE_H

	#define MAX_STRING_LENGTH 64
	#define TX_BUFFER_SIZE    300
	#define MAX_ENDPOINTS     5
	
	#define AUSB_DEVICE_DESCRIPTOR_TYPE              0x01
	#define AUSB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
	#define AUSB_STRING_DESCRIPTOR_TYPE              0x03
	#define AUSB_INTERFACE_DESCRIPTOR_TYPE           0x04
	#define AUSB_ENDPOINT_DESCRIPTOR_TYPE            0x05
		
	#define  AUSB_IDX_LANGID_STR                            0x00 
	#define  AUSB_IDX_MFC_STR                               0x01 
	#define  AUSB_IDX_PRODUCT_STR                           0x02
	#define  AUSB_IDX_SERIAL_STR                            0x03 
	#define  AUSB_IDX_CONFIG_STR                            0x04 
	#define  AUSB_IDX_INTERFACE_STR                         0x05 

	#define  AUSB_REQ_TYPE_STANDARD                          0x00
	#define  AUSB_REQ_TYPE_CLASS                             0x20
	#define  AUSB_REQ_TYPE_VENDOR                            0x40
	#define  AUSB_REQ_TYPE_MASK                              0x60

	#define  AUSB_REQ_GET_STATUS                             0x00
	#define  AUSB_REQ_CLEAR_FEATURE                          0x01
	#define  AUSB_REQ_SET_FEATURE                            0x03
	#define  AUSB_REQ_SET_ADDRESS                            0x05
	#define  AUSB_REQ_GET_DESCRIPTOR                         0x06
	#define  AUSB_REQ_SET_DESCRIPTOR                         0x07
	#define  AUSB_REQ_GET_CONFIGURATION                      0x08
	#define  AUSB_REQ_SET_CONFIGURATION                      0x09
	#define  AUSB_REQ_GET_INTERFACE                          0x0A
	#define  AUSB_REQ_SET_INTERFACE                          0x0B
	#define  AUSB_REQ_SYNCH_FRAME                            0x0C

	// Size of buffer for descriptors and etc
	#define  AUSB_MAX_EP0_BUFFER_SIZE                        256
	
	#define  AUSB_MAX_EP0_SIZE                               64 // 
	
	#ifndef SWAPBYTE
		#define  SWAPBYTE(addr)        (((uint16_t)(*((uint8_t *)(addr)))) + \
									   (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))
	#endif

	#ifndef LOBYTE
		#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
	#endif

	#ifndef HIBYTE
		#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
	#endif

	typedef struct  TS_UsbSetupReq {
		uint8_t   bmRequest;                      
		uint8_t   bRequest;                           
		uint16_t  wValue;                             
		uint16_t  wIndex;                             
		uint16_t  wLength;                            
	} TUsbSetupReq;
	
	typedef enum {
	  AUSB_OK   = 0,
	  AUSB_BUSY,
	  AUSB_FAIL,
	} TUsbStatus;
	
	typedef enum
	{
		EPT_Control,
		EPT_Isochr,
		EPT_Bulk,
		EPT_Interrupt
	} TEPType;
	
	typedef struct 
	{
		uint8_t * Descriptor;
		uint16_t  Length;
	} TDescriptor;

	typedef struct
	{
		uint8_t Endpoint;
		uint8_t Size;
		TEPType Type;
	} TEndpointConfig;
	
	typedef struct
	{
		int             Count;
		TEndpointConfig Items[MAX_ENDPOINTS];
	} TUsbClassEpConfig;
	
	typedef void (* TUSBCFunc)(void);
	typedef uint8_t (* TUSBCResFunc)(void);
	typedef void (* TUSBCInit)(uint8_t Config);
	typedef void (* TUSBCDeInit)(uint8_t Config);

	typedef uint8_t (* TUSBCSetup)(TUsbSetupReq * Req);
	
	typedef TDescriptor * (* TUSBCGetCfgDesc)(uint8_t speed);
	typedef TDescriptor * (* TUSBCGetDeviceDesc)(uint8_t speed);
	
	typedef bool    (*TUSBSetInterface)(uint8_t Interface, uint8_t Alternate);
	typedef TDescriptor * (*TUSBGetDescriptor)(uint8_t DescriptorType);
	
	typedef uint8_t (*TUSBCDataOut)(uint8_t EP, uint8_t * data, uint16_t length);
	typedef uint8_t (*TUSBCDataIn)(uint8_t EP);
	
	typedef TUsbClassEpConfig * (* TUSBCGetConfigEP)(uint8_t Config);
	
	typedef struct
	{
		const char *       Manufacturer;
		const char *       ProductDescription;
		const char *       SerialNumber;
		
		// Описание ендпоинтов для выбранной конфигурации
		TUSBCGetConfigEP   GetConfigEP;
		
		// Дополнительная инициализация
		TUSBCInit          Init;
		// Дополнительная деинициализация
		TUSBCDeInit        DeInit;
		// Основной цикл
		TUSBCFunc          Main;
		// Старт фрейма
		TUSBCResFunc       SOF;
		
		// Настройка
		TUSBCSetup         Setup;
		// Получение дескриптора
		TUSBCGetDeviceDesc GetDeviceDesc;
		// Получение конфигурации
		TUSBCGetCfgDesc    GetCfgDesc;
		
		
		// Установить интерфейс
		TUSBSetInterface   SetInterface;
		// Получить дескриптор по его типу (специфический  для класса)
		TUSBGetDescriptor  GetDescriptor;
		
		// Получение информации с хоста
		TUSBCDataOut       DataOut;
		// Отправка информации на хост
		TUSBCDataIn        DataIn;
		// Получены данные на контрольной конечной точке
		TUSBCResFunc       EP0_RxReady;
		// Отправлены данные на контрольной конечной точке
		TUSBCResFunc       EP0_TxSent;
		// Изохронное событие не завершено??
		TUSBCResFunc       IsoIncplt;
	} TUsbClassConfig;
	
	extern TUsbClassConfig ClassConfig;
	
	// Сконфигурирован ли USB?
	bool ud_IsConfigured(void);

	// Инициализация USB
	void ud_Init(void);
	// Основной цикл USB
	void ud_Main(void);

	void ud_CtlPrepareRx(uint8_t * Data, uint16_t Length);
	// Отправка данных по контрольной конечной точке
	void ud_CtlSendData(uint8_t * Data, uint16_t Length);
	// Отправка данных
	bool ud_SendData(uint8_t EP, uint8_t * data, uint16_t length);
	
#endif 
