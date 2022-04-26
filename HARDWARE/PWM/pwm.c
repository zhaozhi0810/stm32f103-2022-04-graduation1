#include "pwm.h"
#include "led.h"
#include "delay.h"

//pwm time4 Í¨µÀ1£¬2£¬3£¬4  ÓÃÓÚ¿ØÖÆ·çÉÈµ÷ËÙ

uint16_t PWM_DEGREE_MAX = 1000;   //PWMÆµÂÊ    4000==250HZ 1000=1kHZ


uint8_t g_fan_pwm[4];    //¼ÇÂ¼Õ¼¿Õ±È£¬0-100,Ã¿¸öÍ¨µÀ¿ÉÒÔ²»Í¬µÄpwm
static uint8_t fan_started = 0;  //ÒÑ¾­¿ªÆôÁËÂð


 //·çÉÈÒý½Å³õÊ¼»¯£¨PC10\PC9,PC11\PC12,PB5\PD2,PC0\PC8£©
void PWM_GPIO_Init(void);


//PWMÊä³ö³õÊ¼»¯
//arr£º×Ô¶¯ÖØ×°Öµ
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊý
void TIM4_PWM_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//1. Ê±ÖÓÊ¹ÄÜ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	                                                                     	
    //2. ÉèÖÃ¸ÃÒý½ÅÎª¸´ÓÃÊä³ö¹¦ÄÜ,Êä³öPWMÂö³å²¨ÐÎ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3. ¶¨Ê±Æ÷³õÊ¼»¯
	TIM_TimeBaseStructure.TIM_Period = PWM_DEGREE_MAX - 1;  //arr; 	//ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/2/1000000)-1;  //Ô¤·ÖÆµÊý£¬µÃµ½ÊÇ1MhzµÄÂö³å //psc; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ  ²»·ÖÆµ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 					//ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				//Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = 0; 								//ÉèÖÃ´ý×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//Êä³ö¼«ÐÔ:TIMÊä³ö±È½Ï¼«ÐÔ¸ß
	
	//4.4¸öÊä³öÍ¨µÀ³õÊ¼»¯£¬Âö³å¿í¶ÈÎª0
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  						//¸ù¾ÝTIM_OCInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèTIMx
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 	

	//5. Ê¹ÄÜÔ¤×°ÔØ
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1Ô¤×°ÔØÊ¹ÄÜ
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);					//Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æÆ

	//6. ¶¨Ê±Æ÷¿ªÆô
	//TIM_Cmd(TIM4, ENABLE);  							//Ê¹ÄÜTIM4
	//6. ¿ØÖÆ·çÉÈÒý½ÅµÄ³õÊ¼»¯
	PWM_GPIO_Init();
	
	fan_started = 0;
}



/*
	ÉèÖÃPWMÕ¼¿Õ±È£¬Ö÷ÒªÊÇÉèÖÃÕ¼¿Õ±ÈÀ´µ÷ËÙ
	ch ±íÊ¾ÄÄÒ»¸ö·çÉÈ
	degree ±íÊ¾Õ¼¿Õ±È 0-100
*/
void Fan_pwm_out(uint8_t ch,uint8_t degree)
{
	uint32_t value;

	if(ch > 3)  //·ÀÖ¹Ô½½ç 
		return;
	
	if(degree > 100)
	{
		degree = 100;
	}
	g_fan_pwm[ch] = degree;   //¼ÇÂ¼ÏÂÀ´£¬ºÃÏñ²»ÐèÒª
	
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
	ÉèÖÃPWMµÄÆµÂÊ
*/
void setFan_pwm_freq(uint16_t freq)
{
	if(freq < 20)   //ÆµÂÊ²»ÄÜµÍÓÚ20Hz
	{
		freq = 20;
	}

	PWM_DEGREE_MAX = freq;
	TIM_SetAutoreload(TIM4,freq);   //ÉèÖÃÆµÂÊ
}


//·çÉÈ¿ªÆô
void Fan_start(void)
{
	uint8_t i;
	
	if(fan_started == 1)
		return;
	
	TIM_Cmd(TIM4, ENABLE);  							//Ê¹ÄÜTIM4
	
	for(i=0;i<4;i++)
		Fan_Run_status(i, 1);  //ËùÓÐµÄ·çÉÈÕý×ª
	
	fan_started = 1;
}

//·çÉÈ¹Ø±Õ
void Fan_stop(void)
{
	uint8_t i;
	TIM_Cmd(TIM4, DISABLE);  							//Ê¹ÄÜTIM4
	
	for(i=0;i<4;i++)
		Fan_Run_status(i, 0);  //ËùÓÐµÄ·çÉÈ¹Ø±Õ
	
	fan_started = 0;
}






 //·çÉÈÒý½Å³õÊ¼»¯£¨PC10\PC9,PC11\PC12,PB5\PD2,PC0\PC8£©
void PWM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);	 //Ê¹ÄÜPB,¶Ë¿ÚÊ±ÖÓ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOC.10.9.11.12.0.8
	//GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8);//PC10,PC9,PC11,PC12,PC0,PC8Êä³ö¸ß	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB5 ¶Ë¿ÚÅäÖÃ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOB.5
	//GPIO_SetBits(GPIOB,GPIO_Pin_5);//PC10,PC9,PC11,PC12,PC0,PC8Êä³ö¸ß	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PD2¶Ë¿ÚÅäÖÃ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOD.2
