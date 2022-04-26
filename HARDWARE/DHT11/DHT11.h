#ifndef __DHT11_H
#define __DHT11_H	 

#include "sys.h"

#define BOOL unsigned char
 
#ifndef TRUE
#define TRUE 1
#endif
 
#ifndef FALSE
#define FALSE 0
#endif


extern u16 g_tem,g_himi;     //保存了温湿度


//定义DHT11 GPIOC 1
#define DHT11_PORT_RCC  	RCC_APB2Periph_GPIOC
#define DHT11_PIN  			GPIO_Pin_1
#define DHT11_PORT 			GPIOC

//static void DHT11_DataPin_Configure_Output(void);
//static void DHT11_DataPin_Configure_Input(void);
//BOOL DHT11_get_databit(void);
//void DHT11_set_databit(BOOL level);
//void mdelay(u16 ms);
//void udelay(u16 us);
//static uint8_t DHT11_read_byte(void);
//static uint8_t DHT11_start_sampling(void);

//返回值0表示正常，其他值为异常，参数返回温度和湿度
u8 DHT11_get_data(u16 *temp,u16* himi);


//采集温湿度的任务，2s调用一次
void task_get_temp_vol(void);	 				    
#endif


