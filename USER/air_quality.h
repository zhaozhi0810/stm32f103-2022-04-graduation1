

#ifndef __AIR_QUALITY_H__
#define __AIR_QUALITY_H__

#include <stm32f10x.h>


//��ʼ��ADC															   
void  Air_Adc_Init(void);


/*
	��ѹ0-3.3v
	���� 0-4095
*/
void ADCget_Air_Vol(void);   //���ݱ��浽ȫ�ֱ������ˡ�

extern uint16_t g_air_quality_val; 
#endif

