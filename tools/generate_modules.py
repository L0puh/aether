#!/usr/bin/env python3

"""
dynamic build system generator aether 
generates makefile rules for modular compilation based on config.json
"""

import os
import sys
import json
import hashlib
from pathlib import Path

DEFAULT_CONFIG = "config.json"
OUTPUT_MAKEFILE = "modules.mk"

def load_config(config_path):
    try:
        with open(config_path, 'r') as f:
            config = json.load(f)
        
        required_fields = ["origin", "stack_size", "modules"]
        for field in required_fields:
            if field not in config:
                raise ValueError(f"missing required field: {field}")
        
        return config
    except Exception as e:
        print(f"error loading config: {e}")
        sys.exit(1)

def parse_memory_size(size_str):
    if not size_str:
        return 0
    
    size_str = size_str.strip().upper()
    if size_str.endswith("KB"):
        return int(size_str[:-2]) * 1024
    elif size_str.endswith("MB"):
        return int(size_str[:-2]) * 1024 * 1024
    elif size_str.endswith("B"):
        return int(size_str[:-1])
    else:
        return int(size_str)

def calculate_module_layout(config, total_memory):
    origin = int(config["origin"], 16)
    stack_size = parse_memory_size(config["stack_size"])
    ram_origin = int(config.get("ram_origin", "0x20000000"), 16)
    
    current_addr = origin
    modules = config["modules"]
    
    for mod in modules:
        rom_size = parse_memory_size(mod.get("rom", "0"))
        ram_size = parse_memory_size(mod.get("ram", "0"))
        
        mod["rom_start"] = current_addr
        mod["rom_size"] = rom_size
        current_addr += rom_size
        
        mod["ram_size"] = ram_size
    
    ram_current = ram_origin
    for mod in modules:
        mod["ram_start"] = ram_current
        ram_current += mod["ram_size"]
    
    total_rom = current_addr - origin
    total_ram = ram_current - ram_origin + stack_size
    
    return {
        "origin": origin,
        "ram_origin": ram_origin,
        "stack_size": stack_size,
        "total_rom": total_rom,
        "total_ram": total_ram,
        "used_memory": total_rom + total_ram,
        "modules": modules
    }


def generate_module_info_header(layout):
    lines = []

    lines.append('#ifndef MODULE_INFO_H')
    lines.append('#define MODULE_INFO_H')
    lines.append('')
    lines.append('#include <defs.h>')
    lines.append('#include <core/hv.h>')
    lines.append('#include <aether/memory.h>')
    lines.append('')

    lines.append(f'#define MODULES_COUNT {len(layout["modules"])}')
    lines.append('')

    lines.append('static module_t MODULES[MODULES_COUNT] = {')
    for idx, module in enumerate(layout["modules"]):
        name = module["name"]
        lines.append(f'    [{idx}] = {{')
        lines.append(f'        .id = {idx},')
        lines.append(f'        .desc = (app_desc_t*)&_{name}_rom_start,')
        lines.append(f'        .tag = "{name[:7]}",')
        lines.append(f'        .state = STATE_OFF,')
        lines.append(f'        .memory = {{')
        lines.append(f'            .flash_start = (u32)&_{name}_rom_start,')
        lines.append(f'            .flash_size = (u32)&_{name}_rom_size,')
        lines.append(f'            .ram_start = (u32)&_{name}_ram_start,')
        lines.append(f'            .ram_size = (u32)&_{name}_ram_size,')
        lines.append(f'            .periph_start = 0,')
        lines.append(f'            .periph_size = 0')
        lines.append(f'        }},')
        lines.append(f'        .context = {{0}},')
        lines.append(f'        .next_time = 0,')
        lines.append(f'        .period_time = {module.get("period", 0)}')
        lines.append(f'    }},')
    lines.append('};')
    lines.append('')
    lines.append('#endif')

    return '\n'.join(lines)

