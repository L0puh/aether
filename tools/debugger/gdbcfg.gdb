target extended-remote :3333

monitor reset halt
monitor flash erase_address 0x08000000 0x20000

file build/aether-app.elf
load

file build/aether-boot.elf
load

monitor reset run
