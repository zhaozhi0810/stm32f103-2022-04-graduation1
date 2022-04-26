

#ifndef __AIR_QUALITY_H__
#define __AIR_QUALITY_H__

#include <stm32f10x.h>


//初始化ADC															   
void  Air_Adc_Init(void);


/*
	电压0-3.3v
	数据 0-4095
*/
void ADCget_Air_Vol(void);   //数据保存到全局变量中了。

extern uint16_t g_air_quality_val; 
#endif

