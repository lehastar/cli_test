//outputs.h

#if !defined( OUTPUTS_H )
#define       OUTPUTS_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern system_led_t system_led;
extern led_t led;

//************************************
// Prototypes
//************************************
extern void Correct_Outputs_Timers(void);
extern void Outputs_Init(void);
extern void Sys_Led_Flash(void);
extern void System_Led_Processor(void);
extern void Led_Processor(void);

//-----------------------------------------------------------------------------
#endif

