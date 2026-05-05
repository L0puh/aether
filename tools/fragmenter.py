#
# THIS SCRIPT CREATES CUSTOM LINKER AND MEMORY MAP 
# CONFIGURATION SPECIFIC FOR DEFINED BEHAVIOUR
#

import os 
import sys 
import json

DEFAULT_FILE = "config.json"

def detect_config():
    arg = sys.argv
    filename = DEFAULT_FILE

    if len(arg) > 1:
        filename = arg[1]

    if os.path.exists(filename):
        return filename

    print(f"{filename} doesn't exist!")

    return ""

def parse_json(filename, total_memory):
    with open(filename, "r") as file:
        data = json.load(file)

    origin = int(data["origin"], 16) 
    stack_size = parse_memory_size(data["stack_size"])
    available_memory = total_memory - stack_size
    allocated_memory = 0

    modules = data["modules"]

    for mod in modules:
        if mod["length"]:
            size = parse_memory_size(mod["length"])
            mod["allocated"] = size 
            mod["start_addr"] = origin + allocated_memory
            allocated_memory += size


    remaining_memory = available_memory - allocated_memory
    empty_count = sum(1 for m in modules if not m["length"])

    if empty_count > 0:
        leftover = remaining_memory // empty_count
        for mod in modules:
            if not mod["length"]:
                mod["allocated"] = leftover
                mod["start_addr"] = origin + allocated_memory
                allocated_memory += leftover

                if allocated_memory > available_memory:
                    mod["allocated"] -= (allocated_memory -
                                         available_memory)


    return {
            "origin": hex(origin),
            "stack_size": stack_size,
            "total_memory": total_memory,
            "used_memory": allocated_memory + stack_size,
            "free_memory": total_memory - (allocated_memory + stack_size),
            "modules": modules
            }

def parse_memory_size(size_str):
    if not size_str: 
        return 0

    if size_str.endswith("KB"):
        return int(size_str[:-2]) * 1024
    elif size_str.endswith("MB"):
        return int(size_str[:-2]) * 1024**2 
    elif size_str.endswith("B"):
        return int(size_str[:-1]) 

    print("wrong memory format, specify KB, MB or B")
    return 0

def generate_memory_map_linker(config):
    lines = []
    lines.append("/*auto-generated memory map based on config file*/")
    lines.append("\n\n")
    lines.append("MEMORY")
    lines.append("{")
    for i, module in enumerate(config["modules"]):
        name = module['name'].upper()
        start_addr = module.get("start_addr", 0)
        size = module.get("allocated", 0)

        if size > 0:
            if module.get("shared", False):
                attrs = "rwx"  
            else:
                attrs = "rx"   

            if size >= 1024:
                size_str = f"{size // 1024}K"
            else:
                size_str = f"{size}"

            lines.append(f"    {name}_ROM  ({attrs})  : ORIGIN = 0x{start_addr:08X}, LENGTH = {size_str}")
            lines.append(f"    {name}_RAM  (rwx) : ORIGIN = 0x{start_addr + size:08X}, LENGTH = {size // 4}")

    lines.append("}")
    lines.append("")

    for module in config["modules"]:
        name = module['name'].upper()
        start_addr = module.get("start_addr", 0)
        size = module.get("allocated", 0)

        if size > 0:
            lines.append(f"_{name.lower()}_rom_start = ORIGIN({name}_ROM);")
            lines.append(f"_{name.lower()}_rom_size  = LENGTH({name}_ROM);")

    lines.append("")
    lines.append(f"STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x{config['stack_size']:X}; /* {config['stack_size'] // 1024}K */")

    return "\n".join(lines)


def generate_module_linker(config, module_name):
    module = None
    for m in config["modules"]:
        if m['name'] == module_name:
            module = m
            break

    if not module:
        return ""

    name_upper = module['name'].upper()
    name_lower = module['name'].lower()

    lines = []
    lines.append(f"/* auto-genertaed linker script for {module['name']} module */")
    lines.append("")
    lines.append("INCLUDE memory_map.ld")
    lines.append("")
    lines.append("SECTIONS")
    lines.append("{")
    lines.append("   .text :")
    lines.append("   {")
    lines.append("      . = ALIGN(4);")
    lines.append(f"      _{name_lower}_stext = .;")
    lines.append("      KEEP(*(.app_desc))")
    lines.append("")
    lines.append("      *(.text*)")
    lines.append("      *(.rodata*)")
    lines.append("      . = ALIGN(4);")
    lines.append(f"      _{name_lower}_etext = .;")
    lines.append(f"   }} > {name_upper}_ROM")
    lines.append("")
    lines.append(f"   .data : AT (_{name_lower}_etext)")
    lines.append("   {")
    lines.append("      . = ALIGN(4);")
    lines.append(f"      _{name_lower}_sdata = .;")
    lines.append("      *(.data*)")
    lines.append("      . = ALIGN(4);")
    lines.append(f"      _{name_lower}_edata = .;")
    lines.append(f"   }} > {name_upper}_RAM")
    lines.append("")
    lines.append("   .bss (NOLOAD) :")
    lines.append("   {")
    lines.append("      . = ALIGN(4);")
    lines.append(f"      _{name_lower}_sbss = .;")
    lines.append("      *(.bss*)")
    lines.append("      *(COMMON)")
    lines.append("      . = ALIGN(4);")
    lines.append(f"      _{name_lower}_ebss = .;")
    lines.append(f"   }} > {name_upper}_RAM")
    lines.append("")
    lines.append("   .stack :")
    lines.append("   {")
    lines.append("      . = ALIGN(8);")
    lines.append(f"      _{name_lower}_sstack = .;")
    lines.append("      . = . + STACK_SIZE;")
    lines.append("      . = ALIGN(8);")
    lines.append(f"      _{name_lower}_estack = .;")
    lines.append(f"   }} > {name_upper}_RAM")
    lines.append("}")

    return "\n".join(lines)

