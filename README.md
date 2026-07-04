
![Status](https://img.shields.io/badge/status-WIP-yellow) 

<img width="1280" height="640" alt="2" src="https://github.com/user-attachments/assets/fa52e156-32a5-4e40-ae09-c55f461acc27" />

# aether

minimal bootloader with bare metal hypervisor on blue pill.

## overview

mostly it's a show case of security model. bootloader recieves calls, ensures a loaded
app foolows its manifest, handles interrupts and beheaves
as a supervisor for app. this includes MPU, stack protection, SVC, and etc.

## hypervisor api

SVC calls include:
- _region request_ to a peripheral/memory based on app's manifest 
- wdt kick 
- exit / fault 

this model provides scale for apps. UART, SPI,
GPIO and etc. happen as plain MMIO reads once `SVC_REGION_REQ` has granted
the region. 

## architecture

memory layout (assume stm32f103 - 64k flash and 20k RAM). 

1. hypervisor flash - 16K - protected
2. hypervisor's RAM - 4K - privileged 
3. GUARD (MPU) - 1 K - no access 
4. app flash - 48K 
5. app's RAM and stack 

## isolation model

## building

build everything with `make all`, that includes the modules (`modules.mk`) and core
library. 

there are also such commands:
```bash
make flash   # to flash bootloader (!)
make erase   # to erase the board
make clean   # to clean build folder
make debug   # run gdb 
make modules # build apps 

# various debugging stuff
make dump-boot
make sym-boot 
```

no external os or libraries required. the only *optional* dependency is a ed25519 implementation included as a submodule.

