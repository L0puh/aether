#
# modules.mk - a modified-ready makefile 
# for apps to be flashed next to static bootloader
#

#----------------------- CONFIGURATION -----------------------#

# STM32F103C6T6: 32KB flash, 10KB RAM
BOOTLOADER_SIZE = 8192 						# 8KB bootloader
APP_SLOT_SIZE   = 8192 						# 8KB per app slot
APP_SLOT_ADDR   = 0x08002000 			   # start after bootloader

MODULE_NAME     = main_app
MODULE_DIR_APP  = src/app
MODULE_LINKER   = $(LINKER_DIR)/app.ld

#----------------------- AUTO-DETECT SOURCES -----------------------#

MODULE_SRCS_C = $(wildcard $(MODULE_DIR_APP)/*.c)
MODULE_SRCS_S = $(wildcard $(MODULE_DIR_APP)/*.s)
MODULE_OBJS = $(MODULE_SRCS_C:$(MODULE_DIR_APP)/%.c=$(BUILD_DIR)/modules/%.o) \
              $(MODULE_SRCS_S:$(MODULE_DIR_APP)/%.s=$(BUILD_DIR)/modules/%.o)

#----------------------- BUILD RULES -----------------------#

$(BUILD_DIR)/modules/%.o: $(MODULE_DIR_APP)/%.c
	@mkdir -p $(dir $@)
	@echo -e "$(MAGENTA)> [APP]: $<$(RESET)"
	$(CC) $(CFLAGS) -DAPP_BUILD -DAPP_BASE=$(APP_SLOT_ADDR) -c -o $@ $<

$(BUILD_DIR)/modules/%.o: $(MODULE_DIR_APP)/%.s
	@mkdir -p $(dir $@)
	@echo -e "$(MAGENTA)> [APP]: $<$(RESET)"
	$(AS) $(ASFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(MODULE_NAME).elf: $(MODULE_OBJS)
	@echo -e "$(MAGENTA)=== linking $(MODULE_NAME) ===$(RESET)"
	$(CC) $(CFLAGS) -T $(MODULE_LINKER) $(APP_LDFLAGS) \
		-Wl,--defsym=APP_BASE_ADDR=$(APP_SLOT_ADDR) \
		-o $@ $^
	@$(SIZE) $@

$(BUILD_DIR)/$(MODULE_NAME).bin: $(BUILD_DIR)/$(MODULE_NAME).elf
	$(OBJCPY) -O binary $< $@
	@echo -e "$(GREEN)[+] $(MODULE_NAME).bin: $$(wc -c < $@) / $(APP_SLOT_SIZE) bytes$(RESET)"
	@if [ $$(wc -c < $@) -gt $(APP_SLOT_SIZE) ]; then \
		echo -e "$(RED)ERROR: app exceeds slot size!$(RESET)"; \
		exit 1; \
	fi

#----------------------- MODULE TARGETS -----------------------#

modules: $(BUILD_DIR)/$(MODULE_NAME).bin
	@echo -e "$(GREEN)[+] app built successfully!$(RESET)"

flash-app: $(BUILD_DIR)/$(MODULE_NAME).bin
	@echo -e "$(CYAN)flashing $(MODULE_NAME) to 0x$(shell printf "%X" $(APP_SLOT_ADDR))...$(RESET)"
	st-flash write $< $(APP_SLOT_ADDR)
	@echo -e "$(GREEN)app flashed!$(RESET)"

dump-app: $(BUILD_DIR)/$(MODULE_NAME).elf
	$(OBJDMP) -d -S $< > $(BUILD_DIR)/$(MODULE_NAME).lst
	@echo -e "$(GREEN)disassembly saved to $(BUILD_DIR)/$(MODULE_NAME).lst$(RESET)"

sym-app: $(BUILD_DIR)/$(MODULE_NAME).elf
	$(OBJDMP) -t $< | sort

clean-modules:
	@rm -rf $(BUILD_DIR)/modules $(BUILD_DIR)/$(MODULE_NAME).*
	@echo -e "$(GREEN)[+] app cleaned$(RESET)"

#----------------------- HELP -----------------------#

help-modules:
	@echo -e "$(BOLD)app module targets:$(RESET)"
	@echo "  modules      	- build the application"
	@echo "  flash-app    	- flash application to device"
	@echo "  dump-app     	- generate disassembly listing"
	@echo "  sym-app      	- show symbol table"
	@echo "  clean-modules	- clean app build files"
	@echo ""
	@echo -e "$(BOLD)Configuration:$(RESET)"
	@echo "  app name:     $(MODULE_NAME)"
	@echo "  flash addr:   0x$(shell printf "%X" $(APP_SLOT_ADDR))"
	@echo "  max size:     $(APP_SLOT_SIZE) bytes"
