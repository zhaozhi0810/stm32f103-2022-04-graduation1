#ifndef __LED_H
#define __LED_H	 
#include "sys.h"



void LED_Init(void);//初始化
//void GPIO_DUO_Init(void);

//拉高 熄灭led index等于0，表示第一个led，其他表示第二个led
void Led_Off(uint8_t index);	

//拉高 熄灭led index等于0，表示第一个led，其他表示第二个led
void Led_Off(uint8_t index);


//led0 用于闪烁，表示程序正在运行
//每500ms调用一次,这样就每500ms闪烁一次
void task_led1_show(void);

#endif
