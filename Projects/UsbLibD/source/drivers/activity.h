// ***********************************************************
//	activity.h
// 	Монитор активности
//
//  teplofizik, 2015
// ***********************************************************

#include <stdint.h>
#include <stdbool.h>

#ifndef _ACTIVITY_H
#define _ACTIVITY_H

    typedef enum { ACT_ETHERNET, ACT_ETH_DATA, ACT_WIFI, ACT_BLUETOOTH, ACT_COMMANDS, ACT_THREAD } TActivity;

    void act_Init(void);
    
    void act_Activate(TActivity Act);
    bool act_Get(TActivity Act);
    
#endif
