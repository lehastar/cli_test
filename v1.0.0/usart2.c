//usart2.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "usart2_defines.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************
usart2_t usart2;

//************************************
//Prototypes
//************************************
void USART2_Open(unsigned long baud);
void USART2_Rx_Buf_Clr(void);
void USART2_Tx_Buf_Clr(void);
void USART2_Disable_Recive(void);
void USART2_Block_Recive(void);
void USART2_Enable_Recive(void);
u8_t USART2_Receive(void);
void USART2_Transmit(u8_t data);
void USART0_Control_Transmit_Timeout(void);
void USART2_Sendstr(void *strbuf);
void USART2_Send_ASCII(u8_t* lrc, u8_t data);
u8_t USART2_DataInReceiveBuffer(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Инициализация USART2
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Open(unsigned long baud)
{
#ifdef USBADD
#else
  //
  GPIO_InitTypeDef  GPIO_InitStructure;
  //
  USART2_Rx_Buf_Clr();
  USART2_Tx_Buf_Clr();
  //
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  //
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //
  GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);
  //
  GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
  GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);
  //
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  //
  USART_Init(USART2, &USART_InitStructure);
  //
  USART_Cmd(USART2, ENABLE);
  //
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Очистка
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Rx_Buf_Clr(void)
{
  usart2.rx_head = 0;
  usart2.rx_tail = 0;
}

void USART2_Tx_Buf_Clr(void)
{
  usart2.tx_head = 0;
  usart2.tx_tail = 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Запрет приема
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Disable_Recive(void)
{
  //USART2->CR1 &= ~USART_CR1_RE;
  //USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  //Global_Bus_RS485_Dir_TX();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Разрешение приема
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Enable_Recive(void)
{
  //USART2->CR1 |= USART_CR1_RE;
  //Global_Bus_RS485_Dir_RX();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Прием байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USART2_Receive(void)
{
  u16_t tmptail;

  tmptail =  usart2.rx_tail;
  while (usart2.rx_head == tmptail);
  tmptail = (usart2.rx_tail + 1) & USART2_RX_BUFFER_MASK;
  usart2.rx_tail = tmptail;

  return usart2.rx_buf[tmptail];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Transmit(u8_t data)
{
  u16_t tmphead;

  tmphead = (usart2.tx_head + 1) & USART2_TX_BUFFER_MASK;
  //while (tmphead == usart2.tx_tail);

  usart2.tx_buf[tmphead] = data;
  usart2.tx_head = tmphead;

  usart2.flags &= ~(1<<USART2_FLAG_TX_COMPLETE);
  usart2.flags &= ~(1<<USART2_FLAG_SHIFT_REG_EMPTY);

#ifdef ALLOW_USART2_TX_DELAY
  USART2_TX_Timeout_Init();
#else
  #ifdef USBADD
  #else
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
  #endif
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Контроль таймаута передачи
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Control_Transmit_Timeout(void)
{
  if(USART2_TX_Timeout_Flag_Check() != 0)
  {
    if(usart2.timer == 0)
    {
      USART2_TX_Timeout_Flag_Clr();
      USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача строки
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART2_Sendstr(void *strbuf)
{
  u8_t *sram_ptr = 0;
  sram_ptr = (u8_t*)strbuf;
  while (*sram_ptr != 0) USART2_Transmit(*sram_ptr++);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Проверка буфера
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USART2_DataInReceiveBuffer(void)
{
  u16_t tmptail;

  tmptail =  usart2.rx_tail;
  return (usart2.rx_head != tmptail);
}

//******************************************************************************************
//Обработчики прерывания USART2
//******************************************************************************************
void USART2_IRQHandler(void)
{
  u8_t data;
  u16_t tmptail;
  u16_t tmphead;

  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    data = USART_ReceiveData(USART2);

    tmphead = ( usart2.rx_head + 1 ) & USART2_RX_BUFFER_MASK;
    usart2.rx_head = tmphead;

    if ( tmphead == usart2.rx_tail )
    {
      //ERROR! Receive buffer overflow
    }
    usart2.rx_buf[tmphead] = data;
  }

  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {
    tmphead = usart2.tx_head;

    if( tmphead != usart2.tx_tail )
    {
      tmptail = ( usart2.tx_tail + 1 ) & USART2_TX_BUFFER_MASK;
      usart2.tx_tail = tmptail;
      USART_SendData(USART2, usart2.tx_buf[tmptail]);
    }
    else
    {
      USART_ClearITPendingBit(USART2, USART_IT_TC);
      USART_ITConfig(USART2, USART_IT_TC, ENABLE);
      USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
      usart2.flags |= (1<<USART2_FLAG_TX_COMPLETE);
    }
  }

  if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART2, USART_IT_TC);
    if((usart2.flags & (1<<USART2_FLAG_TX_COMPLETE)) != 0)
    {
      usart2.flags &= ~(1<<USART2_FLAG_TX_COMPLETE);
      usart2.flags |= (1<<USART2_FLAG_SHIFT_REG_EMPTY);
      USART2_Enable_Recive();
      USART_ITConfig(USART2, USART_IT_TC, DISABLE);
      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    }
  }

  if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
  {
    USART_ClearITPendingBit(USART2, USART_IT_ORE);
  }
}

//--------------------------------------------------------------------------------------------------------
