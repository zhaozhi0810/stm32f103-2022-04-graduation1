

#include "delay.h"



static uint16_t tick_total;   //���ص�tick����
static uint16_t tick_fact;    //ÿһ��΢���tick��


/*
	systick��ʱ����ʼ��,systick�ж�
	systick ѡ��1ms�жϣ�����1s�ڣ�����жϻ��ж���cpu1000�Σ�
	���� �� sysclock AHBʱ�ӣ���λΪMHz
*/
void my_systick_int_init(void)
{
	//
	tick_total = SystemCoreClock/1000/8;   //1ms ������ticks����
	tick_fact = (SystemCoreClock/1000000L + 7) / 8;   //1us Ҫ������tick��
	
	SysTick_Config(tick_total);   //����ʱ��Ϊ1ms�ж�
	
	//�޸�һ��ʱ��Դ
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);   //8����
}


/*
	us ����ʱʱ��798915us 
	//ȡһ���������ü���

	us����ʱ���ܳ���70,0000us

*/
void delay_us(uint32_t dus)
{
	uint32_t now;
	int ticks;   //����Ҫ�ȴ���������
	uint32_t t;
	int k;
	
	if(dus == 0)
	{
		return;
	}	
		
	now = SysTick->VAL;    //���Ȱѵ�ǰ���������ּ�����

	ticks = dus*tick_fact;   //������������ֵ 21�ڣ�1��us����Լ100s
							 //�ҵȴ�dus��Ҫ������ticks��
	while(ticks > 0)
	{
		t = SysTick->VAL;   //�������ڵļ���
		k = now - t;    //k��ʾ�Ѿ���ȥ��ticks
		if(k>=0)
		{
			ticks -= k;			
		}
		else
		{
			ticks -= tick_total+k;  //��Ϊk�Ǹ���������ֵ��������һ��С���������������Ѿ���ȥ��ticks
		}
		now = t;
	}		
}

static volatile uint32_t delay_count = 0;
/*
	��ʱ����
	��λ��ms
*/
void delay_ms(uint32_t dms)
{
	delay_count = dms;
	
	while(delay_count);   //�����Ϊ0���ͼ�����
}



/*

	�����ʱ���������Ƕ���ʱ���ж�һ�� 1ms

	ÿ��1ms����������ͻᱻ����һ��

*/

static uint32_t count = 0;

void SysTick_Handler(void)
{
	count ++;
	if(count % 20 == 0)  //ÿ��20ms��ɨ��һ�ΰ���
	{
	//	key_scan();  //ִ��ʱ��Խ��Խ��
	}
	
		
	if(delay_count)
	{
		delay_count --;
	}
}


