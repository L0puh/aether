#ifndef GPIO_H
#define GPIO_H

#include "defs.h"

// for stm32f1
typedef struct _GPIO {
   u32 CRL;          // configuration register low 
   u32 CRH;          // configuration register high
   u32 IDR;          // input data register                     
   u32 ODR;          // output data register                    
   u32 BSRR;         // bit set/reset register                  
   u32 BRR;          // bit reset register                      
   u32 LCKR;         // lock register                           
} GPIO_t;

typedef struct _RCC {
   u32 CR;            // clock control register           
   u32 CFGR;          // clock configuration register     
   u32 CIR;           // clock interrupt register         
   u32 APB2RSTR;      // apb2 peripheral reset register   
   u32 APB1RSTR;      // apb1 peripheral reset register   
   u32 AHBENR;        // ahb peripheral clock enable reg  
   u32 APB2ENR;       // apb2 peripheral clock enable reg 
   u32 APB1ENR;       // apb1 peripheral clock enable reg 
   u32 BDCR;          // backup domain control register   
   u32 CSR;           // control/status register          
} RCC_t;


#define PERIPH_BASE     0x40000000UL


#define RCC_BASE          PERIPH_BASE + 0x21000

#define GPIOA_BASE        PERIPH_BASE + 0x10800
#define GPIOB_BASE        PERIPH_BASE + 0x10C00
#define GPIOC_BASE        PERIPH_BASE + 0x11000

#define RCC               ((RCC_t*)RCC_BASE) 
#define GPIOA             ((GPIO_t*) GPIOA_BASE)
#define GPIOB             ((GPIO_t*) GPIOB_BASE)
#define GPIOC             ((GPIO_t*) GPIOC_BASE)



#endif 
