//outputs_defines.h

#if !defined( OUTPUTS_DEFINES_H )
#define       OUTPUTS_DEFINES_H
//-----------------------------------------------------------------------------
//
#define SYS_LED_FLASH_TIME            (0.05*(1000ul/SYSTEM_TICK))
#define LED_LIGHT_TIME                (0.5*(1000ul/SYSTEM_TICK))
#define LED_BLINK_TIME                (0.01*(1000ul/SYSTEM_TICK))
#define LED_FAST_BLINK_TIME           (0.05*(1000ul/SYSTEM_TICK))
#define LED_FAST_BLINK_VIEW_TIME      (0.8*(1000ul/SYSTEM_TICK))
#define LED_FLASH_TIME                (0.05*(1000ul/SYSTEM_TICK))
//
#define LED_2SEC_TIME                 (2*(1000ul/SYSTEM_TICK))
#define LED_1SEC_TIME                 (1*(1000ul/SYSTEM_TICK))
//
#define System_Led_OFF()              {GPIO_SetBits(SYS_LED_GPIO_PORT, SYS_LED_PIN);}
#define System_Led_ON()               {GPIO_ResetBits(SYS_LED_GPIO_PORT, SYS_LED_PIN);}
#define System_Led_State()            ((GPIO_ReadInputDataBit(SYS_LED_GPIO_PORT, SYS_LED_PIN)) == 0)
//
typedef struct system_led_data
{
  u8_t timer;
  u8_t blink_timer;
}system_led_t;

//
#define LED_STATE_INIT                0
#define LED_STATE_LINK_UP             1
#define LED_STATE_ESTABLISHED         2
//
#define Led_Set_Init()                {led.state = LED_STATE_INIT;}
#define Led_Set_Link_UP()             {led.timer = LED_1SEC_TIME; led.state = LED_STATE_LINK_UP;}
#define Led_Set_Established()         {led.timer = LED_1SEC_TIME; led.state = LED_STATE_ESTABLISHED;}
//
typedef struct led_data
{
  u8_t state;
  volatile u16_t timer;
}led_t;

//-----------------------------------------------------------------------------
#endif

