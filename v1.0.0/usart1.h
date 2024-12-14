//usart1.h

#if !defined( USART1_H )
#define       USART1_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern usart1_t usart1;

//************************************
//Prototypes
//************************************
extern void USART1_Open(unsigned long baud);
extern void USART1_Rx_Buf_Clr(void);
extern void USART1_Tx_Buf_Clr(void);
extern void USART1_Disable_Recive(void);
extern void USART1_Block_Recive(void);
extern void USART1_Enable_Recive(void);
extern u8_t USART1_Receive(void);
extern void USART1_Transmit(u8_t data);
extern void USART1_Control_Transmit_Timeout(void);
extern void USART1_Sendstr(void *strbuf);
extern void USART1_Send_ASCII(u8_t* lrc, u8_t data);
extern u8_t USART1_DataInReceiveBuffer(void);

//-----------------------------------------------------------------------------
#endif

