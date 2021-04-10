HOME := .
include Config.mk

all: clean
	@echo "  Building ..."
	@mkdir -p $(OBJ_DIR) && cd $(OBJ_DIR) && mkdir -p $(DIRS)
	@$(MAKE) -s -C $(SRC_DIR)
	@rm -vrf $(OBJ_DIR) > /dev/null

clean:
	@echo "  Cleaning up ..."
	@rm -vrf $(BUILD_DIR) > /dev/null
