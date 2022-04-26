#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

//#define AIN1 PCout(10)	// PC10
//#define AIN2 PCout(9)	// PC9

//#define BIN1 PCout(11)	// PC11
//#define BIN2 PCout(12)	// PC12

//#define CIN1 PBout(5)	// PB5
//#define CIN2 PDout(2)	// PD2

//#define DIN1 PCout(0)	// PC0
//#define DIN2 PCout(8)	// PC8

//#define STEER_PWM1 PAout(6)  //PA6
//#define STEER_PWM2 PAout(7)  //PA7
//#define STEER_PWM3 PBout(0)  //PB0 
//#define STEER_PWM4 PBout(1)  //PB1

//#define IN PC(2)  // PC2
//#define IN_HW GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//��ȡ�����ƽ
//#define BEEP PCout(6)  // PC6


extern uint8_t g_fan_pwm[4];



void TIM4_PWM_Init(void);



//���ȿ���,4��������
void Fan_start(void);

//���ȹر�
void Fan_stop(void);



/*
	����PWMռ�ձȣ���Ҫ������ռ�ձ�������
	ch ��ʾ��һ������
	degree ��ʾռ�ձ� 0-100
*/
void Fan_pwm_out(uint8_t ch,uint8_t degree);




/*
	index ��ʾ�ĸ����� 0-3 �Ϸ�
	status ��ʾ״̬��ֹͣ0����ת1����ת2 ������Ч
*/
void Fan_Run_status(uint8_t index, uint8_t status);

//void PWM_GPIO_Init(void);

//void DJ_TZ(void);//ֹͣ
//void DJ_QJ(void);//ǰ��
//void DJ_HT(void);//����
//void DJ_ZPY(void);//��ƽ��
//void DJ_YPY(void);//��ƽ��
//void DJ_SSZXZ(void);//˳ʱ����ת
//void DJ_NSZXZ(void);//��ʱ����ת
//void DJ_ZHXXYD(void);//���б���ƶ�
//void DJ_YQXXYD(void);//��ǰб���ƶ�
//void DJ_YHXXYD(void);//�Һ�б���ƶ�
//void DJ_ZQXXYD(void);//��ǰб���ƶ�
//void DJ_JJS(void);//����
//void DJ_ZC(void);//����
//void DJ_JS(void); //����

#endif         
