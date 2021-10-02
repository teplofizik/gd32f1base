// ***********************************************************
//	gpio.c
//  ����� �����-������ v2.0.0
//
//  teplofizik, 2016
// ***********************************************************

#include "gpio.h"
#include "gd32f10x_gpio.h"

static uint32_t const Ports[5] = { 
	GPIOA,
	GPIOB, 
	GPIOC, 
	GPIOD, 
	GPIOE
};

// �������������
void gp_Init(void)
{
    // ������������ �� ��� �����!
    rcu_periph_clock_enable (RCU_GPIOA);
    rcu_periph_clock_enable (RCU_GPIOB);
    rcu_periph_clock_enable (RCU_GPIOC);
    rcu_periph_clock_enable (RCU_GPIOD);
    rcu_periph_clock_enable (RCU_GPIOE);
    rcu_periph_clock_enable (RCU_AF);
}

// ��������
void gp_Uninit(void)
{
    rcu_periph_clock_disable(RCU_GPIOA);
    rcu_periph_clock_disable(RCU_GPIOB);
    rcu_periph_clock_disable(RCU_GPIOC);
    rcu_periph_clock_disable(RCU_GPIOD);
    rcu_periph_clock_disable(RCU_GPIOE);
    rcu_periph_clock_disable(RCU_AF);
}

// ��������� ����� �� �����
void gp_Output(const TPin * Pin)
{
    int P = Pin->Pin;
    if(P < 8)
    {
		// 1: output 10Mhz
        GPIO_CTL0(Ports[Pin->Port]) = (GPIO_CTL0(Ports[Pin->Port]) & ~(0x0FUL << (P * 4))) | (0x01UL << (P * 4));
    }
    else
    {
        P &= 0x07;
		// 1: output 10Mhz
        GPIO_CTL1(Ports[Pin->Port]) = (GPIO_CTL1(Ports[Pin->Port]) & ~(0x0FUL << (P * 4))) | (0x01UL << (P * 4));
    }
}

// ��������� ����� �� ����
void gp_Input(const TPin * Pin)
{
    int P = Pin->Pin;
    if(P < 8)
    {
		// 0: input floating
        GPIO_CTL0(Ports[Pin->Port]) = (GPIO_CTL0(Ports[Pin->Port]) & ~(0x0FUL << (P * 4))) | (0x04UL << (P * 4));
    }
    else
    {
        P &= 0x07;
		// 0: input floating
        GPIO_CTL1(Ports[Pin->Port]) = (GPIO_CTL1(Ports[Pin->Port]) & ~(0x0FUL << (P * 4))) | (0x04UL << (P * 4));
    }
}

// ���������� ��� ������ ���������� �����
void gp_Analog(const TPin * Pin)
{
    int P = Pin->Pin;
    if(P < 8)
    {
		// 0: input analog
        GPIO_CTL0(Ports[Pin->Port]) = (GPIO_CTL0(Ports[Pin->Port]) & ~(0x0FUL << (P * 4))) | (0x00UL << (P * 4));
    }
    else
    {
        P &= 0x07;
		// 0: input analog
        GPIO_CTL1(Ports[Pin->Port]) = (GPIO_CTL1(Ports[Pin->Port]) & ~(0x0FUL << (P * 4))) | (0x00UL << (P * 4));
    }
}

// ������� ������� �� �����
void gp_High(const TPin * Pin)
{
	// Port bit operate register (GPIOx_BOP, x=A..G)
    GPIO_BOP(Ports[Pin->Port]) = (1UL << (Pin->Pin & 0x0F));
}

// ������ ������� �� �����
void gp_Low(const TPin * Pin)
{
	// Port bit operate register (GPIOx_BOP, x=A..G)
    GPIO_BC(Ports[Pin->Port]) = (1UL << (Pin->Pin & 0x0F));
}

// �������� �� �����
bool gp_Get(const TPin * Pin)
{
    return ((GPIO_ISTAT(Ports[Pin->Port]) & (1 << Pin->Pin)) != 0);
}

// ���������� �������� �� �����
void gp_Set(const TPin * Pin, bool Value)
{
    if(Value)
        gp_High(Pin);
    else
        gp_Low(Pin);
}

