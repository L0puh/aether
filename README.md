
![Status](https://img.shields.io/badge/Status-WIP-yellow) [![Build](https://github.com/L0puh/aether/actions/workflows/makefile.yml/badge.svg)](https://github.com/L0puh/aether/actions/workflows/makefile.yml) [![MIT License](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

<img width="1280" height="640" alt="2" src="https://github.com/user-attachments/assets/fa52e156-32a5-4e40-ae09-c55f461acc27" />

# aether

minimal bootloader with bare metal hypervisor on blue pill.

## overview

it's a showcase of security model which is based on MPU & SVC with
modular architecture, where app is embedded into bootloader and lacks its
own vector table.

bootloader receives calls, ensures a loaded
app follows its manifest, handles interrupts and behaves 
as a supervisor for the app. 

## architecture
### MPU

memory layout is based on MPU, bootloader & app have separate flash and ram
regions. MPU configuration is as follows:
```c
[REG_NULL_GUARD] = {
  .base = 0x00000000,
  .attr_size = REGION_SIZE_256B | AP_PRIV_RO | XN_ENABLE | REGION_ENABLE,
  .subreg_mask = 0,
},
[REG_HV_FLASH] = {
  .base = FLASH_HV_ORIGIN,
  .attr_size = REGION_SIZE_32KB | AP_PRIV_RO | XN_DISABLE | REGION_ENABLE,
  .subreg_mask = 0,
},
[REG_HV_RAM] = {
  .base = RAM_HV_ORIGIN,
  .attr_size = REGION_SIZE_8KB | AP_PRIV_RW | XN_ENABLE | REGION_ENABLE,
  .subreg_mask = 0,
},
[REG_APP_FLASH] = {
  .base = FLASH_APP_ORIGIN,
  .attr_size = REGION_SIZE_32KB | AP_PRIV_RW_USER_RO | XN_DISABLE | REGION_ENABLE,
  .subreg_mask = 0,
},
[REG_APP_RAM] = {
  .base = RAM_APP_ORIGIN,
  .attr_size = REGION_SIZE_4KB | AP_PRIV_RW_USER_RW | XN_DISABLE | REGION_ENABLE,
  .subreg_mask = 0,
},
[REG_RAM_GUARD] = {
  .base = RAM_GUARD_ORIGIN,
  .attr_size = REGION_SIZE_1KB | AP_NO_ACCESS | XN_ENABLE | REGION_ENABLE,
  .subreg_mask = 0,
},
```
it ensures that critical code is either non-accessible for an app or read
only. it also has protection against stack overflow.

### SVC API
```c
extern int32_t hv_request_periph(u32 id, u32 perms);
extern int32_t hv_wdt_kick(void);
extern void    hv_exit(int32_t code) __attribute__((noreturn));
```
request peripheral call is based on preinitialized peripherals (in bootloader),
specified in manifest. manifest itself is patched in build
(with patch tool) and non-accessible for an app in runtime.

### model
bootloader uses MSP and the highest control:
- initializes system, MPU & 
peripherals
- scans for app
- ensures the integrity (CRC)
- handles interrupts
- handles watchdog, timers, SVC

when app is ready to run, the transition code takes place that drops
privileges, changes to PSP stack and hops to app.

this happens in two stages, `enter_app` goes with bootloader
```asm
enter_app:
    @ r0 = psp value
    @ r1 = entry point

    push  {r4, lr}
    mov   r4, r1

    msr   psp, r0
    isb

    mov   r3, #0
    msr   primask, r3
    msr   basepri, r3
    cpsie i

    mrs   r3, control
    orr   r3, r3, #2
    msr   control, r3
    isb

    bx    r4
```
and `app_start` is embedded into app's flash 
```asm
app_start:
    mrs   r0, control
    orr   r0, r0, #1
    msr   control, r0
    isb

    bl    main

    movs  r0, #0
    svc   #2
    b     .
```
on exit, privileges are restored
```asm
exit_landing:
    mrs   r0, control
    bic.w r0, r0, #2
    msr   control, r0
    isb
    bl    bootloader_exit_hook
    b     .

```

### memory map
```c

FLASH_HV_ORIGIN   0x08000000UL
FLASH_HV_LENGTH   0x8000UL      /* 32K */ 

FLASH_APP_ORIGIN  0x08008000UL
FLASH_APP_LENGTH  0x8000UL      /* 32K */

APP_DESC_OFFSET   0x20UL        /* 32B */

RAM_HV_ORIGIN     0x20000000UL
RAM_HV_LENGTH     0x2000UL      /* 8K */

RAM_GUARD_ORIGIN  0x20002000UL
RAM_GUARD_LENGTH  0x400UL       /* 1K */ 

RAM_APP_ORIGIN    0x20004000UL
RAM_APP_LENGTH    0x1000UL      /* 4K */ 
```
## building

build everything with make all, which includes the current module (`modules.mk`, in `CURRENT_MODULE`) and core
library. 

there are also the following commands:
```bash
make flash   
make erase   
make clean   
make debug   
make modules
make list-modules
make dump-app-%     
make dump-boot
make patch-%        # to patch app with manifest, CRC and etc
make open-serial    # opens minicom with USB0
```

### how to run
```bash
make all 
make flash # flash bootloader
make reset && python tools/flash.py build/apps/{app}.bin.patched /dev/USB{X} {BAUDRATE}
```


# related 
- [experiments with isolation on blue pill](https://l0puh.github.io/log/2026/08/04/experiments_with_isolation/)
