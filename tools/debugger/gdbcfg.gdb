target extended-remote :3333
monitor reset halt

file build/aether-boot.elf
load
add-symbol-file build/main_app.elf 0x08002000

monitor reset halt

hbreak reset_handler
hbreak bootloader_entry 
hbreak setup_system 
hbreak scan_for_apps 


monitor arm semihosting disable
continue