def generate_memory_map(layout):
    lines = []
    
    lines.append("MEMORY")
    lines.append("{")
    
    for module in layout["modules"]:
        name = module["name"].upper()
        lines.append(f"    {name}_ROM  (rx)  : ORIGIN = 0x{module['rom_start']:08X}, LENGTH = {module['rom_size']}")
        lines.append(f"    {name}_RAM  (rwx) : ORIGIN = 0x{module['ram_start']:08X}, LENGTH = {module['ram_size']}")
    
    lines.append("}")
    lines.append("")
    
    for module in layout["modules"]:
        name = module["name"]
        name_upper = name.upper()
        lines.append(f"_{name}_rom_start = ORIGIN({name_upper}_ROM);")
        lines.append(f"_{name}_rom_size  = LENGTH({name_upper}_ROM);")
        lines.append(f"_{name}_ram_start = ORIGIN({name_upper}_RAM);")
        lines.append(f"_{name}_ram_size  = LENGTH({name_upper}_RAM);")
        lines.append(f"_{name}_estack = ORIGIN({name_upper}_RAM) + LENGTH({name_upper}_RAM);")
        lines.append("")
    
    lines.append("STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x800;")
    
    return "\n".join(lines)

def generate_module_linker(module, layout):
    name = module["name"]
    name_upper = name.upper()
    lines = []

    lines.append("/*")
    lines.append(f" * Linker script for {name} module")
    lines.append(" * Auto-generated by generate_modules.py")
    lines.append(" */")
    lines.append("")
    lines.append("INCLUDE module_memory_map.ld")
    lines.append("")
    lines.append("SECTIONS")
    lines.append("{")
    lines.append("   .text :")
    lines.append("   {")
    lines.append("      . = ALIGN(4);")
    lines.append("      _stext = .;")
    lines.append("      KEEP(*(.app_desc))")
    lines.append("")
    lines.append("      *(.text*)")
    lines.append("      *(.rodata*)")
    lines.append("      . = ALIGN(4);")
    lines.append("      _etext = .;")
    lines.append(f"    }} > {name_upper}_ROM")
    lines.append("")
    lines.append("    .data : AT (_etext)")
    lines.append("    {")
    lines.append("      . = ALIGN(4);")
    lines.append("      _sdata = .;")
    lines.append("      *(.data*)")
    lines.append("      . = ALIGN(4);")
    lines.append("      _edata = .;")
    lines.append(f"    }} > {name_upper}_RAM")
    lines.append("")
    lines.append("    .bss (NOLOAD) :")
    lines.append("    {")
    lines.append("      . = ALIGN(4);")
    lines.append("      _sbss = .;")
    lines.append("      *(.bss*)")
    lines.append("      *(COMMON)")
    lines.append("      . = ALIGN(4);")
    lines.append("      _ebss = .;")
    lines.append(f"    }} > {name_upper}_RAM")
    lines.append("")
    lines.append("    .stack :")
    lines.append("    {")
    lines.append("       . = ALIGN(8);")
    lines.append("       _sstack = .;")
    lines.append("       . = . + STACK_SIZE;")
    lines.append("       . = ALIGN(8);")
    lines.append("       _estack = .;")
    lines.append(f"    }} > {name_upper}_RAM")
    lines.append("}")

    return "\n".join(lines)