//	GPIO_SetBits(GPIOD,GPIO_Pin_2);//PC10,PC9,PC11,PC12,PC0,PC8Êä³ö¸ß	


}


//status ±íÊ¾×´Ì¬£¬Í£Ö¹0£¬Õý×ª1£¬·´×ª2 ÆäËûÍ£Ö¹
static void Fan1_run(uint8_t status)
{
	if(status == 1)  //Õý×ª
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
	else if(status == 2) //·´×ª
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_12);
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
	}
	else  //Í£Ö¹
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
}
//status ±íÊ¾×´Ì¬£¬Í£Ö¹0£¬Õý×ª1£¬·´×ª2 ÆäËûÍ£Ö¹
static void Fan2_run(uint8_t status)
{
	if(status == 1)  //Õý×ª
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_10);
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
	else if(status == 2) //·´×ª
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
	else  //Í£Ö¹
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
}


//status ±íÊ¾×´Ì¬£¬Í£Ö¹0£¬Õý×ª1£¬·´×ª2 ÆäËûÍ£Ö¹
static void Fan3_run(uint8_t status)
{
	if(status == 1)  //Õý×ª
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
	else if(status == 2) //·´×ª
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
	else  //Í£Ö¹
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
}


//status ±íÊ¾×´Ì¬£¬Í£Ö¹0£¬Õý×ª1£¬·´×ª2 ÆäËûÍ£Ö¹
static void Fan4_run(uint8_t status)
{
	if(status == 1)  //Õý×ª
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	else if(status == 2) //·´×ª
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
	else  //Í£Ö¹
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
}


/*
	index ±íÊ¾ÄÄ¸ö·çÉÈ 0-3 ºÏ·¨
	status ±íÊ¾×´Ì¬£¬Í£Ö¹0£¬Õý×ª1£¬·´×ª2 ÆäËûÎÞÐ§
*/
void Fan_Run_status(uint8_t index, uint8_t status)
{
	if(index > 3)  //·ÀÖ¹Ô½½ç 
		return;
	
	if(fan_started == 0)   //Æô¶¯µç»úÕý×ª
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
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);  //Ê¹ÄÜGPIOÍâÉèÊ±ÖÓÊ¹ÄÜ
	                                                                     	
   //ÉèÖÃ¸ÃÒý½ÅÎª¸´ÓÃÊä³ö¹¦ÄÜ,Êä³öTIM3 CH3£¬4µÄPWMÂö³å²¨ÐÎ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
   //ÉèÖÃ¸ÃÒý½ÅÎª¸´ÓÃÊä³ö¹¦ÄÜ,Êä³öTIM3 CH1£¬2µÄPWMÂö³å²¨ÐÎ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; //ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ  ²»·ÖÆµ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //Ñ¡Ôñ¶¨Ê±Æ÷Ä£Ê½:TIMÂö³å¿í¶Èµ÷ÖÆÄ£Ê½2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //±È½ÏÊä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_Pulse = 0; //ÉèÖÃ´ý×°Èë²¶»ñ±È½Ï¼Ä´æÆ÷µÄÂö³åÖµ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //Êä³ö¼«ÐÔ:TIMÊä³ö±È½Ï¼«ÐÔ¸ß
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //¸ù¾ÝTIM_OCInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèTIMx
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

 // TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE Ö÷Êä³öÊ¹ÄÜ	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);//Ê¹ÄÜTIMxÔÚARRÉÏµÄÔ¤×°ÔØ¼Ä´æ?

	TIM_Cmd(TIM3, ENABLE);  //Ê¹ÄÜTIM1  
}




 //PWM µç»úÇý¶¯Òý½Å³õÊ¼»¯
