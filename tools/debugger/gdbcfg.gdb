target extended-remote :3333
monitor reset halt

file build/aether-boot.elf
load
file build/aether-app.elf
load

symbol-file build/aether-boot.elf
add-symbol-file build/aether-app.elf 0x08004000

monitor reset halt
break reset_handler
layout asm
continue
