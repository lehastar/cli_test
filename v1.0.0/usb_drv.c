//usb_drv.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "usart2_defines.h"
#include "usart2.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#include "main_defines.h"
#include "main.h"

#include "usb_drv_defines.h"
#include "usb_drv.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************
usb_drv_t usb_drv;
dbg_t dbg;

//************************************
//Prototypes
//************************************
void USB_Drv_Open(void);
void USB_Load_To_USART_RX_Buf(u8_t data);
void USB_BUF_Load_To_USART_RX_Buf(u8_t *data, u32_t len);
u8_t USB_Upload_From_USART_TX_Buf(void);
u8_t USB_Check_USART_TX_Buf(void);
void USB_DRV_VCP_Processor(void);

void DBG_Transmit(u8_t data);
void DBG_Sendstr(void *strbuf);
u8_t USB_Check_DBG_Buf(void);
u8_t USB_Upload_From_DBG_Buf(void);
void DEBUG_Print_Buf(u8_t *buf, u8_t len);
void DEBUG_Print_Str(u8_t *buf);
void DEBUG_Print_Counter(void);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Грузим данные в приемный буфер уарта ската
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USB_Drv_Open(void)
{
#ifdef USBADD
  //
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = USBDP_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(USBDP_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(USBDP_GPIO_PORT, USBDP_PIN);
  DWT_Delay(100000ul);
  GPIO_SetBits(USBDP_GPIO_PORT, USBDP_PIN);
  //
  Set_USBSystem();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
#endif  
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Грузим данные в приемный буфер уарта ската
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USB_Load_To_USART_RX_Buf(u8_t data)
{
  u16_t tmphead;

  tmphead = (usart2.rx_head + 1) & USART2_RX_BUFFER_MASK;
  usart2.rx_head = tmphead;
  usart2.rx_buf[tmphead] = data;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Грузим данные в приемный буфер уарта ската
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USB_BUF_Load_To_USART_RX_Buf(u8_t *data, u32_t len)
{
  u8_t i;

  i = 0;
  do
  {
    USB_Load_To_USART_RX_Buf(data[i]);
    i++;
  }while(i < len);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Забираем данные из буфера передатчика уартв
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USB_Upload_From_USART_TX_Buf(void)
{
  u16_t tmptail;

  tmptail = (usart2.tx_tail + 1) &	USART2_TX_BUFFER_MASK;
  usart2.tx_tail = tmptail;
  return usart2.tx_buf[tmptail];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Проверка буфер передатчика уарта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USB_Check_USART_TX_Buf(void)
{
  u16_t tmptail;

  tmptail =  usart2.tx_tail;
  return (usart2.tx_head != tmptail);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// USB_VCP
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void USB_DRV_VCP_Processor(void)
{
  u32_t i;

  /*
  if(DIN_State_Check(DIN_A_USB) == 0)
  {
    return;
  }
  */

#ifdef DEBUG_OUT_ON
  //
  if(USB_Check_DBG_Buf() != 0)
  {
    if(usb_drv.timer == 0)
    {
      i = 0;
      do
      {
        usb_drv.buf[i] = USB_Upload_From_DBG_Buf();
        i++;
      }while((USB_Check_DBG_Buf() != 0) && (i < USB_DRV_BUF_SIZE));
      usb_drv.data_size = i;
      CDC_Send_DATA ((u8_t*)&usb_drv.buf[0], usb_drv.data_size);
      USB_DRV_SET_TIMEOUT();
    }
  }
#else
  //
  if(USB_Check_USART_TX_Buf() != 0)
  {
    if(usb_drv.timer == 0)
    {
      i = 0;
      do
      {
        usb_drv.buf[i] = USB_Upload_From_USART_TX_Buf();
        i++;
      }while((USB_Check_USART_TX_Buf() != 0) && (i < USB_DRV_BUF_SIZE));
      usb_drv.data_size = i;
      CDC_Send_DATA ((u8_t*)&usb_drv.buf[0], usb_drv.data_size);
      USB_DRV_SET_TIMEOUT();
    }
  }
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача байта
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DBG_Transmit(u8_t data)
{
  u16_t tmphead;
  tmphead = (dbg.head + 1) & DBG_BUFFER_MASK;
  dbg.buf[tmphead] = data;
  dbg.head = tmphead;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Передача строки
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DBG_Sendstr(void *strbuf)
{
  u8_t *sram_ptr = 0;
  sram_ptr = (u8_t*)strbuf;
  while (*sram_ptr != 0) DBG_Transmit(*sram_ptr++);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Проверка буфера
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USB_Check_DBG_Buf(void)
{
  u16_t tmptail;

  tmptail =  dbg.tail;
  return (dbg.head != tmptail);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Забираем данные из буфера
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t USB_Upload_From_DBG_Buf(void)
{
  u16_t tmptail;

  tmptail = (dbg.tail + 1) &	DBG_BUFFER_MASK;
  dbg.tail = tmptail;
  return dbg.buf[tmptail];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DEBUG_Print_Buf(u8_t *buf, u8_t len)
{
  u8_t i,j;

  i = 0;
  do
  {
    j = Get_ASCII(buf[i]>>4);
    DBG_Transmit(j);
    j = Get_ASCII(0x0F&buf[i]);
    DBG_Transmit(j);
    i++;
  }while(i < len);

  DBG_Transmit('\r');
  DBG_Transmit('\n');
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DEBUG_Print_Str(u8_t *buf)
{
  DBG_Sendstr((u8_t *)&buf[0]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DEBUG_Print_Counter(void)
{
  dbg.debug_counter.dc16++;
  dbg.tmp.tmp32 = Int_To_BCD(dbg.debug_counter.dc16);
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[3]>>4));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[3]&0x0F));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[2]>>4));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[2]&0x0F));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[1]>>4));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[1]&0x0F));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[0]>>4));
  DBG_Transmit(Get_ASCII(dbg.tmp.tmp8[0]&0x0F));
}


//--------------------------------------------------------------------------------------------------------

