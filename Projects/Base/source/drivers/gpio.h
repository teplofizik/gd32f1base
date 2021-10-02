// ***********************************************************
//	gpio.h
//  ����� �����-������ v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _GPIO_H
#define _GPIO_H

    typedef enum { 
		PA, 
		PB, 
		PC, 
		PD, 
		PE
	} TPort;

    typedef struct
    {
        TPort   Port;
        uint8_t Pin;
    } TPin;
        
    //
    void gp_Init(void);
    void gp_Uninit(void);
	
    // ��������� ����� �� �����
    void gp_Output(const TPin * Pin);

    // ��������� ����� �� ����
    void gp_Input(const TPin * Pin);

    // ���������� ��� ������ ���������� �����
    void gp_Analog(const TPin * Pin);
    
    // ������� ������� �� �����
    void gp_High(const TPin * Pin);

    // ������ ������� �� �����
    void gp_Low(const TPin * Pin);

    // �������� �� �����
    bool gp_Get(const TPin * Pin);

    // ���������� �������� �� �����
    void gp_Set(const TPin * Pin, bool Value);
    
    // �������� Open Drain (����� �����������)
    void gp_OpenDrain(const TPin * Pin, bool Enable);
    
    // ������������ �������� ������ ������: 50 ���
    void gp_FastSpeed(const TPin * Pin);
    
    // ��������� ��������
    void gp_NoPull(const TPin * Pin);

    // �������� �������� � �������
    void gp_PullUp(const TPin * Pin);
        
    // �������� �������� � �����
    void gp_PullDown(const TPin * Pin);

    // ���������� ��� ������ �������������� ������� (0-3)
    void gp_AlternateFunction(const TPin * Pin, bool Enable);

    // ���������� �� ������?
    bool gp_Equals(const TPin * A, const TPin * B);

#endif
