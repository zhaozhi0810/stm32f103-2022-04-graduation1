#include "sys.h"
#include "usart.h"	  
#include "dht11.h"
#include "air_quality.h"
#include "pwm.h"
/*
	�ƶ�����Э��
	1.��ݮ�ɷ��͵���Ƭ���ġ�
	����
	1.1 �����¶������� 0x3f + '1' + ����+ ���� + 0d + 0a
	1.2 ����ʪ�������� 0x3f + '2' + ����+ ���� + 0d + 0a
	1.3 ���ÿ����������� 0x3f + '3' + �����ֽ�1+ �����ֽ�2 + 0d + 0a
	1.4 ���÷��ȵ�pwm  0x3f + '4' + pwm + ���� + 0d + 0a   (4.5.6.7 ��Ӧ��ͬ�ķ���)
	1.5 ���÷��ȵĹ�  0x3f + '8' + ���ȱ�ţ�1-4��������ʾ���У� +0d + 0a   (ȫ�����ȹرգ��ٴ��޸�pwm����Կ���)
	1.6 ���÷����������� 0x3f + '9' + 0d + 0a
	1.7 ���÷����������� 0x3f + 'a' + 0d + 0a
	
	2. ��Ƭ��������ݮ�ɣ���������
	2.1 �¶�ֵ 0x3f + '1' + �¶����� + �¶�С�� + ʪ������ + ʪ��С�� + ��������1 + ��������2 + 0d + 0a
	2.2 �¶�ֵ 0x3f + '2' + PWM1 + PWM2 + PWM3 + PWM4 + 0d + 0a�����ڵ�����������
	2.3 ����Ӧ�� 0x3f + 'a' + 0��ʾ�ɹ�����0��ʾʧ�� + 0d + 0a�����ڵ�����������
*/





//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
	return x;
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE))  //�ȴ�ǰ������ݷ��ͽ���
			;
	USART_SendData(USART1, ch);      
	return ch;
}
#endif 


 
#if EN_USART1_RX  //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}



//���ڷ���һ���ֽ�
static void UART1_TX(uint8_t ch)
{
	while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE))  //�ȴ�ǰ������ݷ��ͽ���
		;
	USART_SendData(USART1, ch); 		
}

//���ڷ���һ���ֽ�
static void UART1_TX_Buff(uint8_t *dat,uint8_t len)
{
	uint8_t i;

	for(i=0;i<len;i++)
		UART1_TX(dat[i]);
	
}



//����1�Ľ����ж�
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART1);	//��ȡ���յ�������

		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)
					USART_RX_STA=0;//���մ���,���¿�ʼ
				else 
					USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
	} 

} 

//���ڵ�Ƭ��Ӧ��statusΪ0��ʾ�ɹ�����0��ʾʧ��
static void Usart1_Send_answer(uint8_t status)
{
	uint8_t buf[6] = {0x3f};
	
	buf[1]  = 'a' ; //��ʾ��Ӧ��
	
	buf[2]  = !!status;
	buf[3] = 0xd;
	buf[4] = 0xa;
	
	UART1_TX_Buff(buf,5);  //��������
	
}



//����1�������ݵĴ���
void Usart1_Mesaage_Handle(void)
{
	uint8_t len;
	uint8_t i;
	if(USART_RX_STA & 0x8000)
	{
		len = USART_RX_STA & 0xff;   //ֻҪ��8λ�����USART_REC_LEN
		
		if(len < USART_REC_LEN)
		for(i=0;i<len;i++)
			UART1_TX(USART_RX_BUF[i]);   //Ŀǰ�ǰ����ݸ�����ȥ����
		
		USART_RX_STA = 0;    //�����¼
	}

}



//1�� ���� һ�� ��ʪ�� �� pwmֵ
void Task_Usart1_Send_Mesaage(void)
{
	uint8_t buf[16] = {0x3f};
	uint8_t len = 0; 
	static uint8_t n = 0;
				
	if(n&1)  //��ʪ��
	{
		buf[1] = '1';   //��ʾ����ȥ�����¶ȵ�
		buf[2] = g_tem>>8;
		buf[3] = g_tem&0xff;
		buf[4] = g_himi >> 8;
		buf[5] = g_himi&0xff;
		buf[6] = g_air_quality_val>>8;
		buf[7] = g_air_quality_val & 0xff;
		buf[8] = 0xd;
		buf[9] = 0xa;
		len = 10;
	}
	else   //pwmֵ
	{
		buf[1] = '2';   //��ʾ����ȥ�����¶ȵ�
		buf[2] = g_fan_pwm[0];
		buf[3] = g_fan_pwm[1];
		buf[4] = g_fan_pwm[2];
		buf[5] = g_fan_pwm[3];
		buf[6] = 0xd;
		buf[7] = 0xa;
		len = 8;
	}
	UART1_TX_Buff(buf,len);
	n++;   //�ֿ��ϴ��¶Ⱥ�pwmֵ
}




#endif   //#if EN_USART1_RX



#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  
 
void usart_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);    //ʹ��USART2,GPIOAʱ��//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2

	//USART2_RX      GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//��λ����2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//ֹͣ��λ

	//USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);    //����ָ���Ĳ�����ʼ��NVIC�Ĵ���//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�

}

void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART2);	//��ȡ���յ�������
		if((USART2_RX_STA&0x8000)==0)//����δ���
		{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)
					USART2_RX_STA=0;//���մ���,���¿�ʼ
				else 
					USART2_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)
					USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))
						USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
	} 

} 
#endif	

