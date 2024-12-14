//core_defines.h

#if !defined( CORE_DEFINES_H )
#define       CORE_DEFINES_H
//-----------------------------------------------------------------------------

#define VIRTUAL_EEPROM_SIZE 128

//
#define CORE_STATE_READY    0

//
#define CORE_PACK_TIMEOUT   (5*(1000ul/SYSTEM_TICK))
#define CORE_TIMEOUT        (0.25*(1000ul/SYSTEM_TICK))
#define HOST_TIMEOUT        (0.25*(1000ul/SYSTEM_TICK))

//
#define CORE_BUF_SIZE       256

//
#define PACK_FLAG_RX_TIMEOUT          0       // Флаг таймаута между символами
#define PACK_FLAG_IS_PROCESS          1

//Таймер приема пакета
#define Pack_Recive_Timer_Init()      {core.flags |= (1<<PACK_FLAG_RX_TIMEOUT); core.timer = CORE_PACK_TIMEOUT;}
#define Pack_Recive_Timer_Update()    {core.timer = CORE_PACK_TIMEOUT;}
#define Pack_Recive_Timer_Clr()       {core.flags &= ~(1<<PACK_FLAG_RX_TIMEOUT); core.timer = 0;}
#define Is_Pack_Recive_Flag_Set()     ((core.flags & (1<<PACK_FLAG_RX_TIMEOUT)) != 0)

//Команды CLI
#define CLI_CMD_W                               0
#define CLI_CMD_R                               1
#define CLI_CMD_E                               2
#define CLI_CMD_D                               3
#define CLI_CMD_UNDEF                           255
#define CLI_CMD_NUM                             4

//
typedef struct CLISig_data
{
  u8_t *str;
}CLISig_t;

//
typedef struct CLICmd_data
{
  u8_t cmd;
  u8_t addr;
  u8_t data;
}CLICmd_t;

//
typedef struct core_data
{
  u8_t state;
  volatile u16_t timer;
  u8_t buf[CORE_BUF_SIZE];
  u16_t buf_counter;
  u8_t flags;
  CLICmd_t CLICmd;
}core_t;

//-----------------------------------------------------------------------------
#endif