def generate_module_startup(config, module_name):
    module = None
    for m in config["modules"]:
        if m['name'] == module_name:
            module = m
            break

    if not module:
        return ""

    entry_point = module.get("entry_point", f"{module_name}_entry")
    name_lower = module['name'].lower()
    name_upper = module['name'].upper()

    lines = []
    lines.append(f"/* auto-generated startup code for {module['name']} module */")
    lines.append("#include <aether.h>")
    lines.append("")
    lines.append(f"extern u32 _{name_lower}_estack;")
    lines.append("")
    lines.append(f"extern int {entry_point}(void);")
    lines.append("")
    lines.append("__attribute__((section(\".app_desc\"), used))")
    lines.append("const app_desc_t app_vectors = {")
    lines.append(f"   .p_stack = (u32)&_{name_lower}_estack,")
    lines.append(f"   .entry_point = (u32){entry_point},")
    lines.append("   .id = 1, ") # TODO: assign unique ID!
    lines.append("   .flags = 0")
    lines.append("};")
    lines.append("")
    lines.append(f"int {entry_point}(void)")
    lines.append("{")
    lines.append("    return 0;")
    lines.append("}")

    return "\n".join(lines)


def print_memory_layout(filename, config):
    print("=" * 60)
    print(f"{filename.upper():^60}")
    print("=" * 60)
    print(f"total Memory: {config['total_memory'] // 1024}KB")
    print(f"origin: {config['origin']}")
    print(f"stack size: {config['stack_size']} bytes ({config['stack_size'] // 1024}KB)")
    print(f"used Memory: {config['used_memory']} bytes ({config['used_memory'] // 1024}KB)")
    print(f"free Memory: {config['free_memory']} bytes ({config['free_memory'] // 1024}KB)")
    print("-" * 60)

    print("\nmodule memory allocation:")
    print(f"{'module':<12} {'priority':<8} {'size':<10} {'address':<12} {'shared'}")
    print("-" * 60)

    for module in config["modules"]:
        size = module.get("allocated", 0)
        address = module.get("start_addr", 0)
        shared = "yes" if module.get("shared") else "no"

        print(f"{module['name']:<12} {module['priority']:<8} "
              f"{size // 1024}KB{' ' if size < 1024*1024 else '':<7} "
              f"0x{address:08X}    {shared}")



def generate_module_files(config):
    modules_dir = os.path.join("src", "modules")
    if not os.path.exists(modules_dir):
        os.makedirs(modules_dir)

    memory_map = generate_memory_map_linker(config)
    with open(os.path.join(modules_dir, "memory_map.ld"), "w") as f:
        f.write(memory_map)
    print(f"done: {modules_dir}/memory_map.ld")
    
    for module in config["modules"]:
        module_name = module['name']
        module_dir = os.path.join(modules_dir, module_name)

        if not os.path.exists(module_dir):
            os.makedirs(module_dir)

        #TODO:
        # makefile = generate_module_makefile(config, module_name)
        # with open(os.path.join(module_dir, "Makefile"), "w") as f:
        #     f.write(makefile)


        linker = generate_module_linker(config, module_name)
        with open(os.path.join(module_dir, f"{module_name}.ld"), "w") as f:
            f.write(linker)
        print(f"done: {module_dir}/{module_name}.ld")

        startup = generate_module_startup(config, module_name)
        with open(os.path.join(module_dir, f"{module_name}_startup.c"), "w") as f:
            f.write(startup)
        print(f"done: {module_dir}/{module_name}_startup.c")

        source_file = os.path.join(module_dir, f"{module_name}.c")
        if not os.path.exists(source_file):
            with open(source_file, "w") as f:
                entry_point = module.get("entry_point", f"{module_name}_entry")
                f.write(f'#include <aether.h>\n\n'
                       f'int {entry_point}(void) {{\n'
                       f'    // TODO: Implement {module_name} functionality\n'
                       f'    return 0;\n'
                       f'}}\n')
            print(f"done: {source_file}")

    # TODO;
    # build_config = generate_module_build_script(config)
    # with open("modules.mk", "w") as f:
    #     f.write(build_config)
    # integration = generate_root_makefile_integration(config)

if __name__ == "__main__":
    
    filename = detect_config()
    config = parse_json(filename, total_memory = 64 * 1024)

    print_memory_layout(filename, config)
    generate_module_files(config)
