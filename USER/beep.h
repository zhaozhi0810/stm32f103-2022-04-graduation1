

#ifndef BEEP_H
#define BEEP_H

#include <stm32f10x.h>

extern uint8_t beep_control;    //���ڿ��Ʒ���������0��,0���� 


//��ʼ��PC6Ϊ�����.��ʹ��ʱ��		    
//BEEP IO��ʼ��
void Beep_Init(void);

void task_beep_control(void);



#endif

