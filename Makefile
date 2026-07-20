ROOT_DIR = .

#----------------------- TOOLS -----------------------#
PREFIX = arm-none-eabi-
CC 	 = $(PREFIX)gcc
AS 	 = $(PREFIX)as
AR 	 = $(PREFIX)ar
LD 	 = $(PREFIX)ld
OBJCPY = $(PREFIX)objcopy
OBJDMP = $(PREFIX)objdump
SIZE   = $(PREFIX)size
DEBUG  = tools/debugger/debugger.sh

#----------------------- TARGET -----------------------#

PROJECT    = aether
BUILD_DIR  = build
LINKER_DIR = linker
LIB_DIR    = libs
CHIP       = cortex-m3 

#----------------------- SOURCES  -----------------------#

BOOTLOADER_LINKER = $(LINKER_DIR)/bootloader.ld
CORE_LIB 			= $(BUILD_DIR)/libcore.a

LIB_DIRS = $(wildcard $(LIB_DIR)/*)

CORE_SRCS_C   = $(wildcard src/core/*.c)
CORE_SRCS_S   = $(wildcard src/core/*.s)
CORE_SRCS_ASM = $(wildcard src/core/*.S)

BOOT_SRCS_C   = $(wildcard src/boot/*.c)
BOOT_SRCS_S   = $(wildcard src/boot/*.s)

CORE_OBJS = $(CORE_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
CORE_OBJS += $(CORE_SRCS_S:src/%.s=$(BUILD_DIR)/%.o)
CORE_OBJS += $(CORE_SRCS_ASM:src/%.S=$(BUILD_DIR)/%.o)

BOOT_OBJS = $(BOOT_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
BOOT_OBJS += $(BOOT_SRCS_S:src/%.s=$(BUILD_DIR)/%.o)


#----------------------- OTHER -----------------------#

RED    = \033[0;31m
GREEN  = \033[0;32m
YELLOW = \033[0;33m
BLUE   = \033[0;34m
MAGENTA= \033[0;35m
CYAN   = \033[0;36m
BOLD   = \033[1;37m
RESET  = \033[0m
BRED   = \033[1;35m

START_TIME := $(shell date +%s)

#----------------------- FLAGS -----------------------#


INCL   = -I./include\
			-I./linker \
			-I./src 

CFLAGS = -mcpu=$(CHIP)\
			-mthumb\
			-g\
			-O0\
			-Wall\
			-Wextra\
			-nostdlib\
			-fno-builtin\
			-ffreestanding\
			-fdata-sections\
			-ffunction-sections\
			$(INCL)
#			-Werror

ASFLAGS = -mcpu=$(CHIP)\
			 -mthumb\
			 -g\
			 -W 				  				

BOOT_CFLAGS = $(CFLAGS) -DHV_API_VERSION=1

BOOT_LDFLAGS = -T $(BOOTLOADER_LINKER)\
					-L $(LINKER_DIR)\
					-nostdlib\
					-Wl,--no-gc-sections\
					-Wl,--print-memory-usage\
					-Wl,-Map=$(BUILD_DIR)/$(PROJECT)-boot.map

APP_LDFLAGS = -nostdlib\
				  -Wl,--no-gc-sections\
				  -Wl,--print-memory-usage\
				  -Wl,-Map=$(BUILD_DIR)/$(@F:.elf=.map)


#---------------------- MODULES-----------------------#
-include modules.mk

#----------------------- BUILD -----------------------#

all: $(CORE_LIB) convert-memory-map $(BUILD_DIR)/$(PROJECT)-boot.bin modules 

$(CORE_LIB): $(CORE_OBJS)
	$(AR) rcs $@ $^
	@echo -e "$(GREEN)[+] core library created: $$(wc -c < $@) bytes$(RESET)"

$(BUILD_DIR)/core/%.o: src/core/%.c
	@echo -e "$(YELLOW)> [CORE]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/core/%.o: src/core/%.s
	@echo -e "$(YELLOW)> [CORE]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: src/core/%.S
	@echo -e "$(YELLOW)> [CORE]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(CC) -x assembler-with-cpp $(CFLAGS) -c $< -o $@

# --------------------------------------------- BOOTLOADER

$(BUILD_DIR)/boot/%.o: src/boot/%.c
	@echo -e "$(CYAN)> [BOOTLOADER]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/boot/%.o: src/boot/%.s
	@echo -e "$(CYAN)> [BOOTLOADER]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c -o $@ $<


$(BUILD_DIR)/$(PROJECT)-boot.elf: $(BOOT_OBJS) $(CORE_LIB)
	$(CC) $(BOOT_CFLAGS) $(BOOT_LDFLAGS) -o $@ $^ 
	@echo -e "$(GREEN)[+] $@ is done!$(RESET)"
	@echo -e "$(BLUE)size:\n$$($(SIZE) $@)$(RESET)\n"

$(BUILD_DIR)/$(PROJECT)-boot.bin: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJCPY) -O binary $< $@


#--------------------------------------------------------#

dump-boot: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJDMP) -d -S $<

sym-boot: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJDMP) -t $< | sort

flash: $(BUILD_DIR)/$(PROJECT)-boot.bin
	st-flash erase >/dev/null 2>&1
	st-flash write $< 0x08000000 >/dev/null 2>&1
	st-flash reset >/dev/null 2>&1
	@echo -e "$(GREEN)[>>>] $< is flashed @ 0x08000000 $(RESET)"

reset: 
	st-flash reset

erase:
	st-flash erase

clean: clean-modules
	rm -rf $(BUILD_DIR)

debug:
	${DEBUG} all

open-serial:
	minicom -D /dev/ttyUSB0 -b 115200

convert-memory-map: include/memory_map.h 
	./tools/convert_memory_map.sh include/memory_map.h > linker/memory_map.ld
	@echo -e "$(GREEN)memory map is generated to include in linker!$(RESET)"



.PHONY: all flash erase clean dump-boot sym-boot 
