#----------------------- MODULES (APPS) -----------------------#
-include tools/patch-tool/patch-tool.mk

APPS_DIR    = src/apps
APP_LINKER  = linker/app.ld
VERSION     = 1

APP_NAMES := $(notdir $(wildcard $(APPS_DIR)/*))
APP_ELFS  := $(APP_NAMES:%=$(BUILD_DIR)/apps/%.elf)
APP_BINS  := $(APP_NAMES:%=$(BUILD_DIR)/apps/%.bin)

$(LINKER_DIR)/memory_map.ld: $(LINKER_DIR)/../include/memory_map.h
	@echo -e "$(YELLOW)> generating memory_map.ld...$(RESET)"
	./tools/convert_memory_map.sh $< > $@

define APP_template

$(BUILD_DIR)/apps/$(1)/%.o: $(APPS_DIR)/$(1)/%.c | $(LINKER_DIR)/memory_map.ld
	@mkdir -p $$(dir $$@)
	@echo -e "$(MAGENTA)> [APP:$(1)]: $$<$(RESET)"
	$$(CC) $$(CFLAGS) -DAPP_BUILD -DAPP_BASE=0x08004000 -c -o $$@ $$<

$(BUILD_DIR)/apps/$(1)/%.o: $(APPS_DIR)/$(1)/%.s | $(LINKER_DIR)/memory_map.ld
	@mkdir -p $$(dir $$@)
	@echo -e "$(MAGENTA)> [APP:$(1)]: $$<$(RESET)"
	$$(AS) $$(ASFLAGS) -c -o $$@ $$<

APP_$(1)_SRCS_C := $$(wildcard $(APPS_DIR)/$(1)/*.c)
APP_$(1)_SRCS_S := $$(wildcard $(APPS_DIR)/$(1)/*.s)
APP_$(1)_OBJS   := $$(APP_$(1)_SRCS_C:$(APPS_DIR)/$(1)/%.c=$(BUILD_DIR)/apps/$(1)/%.o)
APP_$(1)_OBJS   += $$(APP_$(1)_SRCS_S:$(APPS_DIR)/$(1)/%.s=$(BUILD_DIR)/apps/$(1)/%.o)

$(BUILD_DIR)/apps/$(1).elf: $$(APP_$(1)_OBJS) $(CORE_LIB) $(LINKER_DIR)/memory_map.ld
	@mkdir -p $$(dir $$@)
	@echo -e "$(MAGENTA)=== linking app '$(1)' ===$(RESET)"
	$$(CC) $$(CFLAGS) -T $(APP_LINKER) -L $(LINKER_DIR) \
		-nostdlib -Wl,--no-gc-sections -Wl,--print-memory-usage \
		-Wl,-Map=$(BUILD_DIR)/apps/$(1).map -o $$@ $$^
	@echo -e "$(GREEN)[+] app '$(1)' linked$(RESET)"
	@echo -e "$(BLUE)size:\n$$$$($(SIZE) $$@)$(RESET)\n"


$(BUILD_DIR)/apps/$(1).bin: $(BUILD_DIR)/apps/$(1).elf $(PATCH)
	$$(OBJCPY) -O binary $$< $$@
	@echo -e "$(YELLOW)> patching app descriptor...$(RESET)"
	$(PATCH) -v ${VERSION} -f $$@ -o $$@.patched
	@echo -e "$(GREEN)[+] app '$(1)' descriptor patched$(RESET)"

endef

$(foreach app,$(APP_NAMES),$(eval $(call APP_template,$(app))))

modules: $(APP_BINS)
	@echo -e "$(BOLD)[=] built $(words $(APP_NAMES)) module(s): $(APP_NAMES)$(RESET)"

list-modules:
	@echo -e "$(CYAN)available apps: $(APP_NAMES)$(RESET)"

dump-app-%: $(BUILD_DIR)/apps/%.elf
	$(OBJDMP) -d -S $<

patch-%: $(BUILD_DIR)/apps/%.bin $(PATCH)
	@echo -e "$(YELLOW)> patching app descriptor for $*...$(RESET)"
	$(PATCH) -v $(VERSION) -f $< -o $@.patched
	@echo -e "$(GREEN)[+] app '$*' descriptor patched$(RESET)"

clean-modules:
	rm -rf $(BUILD_DIR)/apps


.PHONY: modules list-modules clean-modules
