
#include <stm32f10x.h>
#include <stdio.h>
#include "systick_delay.h"



uint16_t g_air_quality_val; 


//初始化ADC															   
void  Air_Adc_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //GPIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);  //adc时钟使能
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
	  /* Initialize the ADC_Mode member */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	/* initialize the ADC_ScanConvMode member */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式
	/* Initialize the ADC_ContinuousConvMode member */
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
	/* Initialize the ADC_ExternalTrigConv member */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //禁止触发检测，使用软件触发
	/* Initialize the ADC_DataAlign member */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	/* Initialize the ADC_NbrOfChannel member */
	ADC_InitStructure.ADC_NbrOfChannel = 1;//1个转换在规则序列中 也就是只转换规则序列1 
	
	ADC_Init(ADC2, &ADC_InitStructure);
	
	//先初始化ADC1通道0 IO口 PC4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;//PC0 通道4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 PC4 
	
	ADC_Cmd(ADC2, ENABLE);//开启AD转换器	

}				  
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
static u16 getAdc(u8 ch)   
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	//ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	
	
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC1规则组的转换结果
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
static u16 getAdcAverage(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=getAdc(ch);
		//delay_ms(5);
		delay_ms(5);
	}
	return temp_val/times;
} 
	 


/*
	电压0-3.3v
*/
void ADCget_Air_Vol(void)
{	
	g_air_quality_val = getAdcAverage(ADC_Channel_14,3) >> 2;   //ppm 范围10-1000ppm
	
	printf("g_air_quality_val = %d\n\r",g_air_quality_val);
}







