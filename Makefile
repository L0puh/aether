
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
CHIP       = cortex-m3 

#----------------------- SOURCES  -----------------------#


BOOTLOADER_LINKER = linker/bootloader.ld
APP_LINKER			= linker/app.ld
CORE_LIB 			= $(BUILD_DIR)/libcore.a

CORE_SRCS_C = $(wildcard src/core/*/*.c)
CORE_SRCS_S = $(wildcard src/core/*/*.s)
CORE_SUBDIRS = $(patsubst src/core/%, $(BUILD_DIR)/core/%, $(dir $(BOOT_SRCS_C)))

BOOT_SRCS_C = $(wildcard src/boot/*.c)
BOOT_SRCS_S = $(wildcard src/boot/*.s)

APP_SRCS_C  = $(wildcard src/app/*.c)

CORE_OBJS = $(CORE_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
CORE_OBJS += $(CORE_SRCS_S:src/%.s=$(BUILD_DIR)/%.o)

BOOT_OBJS = $(BOOT_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
BOOT_OBJS += $(BOOT_SRCS_S:src/%.s=$(BUILD_DIR)/%.o)


APP_OBJS  = $(APP_SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
APP_OBJS += $(CORE_LIB) 

#----------------------- FLAGS -----------------------#


INCL   = -I./include
CFLAGS = -mcpu=$(CHIP)\
			-mthumb\
			-O0\
			-Wall\
			-Wextra\
			-nostdlib\
			-fno-builtin\
			-ffreestanding\
         -fdata-sections\
         -ffunction-sections\
			$(INCL)
			
ASFLAGS = -mcpu=$(CHIP)\
			 -mthumb\
			 -g\
			 -W 				  				

BOOT_LDFLAGS = -T $(BOOTLOADER_LINKER)\
					-L $(LINKER_DIR)\
					-nostdlib\
					-Wl,--gc-sections

APP_LDFLAGS = -T $(APP_LINKER)\
			     -L $(LINKER_DIR)\
				  -nostdlib\
				  -Wl,--no-gc-sections\
				  -Wl,-Map=$(BUILD_DIR)/$(PROJECT)-app.map


#----------------------- BUILD -----------------------#

all: $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT)-boot.bin\
									  $(BUILD_DIR)/$(PROJECT)-app.bin 
	cat $^ > $@
	@echo "FINAL IMAGE: $@ ($$(wc -c < $@) bytes)"

$(BUILD_DIR)/app:
	mkdir -p $@

$(BUILD_DIR)/core/%/:
	mkdir -p $@


# --------------------------------------------- CORE

$(CORE_LIB): $(CORE_OBJS)
	@echo "> creating core library: $@"
	$(AR) rcs $@ $^
	@echo "> core library created: $$(wc -c < $@) bytes"

$(BUILD_DIR)/core/%.o: src/core/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/core/%.o: src/core/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c -o $@ $<

# --------------------------------------------- BOOTLOADER

$(BUILD_DIR)/boot/%.o: src/boot/%.c
	@echo "> compiling bootload: $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/boot/%.o: src/boot/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(PROJECT)-boot.elf: $(BOOT_OBJS)
	$(CC) $(CFLAGS) $(BOOT_LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/$(PROJECT)-boot.bin: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJCPY) -O binary $< $@

# --------------------------------------------------- APP

$(BUILD_DIR)/app/%.o: src/app/%.c | $(BUILD_DIR)/app
	@echo "> compiling app: $@"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/app/%.o: src/app/%.s | $(BUILD_DIR)/app
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/$(PROJECT)-app.elf: $(APP_OBJS)
	$(CC) $(CFLAGS) $(APP_LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/$(PROJECT)-app.bin: $(BUILD_DIR)/$(PROJECT)-app.elf
	$(OBJCPY) -O binary $< $@


#--------------------------------------------------------#

dump-boot: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJDMP) -d -S $<

dump-app: $(BUILD_DIR)/$(PROJECT)-app.elf
	$(OBJDMP) -d -S $<

sym-boot: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJDMP) -t $< | sort

flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash write $< 0x08000000

erase:
	st-flash erase

clean:
	rm -rf $(BUILD_DIR)

debug:
	${DEBUG} all

.PHONY: all flash erase clean dump-boot dump-app sym-boot 
