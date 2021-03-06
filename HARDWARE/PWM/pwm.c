#include "pwm.h"
#include "led.h"
#include "delay.h"

//pwm time4 通道1，2，3，4  用于控制风扇调速

uint16_t PWM_DEGREE_MAX = 1000;   //PWM频率    4000==250HZ 1000=1kHZ


uint8_t g_fan_pwm[4];    //记录占空比，0-100,每个通道可以不同的pwm
static uint8_t fan_started = 0;  //已经开启了吗


 //风扇引脚初始化（PC10\PC9,PC11\PC12,PB5\PD2,PC0\PC8）
void PWM_GPIO_Init(void);


//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	//1. 时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
    //2. 设置该引脚为复用输出功能,输出PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//3. 定时器初始化
	TIM_TimeBaseStructure.TIM_Period = PWM_DEGREE_MAX - 1;  //arr; 	//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/2/1000000)-1;  //预分频数，得到是1Mhz的脉冲 //psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 					//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; 								//设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//输出极性:TIM输出比较极性高
	
	//4.4个输出通道初始化，脉冲宽度为0
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  						//根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure); 	

	//5. 使能预装载
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1预装载使能
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);					//使能TIMx在ARR上的预装载寄存?

	//6. 定时器开启
	//TIM_Cmd(TIM4, ENABLE);  							//使能TIM4
	//6. 控制风扇引脚的初始化
	PWM_GPIO_Init();
	
	fan_started = 0;
}



/*
	设置PWM占空比，主要是设置占空比来调速
	ch 表示哪一个风扇
	degree 表示占空比 0-100
*/
void Fan_pwm_out(uint8_t ch,uint8_t degree)
{
	uint32_t value;

	if(ch > 3)  //防止越界 
		return;
	
	if(degree > 100)
	{
		degree = 100;
	}
	g_fan_pwm[ch] = degree;   //记录下来，好像不需要
	
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
	设置PWM的频率
*/
void setFan_pwm_freq(uint16_t freq)
{
	if(freq < 20)   //频率不能低于20Hz
	{
		freq = 20;
	}

	PWM_DEGREE_MAX = freq;
	TIM_SetAutoreload(TIM4,freq);   //设置频率
}


//风扇开启
void Fan_start(void)
{
	uint8_t i;
	
	if(fan_started == 1)
		return;
	
	TIM_Cmd(TIM4, ENABLE);  							//使能TIM4
	
	for(i=0;i<4;i++)
		Fan_Run_status(i, 1);  //所有的风扇正转
	
	fan_started = 1;
}

//风扇关闭
void Fan_stop(void)
{
	uint8_t i;
	TIM_Cmd(TIM4, DISABLE);  							//使能TIM4
	
	for(i=0;i<4;i++)
		Fan_Run_status(i, 0);  //所有的风扇关闭
	
	fan_started = 0;
}






 //风扇引脚初始化（PC10\PC9,PC11\PC12,PB5\PD2,PC0\PC8）
void PWM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);	 //使能PB,端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.10.9.11.12.0.8
	//GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8);//PC10,PC9,PC11,PC12,PC0,PC8输出高	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //PB5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	//GPIO_SetBits(GPIOB,GPIO_Pin_5);//PC10,PC9,PC11,PC12,PC0,PC8输出高	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PD2端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
//	GPIO_SetBits(GPIOD,GPIO_Pin_2);//PC10,PC9,PC11,PC12,PC0,PC8输出高	


}


//status 表示状态，停止0，正转1，反转2 其他停止
static void Fan1_run(uint8_t status)
{
	if(status == 1)  //正转
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
	else if(status == 2) //反转
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_12);
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
	}
	else  //停止
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
}
//status 表示状态，停止0，正转1，反转2 其他停止
static void Fan2_run(uint8_t status)
{
	if(status == 1)  //正转
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_10);
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
	else if(status == 2) //反转
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
	else  //停止
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	}
}


//status 表示状态，停止0，正转1，反转2 其他停止
static void Fan3_run(uint8_t status)
{
	if(status == 1)  //正转
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
	else if(status == 2) //反转
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
	else  //停止
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	}
}


//status 表示状态，停止0，正转1，反转2 其他停止
static void Fan4_run(uint8_t status)
{
	if(status == 1)  //正转
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	else if(status == 2) //反转
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
	else  //停止
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
}


/*
	index 表示哪个风扇 0-3 合法
	status 表示状态，停止0，正转1，反转2 其他无效
*/
void Fan_Run_status(uint8_t index, uint8_t status)
{
	if(index > 3)  //防止越界 
		return;
	
	if(fan_started == 0)   //启动电机正转
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
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	
   //设置该引脚为复用输出功能,输出TIM3 CH3，4的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
   //设置该引脚为复用输出功能,输出TIM3 CH1，2的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

 // TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);//使能TIMx在ARR上的预装载寄存?

	TIM_Cmd(TIM3, ENABLE);  //使能TIM1  
}




 //PWM 电机驱动引脚初始化
void PWM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	 //使能PB,端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_6;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.10.9.11.12.0.8
	GPIO_SetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_0|GPIO_Pin_8);//PC10,PC9,PC11,PC12,PC0,PC8输出高	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_SetBits(GPIOB,GPIO_Pin_5);//PC10,PC9,PC11,PC12,PC0,PC8输出高	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.2
	GPIO_SetBits(GPIOD,GPIO_Pin_2);//PC10,PC9,PC11,PC12,PC0,PC8输出高	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PC 浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	    		 //LED1-->PD.2 端口配置, 推挽输出
 //GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 //GPIO_SetBits(GPIOC,GPIO_Pin_10); 						 //PD.2 输出高 
}
void GPIO_DUO_Init(void)
{
	 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);						 //PA.8 输出高
	
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);						 //PA.8 输出高 

}
void DJ_TZ(void)//停止
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
void DJ_QJ(void)//前进
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
void DJ_HT(void)//后退
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
void DJ_ZPY(void)//左平移
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
void DJ_YPY(void)//右平移
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
void DJ_SSZXZ(void)//顺时针旋转
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
void DJ_NSZXZ(void)//逆时针旋转
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
void DJ_ZHXXYD(void)//左后斜向移动
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
void DJ_YQXXYD(void)//右前斜向移动
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
void DJ_YHXXYD(void)//右后斜向移动
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
void DJ_ZQXXYD(void)//左前斜向移动
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

void DJ_JJS(void)//加速
{
	TIM_SetCompare1(TIM4,5600);
	TIM_SetCompare2(TIM4,5600);
	TIM_SetCompare3(TIM4,5600);
	TIM_SetCompare4(TIM4,5600);
}
void DJ_ZC(void)//正常
{
	TIM_SetCompare1(TIM4,3600);
	TIM_SetCompare2(TIM4,3600);
	TIM_SetCompare3(TIM4,3600);
	TIM_SetCompare4(TIM4,3600);
}
void DJ_JS(void) //减速
{
	TIM_SetCompare1(TIM4,1800);
	TIM_SetCompare2(TIM4,1800);
	TIM_SetCompare3(TIM4,1800);
	TIM_SetCompare4(TIM4,1800);
}

#endif
