// ***********************************************************
//	system.c
//  Системные функции. Прерывания. И т.д.
//
// ***********************************************************

#include "system.h"
#include <gd32f10x.h>

// Разрешить прерывания
void irq_enable(void)
{
    __enable_irq();
}

// Запретить прерывания
void irq_disable(void)
{
    __disable_irq();
}
