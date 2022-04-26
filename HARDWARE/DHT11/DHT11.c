#include "dht11.h"
#include "delay.h"
#include "stdio.h"
/*数据定义:
---以下变量均为全局变量--------
    //----温度高8位== U8T_data_H------
    //----温度低8位== U8T_data_L------
    //----湿度高8位== U8RH_data_H-----
    //----湿度低8位== U8RH_data_L-----
  //-----校验 8位 == U8checkdata-----
*/

//PC1 引脚为通信引脚


//u8 U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
//u8 U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
u16 g_tem,g_himi;    //保持温度和湿度

 
static void DHT11_DataPin_Configure_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(DHT11_PORT_RCC, ENABLE); //使能PD端口时钟
  GPIO_InitStructure.GPIO_Pin = DHT11_PIN; //PD.0 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}
 
static void DHT11_DataPin_Configure_Input(void)
{
	GPIO_InitTypeDef DataPin;
	DataPin.GPIO_Pin = DHT11_PIN;
	DataPin.GPIO_Mode = GPIO_Mode_IN_FLOATING; //悬空 输入    
	DataPin.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_PORT, &DataPin);
}

BOOL DHT11_get_databit(void)
{
	uint8_t val;
	val = GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN);
	if(val == Bit_RESET)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
 
void DHT11_set_databit(BOOL level)
{
	if(level == TRUE)
	{
		GPIO_SetBits(DHT11_PORT, DHT11_PIN);
	}
	else
	{
		GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
	}
}
 
void mdelay(u16 ms)
{
	if(ms != 0)
	{
		delay_ms(ms);
	}
}
 
void udelay(u16 us)
{
	if(us != 0)
	{
		delay_us(us);
	}
}



static uint8_t DHT11_read_byte(void)
{
	uint8_t i;
	uint8_t data = 0;
	for(i = 0; i < 8; i++)
	{
		data <<= 1;
		while((!DHT11_get_databit()));
		udelay(10);
		udelay(10);
		udelay(10);
 
		if(DHT11_get_databit())
		{
			data |= 0x1;
			while(DHT11_get_databit());
		}
		else
		{
		} 
  } 
	return data;
}


static uint8_t DHT11_start_sampling(void)
{
	DHT11_DataPin_Configure_Output();
	//主机拉低18ms   
	DHT11_set_databit(FALSE);
	mdelay(18);
	DHT11_set_databit(TRUE);
	//总线由上拉电阻拉高 主机延时20us
	udelay(10);
	udelay(10);

	//主机设为输入 判断从机响应信号 
	DHT11_set_databit(TRUE);

	DHT11_DataPin_Configure_Input();

	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行      
	if(!DHT11_get_databit()) //T !      
	{
		//判断从机是否发出 80us 的低电平响应信号是否结束     
		while((!DHT11_get_databit()));
		// printf("DHT11 answers.\r\n");
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
		while((DHT11_get_databit()))
		{}
		
			return 1;
	}
 
	return 0;
}
 
u8 DHT11_get_data(u16 *tem,u16* himi)
{
	u8 temp;
	u8 buf[5];
	
	if(DHT11_start_sampling())
	{
		//printf("DHT11 is ready to transmit data\r\n");
		//数据接收状态         
		buf[0] = DHT11_read_byte();
		buf[1] = DHT11_read_byte();
		buf[2]  = DHT11_read_byte();
		buf[3]  = DHT11_read_byte();
		buf[4] = DHT11_read_byte();
	 
		/* Data transmission finishes, pull the bus high */
		DHT11_DataPin_Configure_Output();
		DHT11_set_databit(TRUE);
		//数据校验 
	 
		temp = (buf[0]+buf[1]+buf[2]+buf[3]);
		if((temp == buf[4]) && (buf[4] != 0) && (buf[4] != 0xff))   //校验和
		{
			*tem = buf[2]<<8;
			*tem |= buf[3]; //小数部分丢弃
			*himi = buf[0]<<8;
			*himi |= buf[1];//小数部分丢弃
			printf("temp = %d humi = %d\n\r",*tem>>8,*himi>>8);
			
			return 0;  //正确
		}
	}
	else
	{
		printf("ERROR: DHT11_start_sampling \n\r");
	}
	return 1;   //出错了。
}



void task_get_temp_vol(void)
{
	u16 tem,himi;
	uint8_t ret;
	
	ret = DHT11_get_data(&tem,&himi);
	if(ret == 0)
	{
		g_tem = tem;
		g_himi = himi;
	}		
	
}