// �������� Open Drain (����� �����������)
void gp_OpenDrain(const TPin * Pin, bool Enable)
{
	uint8_t Mode;
	int P = Pin->Pin;
    if(P < 8)
    {
		Mode = (GPIO_CTL0(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		if(Mode > 0)
			GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) | (0x04UL << (P * 4));
    }
    else
    {
        P &= 0x07;
		Mode = (GPIO_CTL1(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		if(Mode > 0)
			GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) | (0x04UL << (P * 4));
    }
}

// ������������ �������� ������ ������: 50 ���
void gp_FastSpeed(const TPin * Pin)
{
	uint8_t Mode;
	int P = Pin->Pin;
    if(P < 8)
    {
		Mode = (GPIO_CTL0(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		if(Mode > 0)
			GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) | (0x03UL << (P * 4));
    }
    else
    {
        P &= 0x07;
		Mode = (GPIO_CTL1(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		if(Mode > 0)
			GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) | (0x03UL << (P * 4));
    }
}

// ��������� ��������
void gp_NoPull(const TPin * Pin)
{
	uint8_t Mode;
	int P = Pin->Pin;
    if(P < 8)
    {
		Mode = (GPIO_CTL0(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 2, reset bit 3: => Floating
		if(Mode == 0)
			GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) & ~(0x08UL << (P * 4)) | (0x04UL << (P * 4));
    }
    else
    {
        P &= 0x07;
		Mode = (GPIO_CTL1(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 2, reset bit 3: => Floating
		if(Mode == 0)
			GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) & ~(0x08UL << (P * 4)) | (0x04UL << (P * 4));
    }
}

// �������� �������� � �������
void gp_PullUp(const TPin * Pin)
{
	uint8_t Mode;
	int P = Pin->Pin;
    if(P < 8)
    {
		Mode = (GPIO_CTL0(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 3, reset bit 2: => Pullup/pulldown
		if(Mode == 0)
		{
			GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) & ~(0x04UL << (P * 4)) | (0x08UL << (P * 4));
			gp_High(Pin);
		}
    }
    else
    {
        P &= 0x07;
		Mode = (GPIO_CTL1(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 3, reset bit 2: => Pullup/pulldown
		if(Mode == 0)
		{
			GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) & ~(0x04UL << (P * 4)) | (0x08UL << (P * 4));
			gp_High(Pin);
		}
    }
}

// �������� �������� � �����
void gp_PullDown(const TPin * Pin)
{
	uint8_t Mode;
	int P = Pin->Pin;
    if(P < 8)
    {
		Mode = (GPIO_CTL0(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 3, reset bit 2: => Pullup/pulldown
		if(Mode == 0)
		{
			GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) & ~(0x04UL << (P * 4)) | (0x08UL << (P * 4));
			gp_Low(Pin);
		}
    }
    else
    {
        P &= 0x07;
		Mode = (GPIO_CTL1(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 3, reset bit 2: => Pullup/pulldown
		if(Mode == 0)
		{
			GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) & ~(0x04UL << (P * 4)) | (0x08UL << (P * 4));
			gp_Low(Pin);
		}
    }
}

// ���������� ��� ������ �������������� ������� (0-3)
void gp_AlternateFunction(const TPin * Pin, bool Enable)
{
	uint8_t Mode;
	int P = Pin->Pin;
    if(P < 8)
    {
		Mode = (GPIO_CTL0(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		// Set bit 3, reset bit 2: => Pullup/pulldown
		if(Mode > 0)
		{
			if(Enable) // set bit 3
				GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) | (0x08UL << (P * 4));
			else // reset bit 3
				GPIO_CTL0(Ports[Pin->Port]) = GPIO_CTL0(Ports[Pin->Port]) & ~(0x08UL << (P * 4));
		}
    }
    else
    {
        P &= 0x07;
		Mode = (GPIO_CTL1(Ports[Pin->Port]) >> (P * 4)) & 0x03;
		
		if(Mode> 0)
		{
			if(Enable) // set bit 3
				GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) | (0x08UL << (P * 4));
			else // reset bit 3
				GPIO_CTL1(Ports[Pin->Port]) = GPIO_CTL1(Ports[Pin->Port]) & ~(0x08UL << (P * 4));
		}
    }
}

// ���������� �� ������
bool gp_Equals(const TPin * A, const TPin * B)
{
    return (A->Port == B->Port) && (A->Pin == B->Pin);
}
