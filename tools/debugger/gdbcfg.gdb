target extended-remote :3333
monitor reset halt

file build/aether-boot.elf
load

add-symbol-file build/aether-app.elf 0x08008000

monitor reset halt

hbreak reset_handler
hbreak main

monitor arm semihosting disable
continue
