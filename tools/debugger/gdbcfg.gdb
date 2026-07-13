target extended-remote :3333
monitor reset halt

file build/aether-boot.elf
load
add-symbol-file build/apps/simple_app/simple_app.elf 0x08002000

monitor reset halt

hbreak bootloader_entry 
hbreak main 


monitor arm semihosting disable
continue
