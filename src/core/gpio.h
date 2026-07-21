#ifndef GPIO_H
#define GPIO_H

#include "defs.h"

// for stm32f1
typedef struct _GPIO {
   volatile u32 CRL;          // config low
   volatile u32 CRH;          // config high
   volatile u32 IDR;          // input data 
   volatile u32 ODR;          // output data 
   volatile u32 BSRR;         // bit set/reset 
   volatile u32 BRR;          // bit reset 
   volatile u32 LCKR;         // lock 
} GPIO_t;

typedef struct _RCC {
   volatile u32 CR;            // clock control 
   volatile u32 CFGR;          // clock config
   volatile u32 CIR;           // clock interrupt
   volatile u32 APB2RSTR;      // apb2 peripheral reset 
   volatile u32 APB1RSTR;      // apb1 peripheral reset 
   volatile u32 AHBENR;        // ahb peripheral clock enable 
   volatile u32 APB2ENR;       // apb2 peripheral clock enable 
   volatile u32 APB1ENR;       // apb1 peripheral clock enable
   volatile u32 BDCR;          // backup domain control 
   volatile u32 CSR;           // control/status 
   volatile u32 AHBSTR;
   volatile u32 CFGR2;
} RCC_t;

typedef struct _USART {
   volatile u32 STATUS;
   volatile u32 DATA; 
   volatile u32 BAUD;
   volatile u32 CR1;    // control 
   volatile u32 CR2;
   volatile u32 CR3;
   volatile u32 GUARD;  // guard time and prescaler 
} USART_t;

typedef struct _FLASH {
   volatile u32 ACR;
   volatile u32 KEYR;
   volatile u32 OPTKEYR;
   volatile u32 SR;
   volatile u32 CR;
   volatile u32 AR;
   volatile u32 RESERVED;
   volatile u32 OBR;
   volatile u32 WRPR;
} FLASH_t;

typedef struct _SCB {
   volatile u32 CPUID;    
   volatile u32 ICSR;     
   volatile u32 VTOR;     
   volatile u32 AIRCR;    
   volatile u32 SCR;      
   volatile u32 CCR;      
   volatile u32 SHPR[3];  
   volatile u32 SHCSR;   
   volatile u32 CFSR;     
   volatile u32 HFSR;     
   volatile u32 DFSR;     
   volatile u32 MMFAR;     
   volatile u32 BFAR;       
   volatile u32 AFSR;       
} SCB_t;

typedef struct _MPU {
   volatile u32 TYPE;
   volatile u32 CTRL;
   volatile u32 REGNUM;
   volatile u32 REGBASE;
   volatile u32 ATTR_AND_SIZE;
} MPU_t;


typedef struct _IWDG {
   volatile u32 KEY_REG;
   volatile u32 PRESCALER;
   volatile u32 RELOAD;
   volatile u32 STATUS;
} IWDG_t;


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

#define SCB_BASE          (SCS_BASE + 0x0D00)
#define MPU_BASE          (SCS_BASE + 0x0D90)

#define SCB               ((SCB_t*) SCB_BASE)
#define MPU               ((MPU_t*) MPU_BASE)

#define IWDG_BASE         (PERIPH_BASE + 0x3000UL)
#define RCC_BASE          (PERIPH_BASE + 0x21000)
#define FLASH_BASE        (PERIPH_BASE + 0x22000)
#define GPIOA_BASE        (PERIPH_BASE + 0x10800)
#define GPIOB_BASE        (PERIPH_BASE + 0x10C00)
#define GPIOC_BASE        (PERIPH_BASE + 0x11000)
#define USART1_BASE       (PERIPH_BASE + 0x13800) 
#define USART2_BASE       (PERIPH_BASE + 0x04400)
#define USART3_BASE       (PERIPH_BASE + 0x04800)

#define RCC               ((RCC_t*)   RCC_BASE) 
#define GPIOA             ((GPIO_t*)  GPIOA_BASE)
#define GPIOB             ((GPIO_t*)  GPIOB_BASE)
#define GPIOC             ((GPIO_t*)  GPIOC_BASE)
#define USART1            ((USART_t*) USART1_BASE)
#define USART2            ((USART_t*) USART2_BASE)
#define USART3            ((USART_t*) USART3_BASE)
#define FLASH             ((FLASH_t*) FLASH_BASE) 
#define IWDG              ((IWDG_t*)  IWDG_BASE)

#define RCC_GPIOAEN           (1U << 2)
#define RCC_GPIOBEN           (1U << 3)
#define RCC_GPIOCEN           (1U << 4)
#define RCC_USART1EN          (1U << 14)
#define RCC_USART2EN          (1U << 17)
#define RCC_USART3EN          (1U << 18)


/* inputs */
#define INPUT_ANALOG        0x0
#define INPUT_FLOATING      0x4
#define INPUT_PULL          0x8   

/* 
* general-purpose outputs 
* PP - push-pull  
* OD - open drain 
*/ 

#define OUTPUT_PP_10MHz     0x1
#define OUTPUT_PP_2MHz      0x2
#define OUTPUT_PP_50MHz     0x3
#define OUTPUT_OD_10MHz     0x5
#define OUTPUT_OD_2MHz      0x6
#define OUTPUT_OD_50MHz     0x7

/* alternate-function outputs */
#define AF_PUSH_PULL_10MHz  0x9
#define AF_PUSH_PULL_2MHz   0xA
#define AF_PUSH_PULL_50MHz  0xB
#define AF_OPEN_DRAIN_10MHz 0xD
#define AF_OPEN_DRAIN_2MHz  0xE
#define AF_OPEN_DRAIN_50MHz 0xF


// FLASH
#define PREFETCH_STATUS_ON  (1U << 5)
#define PREFETCH_STATUS_OFF (0U << 5)
#define PREFETCH_ENABLE     (1U << 4)
#define PREFETCH_DISABLE    (0U << 4)
#define HALF_CYCLE_ACCESS_ENABLE  (1U << 3)
#define HALF_CYCLE_ACCESS_DISABLE (0U << 3)

#define CONTROL_SPSEL (1 << 0) // 0 = privileged, 1 = unprivileged
#define CONTROL_nPRIV (1 << 1) // 0 = MSP, 1 = PSP

void enable_irq(void);
void disable_irq(void);
void data_sync_barrier(void);
void instr_sync_barrier(void);
void cpu_wait_for_interrupt();
void wait_interrupt();
void system_reset(void);
u32 get_msp(void);
u32 get_psp(void);
void gpio_set_mode(GPIO_t* port, u8 pin, u32 mode);

void set_control(u32 control);
u32 get_control(void);
void set_psp(u32 psp);
void set_msp(u32 msp);

#endif 
