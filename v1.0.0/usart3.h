//usart3.h

#if !defined( USART3_H )
#define       USART3_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern usart3_t usart3;

//************************************
//Prototypes
//************************************
extern void USART3_Open(unsigned long baud);
extern void USART3_Rx_Buf_Clr(void);
extern void USART3_Tx_Buf_Clr(void);
extern void USART3_Disable_Recive(void);
extern void USART3_Block_Recive(void);
extern void USART3_Enable_Recive(void);
extern u8_t USART3_Receive(void);
extern void USART3_Transmit(u8_t data);
extern void USART3_Control_Transmit_Timeout(void);
extern void USART3_Sendstr(void *strbuf);
extern void USART3_Send_ASCII(u8_t* lrc, u8_t data);
extern u8_t USART3_DataInReceiveBuffer(void);

//-----------------------------------------------------------------------------
#endif

