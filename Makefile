

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

CORE_SRCS_C = $(wildcard src/core/*.c)
CORE_SRCS_S = $(wildcard src/core/*.s)

BOOT_SRCS_C = $(wildcard src/boot/*.c)
BOOT_SRCS_S = $(wildcard src/boot/*.s)

CORE_OBJS = $(CORE_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
CORE_OBJS += $(CORE_SRCS_S:src/%.s=$(BUILD_DIR)/%.o)

BOOT_OBJS = $(BOOT_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
BOOT_OBJS += $(BOOT_SRCS_S:src/%.s=$(BUILD_DIR)/%.o)
BOOT_OBJS += $(CORE_LIB) 


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

#----------------------- COMPACT25519 (only with FEATURE_SIGN_APP in target.h) -----------------------#

HAVE_FEATURE_SIGN := $(shell grep -E "^\s*#define\s+FEATURE_SIGN_APP(\s|$$)" include/target.h >/dev/null 2>&1 && echo yes)

ifeq ($(HAVE_FEATURE_SIGN),yes)	
$(info building with compact25519 crypto library (feature_sign_app enabled))
COMPACT25519_DIR  = libs/compact25519/src
COMPACT25519_SRCS = $(COMPACT25519_DIR)/compact_ed25519.c \
$(COMPACT25519_DIR)/compact_wipe.c \
							  $(wildcard $(COMPACT25519_DIR)/c25519/*.c)
COMPACT25519_OBJS = $(COMPACT25519_SRCS:$(COMPACT25519_DIR)/%.c=$(BUILD_DIR)/thirdparty/compact25519/%.o)
CFLAGS += -I$(COMPACT25519_DIR) -I$(COMPACT25519_DIR)/c25519
CORE_OBJS += $(COMPACT25519_OBJS)

$(BUILD_DIR)/thirdparty/compact25519/%.o: $(COMPACT25519_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)> [COMPACT]: $<$(RESET)"
	$(CC) -mcpu=$(CHIP) -mthumb -Os -fno-builtin -ffreestanding \
		-I$(COMPACT25519_DIR) -I$(COMPACT25519_DIR)/c25519 \
		-DCOMPACT_DISABLE_X25519 -DCOMPACT_DISABLE_X25519_DERIVE \
		-c -o $@ $<
else
$(info building without compact25519 crypto library)
COMPACT25520_OBJS :=
endif

# --------------------------------------------- CORE

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

# --------------------------------------------- BOOTLOADER

$(BUILD_DIR)/boot/%.o: src/boot/%.c
	@echo -e "$(CYAN)> [BOOTLOADER]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/boot/%.o: src/boot/%.s
	@echo -e "$(CYAN)> [BOOTLOADER]: $@ $@$(RESET)"
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(PROJECT)-boot.elf: $(BOOT_OBJS)
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
	st-flash erase
	st-flash write $< 0x08000000
	st-flash reset

erase:
	st-flash erase

clean: clean-modules
	rm -rf $(BUILD_DIR)

debug:
	${DEBUG} all

convert-memory-map: include/memory_map.h 
	./tools/convert_memory_map.sh include/memory_map.h > linker/memory_map.ld
	@echo -e "$(GREEN)memory map is generated to include in linker!$(RESET)"

.PHONY: all flash erase clean dump-boot sym-boot 
