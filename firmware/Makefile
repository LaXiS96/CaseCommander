# https://github.com/libopencm3/libopencm3-template/blob/master/my-project/Makefile

PROJECT = CaseCommander
BUILD_DIR = bin

CFILES := $(shell find src/ -name '*.c')
AFILES := $(shell find src/ -name '*.S')

DEVICE = stm32f103c8t6
OOCD_FILE = openocd.cfg

OPENCM3_DIR = lib/libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
