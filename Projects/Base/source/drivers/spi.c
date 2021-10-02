// ***********************************************************
//  spi.c
//  Драйвер модуля SPI
//
//  teplofizik, 2013
// ***********************************************************

#include "spi.h"
#include "gpio.h"
#include "gd32f10x_spi.h"

#define SPI_MODULE 0

static const TPin SCK = { PA, 5 };
static const TPin MISO = { PA, 6 };
static const TPin MOSI = { PA, 7 };

// Инициализация модуля SPI
void spi_Init(void)
{	spi_parameter_struct spi_struct;
	
    gp_AlternateFunction(&SCK, true);
    gp_AlternateFunction(&MISO, true);
    gp_AlternateFunction(&MOSI, true);
	
	spi_struct_para_init(&spi_struct);
	spi_init(SPI_MODULE, &spi_struct);
	spi_enable(SPI_MODULE);
}

// Отправить байт по SPI
uint8_t spi_Send(uint8_t Data)
{
	spi_i2s_data_transmit(SPI_MODULE, Data);
	
	while(spi_i2s_flag_get(SPI_MODULE, SPI_FLAG_RBNE) == RESET) { }
	
	return spi_i2s_data_receive(SPI_MODULE);
}
