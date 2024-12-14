//usart2.h

#if !defined( USART2_H )
#define       USART2_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern usart2_t usart2;

//************************************
//Prototypes
//************************************
extern void USART2_Open(unsigned long baud);
extern void USART2_Rx_Buf_Clr(void);
extern void USART2_Tx_Buf_Clr(void);
extern void USART2_Disable_Recive(void);
extern void USART2_Block_Recive(void);
extern void USART2_Enable_Recive(void);
extern u8_t USART2_Receive(void);
extern void USART2_Transmit(u8_t data);
extern void USART2_Control_Transmit_Timeout(void);
extern void USART2_Sendstr(void *strbuf);
extern void USART2_Send_ASCII(u8_t* lrc, u8_t data);
extern u8_t USART2_DataInReceiveBuffer(void);

//-----------------------------------------------------------------------------
#endif

