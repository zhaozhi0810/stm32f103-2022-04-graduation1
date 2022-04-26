#ifndef __DELAY_H
#define __DELAY_H 			   


#include <stm32f10x.h>



void my_systick_init(void);

//中断方式
//参数 ： sysclock AHB时钟，单位为MHz
void my_systick_int_init(void);


/*
	us 的延时时间798915us 
	//取一个整数，好计算

	us的延时不能超过70,0000us

*/
void delay_us(uint32_t dus);


/*
	毫秒延时
*/
void delay_ms(uint32_t dms);



#endif





























