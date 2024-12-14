//outputs.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "outputs_defines.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************
system_led_t system_led;
led_t led;

//************************************
// Prototypes
//************************************
void Correct_Outputs_Timers(void);
void Outputs_Init(void);
void Sys_Led_Flash(void);
void System_Led_Processor(void);
void Led_Processor(void);

void Bip(void);
void Bip_Short(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Коррекция таймеров
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Correct_Outputs_Timers(void)
{
  if(system_led.timer != 0) system_led.timer--;
  if(system_led.blink_timer != 0) system_led.blink_timer--;

  if(led.timer != 0) led.timer--;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Инициализация
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Outputs_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //LED
  GPIO_InitStructure.GPIO_Pin = SYS_LED_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SYS_LED_GPIO_PORT, &GPIO_InitStructure);

  //SOUND
  //GPIO_InitStructure.GPIO_Pin = SOUND_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_Init(SOUND_GPIO_PORT, &GPIO_InitStructure);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//Системный светодиод
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Sys_Led_Flash(void)
{
  //system_led.timer = SYS_LED_FLASH_TIME;
  system_led.blink_timer = SYS_LED_FLASH_TIME;
  System_Led_ON();
}

void System_Led_Processor(void)
{
  if(system_led.timer != 0)
  {
    if(system_led.blink_timer == 0)
    {
      system_led.blink_timer = LED_BLINK_TIME;

      if(System_Led_State() == 0)
      {
        System_Led_ON();
      }
      else
      {
        system_led.blink_timer = 8*LED_BLINK_TIME;
        System_Led_OFF();
      }
    }
  }
  else
  {
    if(system_led.blink_timer == 0)
    {
      if(System_Led_State() != 0)
      {
        System_Led_OFF();
      }
    }
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//Процессор
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Led_Processor(void)
{
  switch (led.state)
  {
  default:
  case LED_STATE_INIT:
    if(system_led.timer == 0)
    {
      system_led.timer = LED_LIGHT_TIME;
      if(System_Led_State() == 0)
      {
        System_Led_ON();
      }
      else
      {
        System_Led_OFF();
      }
    }
    break;

  case LED_STATE_LINK_UP:
    if(led.timer != 0)
    {
      if(System_Led_State() == 0)
      {
        System_Led_ON();
      }
    }
    else
    {
      led.state = LED_STATE_INIT;
    }
    break;

  case LED_STATE_ESTABLISHED:
    if(led.timer != 0)
    {
      System_Led_Processor();
      system_led.timer = SYS_LED_FLASH_TIME;
    }
    else
    {
      system_led.timer = 0;
      led.timer = LED_1SEC_TIME; 
      led.state = LED_STATE_LINK_UP;
    }
    break;
  }
}

//--------------------------------------------------------------------------------------------------------
