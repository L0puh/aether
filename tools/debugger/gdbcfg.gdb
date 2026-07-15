target extended-remote :3333

file build/aether-boot.elf
load
add-symbol-file build/apps/simple_app.elf 0x08002000

monitor reset halt

#break bootloader_entry 
break run_app
break main 

tui enable
layout split 
display /x $pc
display $r0
display $r1
define hook-stop
    refresh
end


monitor arm semihosting disable
continue
