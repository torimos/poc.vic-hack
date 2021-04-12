SRC_DIR = $(HOME)/src
INC_DIR = $(HOME)/include
BUILD_DIR = $(HOME)/build
OBJ_DIR = $(BUILD_DIR)/obj

PREFIX	 ?= arm-linux-gnueabi-
TARGET = vic-hack

CC	 = $(PREFIX)gcc

CCFLAGS +=
LDFLAGS +=

OBJS = $(patsubst $(SRC_DIR)/%, %.o, $(shell find $(SRC_DIR)/ -type f -name '*.*'))
DIRS = $(filter-out $(HOME),$(sort $(dir $(OBJS))))