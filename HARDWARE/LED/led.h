#ifndef __LED_H
#define __LED_H	 
#include "sys.h"



void LED_Init(void);//��ʼ��
//void GPIO_DUO_Init(void);

//���� Ϩ��led index����0����ʾ��һ��led��������ʾ�ڶ���led
void Led_Off(uint8_t index);	

//���� Ϩ��led index����0����ʾ��һ��led��������ʾ�ڶ���led
void Led_Off(uint8_t index);


//led0 ������˸����ʾ������������
//ÿ500ms����һ��,������ÿ500ms��˸һ��
void task_led1_show(void);

#endif
