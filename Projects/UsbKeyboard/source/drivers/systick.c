// ***********************************************************
//	systick.с
// 	Системный таймер
//
// ***********************************************************

#include "systick.h"
#include <gd32f10x.h>
#include <string.h>

// Количество обработчиков максимальное
#define TIMER_HANDLERS    15

volatile uint32_t delay = 0;

typedef struct 
{
	void  (* Handler)(void);
	uint16_t Countdown;
	uint16_t Reload;
    
    // Запущено
    bool     Run;
    // Сработало
    bool     Fired;
} THandlers;

// Функции, кои будут вызываться
static THandlers Handlers[TIMER_HANDLERS];
static uint32_t  TimerFrequency;

void SysTick_Handler(void)
{
    int i;
    if(delay > 0) delay--;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Run)
		{
			if(--Handlers[i].Countdown == 0)
			{
				Handlers[i].Countdown = Handlers[i].Reload;
		
				// Запишем, что сработало
				Handlers[i].Fired = true;
			}
		}
	}
}

void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay){
    }
}

// Запуск таймера с заданной частотой
void timer_Init(uint32_t Frequency)
{
    if(!Frequency) return;
    
    // Сбросим параметры
    TimerFrequency = Frequency;
    memset(&Handlers[0], 0, sizeof(Handlers));
    
    // Настройка частоты
    // По умолчанию на SysTick Timer идёт частота, меньшая, чем системная
    SysTick_Config(SystemCoreClock / Frequency);
}


// Добавить функцию в список вызова, Handler будет вызываться с заданной частотой
void timer_AddFunction(uint16_t Frequency, void * Handler)
{
	int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		// Найдём пустой слот
		if(!Handlers[i].Handler)
		{
			// Обработчик, частота опроса
			Handlers[i].Run       =  true;
			Handlers[i].Fired     =  false;
			Handlers[i].Handler   = (void (*)(void))Handler;
			Handlers[i].Reload    = (TimerFrequency / Frequency);
			Handlers[i].Countdown =  Handlers[i].Reload;
			
			break;
		}
	}
}

// Изменить частоту таймера
void timer_ChangeFrequency(void * Handler, uint16_t Frequency)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Handler == Handler)
		{
            Handlers[i].Reload = (TimerFrequency / Frequency);
            Handlers[i].Countdown = Handlers[i].Reload;
            break;
        }
    }
}

// Включить таймер
void timer_Resume(void * Handler)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Handler == (void(*)(void))Handler)
		{
            Handlers[i].Run = true;
            break;
        }
    }
}

// Остановить таймер
void timer_Stop(void * Handler)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
		if(Handlers[i].Handler == Handler)
		{
            Handlers[i].Run = false;
            Handlers[i].Fired =  false;
            break;
        }
    }
}

void timer_Main(void)
{
    int i;
	
	for(i = 0; i < TIMER_HANDLERS; i++)
	{
        // Если сработало - вызовем.
        if(Handlers[i].Fired)
        {
            Handlers[i].Fired = false;

            Handlers[i].Handler();
        }
    }
}
