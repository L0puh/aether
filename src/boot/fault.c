#include <aether.h>

static volatile bool already_faulting_g = false;

static bool is_pc_from_app(u32 pc)
{
   u32 addr = pc & ~1u;
   return addr >= FLASH_APP_ORIGIN && addr < (FLASH_APP_ORIGIN + FLASH_APP_LENGTH);
}

static u32 process_fault(frame_t *frame, u32 exc_ret, bool from_psp, bool from_app)
{
   if (already_faulting_g){
      systick_reset();
   }

   already_faulting_g = true;

   if (from_psp && from_app)
   {
      frame->pc = (u32) exit_landing | 1;
      frame->r0 = (u32) -1;
      
      u32 ctrl = get_control();
      ctrl &= ~1u;
      set_control(ctrl);

      already_faulting_g = false;
      return exc_ret;
   }

   system_reset();

   __builtin_unreachable();
}

void enable_fault_handlers(void)
{
   SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk |
         SCB_SHCSR_BUSFAULTENA_Msk |
         SCB_SHCSR_USGFAULTENA_Msk);

   SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;     // trap divide by zero
   SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;   // trap unaligned access
}

u32 busfault_handler_c(frame_t *frame, u32 exc_ret) 
{
    u32 cfsr  = SCB->CFSR;
    u32 bfar  = SCB->BFAR;
    bool bfar_valid = (cfsr & (1 << 15)) != 0;  
    bool from_psp = (exc_ret & 0x4) != 0;
    bool from_app = is_pc_from_app(frame->pc);

    PLAIN_PRINT("[FAULT] bus fault: pc=0x%x bfar=0x%x valid=%d ", frame->pc, bfar_valid ? bfar : 0, bfar_valid);

    u32 bfsr = (cfsr >> 8) & 0xFF; 

    switch(bfsr) {
        case 0x01: { PLAIN_PRINT("-> instruction access violation at 0x%x\r\n", bfar); break; }
        case 0x02: { PLAIN_PRINT("-> data access violation at 0x%x\r\n", bfar); break; }
        case 0x04: { PLAIN_PRINT("-> unstacking error (BFAR may be invalid)\r\n"); break; }
        case 0x08: { PLAIN_PRINT("-> stacking error (BFAR may be invalid)\r\n"); break; }
        case 0x10: { PLAIN_PRINT("-> precise data access error at 0x%x\r\n", bfar); break; }
        case 0x20: { PLAIN_PRINT("-> imprecise data access error (BFAR invalid)\r\n"); break; }
        case 0x03: { PLAIN_PRINT("-> both instruction and data access violation\r\n"); break; }
        default:   { PLAIN_PRINT("-> unknown fault: 0x%02X\r\n", bfsr); break; }
    }

    SCB->CFSR = (bfsr << 8); 
    return process_fault(frame, exc_ret, from_psp, from_app);
}

u32 memmanage_handler_c(frame_t *frame, u32 exc_ret) 
{
   u32 cfsr  = SCB->CFSR;
   u32 mmfar = SCB->MMFAR;
   bool mmar_valid = (cfsr & (1 << 7)) != 0;
   bool from_psp = (exc_ret & 0x4) != 0;
   bool from_app = is_pc_from_app(frame->pc);


   PLAIN_PRINT("[FAULT] mem manage: pc=0x%x mmfar=0x%x valid=%d ", frame->pc, mmar_valid ? mmfar : 0, mmar_valid);

   u32 mmfsr = cfsr & 0xFF;

   switch(mmfsr) {
        case 0x01: { PLAIN_PRINT("-> instruction access violation at 0x%x\r\n", mmfar); break; }
        case 0x02: { PLAIN_PRINT("-> data access violation at 0x%x\r\n", mmfar);        break; }
        case 0x08: { PLAIN_PRINT("-> unstacking error\r\n");                            break; }
        case 0x10: { PLAIN_PRINT("-> stacking error\r\n");                              break; }
        case 0x03: { PLAIN_PRINT("-> both instruction and data access violation\r\n");  break; }
        case 0x88: { PLAIN_PRINT("-> unstacking error (MMFAR may be invalid)\r\n");     break; }
        default:   { PLAIN_PRINT("-> unknown fault: 0x%02X\r\n", mmfar);                break; }
    }

   SCB->CFSR = mmfsr;;
   return process_fault(frame, exc_ret, from_psp, from_app);
}

u32 hardfault_handler_c(frame_t *frame, u32 exc_ret)
{
    bool from_psp = (exc_ret & 0x4) != 0;
    bool from_app = is_pc_from_app(frame->pc);

    u32 cfsr = SCB->CFSR;
    u32 hfsr = SCB->HFSR;

    PLAIN_PRINT("[FAULT] hard fault: pc=0x%x hfsr=0x%x ", frame->pc, hfsr);

    if (hfsr & (1 << 1)) {  
        u32 bfsr = (cfsr >> 8) & 0xFF;
        PLAIN_PRINT("(escalated from bus fault: 0x%02x) ", bfsr);
    }

    if (hfsr & (1 << 4)) {  
        u32 mmfsr = cfsr & 0xFF;
        PLAIN_PRINT("(escalated from memmanage fault: 0x%02x) ", mmfsr);
    }

    if (hfsr & (1 << 3)) {  
        u32 ufsr = (cfsr >> 16) & 0xFF;
        PLAIN_PRINT("(escalated from usage fault: 0x%02x) ", ufsr);
    }

    if (hfsr & (1 << 0)) {  
        PLAIN_PRINT("(vector table read fault) ");
    }

    PLAIN_PRINT("\r\n");

    SCB->HFSR = hfsr;

    return process_fault(frame, exc_ret, from_psp, from_app);
}

u32 usagefault_handler_c(frame_t *frame, u32 exc_ret)
{
    u32 cfsr = SCB->CFSR;
    bool from_psp = (exc_ret & 0x4) != 0;
    bool from_app = is_pc_from_app(frame->pc);

    PLAIN_PRINT("[FAULT] usage fault: pc=0x%x ", frame->pc);

    u32 ufsr = (cfsr >> 16) & 0xFF;  

    if (ufsr & 0x01) PLAIN_PRINT("-> invalid state (UNDEFINSTR) ");
    if (ufsr & 0x02) PLAIN_PRINT("-> invalid state (INVSTATE) ");
    if (ufsr & 0x04) PLAIN_PRINT("-> invalid PC (INVPC) ");
    if (ufsr & 0x08) PLAIN_PRINT("-> no coprocessor (NOCP) ");
    if (ufsr & 0x10) PLAIN_PRINT("-> unaligned access (UNALIGNED) ");
    if (ufsr & 0x20) PLAIN_PRINT("-> divide by zero (DIVBYZERO) ");

    if (ufsr == 0) {
        PLAIN_PRINT("-> unknown usage fault: 0x%02X", ufsr);
    }

    PLAIN_PRINT("\r\n");

    SCB->CFSR = (ufsr << 16);

    return process_fault(frame, exc_ret, from_psp, from_app);
}