def generate_module_makefile(layout):
    lines = []

    lines.append("#" + "=" * 70)
    lines.append("# AUTO-GENERATED MODULE BUILD RULES")
    lines.append("# Generated by generate_modules.py - DO NOT EDIT MANUALLY")
    lines.append("#" + "=" * 70)
    lines.append("")

    lines.append("#----------------------- MODULE CONFIGURATION -----------------------#")
    lines.append("")

    module_names = [m["name"] for m in layout["modules"]]
    lines.append(f"MODULES = {' '.join(module_names)}")
    lines.append(f"MODULE_DIR = src/modules")
    lines.append(f"MODULE_BUILD_DIR = $(BUILD_DIR)/modules")
    lines.append("")

    for module in layout["modules"]:
        name = module["name"]
        name_upper = name.upper()

        lines.append(f"# {name} module")
        lines.append(f"{name_upper}_SRCS_C = $(wildcard $(MODULE_DIR)/{name}/*.c)")
        lines.append(f"{name_upper}_SRCS_S = $(wildcard $(MODULE_DIR)/{name}/*.s)")
        lines.append(f"{name_upper}_OBJS = $({name_upper}_SRCS_C:$(MODULE_DIR)/{name}/%.c=$(MODULE_BUILD_DIR)/{name}/%.o)")
        lines.append(f"{name_upper}_OBJS += $({name_upper}_SRCS_S:$(MODULE_DIR)/{name}/%.s=$(MODULE_BUILD_DIR)/{name}/%.o)")
        lines.append(f"{name_upper}_LINKER = $(MODULE_DIR)/{name}/{name}.ld")
        lines.append(f"{name_upper}_ELF = $(BUILD_DIR)/$(PROJECT)-{name}.elf")
        lines.append(f"{name_upper}_BIN = $(BUILD_DIR)/$(PROJECT)-{name}.bin")
        lines.append(f"{name_upper}_ADDR = 0x{module['rom_start']:08X}")
        lines.append(f"{name_upper}_SIZE = {module['rom_size']}")
        lines.append("")

    lines.append("#----------------------- MODULE FLAGS -----------------------#")
    lines.append("")
    lines.append("MODULE_CFLAGS = $(CFLAGS) -DMODULE_BUILD")
    lines.append("MODULE_ASFLAGS = $(ASFLAGS)")
    lines.append("")

    lines.append("define MODULE_LDFLAGS")
    lines.append("-T $(1) \\")
    lines.append("\t-L $(LINKER_DIR) \\")
    lines.append("\t-nostdlib \\")
    lines.append("\t-Wl,--no-gc-sections \\")
    lines.append("\t-Wl,-Map=$(2)")
    lines.append("endef")
    lines.append("")

    lines.append("#----------------------- MODULE TARGETS -----------------------#")
    lines.append("")

    all_module_bins = " ".join([f"$({m['name'].upper()}_BIN)" for m in layout["modules"]])
    lines.append(f"MODULES_BINS = {all_module_bins}")
    lines.append("")
    lines.append("modules: $(MODULES_BINS)")
    lines.append("\t@echo -e \"$(GREEN)[+] All modules built successfully$(RESET)\"")
    lines.append("")

    for module in layout["modules"]:
        name = module["name"]
        name_upper = name.upper()

        lines.append(f"# {name} module build rules")
        lines.append(f"$(MODULE_BUILD_DIR)/{name}/%.o: $(MODULE_DIR)/{name}/%.c")
        lines.append(f"\t@echo -e \"$(MAGENTA)> [MODULE:{name_upper}]: $@$(RESET)\"")
        lines.append(f"\t@mkdir -p $(dir $@)")
        lines.append(f"\t$(CC) $(MODULE_CFLAGS) -c -o $@ $<")
        lines.append("")

        lines.append(f"$(MODULE_BUILD_DIR)/{name}/%.o: $(MODULE_DIR)/{name}/%.s")
        lines.append(f"\t@echo -e \"$(MAGENTA)> [MODULE:{name_upper}]: $@$(RESET)\"")
        lines.append(f"\t@mkdir -p $(dir $@)")
        lines.append(f"\t$(AS) $(MODULE_ASFLAGS) -c -o $@ $<")
        lines.append("")

        lines.append(f"$({name_upper}_ELF): $({name_upper}_OBJS) $(CORE_LIB)")
        lines.append(f"\t@echo -e \"$(CYAN)[LINK] {name} module$(RESET)\"")
        lines.append(f"\t$(CC) $(CFLAGS) $(call MODULE_LDFLAGS,$({name_upper}_LINKER),$(BUILD_DIR)/$(PROJECT)-{name}.map) -o $@ $^")
        lines.append(f"\t@echo -e \"$(BLUE)$$($(SIZE) $@)$(RESET)\"")
        lines.append("")

        lines.append(f"$({name_upper}_BIN): $({name_upper}_ELF)")
        lines.append(f"\t@echo -e \"$(GREEN)[BIN] {name}.bin @ $({name_upper}_ADDR)$(RESET)\"")
        lines.append(f"\t$(OBJCPY) -O binary $< $@")
        lines.append("")


    lines.append("#----------------------- MODULE UTILITIES -----------------------#")
    lines.append("")
    for module in layout["modules"]:
        name = module["name"]
        name_upper = name.upper()
        lines.append(f"dump-{name}: $({name_upper}_ELF)")
        lines.append(f"\t$(OBJDMP) -d -S $<")
        lines.append("")

    lines.append("dump-modules: " + " ".join([f"dump-{m['name']}" for m in layout["modules"]]))
    lines.append("")

    lines.append("clean-modules:")
    lines.append("\trm -rf $(MODULE_BUILD_DIR)")
    lines.append("\trm -f $(MODULES_BINS)")
    lines.append("\trm -f $(BUILD_DIR)/modules.bin")
    lines.append("")

    phony_targets = ["modules", "clean-modules", "dump-modules"]
    phony_targets.extend([f"dump-{m['name']}" for m in layout["modules"]])
    lines.append(f".PHONY: {' '.join(phony_targets)}")
    lines.append("")

    return "\n".join(lines)

