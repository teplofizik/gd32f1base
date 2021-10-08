// ***********************************************************
//	systick.h
// 	Системный таймер
//
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _SYSTICK_H
#define _SYSTICK_H

    // Запуск таймера с заданной частотой
    void timer_Init(uint32_t Frequency);
    
	// Добавить функцию в список вызова, Handler будет вызываться с заданной частотой
	// Аргументы: 2
	//  Frequency - частота вызова (при увеличении снижается точность)
	//  Handler - функция, которая будет вызываться
	// Результат: нет
	void timer_AddFunction(uint16_t Frequency, void * Handler);
	
    // Остановить таймер
    void timer_Stop(void * Handler);
	
    // Включить таймер
    void timer_Resume(void * Handler);

    // Изменить частоту таймера
    void timer_ChangeFrequency(void * Handler, uint16_t Frequency);
    
    // Главный цикл. В нём вызываются обработчики таймера
    void timer_Main(void);

	void delay_1ms(uint32_t count);

#endif
