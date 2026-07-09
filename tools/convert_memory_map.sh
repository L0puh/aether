#!/bin/bash
# convert memory_map.h to memory_map.ld
#
echo "/* auto-generated from memory_map.h */"
echo ""


grep -E "^#define[[:space:]]+[A-Z_][A-Z0-9_]*[[:space:]]+" "$1" | while read -r line; do
    name=$(echo "$line" | awk '{print $2}')
    [ "$name" = "MEMORY_MAP_H" ] && continue

    value=$(echo "$line" | sed -E "s/^#define[[:space:]]+${name}[[:space:]]+//")
    value=$(echo "$value" | sed -E 's/([0-9])[UuLl]+([^0-9A-Za-z]|$)/\1\2/g; s/\/\*.*\*\///g; s/[[:space:]]*$//')

    echo "$name = $value;"
done
