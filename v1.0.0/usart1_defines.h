//usart1_defines.h

#if !defined( USART1_DEFINES_H )
#define       USART1_DEFINES_H
//-----------------------------------------------------------------------------

//*********************************************************
//Конфигурация
//*********************************************************
//#define ALLOW_USART1_TX_DELAY   1

//*********************************************************
//USART1
//*********************************************************
#define USART1_RX_BUFFER_SIZE 4       /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART1_TX_BUFFER_SIZE 1024    /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART1_RX_BUFFER_MASK (USART1_RX_BUFFER_SIZE - 1)
#define USART1_TX_BUFFER_MASK (USART1_TX_BUFFER_SIZE - 1)
#if ( USART1_RX_BUFFER_SIZE & USART1_RX_BUFFER_MASK )
#error RX1 buffer size is not a power of 2
#endif
#if ( USART1_TX_BUFFER_SIZE & USART1_TX_BUFFER_MASK )
#error TX1 buffer size is not a power of 2
#endif

//Флаги
#define USART1_FLAG_TX_IN_PROGRESS    0
#define USART1_FLAG_TX_COMPLETE       1
#define USART1_FLAG_SHIFT_REG_EMPTY   2
#define USART1_FLAG_ERR               3

#define USART1_FLAG_TX_TIMEOUT        4
#define USART1_TX_Timeout_Flag_Set()  {usart1.flags |= (1<<USART1_FLAG_TX_TIMEOUT);}
#define USART1_TX_Timeout_Flag_Clr()  {usart1.flags &= ~(1<<USART1_FLAG_TX_TIMEOUT);}
#define USART1_TX_Timeout_Flag_Check()(usart1.flags & (1<<USART1_FLAG_TX_TIMEOUT))

#define USART1_TX_DELAY_TIME          10
#define USART1_TX_Timeout_Init()      {usart1.timer = USART1_TX_DELAY_TIME; USART1_TX_Timeout_Flag_Set();}

//#define Global_Bus_RS485_Dir_TX()     {GPIO_SetBits(GL_BUS_DIR_GPIO_PORT, GL_BUS_DIR_PIN);}
//#define Global_Bus_RS485_Dir_RX()     {GPIO_ResetBits(GL_BUS_DIR_GPIO_PORT, GL_BUS_DIR_PIN);}

typedef struct usart1_data
{
  u8_t rx_buf[USART1_RX_BUFFER_SIZE];
  u8_t tx_buf[USART1_TX_BUFFER_SIZE];
  u16_t rx_head;
  u16_t rx_tail;
  u16_t tx_head;
  u16_t tx_tail;
  u8_t timer;
  u8_t flags;
}usart1_t;

//-----------------------------------------------------------------------------
#endif

