# https://github.com/libopencm3/libopencm3-template/blob/master/my-project/Makefile

PROJECT = CaseCommander
BUILD_DIR = bin

# Project sources
CFILES := $(shell find src/ -name '*.c')
AFILES := $(shell find src/ -name '*.S')
INCLUDES += -Iinc

# FreeRTOS sources
CFILES += lib/FreeRTOS/portable/GCC/ARM_CM3/port.c \
		  lib/FreeRTOS/portable/MemMang/heap_4.c \
		  lib/FreeRTOS/tasks.c \
		  lib/FreeRTOS/queue.c \
		  lib/FreeRTOS/list.c \
		  lib/FreeRTOS/stream_buffer.c
INCLUDES += -Ilib/FreeRTOS/include \
			-Ilib/FreeRTOS/portable/GCC/ARM_CM3

DEVICE = stm32f103c8t6
OOCD_FILE = openocd.cfg

OPENCM3_DIR = lib/libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
