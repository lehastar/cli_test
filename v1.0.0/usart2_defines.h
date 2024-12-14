//usart2_defines.h

#if !defined( USART2_DEFINES_H )
#define       USART2_DEFINES_H
//-----------------------------------------------------------------------------

//*********************************************************
//Конфигурация
//*********************************************************
//#define ALLOW_USART2_TX_DELAY   1

//*********************************************************
//USART2
//*********************************************************
#define USART2_RX_BUFFER_SIZE 512     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART2_TX_BUFFER_SIZE 2048     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART2_RX_BUFFER_MASK (USART2_RX_BUFFER_SIZE - 1)
#define USART2_TX_BUFFER_MASK (USART2_TX_BUFFER_SIZE - 1)
#if ( USART2_RX_BUFFER_SIZE & USART2_RX_BUFFER_MASK )
#error RX1 buffer size is not a power of 2
#endif
#if ( USART2_TX_BUFFER_SIZE & USART2_TX_BUFFER_MASK )
#error TX1 buffer size is not a power of 2
#endif

//Флаги
#define USART2_FLAG_TX_IN_PROGRESS    0
#define USART2_FLAG_TX_COMPLETE       1
#define USART2_FLAG_SHIFT_REG_EMPTY   2
#define USART2_FLAG_ERR               3

#define USART2_FLAG_TX_TIMEOUT        4
#define USART2_TX_Timeout_Flag_Set()  {usart2.flags |= (1<<USART2_FLAG_TX_TIMEOUT);}
#define USART2_TX_Timeout_Flag_Clr()  {usart2.flags &= ~(1<<USART2_FLAG_TX_TIMEOUT);}
#define USART2_TX_Timeout_Flag_Check()(usart2.flags & (1<<USART2_FLAG_TX_TIMEOUT))

#define USART2_TX_DELAY_TIME          10
#define USART2_TX_Timeout_Init()      {usart1.timer = USART2_TX_DELAY_TIME; USART2_TX_Timeout_Flag_Set();}

typedef struct usart2_data
{
  u8_t rx_buf[USART2_RX_BUFFER_SIZE];
  u8_t tx_buf[USART2_TX_BUFFER_SIZE];
  u16_t rx_head;
  u16_t rx_tail;
  u16_t tx_head;
  u16_t tx_tail;
  u8_t timer;
  u8_t flags;
}usart2_t;

//-----------------------------------------------------------------------------
#endif

