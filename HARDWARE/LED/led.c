#include "led.h"


//��ʼ��PA8��led2����PC7��led1��Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 �����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //LED1-->PD.2 �˿�����, �������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_SetBits(GPIOC,GPIO_Pin_7); 						 //PD.2 ����� 
}
 

//�õ� ����led index����0����ʾ��һ��led��������ʾ�ڶ���led
void Led_On(uint8_t index)
{
	if(index == 0)
		GPIO_ResetBits(GPIOC,GPIO_Pin_7); 
	else
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);	 
}



//���� Ϩ��led index����0����ʾ��һ��led��������ʾ�ڶ���led
void Led_Off(uint8_t index)
{
	if(index == 0)
		GPIO_SetBits(GPIOC,GPIO_Pin_7); 
	else
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
}



//led0 ������˸����ʾ������������
//ÿ500ms����һ��,������ÿ500ms��˸һ��
void task_led1_show(void)
{
	static uint8_t n;
	
	if(n&1)
		Led_On(0);
	else	
		Led_Off(0);
	
	n++;
}

