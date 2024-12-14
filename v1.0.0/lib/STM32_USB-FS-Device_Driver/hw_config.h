#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "usb_type.h"

#define ID1 (0x1FFFF7E8)
#define ID2 (0x1FFFF7EC)
#define ID3 (0x1FFFF7F0)

/* Exported functions ------------------------------------------------------- */
extern void Set_USBSystem(void);
extern void Set_USBClock(void);

extern void Enter_LowPowerMode(void);
extern void Leave_LowPowerMode(void);

extern void USB_Interrupts_Config(void);
extern void USB_Cable_Config (FunctionalState NewState);
extern void USART_Config_Default(void);
extern bool USART_Config(void);
extern void USB_To_USART_Send_Data(uint8_t* data_buffer, uint8_t Nb_bytes);
extern void Handle_USBAsynchXfer (void);
extern void Get_SerialNum(void);

extern uint32_t CDC_Send_DATA (uint8_t *ptrBuffer, uint8_t Send_length);

/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
