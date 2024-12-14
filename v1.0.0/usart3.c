//usart3.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "usart3_defines.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************
usart3_t usart3;

//************************************
//Prototypes
//************************************
void USART3_Open(unsigned long baud);
void USART3_Rx_Buf_Clr(void);
void USART3_Tx_Buf_Clr(void);
void USART3_Disable_Recive(void);
void USART3_Block_Recive(void);
void USART3_Enable_Recive(void);
u8_t USART3_Receive(void);
void USART3_Transmit(u8_t data);
void USART3_Control_Transmit_Timeout(void);
void USART3_Sendstr(void *strbuf);
void USART3_Send_ASCII(u8_t* lrc, u8_t data);
u8_t USART3_DataInReceiveBuffer(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Инициализация USART3
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Open(unsigned long baud)
{
  //
  GPIO_InitTypeDef  GPIO_InitStructure;
  //
  USART3_Rx_Buf_Clr();
  USART3_Tx_Buf_Clr();
  //
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  //
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //
  GPIO_InitStructure.GPIO_Pin = USART3_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);
  //
  GPIO_InitStructure.GPIO_Pin = USART3_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
  GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);
  //
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  //
  USART_Init(USART3, &USART_InitStructure);
  //
  USART_Cmd(USART3, ENABLE);
  //
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Очистка
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Rx_Buf_Clr(void)
{
  usart3.rx_head = 0;
  usart3.rx_tail = 0;
}

void USART3_Tx_Buf_Clr(void)
{
  usart3.tx_head = 0;
  usart3.tx_tail = 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Запрет приема
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Disable_Recive(void)
{
  USART3->CR1 &= ~USART_CR1_RE;
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  //Global_Bus_RS485_Dir_TX();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Разрешение приема
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Enable_Recive(void)
{
  USART3->CR1 |= USART_CR1_RE;
  //Global_Bus_RS485_Dir_RX();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Прием байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USART3_Receive(void)
{
  u16_t tmptail;

  tmptail =  usart3.rx_tail;
  while (usart3.rx_head == tmptail);
  tmptail = (usart3.rx_tail + 1) & USART3_RX_BUFFER_MASK;
  usart3.rx_tail = tmptail;

  return usart3.rx_buf[tmptail];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Transmit(u8_t data)
{
  u16_t tmphead;

  tmphead = (usart3.tx_head + 1) & USART3_TX_BUFFER_MASK;
  while (tmphead == usart3.tx_tail);

  usart3.tx_buf[tmphead] = data;
  usart3.tx_head = tmphead;

  usart3.flags &= ~(1<<USART3_FLAG_TX_COMPLETE);
  usart3.flags &= ~(1<<USART3_FLAG_SHIFT_REG_EMPTY);

#ifdef ALLOW_USART3_TX_DELAY
  USART3_TX_Timeout_Init();
#else
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Контроль таймаута передачи
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Control_Transmit_Timeout(void)
{
  if(USART3_TX_Timeout_Flag_Check() != 0)
  {
    if(usart3.timer == 0)
    {
      USART3_TX_Timeout_Flag_Clr();
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача строки
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART3_Sendstr(void *strbuf)
{
  u8_t *sram_ptr = 0;
  sram_ptr = (u8_t*)strbuf;
  while (*sram_ptr != 0) USART3_Transmit(*sram_ptr++);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Проверка буфера
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USART3_DataInReceiveBuffer(void)
{
  u16_t tmptail;

  tmptail =  usart3.rx_tail;
  return (usart3.rx_head != tmptail);
}

//******************************************************************************************
//Обработчики прерывания USART3
//******************************************************************************************
void USART3_IRQHandler(void)
{
  u8_t data;
  u16_t tmptail;
  u16_t tmphead;

  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    data = USART_ReceiveData(USART3);

    tmphead = ( usart3.rx_head + 1 ) & USART3_RX_BUFFER_MASK;
    usart3.rx_head = tmphead;

    if ( tmphead == usart3.rx_tail )
    {
      //ERROR! Receive buffer overflow
    }
    usart3.rx_buf[tmphead] = data;
  }

  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
    tmphead = usart3.tx_head;

    if( tmphead != usart3.tx_tail )
    {
      tmptail = ( usart3.tx_tail + 1 ) & USART3_TX_BUFFER_MASK;
      usart3.tx_tail = tmptail;
      USART_SendData(USART3, usart3.tx_buf[tmptail]);
    }
    else
    {
      USART_ClearITPendingBit(USART3, USART_IT_TC);
      USART_ITConfig(USART3, USART_IT_TC, ENABLE);
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      usart3.flags |= (1<<USART3_FLAG_TX_COMPLETE);
    }
  }

  if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART3, USART_IT_TC);
    if((usart3.flags & (1<<USART3_FLAG_TX_COMPLETE)) != 0)
    {
      usart3.flags &= ~(1<<USART3_FLAG_TX_COMPLETE);
      usart3.flags |= (1<<USART3_FLAG_SHIFT_REG_EMPTY);
      USART3_Enable_Recive();
      USART_ITConfig(USART3, USART_IT_TC, DISABLE);
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    }
  }

  if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET)
  {
    USART_ClearITPendingBit(USART3, USART_IT_ORE);
  }
}

//--------------------------------------------------------------------------------------------------------
