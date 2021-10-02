// ***********************************************************
//	system.c
//  Системные функции. Прерывания. И т.д.
//
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _SYSTEM_H
#define _SYSTEM_H

    // Разрешить прерывания
    void irq_enable(void);
    
    // Запретить прерывания
    void irq_disable(void);

#endif
