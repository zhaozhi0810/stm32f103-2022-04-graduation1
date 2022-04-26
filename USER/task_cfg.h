
#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__


#include <stm32f10x.h>

//任务由main函数调用。

//使用宏定义任务的间隔时间，任务的定时由systick_delay.c

//任务调用的格式 void fun(void)

#define TASK_MAX 6   //目前最大的任务数

//#define TASK1_TICKS_INTERVAL 10   //任务1，上电按钮和视频切换开关扫描。
#define TASK2_TICKS_INTERVAL 10   //任务2 10ms的间隔,在systick中断中使用
#define TASK3_TICKS_INTERVAL 10   //任务3，10ms扫描，检测操作系统关机和重启，读io引脚
#define TASK4_TICKS_INTERVAL 1500   //任务4，刷新oled显示
#define TASK5_TICKS_INTERVAL 1000   //任务5，用于空气质量采集 1s一次
#define TASK6_TICKS_INTERVAL 10   //任务6，蜂鸣器控制任务
#define TASK1_TICKS_INTERVAL 2000   //任务6，采集温湿度 2s采集一次

extern uint16_t task_id;   //每一个位对应一个任务，为1表示需要启动任务，在任务中清零该位
//2021-09-30增加task_allow,控制定时器，设置任务运行
//extern uint16_t task_allow; //每一个位对应一个任务，为1表示允许定时扫描该任务，关机后，可以将不必要的任务设置为不允许


#endif
