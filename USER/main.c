
#include <stm32f10x.h>

#include "led.h"
#include "systick_delay.h"
#include "sys.h"
#include "pwm.h"
#include "usart.h"
#include "oled.h"
#include "dht11.h"
#include "adc.h"
#include "Tim.h"
#include "stm32f10x_it.h" 

#include "task_cfg.h"
#include "air_quality.h"

void dht11_show(uint8_t tem,uint8_t humi); //oled显示
extern __IO uint8_t TEMP_REQ;
extern __IO uint8_t DIANYA_REQ;




/*
	板级初始化
	
	返回
	0： 表示成功
	非零： 失败
*/
int boardInit(void)
{
	//1.配置中断分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    //2. 系统tik配置，需要确保这个函数最先调用，否则delay类似的函数不能使用！！！！
    systick_config();
    	
    // 3.  //led初始化
	LED_Init();//led初始化
	    
    //4. 串口1 的初始化
	uart_init(115200);

    
	//5. ADC 初始化
	Air_Adc_Init();
	
	
	//6. OLED的初始化
	OLED_Init();
	
	//6. 初始化I2C1 温度采集
//	vol_temp_init();
	

	return 0;
}

uint16_t task_id;   //每一个位对应一个任务，为1表示需要启动任务，在任务中清零该位
typedef void(* task_t)(void);    //定义函数指针类型

int main(void)
{	
	uint16_t i;
	
	//这里定义了任务
	const task_t task[TASK_MAX]={task_get_temp_vol  //任务1，采集温湿度 2s采集一次
								,0
								,0
								,task_oled_show
								,ADCget_Air_Vol     //任务5，用于空气质量采集
								,0       //任务6，采集温湿度 2s采集一次
					
	};
	
	
	
	
	boardInit();    //开发板上硬件初始化
	 
	
	printf("boardInit done 2022-04-25\n\r");
	

	while(1)     //主循环用于执行任务
	{
		for(i=0;i<TASK_MAX && task_id;i++){
			if(task_id & (1<<i))   //定时时间到，要执行
			{
				task_id &= ~(1<<i);  //对应的位置被清零，等待定时器设置
			
				if(task[i])  //指针不能为空
				{	
					task[i](); //执行该任务
					break;    //一次只执行一个任务，任务靠前的优先级高，任务靠后的优先级低
				}				
			}
		}//end for	
	}
		
	 
	 
	 

#if 0	 
	u8 len;	
//	u16 times=0; 
	u16 adcx,adci,adc1,adc2;
	float temp,tempi;
	u32 DTH11_data[4]={0};
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 优先级分组为 2 
	 	
	BASIC_TIM_Init();
	my_systick_int_init();    //延时函数初始化

	GPIO_DUO_Init();	    	 
	
	uart_init(9600);   //串口初始化为9600
	usart_init(9600);	 //蓝牙初始化为9600
	PWM_GPIO_Init();	 //初始化与PWM连接的硬件接口

	OLED_Init();
	Adc_Init();	
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示	 
      
	TIM4_PWM_Init(7199,0);//不分频。PWM频率=72000/(899+1)=80Khz 
	delay_ms(10);	 
				 
	TIM_SetCompare1(TIM4,3600);
	TIM_SetCompare2(TIM4,3600);
	TIM_SetCompare3(TIM4,3600);
	TIM_SetCompare4(TIM4,3600);		
		
   	while(1)
	{
		if(TEMP_REQ)  
		{
			DHT11_get_data(DTH11_data); //获取到温湿度
			dht11_show(DTH11_data);

		}
		if(DIANYA_REQ)  
		{
			adcx=Get_Adc_Average(ADC_Channel_15,10);
			//OLED_ShowNum(0,0,adcx,1,16,1);//显示ADC的值
			temp=(float)adcx*(3.3/4096);//把AD采样回来的值转化为电压值
			tempi=(float)temp*(12/5);
			adcx=temp;//电压值的整数部分   float型赋给整型，结果是保留了整数部分
			adci=(int)(tempi*10);
			OLED_ShowChinese(1,1,4,16,1);//剩余电量
			OLED_ShowChinese(12,1,5,16,1);
			OLED_ShowChinese(24,1,6,16,1);
			OLED_ShowChinese(36,1,7,16,1);
			OLED_ShowString(48,1,":",16,1);
			OLED_ShowNum(60,1,adci,2,16,1);
			tempi=-adci;//剩余电量的小数部分
			tempi*=1000;//小数部分×1000变成整型
			OLED_ShowString(75,1,".",16,1);
			OLED_ShowNum(85,1,tempi,1,16,0X80);
			delay_ms(5000);
			OLED_Refresh();
		}
		if(!USART_RX_STA)
		{
			len=USART_RX_STA&0x3fff;
			USART_RX_STA=0;
			switch(USART_RX_BUF[0])
			{
				case 1:
					DJ_TZ();//停止
				break;					 
				case 2:
					DJ_QJ();//前进				
				break;
				case 3:
					DJ_HT();//后退
				break;
				case 4:
					DJ_ZPY();//左平移
				break;
				case 5:
					DJ_YPY();//右平移
				break;
				case 6:
					DJ_SSZXZ();//顺时针旋转
				break;
				case 7:
					DJ_NSZXZ();//逆时针旋转
				break;
				case 8:       
					DJ_ZHXXYD();//左后斜向移动
				break;
				case 9:
					DJ_YQXXYD();//右前斜向移动
				break;
				case 10:
					DJ_YHXXYD();//右后斜向移动
				break;
				case 11:
					DJ_ZQXXYD();//左前斜向移动
				break;
				case 12:
					DJ_JJS();//加速
				break;
				case 23:
					DJ_ZC();//正常
				break;
				case 14:
					DJ_JS(); //减速
				break;
			}
		}
		if(!USART2_RX_STA)
		{
			len=USART2_RX_STA&0x3fff;//
			USART2_RX_STA=0;
			switch(USART2_RX_BUF[0])
			{
				case 1:
					DJ_TZ();//停止
				break;					 
				case 2:
					DJ_QJ();//前进				
				break;
				case 3:
					DJ_HT();//后退
				break;
				case 4:
					DJ_ZPY();//左平移
				break;
				case 5:
					DJ_YPY();//右平移
				break;
				case 6:
					DJ_SSZXZ();//顺时针旋转
				break;
				case 7:
					DJ_NSZXZ();//逆时针旋转
				break;
				case 8:       
					DJ_ZHXXYD();//左后斜向移动
				break;
				case 9:
					DJ_YQXXYD();//右前斜向移动
				break;
				case 10://A
					DJ_YHXXYD();//右后斜向移动
				break;
				case 11://B
					DJ_ZQXXYD();//左前斜向移动
				break;
				case 12://C
					DJ_JJS();//加速
				break;
				case 23://17
					DJ_ZC();//正常
				break;
				case 14://E
					DJ_JS(); //减速
				break;
			}
     }
   }
#endif    
}