void PWM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	 //Ê¹ÄÜPB,¶Ë¿ÚÊ±ÖÓ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_6;				 //LED0-->PA.8 ¶Ë¿ÚÅäÖÃ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOC.10.9.11.12.0.8
	GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8);//PC10,PC9,PC11,PC12,PC0,PC8Êä³ö¸ß	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PA.8 ¶Ë¿ÚÅäÖÃ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOB.5
	GPIO_SetBits(GPIOB,GPIO_Pin_5);//PC10,PC9,PC11,PC12,PC0,PC8Êä³ö¸ß	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED0-->PA.8 ¶Ë¿ÚÅäÖÃ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOD.2
	GPIO_SetBits(GPIOD,GPIO_Pin_2);//PC10,PC9,PC11,PC12,PC0,PC8Êä³ö¸ß	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PC ¸¡¿ÕÊäÈë
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	    		 //LED1-->PD.2 ¶Ë¿ÚÅäÖÃ, ÍÆÍìÊä³ö
 //GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //ÍÆÍìÊä³ö £¬IO¿ÚËÙ¶ÈÎª50MHz
 //GPIO_SetBits(GPIOC,GPIO_Pin_10); 						 //PD.2 Êä³ö¸ß 
}
void GPIO_DUO_Init(void)
{
	 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //Ê¹ÄÜPA,PD¶Ë¿ÚÊ±ÖÓ
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;				 //LED0-->PA.8 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOA.8
 GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);						 //PA.8 Êä³ö¸ß
	
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //LED0-->PA.8 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOA.8
 GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);						 //PA.8 Êä³ö¸ß 

}
void DJ_TZ(void)//Í£Ö¹
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
void DJ_QJ(void)//Ç°½ø
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
void DJ_HT(void)//ºóÍË
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
void DJ_ZPY(void)//×óÆ½ÒÆ
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
void DJ_YPY(void)//ÓÒÆ½ÒÆ
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
void DJ_SSZXZ(void)//Ë³Ê±ÕëÐý×ª
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
void DJ_NSZXZ(void)//ÄæÊ±ÕëÐý×ª
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
void DJ_ZHXXYD(void)//×óºóÐ±ÏòÒÆ¶¯
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
void DJ_YQXXYD(void)//ÓÒÇ°Ð±ÏòÒÆ¶¯
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
void DJ_YHXXYD(void)//ÓÒºóÐ±ÏòÒÆ¶¯
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
void DJ_ZQXXYD(void)//×óÇ°Ð±ÏòÒÆ¶¯
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

void DJ_JJS(void)//¼ÓËÙ
{
	TIM_SetCompare1(TIM4,5600);
	TIM_SetCompare2(TIM4,5600);
	TIM_SetCompare3(TIM4,5600);
	TIM_SetCompare4(TIM4,5600);
}
void DJ_ZC(void)//Õý³£
{
	TIM_SetCompare1(TIM4,3600);
	TIM_SetCompare2(TIM4,3600);
	TIM_SetCompare3(TIM4,3600);
	TIM_SetCompare4(TIM4,3600);
}
void DJ_JS(void) //¼õËÙ
{
	TIM_SetCompare1(TIM4,1800);
	TIM_SetCompare2(TIM4,1800);
	TIM_SetCompare3(TIM4,1800);
	TIM_SetCompare4(TIM4,1800);
}

#endif
