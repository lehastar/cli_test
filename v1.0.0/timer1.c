//timer1.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "main_defines.h"
#include "main.h"

#include "usb_drv_defines.h"
#include "usb_drv.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
void TIMER1_Init(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Обработчик прерывания
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void TIMER1_Init(void)
{
  #define TIMER1_PSC       (uint16_t) (SystemCoreClock / 1000000ul) - 1; // Запускаем таймер на тактовой частоте в 1000 kHz
  #define TIMER1_PERIOD    999ul   // Период - 8 тактов => 1000/1000 = 1 kHz

  TIM_TimeBaseInitTypeDef TIMER_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  TIM_TimeBaseStructInit(&TIMER_InitStructure);
  TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIMER_InitStructure.TIM_Prescaler = TIMER1_PSC;
  TIMER_InitStructure.TIM_Period = TIMER1_PERIOD;
  TIM_TimeBaseInit(TIM1, &TIMER_InitStructure);
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  //TIM_Cmd(TIM1, DISABLE);
  TIM_Cmd(TIM1, ENABLE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Обработчик прерывания
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    if(usb_drv.timer != 0) usb_drv.timer--;
  }
}

//--------------------------------------------------------------------------------------------------------

