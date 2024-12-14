//systick.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "main_defines.h"
#include "main.h"

#include "outputs_defines.h"
#include "outputs.h"

#include "core_defines.h"
#include "core.h"

#include "systick_defines.h"
#include "systick.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
void SysTick_Init(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Обработчик прерывания
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void SysTick_Init(void)
{
  /*
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / SYS_TICK_PERIOD_VALUE);
  */
  
  SysTick->LOAD = TimerTick;		// Загрузка значения
  SysTick->VAL = TimerTick;		// Обнуляем таймеры и флаги. Записью, помните?
  
  SysTick->CTRL =	SysTick_CTRL_CLKSOURCE_Msk |
                SysTick_CTRL_TICKINT_Msk   |
                SysTick_CTRL_ENABLE_Msk;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Обработчик прерывания
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//SysTick Interrupt
void SysTick_Handler(void)
{
  Correct_Main_Timers();
  Correct_Outputs_Timers();
  Correct_Core_Timers();  
}

//--------------------------------------------------------------------------------------------------------

