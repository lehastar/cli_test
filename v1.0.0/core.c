//core.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>
#include <stdlib.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "outputs_defines.h"
#include "outputs.h"

#include "usart2_defines.h"
#include "usart2.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_drv_defines.h"
#include "usb_drv.h"

#include "core_defines.h"
#include "core.h"

//--------------------------------------------------------------------------------------------------------
//************************************
// Variables
//************************************
core_t core;
u8_t VIRTUAL_EEPROM[VIRTUAL_EEPROM_SIZE];

//Команды в SMS
const CLISig_t CLICmdSig[CLI_CMD_NUM] = 
{
  {//1
    "eeprom -w -a"                    //“Запись ячейки”
  },
  {//2
    "eeprom -r -a"                    //“Чтение ячейки”
  },
  {//3
    "eeprom -e -a"                    //“Стирание ячейки”
  },
  {//4
    "eeprom -d"                       //“Дамп всех ячеек”
  }
};

//************************************
// Prototypes
//************************************
void Correct_Core_Timers(void);
u8_t Resive_Paket(void);
u8_t Get_CLI_CMD(CLICmd_t *CLIcmd, u8_t *buf, u8_t len);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Коррекция таймеров
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Correct_Core_Timers(void)
{
  if(core.timer != 0) core.timer--;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Прием и декодирование пакета
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t Is_Dig(u8_t b)
{
  if( ((b >= '0') && (b <= '9')) ||
      ((b >= 'a') && (b <= 'f')) ||
      ((b >= 'f') && (b <= 'F')) ){
    return 1;
  }
  return 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void CLR_Init(void)
{
  core.buf_counter = 0;
  Pack_Recive_Timer_Clr();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t Get_CLI_CMD(CLICmd_t *CLIcmd, u8_t *buf, u8_t len)
{
  u8_t i, j, n;
  ux32_t bufx;
  u16_t b;

  i = 0;
  do
  {
    n = strlen((const char*)CLICmdSig[i].str);//Длина шаблона
    //
    if(memcmp((u8_t*)&buf[0], //Сравним с шаблоном
              (u8_t*)&CLICmdSig[i].str[0], 
              n) == 0) {
      //
      if(len < n) {
        return 0;
      }
      //
      switch(i) {
      //Команда
      case CLI_CMD_D://Команда вывести дамп
        CLIcmd->cmd = i;
        return 1;
      case CLI_CMD_R://Команда чтения
      case CLI_CMD_E://Команда стереть
      case CLI_CMD_W://Команда записать
        CLIcmd->cmd = i;
        bufx.value32 = 0;
        n++;
        bufx.value8[0] = buf[n++];
        bufx.value8[1] = buf[n++];
        bufx.value8[2] = buf[n++];
        if( (b = atoi((const char*)&bufx.value8[0])) >= VIRTUAL_EEPROM_SIZE ){//Выделим значение адреса
          return 0;
        }
        CLIcmd->addr = b;
        if(i == CLI_CMD_W) {
          j = 0;
          do
          {
            if( (buf[j] == 'v') || (buf[j] == 'V') ) {
              bufx.value32 = 0;
              j += 2;
              bufx.value8[0] = buf[j++];
              bufx.value8[1] = buf[j++];
              bufx.value8[2] = buf[j++];
              b = atoi((const char*)&bufx.value8[0]);//Выделим значение данных
              if(b > 255){
                return 0;
              }
              CLIcmd->data = (u8_t)b;
              return 1;
            }
            j++;
          }while(j < len);
          if(j >= len){
            return 0;
          }
        }else{
          return 1;
        }
        break;
      }//switch(i)
    }//if(memcmp
    i++;
  }while(i < CLI_CMD_NUM);
  return 0;
}
  
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Прием и декодирование пакета
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t Resive_Paket(void)
{
  u8_t rx_byte;
  u8_t str[8];
  u8_t i,j;

  //
  if(core.timer == 0) {
    if(Is_Pack_Recive_Flag_Set() != 0) {
      USART2_Sendstr("ERROR\r\n");
      CLR_Init();
    }
  }

  //Проверка - не пришло ли чего в буфер
  if(USART2_DataInReceiveBuffer() != 0)             //Если в буфере что-то есть
  {
    rx_byte = USART2_Receive();                     //Примем байта из буфера

    //
    switch(core.buf_counter) {
    case 0:
      if(rx_byte == 'e') {
        Pack_Recive_Timer_Init();
        core.buf[core.buf_counter++] = rx_byte;
      }else{
        if(rx_byte == '\r') {
          USART2_Sendstr("ERROR\r\n");
          CLR_Init();
        }
      }
      break;
    default:
      Pack_Recive_Timer_Update();
      if(rx_byte == '\r') {
        if(core.buf_counter >= sizeof(CLICmdSig[CLI_CMD_D].str)) {
          if(Get_CLI_CMD((CLICmd_t *)&core.CLICmd, 
                      (u8_t *)&core.buf[0], 
                      core.buf_counter) == 0){
            USART2_Sendstr("ERROR\r\n");
            CLR_Init();
            return 0;
          }

          //Команда
          switch(core.CLICmd.cmd) {
          case CLI_CMD_D://Команда вывести дамп
            i = 0;
            do
            {
              USART2_Transmit(Get_ASCII(0 >> 4));
              USART2_Transmit(Get_ASCII(0 & 0x0F));
              USART2_Transmit(Get_ASCII(i >> 4));
              USART2_Transmit(Get_ASCII(i & 0x0F));
              USART2_Transmit(':');
              USART2_Transmit(' ');
              j = 0;
              do
              {
                USART2_Transmit(Get_ASCII(VIRTUAL_EEPROM[i + j] >> 4));
                USART2_Transmit(Get_ASCII(VIRTUAL_EEPROM[i + j] & 0x0F));
                /*
                memset((u8_t*)&str[0],0,3);
                uitoa((u16_t)VIRTUAL_EEPROM[i + j], (u8_t*)&str[0]);
                USART2_Sendstr((u8_t*)&str[0]);
                */
                USART2_Transmit(' ');
                j++;
              }while(j < 8);
              USART2_Sendstr("\r\n");
              i += 8;
            }while(i < VIRTUAL_EEPROM_SIZE);
            break;
          case CLI_CMD_W:
            VIRTUAL_EEPROM[core.CLICmd.addr] = core.CLICmd.data;
            USART2_Sendstr("WRITE OK\r\n");
            break;
          case CLI_CMD_R:
            memset((u8_t*)&str[0],0,sizeof(str));
            uitoa((u16_t)VIRTUAL_EEPROM[core.CLICmd.addr], (u8_t*)&str[0]);
            USART2_Sendstr("READING RESULT: ");
            USART2_Sendstr((u8_t*)&str[0]);
            USART2_Sendstr("\r\n");
            break;
          case CLI_CMD_E:
            VIRTUAL_EEPROM[core.CLICmd.addr] = 0x00;
            USART2_Sendstr("ERASE OK..\r\n");
            break;
          default:
            USART2_Sendstr("ERROR\r\n");
            CLR_Init();
            return 0;
          }
          CLR_Init();
          return 1;
        }else{
          USART2_Sendstr("ERROR\r\n");
          CLR_Init();          
        }
      }else{
        core.buf[core.buf_counter++] = rx_byte;
        if(core.buf_counter >= CORE_BUF_SIZE) {
          USART2_Sendstr("ERROR\r\n");
          CLR_Init();
        }
      }
      break;
    }
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------------

