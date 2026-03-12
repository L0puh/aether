
#----------------------- TOOLS -----------------------#
PREFIX = arm-none-eabi-
CC 	 = $(PREFIX)gcc
AS 	 = $(PREFIX)as
LD 	 = $(PREFIX)ld
OBJCPY = $(PREFIX)objcopy
OBJDMP = $(PREFIX)objdump
SIZE   = $(PREFIX)size

#----------------------- TARGET -----------------------#

PROJECT    = aether
BUILD_DIR  = build
LINKER_DIR = linker
CHIP       = cortex-m4

#----------------------- SOURCES  -----------------------#


BOOTLOADER_LINKER = linker/bootloader.ld
APP_LINKER			= linker/app.ld

BOOT_SRCS_C = $(wildcard src/boot/*.c)
BOOT_SRCS_S = $(wildcard src/boot/*.s)
APP_SRCS_C  = $(wildcard src/app/*.c)

BOOT_OBJS = $(patsubst src/boot/%.c, $(BUILD_DIR)/boot/%.o, $(BOOT_SRCS_C)) \
            $(patsubst src/boot/%.s, $(BUILD_DIR)/boot/%.o, $(BOOT_SRCS_S))

APP_OBJS  = $(patsubst src/app/%.c,  $(BUILD_DIR)/app/%.o,  $(APP_SRCS_C))

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
				  -Wl,--gc-sections 	


#----------------------- BUILD -----------------------#

all: $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT)-boot.bin\
									  $(BUILD_DIR)/$(PROJECT)-app.bin 
	cat $^ > $@
	@echo "FINAL IMAGE: $@ ($$(wc -c < $@) bytes)"

$(BUILD_DIR)/boot $(BUILD_DIR)/app:
	mkdir -p $@

# --------------------------------------------- BOOTLOADER

$(BUILD_DIR)/boot/%.o: src/boot/%.c | $(BUILD_DIR)/boot
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/boot/%.o: src/boot/%.s | $(BUILD_DIR)/boot
	$(AS) $(ASFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(PROJECT)-boot.elf: $(BOOT_OBJS)
	$(CC) $(CFLAGS) $(BOOT_LDFLAGS) -o $@ $^
	$(SIZE) $@

$(BUILD_DIR)/$(PROJECT)-boot.bin: $(BUILD_DIR)/$(PROJECT)-boot.elf
	$(OBJCPY) --pad-to=0x4000 --gap-fill=0xFF -O binary $< $@


# --------------------------------------------------- APP

$(BUILD_DIR)/app/%.o: src/app/%.c | $(BUILD_DIR)/app
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

.PHONY: all flash erase clean dump-boot dump-app sym-boot 
