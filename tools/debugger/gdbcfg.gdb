target extended-remote :3333

monitor reset halt

#### FILES:
file build/aether-boot.elf
load

file build/aether-app.elf
load

add-symbol-file build/aether-app.elf 0x08008000

monitor reset halt


#### BREAKS:
hbreak reset_handler
hbreak main

monitor arm semihosting disable
continue
layout asm
