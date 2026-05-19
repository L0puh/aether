#include "compact_ed25519.h"
#include "public_key.h"
#include <aether.h>

bool verify_app_buffer(const u8* buff, u32 size, const u8* sign)
{
   if (compact_ed25519_verify(sign, APP_PUBLIC_KEY, buff, size)) {
      return true;
   }
   FLASHER_ERROR("signature invalid!\r\n");
   return false;
}
