// ***********************************************************
//	activity.с
// 	Монитор активности
//
//  teplofizik, 2015
// ***********************************************************

#include "activity.h"
#include "systick.h"

int Timeouts[10];

static const int Count = sizeof(Timeouts)/sizeof(Timeouts[0]);

static void act_Timer(void)
{
    int i;
    for(i = 0; i < Count; i++) if(Timeouts[i]) Timeouts[i]--;
}

void act_Init(void)
{
    int i;
    for(i = 0; i < Count; i++) Timeouts[i] = 0;
    
    timer_AddFunction(100, &act_Timer);
}
    
void act_Activate(TActivity Act)
{
    if(Act >= Count) return;
        
    Timeouts[Act] = 30;
}

bool act_Get(TActivity Act)
{
    if(Act >= Count) 
        return false;
    else
        return Timeouts[Act] > 0;
}
