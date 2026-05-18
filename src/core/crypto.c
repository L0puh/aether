#include <aether.h>

bool verify_app_buffer(const u8* buff, u32 size, const u8* sign)
{

   //TODO: ED25519 verification
   FLASHER_ERROR("signature invalid!\r\n");
   return false;
}
