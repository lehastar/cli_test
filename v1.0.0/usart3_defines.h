//usart3_defines.h

#if !defined( USART3_DEFINES_H )
#define       USART3_DEFINES_H
//-----------------------------------------------------------------------------

//*********************************************************
//Конфигурация
//*********************************************************
//#define ALLOW_USART3_TX_DELAY   1

//*********************************************************
//USART3
//*********************************************************
#define USART3_RX_BUFFER_SIZE 512     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART3_TX_BUFFER_SIZE 512     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART3_RX_BUFFER_MASK (USART3_RX_BUFFER_SIZE - 1)
#define USART3_TX_BUFFER_MASK (USART3_TX_BUFFER_SIZE - 1)
#if ( USART3_RX_BUFFER_SIZE & USART3_RX_BUFFER_MASK )
#error RX1 buffer size is not a power of 2
#endif
#if ( USART3_TX_BUFFER_SIZE & USART3_TX_BUFFER_MASK )
#error TX1 buffer size is not a power of 2
#endif

//Флаги
#define USART3_FLAG_TX_IN_PROGRESS    0
#define USART3_FLAG_TX_COMPLETE       1
#define USART3_FLAG_SHIFT_REG_EMPTY   2
#define USART3_FLAG_ERR               3

#define USART3_FLAG_TX_TIMEOUT        4
#define USART3_TX_Timeout_Flag_Set()  {usart3.flags |= (1<<USART3_FLAG_TX_TIMEOUT);}
#define USART3_TX_Timeout_Flag_Clr()  {usart3.flags &= ~(1<<USART3_FLAG_TX_TIMEOUT);}
#define USART3_TX_Timeout_Flag_Check()(usart3.flags & (1<<USART3_FLAG_TX_TIMEOUT))

#define USART3_TX_DELAY_TIME          10
#define USART3_TX_Timeout_Init()      {usart3.timer = USART3_TX_DELAY_TIME; USART3_TX_Timeout_Flag_Set();}

typedef struct usart3_data
{
  u8_t rx_buf[USART3_RX_BUFFER_SIZE];
  u8_t tx_buf[USART3_TX_BUFFER_SIZE];
  u16_t rx_head;
  u16_t rx_tail;
  u16_t tx_head;
  u16_t tx_tail;
  u8_t timer;
  u8_t flags;
}usart3_t;

//-----------------------------------------------------------------------------
#endif

