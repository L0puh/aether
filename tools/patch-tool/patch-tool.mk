
#----------------------- PATCH TOOL -----------------------#

PATCH_TOOL_DIR = tools/patch-tool
PATCH_TOOL_SRC = $(PATCH_TOOL_DIR)/patch-tool.c
PATCH_TOOL_BIN = $(BUILD_DIR)/patch-tool

PATCH_TOOL_INCLUDES = -I$(ROOT_DIR)/include -I$(ROOT_DIR)/src/boot

$(PATCH_TOOL_BIN): $(PATCH_TOOL_SRC)
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)> building patch-tool...$(RESET)"
	gcc -Wall -Wextra -O2 $(PATCH_TOOL_INCLUDES) -o $@ $^
	@echo -e "$(GREEN)[+] patch-tool built: $@$(RESET)"

PATCH = $(PATCH_TOOL_BIN)

clean-patch-tool:
	rm -f $(PATCH_TOOL_BIN)

.PHONY: clean-patch-tool
