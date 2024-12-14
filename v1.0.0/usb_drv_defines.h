//usb_drv_defines.h

#if !defined( USB_DRV_DEFINES_H )
#define       USB_DRV_DEFINES_H
//-----------------------------------------------------------------------------

//
#define USB_DRV_TIMEOUT         ((unsigned)(0.02*(1000ul/SYSTEM_TICK)))
#define USB_DRV_BUF_SIZE        60
#define USB_DRV_SET_TIMEOUT()   {usb_drv.timer = USB_DRV_TIMEOUT;}

typedef struct usb_drv_data
{
  u8_t buf[USB_DRV_BUF_SIZE];
  u8_t data_size;
  volatile u8_t timer;
}usb_drv_t;

//
#define DBG_BUFFER_SIZE 1024     /* 2,4,8,16,32,64,128 or 256 bytes */
#define DBG_BUFFER_MASK (DBG_BUFFER_SIZE - 1)
#if ( DBG_BUFFER_SIZE & DBG_BUFFER_MASK )
#error DBG buffer size is not a power of 2
#endif

typedef struct dbg_data
{
  u8_t buf[DBG_BUFFER_SIZE];
  u16_t head;
  u16_t tail;
  //
  union debug_counter_u
  {
    u16_t dc16;
    u8_t dc8[2];
  }debug_counter;
  //
  union tmp_u
  {
    u32_t tmp32;
    u8_t tmp8[4];
  }tmp;
}dbg_t;

//-----------------------------------------------------------------------------
#endif

