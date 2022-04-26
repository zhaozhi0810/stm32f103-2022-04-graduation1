#include "pwm.h"
#include "led.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//PWM  ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  


//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     	
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 	
 // TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ��
	//TIM_ARRPreloadConfig(TIM4, ENABLE);
//	TIM_ARRPreloadConfig(TIM4, ENABLE);
//	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM1
 
}
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     	
   //���ø�����Ϊ�����������,���TIM3 CH3��4��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
   //���ø�����Ϊ�����������,���TIM3 CH1��2��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

 // TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ�?

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM1  
}
 //����������ų�ʼ��
void PWM_GPIO_Init(void)
{
 //��ǰ AIN1 PC10 AIN2 PC9
 //��� BIN1 PC11 BIN2 PC12
 //��ǰ CIN1 PB5  CIN2 PD2
 //�Һ� DIN1 PC0  DIN2 PC8

 GPIO_InitTypeDef GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);	 //ʹ��P,PD,�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_6;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.10.9.11.12.0.8
 GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8);//PC10,PC9,PC11,PC12,PC0,PC8�����	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);//PC10,PC9,PC11,PC12,PC0,PC8�����	

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
 GPIO_SetBits(GPIOD,GPIO_Pin_2);//PC10,PC9,PC11,PC12,PC0,PC8�����	
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PC ��������
 GPIO_Init(GPIOC, &GPIO_InitStructure);
 
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	    		 //LED1-->PD.2 �˿�����, �������
 //GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 //GPIO_SetBits(GPIOC,GPIO_Pin_10); 						 //PD.2 ����� 
}
void GPIO_DUO_Init(void)
{
	 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);						 //PA.8 �����
	
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);						 //PA.8 ����� 

}
void DJ_TZ(void)//ֹͣ
{
	 AIN1=0;  //PC10
   AIN2=0;	//PC9

	 BIN1=0;  //PC11
   BIN2=0;	//PC12
	 
	 CIN1=0;  //PB5
   CIN2=0;	//PD2
	 
	 DIN1=0;  //PC0
   DIN2=0;  //PC8

}
void DJ_QJ(void)//ǰ��
{
	 AIN1=1;  //PC10
   AIN2=0;	//PC9

	 BIN1=1;  //PC11
   BIN2=0;	//PC12
	 
	 CIN1=1;  //PB5
   CIN2=0;	//PD2
	 
	 DIN1=1;  //PC0
   DIN2=0;  //PC8
}
void DJ_HT(void)//����
{
	 AIN1=0;  //PC10
   AIN2=1;	//PC9

	 BIN1=0;  //PC11
   BIN2=1;	//PC12
	 
	 CIN1=0;  //PB5
   CIN2=1;	//PD2
	 
	 DIN1=0;  //PC0
   DIN2=1;  //PC8
}
void DJ_ZPY(void)//��ƽ��
{
	 AIN1=0;  //PC10
   AIN2=1;	//PC9
		
	 BIN1=1;  //PC11
   BIN2=0;	//PC12
		
	 CIN1=1;  //PB5
   CIN2=0;	//PD2
		
	 DIN1=0;  //PC0
   DIN2=1;  //PC8
}
void DJ_YPY(void)//��ƽ��
	{
	 AIN1=1;  //PC10
   AIN2=0;	//PC9
	
	 BIN1=0;  //PC11
   BIN2=1;	//PC12
	
	 CIN1=0;  //PB5
   CIN2=1;	//PD2
	
	 DIN1=1;  //PC0
   DIN2=0;  //PC8
}
void DJ_SSZXZ(void)//˳ʱ����ת
{
	 AIN1=1;  //PC10
   AIN2=0;	//PC9

	 BIN1=1;  //PC11
   BIN2=0;	//PC12

	 CIN1=0;  //PB5
   CIN2=1;	//PD2
	
	 DIN1=0;  //PC0
   DIN2=1;  //PC8
	}
void DJ_NSZXZ(void)//��ʱ����ת
{
	 AIN1=0;  //PC10
   AIN2=1;	//PC9

	 BIN1=0;  //PC11
   BIN2=1;	//PC12
	
	 CIN1=1;  //PB5
   CIN2=0;	//PD2
	 
	 DIN1=1;  //PC0
   DIN2=0;  //PC8
}
void DJ_ZHXXYD(void)//���б���ƶ�
{
	 AIN1=0;  //PC10
   AIN2=0;	//PC9

	 BIN1=0;  //PC11
   BIN2=1;	//PC12
	 
	 CIN1=0;  //PB5
   CIN2=1;	//PD2
	 
	 DIN1=0;  //PC0
   DIN2=0;  //PC8
}
void DJ_YQXXYD(void)//��ǰб���ƶ�
{
	 AIN1=0;  //PC10
   AIN2=0;	//PC9

	 BIN1=1;  //PC11
   BIN2=0;	//PC12
	 
	 CIN1=1;  //PB5
   CIN2=0;	//PD2
	 
	 DIN1=0;  //PC0
   DIN2=0;  //PC8
}
void DJ_YHXXYD(void)//�Һ�б���ƶ�
{
	 AIN1=0;  //PC10
   AIN2=1;	//PC9

	 BIN1=0;  //PC11
   BIN2=0;	//PC12
	 
	 CIN1=0;  //PB5
   CIN2=0;	//PD2
	 
	 DIN1=0;  //PC0
   DIN2=1;  //PC8
}
void DJ_ZQXXYD(void)//��ǰб���ƶ�
{
	 AIN1=1;  //PC10
   AIN2=0;	//PC9

	 BIN1=0;  //PC11
   BIN2=0;	//PC12
	 
	 CIN1=0;  //PB5
   CIN2=0;	//PD2
	 
	 DIN1=1;  //PC0
   DIN2=0;  //PC8
}

void DJ_JJS(void)//����
{

		TIM_SetCompare1(TIM4,5600);
		TIM_SetCompare2(TIM4,5600);
		TIM_SetCompare3(TIM4,5600);
	  TIM_SetCompare4(TIM4,5600);
}
void DJ_ZC(void)//����
{
		TIM_SetCompare1(TIM4,3600);
		TIM_SetCompare2(TIM4,3600);
		TIM_SetCompare3(TIM4,3600);
	  TIM_SetCompare4(TIM4,3600);
}
void DJ_JS(void) //����
{

	  TIM_SetCompare1(TIM4,1800);
		TIM_SetCompare2(TIM4,1800);
		TIM_SetCompare3(TIM4,1800);
	  TIM_SetCompare4(TIM4,1800);
}


