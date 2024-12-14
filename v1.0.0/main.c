//main.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>
#include <stdlib.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "pt.h"

#include "systick_defines.h"
#include "systick.h"

#include "timer1.h"

#include "outputs_defines.h"
#include "outputs.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_drv_defines.h"
#include "usb_drv.h"

#include "eeprom_defines.h"
#include "eeprom.h"

#include "usart2_defines.h"
#include "usart2.h"

#include "spi_defines.h"
#include "spi.h"

#include "core_defines.h"
#include "core.h"

#include "main_defines.h"
#include "main.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************
mysystem_t mysystem;

__root const u8_t pref_compile[] = "<<< ";
__root const u8_t devs_compile[] = "ALICE";
__root const u8_t date_compile[] = __DATE__;
__root const u8_t time_compile[] = __TIME__;
__root const u8_t autor[] = "AUTOR: ALEXEY STAROSTIN";
__root const u8_t post_compile[] = " >>>";

//************************************
// Prototypes
//************************************
void Correct_Main_Timers(void);
void GPIO_Open(void);
void Hardware_Init(void);
void DWT_On(void);
#pragma inline
u8_t DWT_Compare(signed long tp);
void DWT_Delay(u32_t us);
void IWDG_Init(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Коррекция таймеров
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Correct_Main_Timers(void)
{
  if(mysystem.timer != 0) mysystem.timer--;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Инициализация
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void GPIO_Open(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

  //Enable clock for AFIO
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Подпрограмма общей инициализации
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Hardware_Init(void)
{
  DWT_On();

  GPIO_Open();

  USB_Drv_Open();

  SysTick_Init();

  TIMER1_Init();

  Outputs_Init();

#ifdef WDTON
  IWDG_Init();
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Программная задержка
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DWT_On(void)
{
  #define    DWT_CYCCNT    *(volatile uint32_t *)0xE0001004ul
  #define    DWT_CONTROL   *(volatile uint32_t *)0xE0001000ul
  #define    SCB_DEMCR     *(volatile uint32_t *)0xE000EDFCul

  SCB_DEMCR |= 0x01000000ul;
  DWT_CYCCNT  = 0;
  DWT_CONTROL |= 1;
}

#pragma inline
u8_t DWT_Compare(signed long tp)
{
  return (((signed long)DWT_CYCCNT - tp) < 0);
}

void DWT_Delay(u32_t us)
{
  signed long tp = DWT_CYCCNT + us * (SYSCLK/1000000ul);
  while (DWT_Compare(tp));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// IWDG
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef WDTON
#define IWDT_PERIOD_2_SEC   (156*2)
void IWDG_Init(void)
{
  // включаем LSI
  RCC_LSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
  // разрешается доступ к регистрам IWDG
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  // устанавливаем предделитель
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  // значение для перезагрузки
  IWDG_SetReload(IWDT_PERIOD_2_SEC);
  // перезагрузим значение
  IWDG_ReloadCounter();
  // LSI должен быть включен
  IWDG_Enable();
}
#endif

//**************************************************************************************************
//  main
//**************************************************************************************************
int main( void )
{
  Hardware_Init();
  
#ifdef NDEBUG
  if (FLASH_GetReadOutProtectionStatus() == RESET)
  {
    FLASH_Unlock();
    FLASH_ReadOutProtection(ENABLE);
    FLASH_Lock();
  }
#endif

  __enable_irq();

#ifdef USBADD
  Set_USBSystem();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
#endif
  
  mysystem.timer = STARTUP_TIME;
  do
  {
#ifdef WDTON
    IWDG_ReloadCounter();
#endif
  }while(mysystem.timer != 0);

  //************************************
  // Инициализация процессов
  //************************************

  //************************************
  // Основной цикл
  //************************************
  while(1)
  {
#ifdef WDTON
    IWDG_ReloadCounter();
#endif

    System_Led_Processor();
    system_led.timer = SYS_LED_FLASH_TIME;

    USB_DRV_VCP_Processor();

    if(Resive_Paket() != 0){
      Sys_Led_Flash();
    }
  }
}

//--------------------------------------------------------------------------------------------------------
