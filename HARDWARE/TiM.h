#ifndef __TIM_H
#define __TIM_H

#include "stm32f10x.h"


/********************基本定时器TIM参数定义，只限TIM3、4************/
#define BASIC_TIM4 // 如果使用TIM4，注释掉这个宏即可

#ifdef  BASIC_TIM4 // 使用基本定时器TIM6
#define            BASIC_TIM                   TIM4
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM4
#define            BASIC_TIM_Period            1000-1
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM4_IRQn
#define            BASIC_TIM_IRQHandler        TIM4_IRQHandler

#else  // 使用基本定时器TIM3
#define            BASIC_TIM                   TIM3
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM3
#define            BASIC_TIM_Period            1000-1
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM3_IRQn
#define            BASIC_TIM_IRQHandler        TIM3_IRQHandler

#endif
/**************************函数声明********************************/

void BASIC_TIM_Init(void);


#endif	/* __BSP_TIMEBASE_H */


