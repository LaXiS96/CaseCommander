# https://github.com/libopencm3/libopencm3-template/blob/master/my-project/Makefile

PROJECT = CaseCommander
BUILD_DIR = bin

CFILES := $(shell find src/ -name '*.c')
AFILES := $(shell find src/ -name '*.S')

DEVICE = stm32f103c8t6
# TODO check OpenOCD settings
OOCD_INTERFACE = jlink
OOCD_TARGET = stm32f1x

OPENCM3_DIR = lib/libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk