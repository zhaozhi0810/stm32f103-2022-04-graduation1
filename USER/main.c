
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
#include "beep.h"



void dht11_show(uint8_t tem,uint8_t humi); //oled��ʾ
extern __IO uint8_t TEMP_REQ;
extern __IO uint8_t DIANYA_REQ;




/*
	�弶��ʼ��
	
	����
	0�� ��ʾ�ɹ�
	���㣺 ʧ��
*/
int boardInit(void)
{
	//1.�����жϷ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    //2. ϵͳtik���ã���Ҫȷ������������ȵ��ã�����delay���Ƶĺ�������ʹ�ã�������
    systick_config();
    	
    // 3.  //led��ʼ��
	LED_Init();//led��ʼ��
	    
    //4. ����1 �ĳ�ʼ��
	uart_init(115200);

    
	//5. ADC ��ʼ��
	Air_Adc_Init();
	
	
	//6. OLED�ĳ�ʼ��
	OLED_Init();
	
	
	//7. ���ȿ���
	TIM4_PWM_Init();
	
	//6. ��ʼ��I2C1 �¶Ȳɼ�
//	vol_temp_init();
	

	return 0;
}

uint16_t task_id;   //ÿһ��λ��Ӧһ������Ϊ1��ʾ��Ҫ���������������������λ
typedef void(* task_t)(void);    //���庯��ָ������

int main(void)
{	
	uint16_t i;
	
	//���ﶨ��������
	const task_t task[TASK_MAX]={task_get_temp_vol  //����1���ɼ���ʪ�� 2s�ɼ�һ��
								,0
								,0
								,task_oled_show		//����4��oled��ʾˢ��
								,ADCget_Air_Vol     //����5�����ڿ��������ɼ�
								,task_beep_control      //����6���������Ŀ��� 10ms���һ��
					
	};
	
	
	
	
	boardInit();    //��������Ӳ����ʼ��
	 
	
	printf("boardInit done 2022-04-25\n\r");
	

	while(1)     //��ѭ������ִ������
	{
		for(i=0;i<TASK_MAX && task_id;i++){
			if(task_id & (1<<i))   //��ʱʱ�䵽��Ҫִ��
			{
				task_id &= ~(1<<i);  //��Ӧ��λ�ñ����㣬�ȴ���ʱ������
			
				if(task[i])  //ָ�벻��Ϊ��
				{	
					task[i](); //ִ�и�����
					break;    //һ��ִֻ��һ����������ǰ�����ȼ��ߣ����񿿺�����ȼ���
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// ���ȼ�����Ϊ 2 
	 	
	BASIC_TIM_Init();
	my_systick_int_init();    //��ʱ������ʼ��

	GPIO_DUO_Init();	    	 
	
	uart_init(9600);   //���ڳ�ʼ��Ϊ9600
	usart_init(9600);	 //������ʼ��Ϊ9600
	PWM_GPIO_Init();	 //��ʼ����PWM���ӵ�Ӳ���ӿ�

	OLED_Init();
	Adc_Init();	
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
	OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ	 
      
	TIM4_PWM_Init(7199,0);//����Ƶ��PWMƵ��=72000/(899+1)=80Khz 
	delay_ms(10);	 
				 
	TIM_SetCompare1(TIM4,3600);
	TIM_SetCompare2(TIM4,3600);
	TIM_SetCompare3(TIM4,3600);
	TIM_SetCompare4(TIM4,3600);		
		
   	while(1)
	{
		if(TEMP_REQ)  
		{
			DHT11_get_data(DTH11_data); //��ȡ����ʪ��
			dht11_show(DTH11_data);

		}
		if(DIANYA_REQ)  
		{
			adcx=Get_Adc_Average(ADC_Channel_15,10);
			//OLED_ShowNum(0,0,adcx,1,16,1);//��ʾADC��ֵ
			temp=(float)adcx*(3.3/4096);//��AD����������ֵת��Ϊ��ѹֵ
			tempi=(float)temp*(12/5);
			adcx=temp;//��ѹֵ����������   float�͸������ͣ�����Ǳ�������������
			adci=(int)(tempi*10);
			OLED_ShowChinese(1,1,4,16,1);//ʣ�����
			OLED_ShowChinese(12,1,5,16,1);
			OLED_ShowChinese(24,1,6,16,1);
			OLED_ShowChinese(36,1,7,16,1);
			OLED_ShowString(48,1,":",16,1);
			OLED_ShowNum(60,1,adci,2,16,1);
			tempi=-adci;//ʣ�������С������
			tempi*=1000;//С�����֡�1000�������
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
					DJ_TZ();//ֹͣ
				break;					 
				case 2:
					DJ_QJ();//ǰ��				
				break;
				case 3:
					DJ_HT();//����
				break;
				case 4:
					DJ_ZPY();//��ƽ��
				break;
				case 5:
					DJ_YPY();//��ƽ��
				break;
				case 6:
					DJ_SSZXZ();//˳ʱ����ת
				break;
				case 7:
					DJ_NSZXZ();//��ʱ����ת
				break;
				case 8:       
					DJ_ZHXXYD();//���б���ƶ�
				break;
				case 9:
					DJ_YQXXYD();//��ǰб���ƶ�
				break;
				case 10:
					DJ_YHXXYD();//�Һ�б���ƶ�
				break;
				case 11:
					DJ_ZQXXYD();//��ǰб���ƶ�
				break;
				case 12:
					DJ_JJS();//����
				break;
				case 23:
					DJ_ZC();//����
				break;
				case 14:
					DJ_JS(); //����
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
					DJ_TZ();//ֹͣ
				break;					 
				case 2:
					DJ_QJ();//ǰ��				
				break;
				case 3:
					DJ_HT();//����
				break;
				case 4:
					DJ_ZPY();//��ƽ��
				break;
				case 5:
					DJ_YPY();//��ƽ��
				break;
				case 6:
					DJ_SSZXZ();//˳ʱ����ת
				break;
				case 7:
					DJ_NSZXZ();//��ʱ����ת
				break;
				case 8:       
					DJ_ZHXXYD();//���б���ƶ�
				break;
				case 9:
					DJ_YQXXYD();//��ǰб���ƶ�
				break;
				case 10://A
					DJ_YHXXYD();//�Һ�б���ƶ�
				break;
				case 11://B
					DJ_ZQXXYD();//��ǰб���ƶ�
				break;
				case 12://C
					DJ_JJS();//����
				break;
				case 23://17
					DJ_ZC();//����
				break;
				case 14://E
					DJ_JS(); //����
				break;
			}
     }
   }
#endif    
}


