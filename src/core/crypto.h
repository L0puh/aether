#ifndef CRYPTO_H
#define CRYPTO_H

#include "defs.h"
#include <stdbool.h>

//auto-generated public key with tools/gen_keys.sh
#include <public_key.h> 

bool verify_app_buffer(const u8* buff, u32 size, const u8* sign);

#endif
