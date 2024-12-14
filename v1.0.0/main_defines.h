//main_defines.h

#if !defined( MAIN_DEFINES_H )
#define       MAIN_DEFINES_H
//-----------------------------------------------------------------------------

#define SYSTEM_STATE_INIT       0
#define SYSTEM_STATE_WORK       1

#define Delay(us)               {DWT_Delay(us);}
#define Delay_ms(ms)            {DWT_Delay(ms * 1000ul);}

#define USB_TIMEOUT             ((unsigned)(0.02*(1000ul/SYSTEM_TICK)))
#define SYS_1SEC_TIME           (1*(1000ul/SYSTEM_TICK))
#define LINK_WDT_TIME           (5*(1000ul/SYSTEM_TICK))

#define USB_BUF_SIZE            64

typedef struct mysystem_data
{
  u8_t state;

  volatile u8_t timer;
  volatile u8_t usb_timer;
  u8_t usb_buf[USB_BUF_SIZE];

  u32_t ldatatime;
  u8_t sec_timer;
  u8_t sec;
  u8_t min;
  u8_t hour;
  u8_t day;
  u8_t month;
  u8_t year;

  volatile u32_t link_wdt_timer;
  u16_t phy_addr;
  u16_t ip_work_port;
  u8_t ip_reset;
  u8_t pbr_br;
}mysystem_t;

//-----------------------------------------------------------------------------
#endif

