# https://github.com/libopencm3/libopencm3-template/blob/master/my-project/Makefile

PROJECT = CaseCommander
BUILD_DIR = bin

# FreeRTOS sources
CFILES += lib/FreeRTOS/portable/GCC/ARM_CM3/port.c \
		  lib/FreeRTOS/portable/MemMang/heap_4.c \
		  lib/FreeRTOS/tasks.c \
		  lib/FreeRTOS/queue.c \
		  lib/FreeRTOS/list.c \
		  lib/FreeRTOS/stream_buffer.c
INCLUDES += -Ilib/FreeRTOS/include \
			-Ilib/FreeRTOS/portable/GCC/ARM_CM3

# Project sources
SRC_CFILES := $(shell find src/ -name '*.c')
SRC_AFILES := $(shell find src/ -name '*.S')
CFILES += $(SRC_CFILES)
AFILES += $(SRC_AFILES)
INCLUDES += -Iinc

DEVICE = stm32f103c8t6
# OOCD_FILE = openocd.cfg

OPENCM3_DIR = lib/libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
