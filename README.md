
![Status](https://img.shields.io/badge/status-WIP-yellow) 

<img width="1280" height="640" alt="2" src="https://github.com/user-attachments/assets/fa52e156-32a5-4e40-ae09-c55f461acc27" />

# aether

A bare-metal Type-1 hypervisor for STM32F103C6T6 (Cortex-M3) that provides MPU-based task isolation without an MMU.

## overview

classical hypervisors rely on mmu-based virtual memory, which cortex-m microcontrollers simply don't have. **aether** uses only the mpu as the hardware isolation boundary, fitting into the tight resource constraints of a blue pill board. current target is STM32F103C6T6 (Blue Pill)

## architecture

The system is composed of three layers:

- **bootloader** — privileged core that initializes the mpu, sets up the hypercall table, and launches application slots.
- **hypervisor api** — a fixed-address function pointer table through which unprivileged applications access peripherals in a controlled way.
- **applications** — user modules described by an `app_desc_t` descriptor (magic signature + entry point), loaded into dedicated flash slots.

on startup, the bootloader scans flash slots sequentially. for each valid module it reconfigures the mpu to that module's memory map, transfers control to the entry point, and resumes scanning after the application returns.

## isolation model

mpu regions are configured per-application before each launch:

| access | allowed |
|--------|---------|
| own flash slot | read / execute |
| own ram region | read / write |
| hv api table | read / execute |
| bootloader memory | ✗ |
| other app slots | ✗ |
| critical peripherals | ✗ |

after an application returns, the mpu is placed into background mode, blocking all unprivileged accesses.
every hv-call implementation validates that the call originates from unprivileged thread mode (not handler mode), preventing privilege escalation.

## building

```sh
make
```

no external os or libraries required. the only dependency is a ed25519 implementation included as a submodule.

## license

MIT
