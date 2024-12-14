//core.h

#if !defined( CORE_H )
#define       CORE_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern core_t core;

//************************************
// Prototypes
//************************************
extern void Correct_Core_Timers(void);
extern u8_t Resive_Paket(void);
extern u8_t Get_CLI_CMD(CLICmd_t *CLIcmd, u8_t *buf, u8_t len);

//-----------------------------------------------------------------------------
#endif

