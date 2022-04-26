

#ifndef BEEP_H
#define BEEP_H

#include <stm32f10x.h>

extern uint8_t beep_control;    //用于控制蜂鸣器，非0响,0则灭 


//初始化PC6为输出口.并使能时钟		    
//BEEP IO初始化
void Beep_Init(void);

void task_beep_control(void);



#endif

