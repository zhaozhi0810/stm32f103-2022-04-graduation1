#include "sys.h"
#include "usart.h"	  
#include "dht11.h"
#include "air_quality.h"
#include "pwm.h"
/*
	制定串口协议
	1.树莓派发送到单片机的。
	设置
	1.1 设置温度上下限 0x3f + '1' + 下限+ 上限 + 0d + 0a
	1.2 设置湿度上下限 0x3f + '2' + 下限+ 上限 + 0d + 0a
	1.3 设置空气质量上限 0x3f + '3' + 上限字节1+ 上限字节2 + 0d + 0a
	1.4 设置风扇的pwm  0x3f + '4' + pwm + 正反 + 0d + 0a   (4.5.6.7 对应不同的风扇)
	1.5 设置风扇的关  0x3f + '8' + 风扇编号（1-4，其他表示所有） +0d + 0a   (全部风扇关闭，再次修改pwm则可以开启)
	1.6 设置蜂鸣器报警关 0x3f + '9' + 0d + 0a
	1.7 设置蜂鸣器报警关 0x3f + 'a' + 0d + 0a
	
	2. 单片机发给树莓派（主动发）
	2.1 温度值 0x3f + '1' + 温度整数 + 温度小数 + 湿度整数 + 湿度小数 + 空气质量1 + 空气质量2 + 0d + 0a
	2.2 温度值 0x3f + '2' + PWM1 + PWM2 + PWM3 + PWM4 + 0d + 0a（用于调整进度条）
	2.3 设置应答 0x3f + 'a' + 0表示成功，非0表示失败 + 0d + 0a（用于调整进度条）
*/





//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
	return x;
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE))  //等待前面的数据发送结束
			;
	USART_SendData(USART1, ch);      
	return ch;
}
#endif 


 
#if EN_USART1_RX  //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 

}



//串口发送一个字节
static void UART1_TX(uint8_t ch)
{
	while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE))  //等待前面的数据发送结束
		;
	USART_SendData(USART1, ch); 		
}

//串口发送一个字节
static void UART1_TX_Buff(uint8_t *dat,uint8_t len)
{
	uint8_t i;

	for(i=0;i<len;i++)
		UART1_TX(dat[i]);
	
}



//串口1的接收中断
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART1);	//读取接收到的数据

		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)
					USART_RX_STA=0;//接收错误,重新开始
				else 
					USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	} 

} 

//用于单片机应答，status为0表示成功，非0表示失败
static void Usart1_Send_answer(uint8_t status)
{
	uint8_t buf[6] = {0x3f};
	
	buf[1]  = 'a' ; //表示是应答
	
	buf[2]  = !!status;
	buf[3] = 0xd;
	buf[4] = 0xa;
	
	UART1_TX_Buff(buf,5);  //发送数据
	
}



//串口1接收数据的处理
void Usart1_Mesaage_Handle(void)
{
	uint8_t len;
	uint8_t i;
	if(USART_RX_STA & 0x8000)
	{
		len = USART_RX_STA & 0xff;   //只要低8位，最多USART_REC_LEN
		
		if(len < USART_REC_LEN)
		for(i=0;i<len;i++)
			UART1_TX(USART_RX_BUF[i]);   //目前是把数据给发回去！！
		
		USART_RX_STA = 0;    //清除记录
	}

}



//1秒 发送 一次 温湿度 和 pwm值
void Task_Usart1_Send_Mesaage(void)
{
	uint8_t buf[16] = {0x3f};
	uint8_t len = 0; 
	static uint8_t n = 0;
				
	if(n&1)  //温湿度
	{
		buf[1] = '1';   //表示发上去的是温度等
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
	else   //pwm值
	{
		buf[1] = '2';   //表示发上去的是温度等
		buf[2] = g_fan_pwm[0];
		buf[3] = g_fan_pwm[1];
		buf[4] = g_fan_pwm[2];
		buf[5] = g_fan_pwm[3];
		buf[6] = 0xd;
		buf[7] = 0xa;
		len = 8;
	}
	UART1_TX_Buff(buf,len);
	n++;   //分开上传温度和pwm值
}




#endif   //#if EN_USART1_RX



#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记	  
 
void usart_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);    //使能USART2,GPIOA时钟//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

	//USART2_RX      GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位

	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化NVIC寄存器//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_Cmd(USART2, ENABLE);                    //使能串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断

}

void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART2);	//读取接收到的数据
		if((USART2_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)
					USART2_RX_STA=0;//接收错误,重新开始
				else 
					USART2_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)
					USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))
						USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	} 

} 
#endif	

