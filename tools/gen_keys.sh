#!/bin/bash
#
# generates the pair of private and public key for 
# signing app to flash in device.
# private key must be kept secrect on dev machine
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/base.sh"

SECRET_DIR="$SCRIPT_DIR/../.secrets"
PRIVATE_KEY="$SECRET_DIR/private.pem"
PUBLIC_KEY="$SECRET_DIR/public.pem"
PUBLIC_HEADER="$SCRIPT_DIR/../include/public_key.h"

command -v openssl >/dev/null 2>&1 || error "openssl not found"
success "openssl found"

mkdir -p "$SECRET_DIR"

if [ -f "$PRIVATE_KEY" ]; then
    warn "keys already exist, skipping"
else
    info "generating Ed25519 keypair..."
    openssl genpkey -algorithm ED25519 -out "$PRIVATE_KEY" 2>/dev/null
    openssl pkey -in "$PRIVATE_KEY" -pubout -out "$PUBLIC_KEY" 2>/dev/null
    success "keys generated"
fi

info "generating public_key.h..."

PUBLIC_BYTES=$(openssl pkey -in "$PUBLIC_KEY" -pubin -outform DER 2>/dev/null | tail -c 32 | xxd -i)

cat > "$PUBLIC_HEADER" <<EOF
#ifndef PUBLIC_KEY_H
#define PUBLIC_KEY_H
#include <stdint.h>
static const uint8_t APP_PUBLIC_KEY[32] = {
$PUBLIC_BYTES
};
#endif
EOF

success "public_key.h generated"


echo ""
say "private key: $PRIVATE_KEY"
say "public header: $PUBLIC_HEADER"
