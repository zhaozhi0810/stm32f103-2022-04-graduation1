
#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__


#include <stm32f10x.h>

//������main�������á�

//ʹ�ú궨������ļ��ʱ�䣬����Ķ�ʱ��systick_delay.c

//������õĸ�ʽ void fun(void)

#define TASK_MAX 6   //Ŀǰ����������

//#define TASK1_TICKS_INTERVAL 10   //����1���ϵ簴ť����Ƶ�л�����ɨ�衣
#define TASK2_TICKS_INTERVAL 10   //����2 10ms�ļ��,��systick�ж���ʹ��
#define TASK3_TICKS_INTERVAL 10   //����3��10msɨ�裬������ϵͳ�ػ�����������io����
#define TASK4_TICKS_INTERVAL 1500   //����4��ˢ��oled��ʾ
#define TASK5_TICKS_INTERVAL 1000   //����5�����ڿ��������ɼ� 1sһ��
//#define TASK16_TICKS_INTERVAL 433   //����5��ι�����ݶ�433ms
#define TASK1_TICKS_INTERVAL 2000   //����6���ɼ���ʪ�� 2s�ɼ�һ��

extern uint16_t task_id;   //ÿһ��λ��Ӧһ������Ϊ1��ʾ��Ҫ���������������������λ
//2021-09-30����task_allow,���ƶ�ʱ����������������
//extern uint16_t task_allow; //ÿһ��λ��Ӧһ������Ϊ1��ʾ����ʱɨ������񣬹ػ��󣬿��Խ�����Ҫ����������Ϊ������


#endif