def generate_module_startup(module, layout):
    name = module["name"]
    lines = []
    lines.append('#include <aether.h>')
    lines.append('#include <aether/memory.h>')
    lines.append('')
    lines.append(f'extern int {name}_entry(void);')
    lines.append('')
    lines.append('__attribute__((section(".app_desc"), used))')
    lines.append(f'const app_desc_t {name}_desc = {{')
    lines.append(f'    .p_stack = (u32)&_{name}_estack,')
    lines.append(f'    .entry_point = (u32){name}_entry,')
    lines.append(f'    .id = {module.get("id", 0)},')
    lines.append(f'    .flags = {module.get("flags", "0")}')
    lines.append('};')
    return '\n'.join(lines)

def generate_memory_header(layout, config):
    lines = []
    lines.append('#ifndef MEMORY_H')
    lines.append('#define MEMORY_H')
    lines.append('')
    lines.append('#include <defs.h>')
    lines.append('')
    
    lines.append('extern const u32 _boot_rom_start;')
    lines.append('extern const u32 _boot_rom_size;')
    lines.append('')
    
    for module in layout["modules"]:
        name = module["name"]
        lines.append(f'extern const u32 _{name}_rom_start;')
        lines.append(f'extern const u32 _{name}_rom_size;')
        lines.append(f'extern const u32 _{name}_ram_start;')
        lines.append(f'extern const u32 _{name}_ram_size;')
        lines.append(f'extern const u32 _{name}_estack;')
        lines.append('')
    
    lines.append('#endif')
    return '\n'.join(lines)

def main():
    config_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CONFIG

    if not os.path.exists(config_path):
        print(f"error: config file '{config_path}' not found")
        sys.exit(1)

    print(f"loading configuration from {config_path}...")
    config = load_config(config_path)

    layout = calculate_module_layout(config, 0) 

    print(f"\nmemory Layout:")
    print(f"  ROM: {layout['total_rom']//1024}KB")
    print(f"  RAM: {layout['total_ram']//1024}KB")
    print(f"\nModules:")
    for mod in layout['modules']:
        print(f"  {mod['name']:12s} ROM:0x{mod['rom_start']:08X} ({mod['rom_size']//1024}KB) RAM:0x{mod['ram_start']:08X} ({mod['ram_size']//1024}KB)")

    for module in layout["modules"]:
        startup_path = f"src/modules/{module['name']}/{module['name']}_startup.c"
        os.makedirs(os.path.dirname(startup_path), exist_ok=True)
        startup_content = generate_module_startup(module, layout)
        with open(startup_path, 'w') as f:
            f.write(startup_content)
        print(f"generated {startup_path}")

    print(f"\ngenerating {OUTPUT_MAKEFILE}...")
    makefile_content = generate_module_makefile(layout)
    with open(OUTPUT_MAKEFILE, 'w') as f:
        f.write(makefile_content)

    print(f"generating include/aether/memory.h...")
    memory_header = generate_memory_header(layout, config)
    with open("include/aether/memory.h", 'w') as f:
        f.write(memory_header)

    print(f"generating include/module_info.h...")
    os.makedirs("include", exist_ok=True)
    header_content = generate_module_info_header(layout)
    with open("include/module_info.h", 'w') as f:
        f.write(header_content)

    print(f"generating linker/module_memory_map.ld...")
    os.makedirs("linker", exist_ok=True)
    memory_map_content = generate_memory_map(layout)
    with open("linker/module_memory_map.ld", 'w') as f:
        f.write(memory_map_content)

    for module in layout["modules"]:
        linker_path = f"src/modules/{module['name']}/{module['name']}.ld"
        os.makedirs(os.path.dirname(linker_path), exist_ok=True)
        linker_content = generate_module_linker(module, layout)
        with open(linker_path, 'w') as f:
            f.write(linker_content)
        print(f"generated {linker_path}")

    print("\nDONE!")

if __name__ == "__main__":
    main()
