#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

//#define AIN1 PCout(10)	// PC10
//#define AIN2 PCout(9)	// PC9

//#define BIN1 PCout(11)	// PC11
//#define BIN2 PCout(12)	// PC12

//#define CIN1 PBout(5)	// PB5
//#define CIN2 PDout(2)	// PD2

//#define DIN1 PCout(0)	// PC0
//#define DIN2 PCout(8)	// PC8

//#define STEER_PWM1 PAout(6)  //PA6
//#define STEER_PWM2 PAout(7)  //PA7
//#define STEER_PWM3 PBout(0)  //PB0 
//#define STEER_PWM4 PBout(1)  //PB1

//#define IN PC(2)  // PC2
//#define IN_HW GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//读取红外电平
//#define BEEP PCout(6)  // PC6


extern uint8_t g_fan_pwm[4];



void TIM4_PWM_Init(void);



//风扇开启,4个都开启
void Fan_start(void);

//风扇关闭
void Fan_stop(void);



/*
	设置PWM占空比，主要是设置占空比来调速
	ch 表示哪一个风扇
	degree 表示占空比 0-100
*/
void Fan_pwm_out(uint8_t ch,uint8_t degree);




/*
	index 表示哪个风扇 0-3 合法
	status 表示状态，停止0，正转1，反转2 其他无效
*/
void Fan_Run_status(uint8_t index, uint8_t status);

//void PWM_GPIO_Init(void);

//void DJ_TZ(void);//停止
//void DJ_QJ(void);//前进
//void DJ_HT(void);//后退
//void DJ_ZPY(void);//左平移
//void DJ_YPY(void);//右平移
//void DJ_SSZXZ(void);//顺时针旋转
//void DJ_NSZXZ(void);//逆时针旋转
//void DJ_ZHXXYD(void);//左后斜向移动
//void DJ_YQXXYD(void);//右前斜向移动
//void DJ_YHXXYD(void);//右后斜向移动
//void DJ_ZQXXYD(void);//左前斜向移动
//void DJ_JJS(void);//加速
//void DJ_ZC(void);//正常
//void DJ_JS(void); //减速

#endif         
