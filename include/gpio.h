#ifndef GPIO_H
#define GPIO_H

#include <aether.h>

// for stm32f1
typedef struct _GPIO {
   volatile u32 CRL;          // configuration register low 
   volatile u32 CRH;          // configuration register high
   volatile u32 IDR;          // input data register                     
   volatile u32 ODR;          // output data register                    
   volatile u32 BSRR;         // bit set/reset register                  
   volatile u32 BRR;          // bit reset register                      
   volatile u32 LCKR;         // lock register                           
} GPIO_t;

typedef struct _RCC {
   volatile u32 CR;            // clock control register           
   volatile u32 CFGR;          // clock configuration register     
   volatile u32 CIR;           // clock interrupt register         
   volatile u32 APB2RSTR;      // apb2 peripheral reset register   
   volatile u32 APB1RSTR;      // apb1 peripheral reset register   
   volatile u32 AHBENR;        // ahb peripheral clock enable reg  
   volatile u32 APB2ENR;       // apb2 peripheral clock enable reg 
   volatile u32 APB1ENR;       // apb1 peripheral clock enable reg 
   volatile u32 BDCR;          // backup domain control register   
   volatile u32 CSR;           // control/status register          
} RCC_t;

#define PERIPH_BASE  0x40000000UL

//stm32f1xx
#define RCC_BASE          PERIPH_BASE + 0x21000

#define GPIOA_BASE        PERIPH_BASE + 0x10800
#define GPIOB_BASE        PERIPH_BASE + 0x10C00
#define GPIOC_BASE        PERIPH_BASE + 0x11000

#define RCC               ((RCC_t*)RCC_BASE) 
#define GPIOA             ((GPIO_t*) GPIOA_BASE)
#define GPIOB             ((GPIO_t*) GPIOB_BASE)
#define GPIOC             ((GPIO_t*) GPIOC_BASE)

#endif 
