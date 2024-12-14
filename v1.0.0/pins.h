//pins.h

#if !defined( PINS_H )
#define       PINS_H
//-----------------------------------------------------------------------------

//SPI1
#define SPI1_SCK_PIN                  GPIO_Pin_5
#define SPI1_SCK_GPIO_PORT            GPIOA
#define SPI1_MISO_PIN                 GPIO_Pin_6
#define SPI1_MISO_GPIO_PORT           GPIOA
#define SPI1_MOSI_PIN                 GPIO_Pin_7
#define SPI1_MOSI_GPIO_PORT           GPIOA

//SYS_LED
#define SYS_LED_PIN                   GPIO_Pin_13
#define SYS_LED_GPIO_PORT             GPIOC

//SOUND
#define SOUND_PIN                     GPIO_Pin_0
#define SOUND_GPIO_PORT               GPIOA

//USART1
#define USART1_TX_PIN                 GPIO_Pin_9
#define USART1_RX_PIN                 GPIO_Pin_10
#define USART1_GPIO_PORT              GPIOA
#define USART1_CLK                    RCC_APB2Periph_USART1

//USART2
#define USART2_TX_PIN                 GPIO_Pin_2  //12
#define USART2_RX_PIN                 GPIO_Pin_3  //13
#define USART2_GPIO_PORT              GPIOA
#define USART2_CLK                    RCC_APB1Periph_USART2

//USART3
#define USART3_TX_PIN                 GPIO_Pin_10  //21
#define USART3_RX_PIN                 GPIO_Pin_11  //22
#define USART3_GPIO_PORT              GPIOB
#define USART3_CLK                    RCC_APB1Periph_USART3

//GL_BUS_DIR
#define GL_BUS_DIR_PIN                GPIO_Pin_8
#define GL_BUS_DIR_GPIO_PORT          GPIOA

#define USBDM_PIN                     GPIO_Pin_11
#define USBDM_GPIO_PORT               GPIOA
#define USBDP_PIN                     GPIO_Pin_12
#define USBDP_GPIO_PORT               GPIOA

/*
#define USB_DISCONNECT                GPIOB
#define USB_DISCONNECT_PIN            GPIO_Pin_12
#define RCC_APB2Periph_GPIO_DISCONNECT RCC_APB2Periph_GPIOB

#define USBDM_PIN                     GPIO_Pin_11
#define USBDM_GPIO_PORT               GPIOA
#define USBDM_GPIO_CLK                RCC_APB2Periph_GPIOA

#define USBDP_PIN                     GPIO_Pin_12
#define USBDP_GPIO_PORT               GPIOA
#define USBDP_GPIO_CLK                RCC_APB2Periph_GPIOA
#define USBDP_ON()                    {GPIOA->CRH &= ~(GPIO_CRL_CNF0_0 | GPIO_CRL_CNF0_1); GPIOA->CRH |= GPIO_CRL_CNF0_1;}
#define USBDP_OFF()                   {GPIOA->CRH &= ~(GPIO_CRL_CNF0_0 | GPIO_CRL_CNF0_1); GPIOA->CRH |= GPIO_CRL_CNF0_0;}
#define USBDP_State()                 (GPIOA->CRH & (GPIO_CRL_CNF0_1))
*/

//-----------------------------------------------------------------------------
#endif

