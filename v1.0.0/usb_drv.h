//usb_drv.h

#if !defined( USB_DRV_H )
#define       USB_DRV_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern usb_drv_t usb_drv;
extern dbg_t dbg;

//************************************
//Prototypes
//************************************
extern void USB_Drv_Open(void);
extern void USB_Load_To_USART_RX_Buf(u8_t data);
extern void USB_BUF_Load_To_USART_RX_Buf(u8_t *data, u32_t len);
extern u8_t USB_Upload_From_USART_TX_Buf(void);
extern u8_t USB_Check_USART_TX_Buf(void);
extern void USB_DRV_VCP_Processor(void);

extern void DBG_Transmit(u8_t data);
extern void DBG_Sendstr(void *strbuf);
extern u8_t USB_Check_DBG_Buf(void);
extern u8_t USB_Upload_From_DBG_Buf(void);
extern void DEBUG_Print_Buf(u8_t *buf, u8_t len);
extern void DEBUG_Print_Str(u8_t *buf);
extern void DEBUG_Print_Counter(void);

//-----------------------------------------------------------------------------
#endif

