// ***********************************************************
//  spi.h
//  Драйвер модуля SPI
// 
//  teplofizik, 2013
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _SPI_H
#define _SPI_H

    // Инициализация модуля SPI
    void spi_Init(void);
	
    // Отправить байт по SPI
    uint8_t spi_Send(uint8_t Data);

#endif
