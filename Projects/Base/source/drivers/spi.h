// ***********************************************************
//  spi.h
//  ������� ������ SPI
// 
//  teplofizik, 2013
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _SPI_H
#define _SPI_H

    // ������������� ������ SPI
    void spi_Init(void);
	
    // ��������� ���� �� SPI
    uint8_t spi_Send(uint8_t Data);

#endif
