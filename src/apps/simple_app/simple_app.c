#include <aether.h>


void app_main(void);

__attribute__((section(".app_desc"), used))
const app_desc_t app_desc =
{
   .magic  = APP_MAGIC,
   .version = 0x0100,
   .size = 0            
};

void app_main(void)
{
   //TODO
}
