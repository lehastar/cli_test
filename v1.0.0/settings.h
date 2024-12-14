//settings.h

#if !defined( SETTINGS_H )
#define       SETTINGS_H
//-----------------------------------------------------------------------------

//#define NDEBUG                1
#define USBADD                1
//#define WDTON                 1

//#define DEBUG_OUT_ON          1

//*********************************************************
//ВЕРСИЯ ПРОГРАММЫ
//*********************************************************
#define  DEVICE_TYPE          0x41 	//Контроллер СКАТ
#define  DEVICE_VERSION       1    	//Версия
#define  DEVICE_SUBVERSION    0  	  //Субверсия

//*********************************************************
//SYSYEM
//*********************************************************
#define  XTAL                 8000000ul
#define  SYSCLK               72000000ul

//*********************************************************
//TIMER
//*********************************************************
#define TIMER2_PSC            35999ul
#define TIMER2_PERIOD         20ul

#define SYSTEM_TICK           10
#define STARTUP_TIME          ((unsigned)(0.1*(1000ul/SYSTEM_TICK)))

//*********************************************************
// CRC
//*********************************************************
#define CRC_INI_VAL           0xDE
#define CRC_DB_INI_VAL        0xFFFFFFFFul

//*********************************************************
//Тип идентификатора
//*********************************************************
#define IDENT_TYPE_CARD                 1
#define IDENT_TYPE_PIN                  2

//*********************************************************
//
//*********************************************************
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned long u32_t;
typedef signed char s8_t;
typedef signed short s16_t;
typedef signed long s32_t;

typedef union ux16
{
  u16_t value16;
  u8_t value8[2];
}ux16_t;

typedef union ux32
{
  u32_t value32;
  u8_t value8[4];
}ux32_t;

typedef union ux64
{
  long long value64;
  u8_t value8[8];
}ux64_t;

#define GLOBAL_CARD_LEN       8         //Длинна кода карты

//Окружение считывателя
typedef struct key_data
{
  u8_t code[GLOBAL_CARD_LEN];
  u8_t code_tmp[GLOBAL_CARD_LEN];
  u8_t timer;
  u8_t timer_join_ctrl;
  u8_t ident_type;
  u8_t multiple_reading_counter;
  u8_t pin_counter;
  u16_t pin_assembling_timer;
  u16_t pin;
  u8_t flags;
}key_t;

//-----------------------------------------------------------------------------
#endif
