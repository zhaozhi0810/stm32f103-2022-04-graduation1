
#include <stm32f10x.h>


// PC6 引脚，高电平响/

volatile uint8_t beep_control = 0;    //用于控制蜂鸣器，非0响 

//初始化PC6为输出口.并使能时钟		    
//BEEP IO初始化
void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO口速度为50MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //LED1-->PD.2 端口配置, 推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_ResetBits(GPIOC,GPIO_Pin_6); 						 //PD.2 输出高 
}
 

//置低 点亮led index等于0，表示第一个led，其他表示第二个led
void Beep_On(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_6); 	 
}



//拉高 熄灭led index等于0，表示第一个led，其他表示第二个led
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
