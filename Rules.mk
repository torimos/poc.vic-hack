include $(HOME)/Config.mk

%.S.o: %.S
	@echo "  CC $@"
	@$(CC) $(CCFLAGS) -I$(INC_DIR) -c $< -o $(OBJ_DIR)/$@
%.c.o: %.c
	@echo "  CC $@"
	@$(CC) $(CCFLAGS) -I$(INC_DIR) -c $< -o $(OBJ_DIR)/$@
%.cpp.o: %.cpp
	@echo "  CC $@"
	@$(CC) $(CCFLAGS) -I$(INC_DIR) -c $< -o $(OBJ_DIR)/$@

$(TARGET): $(OBJS)
	@echo "  LD $(TARGET)"
	@$(CC) -o $(BUILD_DIR)/$(TARGET) $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS)) $(LDFLAGS)