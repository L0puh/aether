#!/bin/bash

# convert memory_map.h to memory_map.ld

echo "/* auto-generated from memory_map.h */"
echo ""

grep "^#define[[:space:]]\+[A-Z_][A-Z0-9_]*[[:space:]]\+" "$1" | while read -r line; do
    name=$(echo "$line" | awk '{print $2}')
    value=$(echo "$line" | awk '{print $3}')

    value=$(echo "$value" | sed 's/UL$//; s/\/\*.*\*\///g; s/[[:space:]]*$//')

    [ "$name" = "MEMORY_MAP_H" ] && continue

    echo "$name = $value;"
done
