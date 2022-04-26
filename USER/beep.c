
#include <stm32f10x.h>


// PC6 ���ţ��ߵ�ƽ��/

volatile uint8_t beep_control = 0;    //���ڿ��Ʒ���������0�� 

//��ʼ��PC6Ϊ�����.��ʹ��ʱ��		    
//BEEP IO��ʼ��
void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //LED1-->PD.2 �˿�����, �������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_ResetBits(GPIOC,GPIO_Pin_6); 						 //PD.2 ����� 
}
 

//�õ� ����led index����0����ʾ��һ��led��������ʾ�ڶ���led
void Beep_On(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_6); 	 
}



//���� Ϩ��led index����0����ʾ��һ��led��������ʾ�ڶ���led
void Beep_Off(void)
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
}




void task_beep_control(void)
{	
	if(beep_control)
		Beep_On();
	else	
		Beep_Off();
	
}
