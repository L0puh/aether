#include <aether.h>

static volatile u32 last_livness_tick_g = 0;
volatile bool iwdg_enabled_g = false;

void watchdog_init(u32 timeout)
{
   IWDG->KEY_REG = IWDG_KEY_UNLOCK;
   IWDG->PRESCALER = 4; //~625Hz tick
   IWDG->RELOAD = (timeout * 625) / 1000;
   while (!IWDG->STATUS) {}
   IWDG->KEY_REG = IWDG_KEY_REFRESH;
   IWDG->KEY_REG = IWDG_KEY_ENABLE;
   iwdg_enabled_g = true;
   last_livness_tick_g = get_system_ticks();
}

void watchdog_kick(void)
{
   IWDG->KEY_REG = IWDG_KEY_REFRESH;
}

void mark_app_alive(void)
{
   DEBUG_PRINT("mark app alive from %d ticks (%d)", get_system_ticks() - last_livness_tick_g, last_livness_tick_g);
   last_livness_tick_g = get_system_ticks();
}

bool is_iwdg_enabled(void){
   return iwdg_enabled_g;
}

bool is_app_healthy(void)
{
   u32 cur = get_system_ticks();
   u32 last = last_livness_tick_g;
   return (cur - last) < APP_LIVENESS_TIMEOUT_MS;
}
