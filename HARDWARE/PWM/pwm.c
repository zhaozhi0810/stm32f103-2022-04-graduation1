#include "pwm.h"
#include "led.h"
#include "delay.h"

//pwm time4 ͨ��1��2��3��4  ���ڿ��Ʒ��ȵ���

uint16_t PWM_DEGREE_MAX = 1000;   //PWMƵ��    4000==250HZ 1000=1kHZ


uint8_t g_fan_pwm[4];    //��¼ռ�ձȣ�0-100,ÿ��ͨ�����Բ�ͬ��pwm
static uint8_t fan_started = 0;  //�Ѿ���������


 //�������ų�ʼ����PC10\PC9,PC11\PC12,PB5\PD2,PC0\PC8��
void PWM_GPIO_Init(void);


//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//1. ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     	
    //2. ���ø�����Ϊ�����������,���PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3. ��ʱ����ʼ��
	TIM_TimeBaseStructure.TIM_Period = PWM_DEGREE_MAX - 1;  //arr; 	//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/2/1000000)-1;  //Ԥ��Ƶ�����õ���1Mhz������ //psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 					//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; 								//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//�������:TIM����Ƚϼ��Ը�
	
	//4.4�����ͨ����ʼ����������Ϊ0
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  						//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 	

	//5. ʹ��Ԥװ��
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);					//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ��

	//6. ��ʱ������
	//TIM_Cmd(TIM4, ENABLE);  							//ʹ��TIM4
	//6. ���Ʒ������ŵĳ�ʼ��
	PWM_GPIO_Init();
	
	fan_started = 0;
}



/*
	����PWMռ�ձȣ���Ҫ������ռ�ձ�������
	ch ��ʾ��һ������
	degree ��ʾռ�ձ� 0-100
*/
void Fan_pwm_out(uint8_t ch,uint8_t degree)
{
	uint32_t value;

	if(ch > 3)  //��ֹԽ�� 
		return;
	
	if(degree > 100)
	{
		degree = 100;
	}
	g_fan_pwm[ch] = degree;   //��¼������������Ҫ
	
	value = (degree) * PWM_DEGREE_MAX / 100;
	/* CH configuration in PWM mode1,duty cycle  */
	//timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,value);
	if(ch==0)
		TIM_SetCompare1(TIM4, value);
	else if(ch == 1)
		TIM_SetCompare2(TIM4, value);
	else if(ch == 2)
		TIM_SetCompare3(TIM4, value);
	else
		TIM_SetCompare4(TIM4, value);
}


/*
	����PWM��Ƶ��
*/
void setFan_pwm_freq(uint16_t freq)
{
	if(freq < 20)   //Ƶ�ʲ��ܵ���20Hz
	{
		freq = 20;
	}

	PWM_DEGREE_MAX = freq;
	TIM_SetAutoreload(TIM4,freq);   //����Ƶ��
}


//���ȿ���
void Fan_start(void)
{
	uint8_t i;
	
	if(fan_started == 1)
		return;
	
	TIM_Cmd(TIM4, ENABLE);  							//ʹ��TIM4
	
	for(i=0;i<4;i++)
		Fan_Run_status(i, 1);  //���еķ�����ת
	
	fan_started = 1;
}

//���ȹر�
void Fan_stop(void)
{
	uint8_t i;
	TIM_Cmd(TIM4, DISABLE);  							//ʹ��TIM4
	
	for(i=0;i<4;i++)
		Fan_Run_status(i, 0);  //���еķ��ȹر�
	
	fan_started = 0;
}






 //�������ų�ʼ����PC10\PC9,PC11\PC12,PB5\PD2,PC0\PC8��
void PWM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);	 //ʹ��PB,�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.10.9.11.12.0.8
	//GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8);//PC10,PC9,PC11,PC12,PC0,PC8�����	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	//GPIO_SetBits(GPIOB,GPIO_Pin_5);//PC10,PC9,PC11,PC12,PC0,PC8�����	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PD2�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.2
//	GPIO_SetBits(GPIOD,GPIO_Pin_2);//PC10,PC9,PC11,PC12,PC0,PC8�����	


}


//status ��ʾ״̬��ֹͣ0����ת1����ת2 ����ֹͣ
static void Fan1_run(uint8_t status)
{
	if(status == 1)  //��ת
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
	else if(status == 2) //��ת
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_12);
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
	}
	else  //ֹͣ
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
}
//status ��ʾ״̬��ֹͣ0����ת1����ת2 ����ֹͣ
static void Fan2_run(uint8_t status)
{
	if(status == 1)  //��ת
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_10);
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
	else if(status == 2) //��ת
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
	else  //ֹͣ
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
}


//status ��ʾ״̬��ֹͣ0����ת1����ת2 ����ֹͣ
static void Fan3_run(uint8_t status)
{
	if(status == 1)  //��ת
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
	else if(status == 2) //��ת
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
	else  //ֹͣ
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
}


//status ��ʾ״̬��ֹͣ0����ת1����ת2 ����ֹͣ
static void Fan4_run(uint8_t status)
{
	if(status == 1)  //��ת
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	else if(status == 2) //��ת
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
	else  //ֹͣ
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
}


/*
	index ��ʾ�ĸ����� 0-3 �Ϸ�
	status ��ʾ״̬��ֹͣ0����ת1����ת2 ������Ч
*/
void Fan_Run_status(uint8_t index, uint8_t status)
{
	if(index > 3)  //��ֹԽ�� 
		return;
	
	if(fan_started == 0)   //���������ת
	{
		Fan_start();
	}
	
	
	if(index == 0)
	{
		Fan1_run(status);	
	}
	else if (index == 1)
	{
		Fan2_run(status);	
	}
	else if(index == 2)
	{
		Fan3_run(status);	
	}
	else
	{
		Fan4_run(status);	
	}	
}





void task_fan_control(void)
{
	

}






#if 0
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




 //PWM ����������ų�ʼ��
void PWM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	 //ʹ��PB,�˿�ʱ��

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

#endif
