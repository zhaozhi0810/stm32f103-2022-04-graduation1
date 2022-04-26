
#include <stm32f10x.h>
#include "systick_delay.h"
#include "task_cfg.h"
#include "led.h"


static __IO uint32_t g_localtime = 0; /* for creating a time reference incremented by 1ms */

//static uint32_t ticks = 0;
//static uint32_t tflag = 0;




/*!
    \brief      insert a delay time
    \param[in]  ncount: number of 10ms periods to wait for
    \param[out] none
    \retval     none
*/

static uint32_t delay_n;

void delay_ms(uint32_t ncount)
{
    /* capture the current local time */
	delay_n =  ncount;
	
	/* wait until the desired delay finish */  
	while (delay_n);	
}


static u8  fac_us=72;	//systick多少频率		72MHz 没有8分频			//us延时倍乘数
//59652323ns，59652ms，59s
void delay_us(uint32_t nus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值
	ticks=nus*fac_us; 						//需要的节拍数
//	delay_osschedlock();					//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}
	}
}



/*!
    \brief      updates the system local time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_int_update(void)
{
//    static uint16_t times = 0;
	g_localtime ++;

	if(delay_n)   //用于延时的
		delay_n--;
	

			
	if(g_localtime % TASK1_TICKS_INTERVAL == 1000)
	{
		task_id |= 1;   //任务1，采集温湿度 2s采集一次
	}
	
	if(g_localtime % TASK5_TICKS_INTERVAL == 20)
	{
		task_id |= 1<<4;   //任务5，采集空气质量 1s采集一次
	}
	
		
	if(g_localtime % TASK4_TICKS_INTERVAL == 100)
	{
		task_id |= 1<<3;   //任务4，刷新oled 1s刷新一次
	}
	
	
//	if(g_localtime % TASK5_TICKS_INTERVAL == 0)
//	{
//		task_id |= (1<<4);   //任务5，500ms扫描，定时汇报温度等任务
//	}
			
		
	
	
	if(g_localtime % 500 == 1)
	{
		task_led1_show();
	}
  
}



/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     0 ： 成功 ； 非0： 失败
*/
int systick_config(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    //直接是HCLK时钟，
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U )){
        /* capture error */
        return 1;     //设置失败
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x02U);
	
	return 0;
}



