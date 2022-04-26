
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


static u8  fac_us=72;	//systick����Ƶ��		72MHz û��8��Ƶ			//us��ʱ������
//59652323ns��59652ms��59s
void delay_us(uint32_t nus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ
	ticks=nus*fac_us; 						//��Ҫ�Ľ�����
//	delay_osschedlock();					//��ֹOS���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
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

	if(delay_n)   //������ʱ��
		delay_n--;
	

			
	if(g_localtime % TASK1_TICKS_INTERVAL == 1000)
	{
		task_id |= 1;   //����1���ɼ���ʪ�� 2s�ɼ�һ��
	}
	
	if(g_localtime % TASK5_TICKS_INTERVAL == 20)
	{
		task_id |= 1<<4;   //����5���ɼ��������� 1s�ɼ�һ��
	}
	
		
	if(g_localtime % TASK4_TICKS_INTERVAL == 100)
	{
		task_id |= 1<<3;   //����4��ˢ��oled 1sˢ��һ��
	}
	
	
//	if(g_localtime % TASK5_TICKS_INTERVAL == 0)
//	{
//		task_id |= (1<<4);   //����5��500msɨ�裬��ʱ�㱨�¶ȵ�����
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
    \retval     0 �� �ɹ� �� ��0�� ʧ��
*/
int systick_config(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    //ֱ����HCLKʱ�ӣ�
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U )){
        /* capture error */
        return 1;     //����ʧ��
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x02U);
	
	return 0;
}



