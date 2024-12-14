//usart1.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_drv_defines.h"
#include "usb_drv.h"

#include "gsm_defines.h"
#include "gsm.h"

#include "usart1_defines.h"
#include "usart1.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************
usart1_t usart1;

//************************************
//Prototypes
//************************************
void USART1_Open(unsigned long baud);
void USART1_Rx_Buf_Clr(void);
void USART1_Tx_Buf_Clr(void);
void USART1_Disable_Recive(void);
void USART1_Block_Recive(void);
void USART1_Enable_Recive(void);
u8_t USART1_Receive(void);
void USART1_Transmit(u8_t data);
void USART1_Control_Transmit_Timeout(void);
void USART1_Sendstr(void *strbuf);
void USART1_Send_ASCII(u8_t* lrc, u8_t data);
u8_t USART1_DataInReceiveBuffer(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Инициализация USART1
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Open(unsigned long baud)
{
  //
  GPIO_InitTypeDef  GPIO_InitStructure;
/*
  //
  RCC_APB2PeriphClockCmd(GL_BUS_DIR_GPIO_CLK, ENABLE);
  //
  Global_Bus_RS485_Dir_RX();
  GPIO_InitStructure.GPIO_Pin = GL_BUS_DIR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GL_BUS_DIR_GPIO_PORT, &GPIO_InitStructure);
*/
  //
  USART1_Rx_Buf_Clr();
  USART1_Tx_Buf_Clr();
  //
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  //
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //
  GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
  //
  GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
  GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
  //
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  //
  USART_Init(USART1, &USART_InitStructure);
  //
  USART_Cmd(USART1, ENABLE);
  //
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Очистка
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Rx_Buf_Clr(void)
{
  usart1.rx_head = 0;
  usart1.rx_tail = 0;
}

void USART1_Tx_Buf_Clr(void)
{
  usart1.tx_head = 0;
  usart1.tx_tail = 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Запрет приема
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Disable_Recive(void)
{
  USART1->CR1 &= ~USART_CR1_RE;
  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  //Global_Bus_RS485_Dir_TX();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Разрешение приема
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Enable_Recive(void)
{
  USART1->CR1 |= USART_CR1_RE;
  //Global_Bus_RS485_Dir_RX();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Прием байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USART1_Receive(void)
{
  u16_t tmptail;

  tmptail =  usart1.rx_tail;
  while (usart1.rx_head == tmptail);
  tmptail = (usart1.rx_tail + 1) & USART1_RX_BUFFER_MASK;
  usart1.rx_tail = tmptail;

  return usart1.rx_buf[tmptail];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Transmit(u8_t data)
{
  u16_t tmphead;

  tmphead = (usart1.tx_head + 1) & USART1_TX_BUFFER_MASK;
  while (tmphead == usart1.tx_tail);

  usart1.tx_buf[tmphead] = data;
  usart1.tx_head = tmphead;

  usart1.flags &= ~(1<<USART1_FLAG_TX_COMPLETE);
  usart1.flags &= ~(1<<USART1_FLAG_SHIFT_REG_EMPTY);

#ifdef ALLOW_USART1_TX_DELAY
  USART1_TX_Timeout_Init();
#else
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Контроль таймаута передачи
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Control_Transmit_Timeout(void)
{
  if(USART1_TX_Timeout_Flag_Check() != 0)
  {
    if(usart1.timer == 0)
    {
      USART1_TX_Timeout_Flag_Clr();
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача строки
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USART1_Sendstr(void *strbuf)
{
  u8_t *sram_ptr = 0;
  sram_ptr = (u8_t*)strbuf;
  while (*sram_ptr != 0) USART1_Transmit(*sram_ptr++);

#ifdef DEBUG_OUT_ON
  //DEBUG_Print_Str("#");
  //DEBUG_Print_Counter();  
  //DEBUG_Print_Str(" TX:\r\n");  
  DEBUG_Print_Str("\r\n");
  DEBUG_Print_Str("TX: ");
  DEBUG_Print_Str((u8_t*)strbuf);
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Проверка буфера
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USART1_DataInReceiveBuffer(void)
{
  u16_t tmptail;

  tmptail =  usart1.rx_tail;
  return (usart1.rx_head != tmptail);
}

//******************************************************************************************
//Обработчики прерывания USART1
//******************************************************************************************
void USART1_IRQHandler(void)
{
  u8_t data;
  u16_t tmptail;
  u16_t tmphead;

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    data = USART_ReceiveData(USART1);

    tmphead = ( usart1.rx_head + 1 ) & USART1_RX_BUFFER_MASK;
    usart1.rx_head = tmphead;

    if ( tmphead == usart1.rx_tail )
    {
      //ERROR! Receive buffer overflow
    }
    usart1.rx_buf[tmphead] = data;

#ifdef DEBUG_OUT_ON
    if(gsm.chr_counter == 0) {
      DEBUG_Print_Str("\r\n");
      DEBUG_Print_Str("RX: ");
    }
#endif

    if(gsm.chr_counter < (GSM_BUF_SIZE - 1)) gsm.buf[gsm.chr_counter++] = data;

#ifdef DEBUG_OUT_ON
    DBG_Transmit(data);
#endif
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    tmphead = usart1.tx_head;

    if( tmphead != usart1.tx_tail )
    {
      tmptail = ( usart1.tx_tail + 1 ) & USART1_TX_BUFFER_MASK;
      usart1.tx_tail = tmptail;
      USART_SendData(USART1, usart1.tx_buf[tmptail]);
    }
    else
    {
      USART_ClearITPendingBit(USART1, USART_IT_TC);
      USART_ITConfig(USART1, USART_IT_TC, ENABLE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      usart1.flags |= (1<<USART1_FLAG_TX_COMPLETE);
    }
  }

  if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    if((usart1.flags & (1<<USART1_FLAG_TX_COMPLETE)) != 0)
    {
      usart1.flags &= ~(1<<USART1_FLAG_TX_COMPLETE);
      usart1.flags |= (1<<USART1_FLAG_SHIFT_REG_EMPTY);
      USART1_Enable_Recive();
      USART_ITConfig(USART1, USART_IT_TC, DISABLE);
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
  }

  if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_ORE);
  }
}

//--------------------------------------------------------------------------------------------------------

