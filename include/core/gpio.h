#ifndef GPIO_H
#define GPIO_H

#include "defs.h"

// for stm32f1
typedef struct _GPIO {
   u32 CRL;          // config low
   u32 CRH;          // config high
   u32 IDR;          // input data 
   u32 ODR;          // output data 
   u32 BSRR;         // bit set/reset 
   u32 BRR;          // bit reset 
   u32 LCKR;         // lock 
} GPIO_t;

typedef struct _RCC {
   u32 CR;            // clock control 
   u32 CFGR;          // clock config
   u32 CIR;           // clock interrupt
   u32 APB2RSTR;      // apb2 peripheral reset 
   u32 APB1RSTR;      // apb1 peripheral reset 
   u32 AHBENR;        // ahb peripheral clock enable 
   u32 APB2ENR;       // apb2 peripheral clock enable 
   u32 APB1ENR;       // apb1 peripheral clock enable
   u32 BDCR;          // backup domain control 
   u32 CSR;           // control/status 
} RCC_t;

typedef struct _USART {
   u32 STATUS;
   u32 DATA; 
   u32 BAUD;
   u32 CR1;    // control 
   u32 CR2;
   u32 CR3;
   u32 GUARD;  // guard time and prescaler 
} USART_t;

typedef struct _FLASH {
   u32 ACR;
   u32 KEYR;
   u32 OPTKEYR;
   u32 SR;
   u32 CR;
   u32 AR;
   u32 RESERVED;
   u32 OBR;
   u32 WRPR;
} FLASH_t;

typedef struct _SCB {
    u32 CPUID;    
    u32 ICSR;     
    u32 VTOR;     
    u32 AIRCR;    
    u32 SCR;      
    u32 CCR;      
    u32 SHPR[3];  
    u32 SHCSR;   
    u32 CFSR;     
    u32 HFSR;     
    u32 DFSR;     
    u32 MMFAR;     
    u32 BFAR;       
    u32 AFSR;       
} SCB_t;

#define PIN0            (u8) 0x0000
#define PIN1            (u8) 0x0001
#define PIN2            (u8) 0x0002
#define PIN3            (u8) 0x0003
#define PIN4            (u8) 0x0004
#define PIN5            (u8) 0x0005
#define PIN6            (u8) 0x0006
#define PIN7            (u8) 0x0007
#define PIN8            (u8) 0x0008
#define PIN9            (u8) 0x0009
#define PIN10           (u8) 0x000A
#define PIN11           (u8) 0x000B
#define PIN12           (u8) 0x000C
#define PIN13           (u8) 0x000D
#define PIN14           (u8) 0x000E
#define PIN15           (u8) 0x000F

#define PERIPH_BASE       0x40000000UL
#define SCS_BASE          0xE000E000UL

#define RCC_BASE          (PERIPH_BASE + 0x21000)
#define FLASH_BASE        (PERIPH_BASE + 0x22000)
#define GPIOA_BASE        (PERIPH_BASE + 0x10800)
#define GPIOB_BASE        (PERIPH_BASE + 0x10C00)
#define GPIOC_BASE        (PERIPH_BASE + 0x11000)
#define USART1_BASE       (PERIPH_BASE + 0x13800)
#define USART3_BASE       (PERIPH_BASE + 0x04800)
#define USART2_BASE       (PERIPH_BASE + 0x04400)

#define RCC               ((RCC_t*)   RCC_BASE) 
#define GPIOA             ((GPIO_t*)  GPIOA_BASE)
#define GPIOB             ((GPIO_t*)  GPIOB_BASE)
#define GPIOC             ((GPIO_t*)  GPIOC_BASE)
#define USART1            ((USART_t*) USART1_BASE)
#define USART2            ((USART_t*) USART2_BASE)
#define FLASH             ((FLASH_t*) FLASH_BASE) 

#define SCB_BASE          SCS_BASE + 0x0D00
#define SCB               ((SCB_t*) SCB_BASE)


void enable_irqq(void);
void disable_irq(void);

#endif 
