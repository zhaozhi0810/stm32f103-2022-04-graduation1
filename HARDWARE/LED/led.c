#include "led.h"


//初始化PA8（led2）和PC7（led1）为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //LED1-->PD.2 端口配置, 推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_SetBits(GPIOC,GPIO_Pin_7); 						 //PD.2 输出高 
}
 

//置低 点亮led index等于0，表示第一个led，其他表示第二个led
void Led_On(uint8_t index)
{
	if(index == 0)
		GPIO_ResetBits(GPIOC,GPIO_Pin_7); 
	else
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);	 
}



//拉高 熄灭led index等于0，表示第一个led，其他表示第二个led
void Led_Off(uint8_t index)
{
	if(index == 0)
		GPIO_SetBits(GPIOC,GPIO_Pin_7); 
	else
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
}



//led0 用于闪烁，表示程序正在运行
//每500ms调用一次,这样就每500ms闪烁一次
void task_led1_show(void)
{
	static uint8_t n;
	
	if(n&1)
		Led_On(0);
	else	
		Led_Off(0);
	
	n++;
}

