ARES_BIN := /Applications/ares.app/Contents/MacOS/ares

.PHONY: all build docker rebuild setup resetup ares flashair clean help

BUILD_DIR := build
SOURCE_DIR := src
ROM_NAME := TBD-64
BUILD_TYPE ?= debug

N64_MK_PATH := $(N64_INST)/include/n64.mk
ifneq (,$(wildcard $(N64_MK_PATH)))
include $(N64_MK_PATH)
endif

N64_CFLAGS += -Iinclude
ifeq ($(BUILD_TYPE),release)
N64_CFLAGS += -O3 -DNDEBUG
else
N64_CFLAGS += -O0 -g -DDEBUG
endif

all: build

build: ##    Create rom.
	@if command -v docker >/dev/null 2>&1; then \
		echo "Building rom inside docker environment..."; \
		$(MAKE) docker; \
	else \
		echo "Building rom..."; \
		$(MAKE) $(ROM_NAME).z64; \
	fi

docker: setup
	@docker run --user $(shell id -u):$(shell id -g) -v ${CURDIR}:/game build make BUILD_TYPE=$(BUILD_TYPE) $(ROM_NAME).z64

rebuild: clean build	##  Erase temp files and create the rom.

release: ##    Create release rom.
	@$(MAKE) BUILD_TYPE=release build

# gfx #
PNGS := $(wildcard resources/gfx/sprites/*.png) $(wildcard resources/gfx/sprites/*/*.png)
SPRITES := $(subst .png,.sprite,$(subst resources/,filesystem/,$(PNGS)))

# Convert all sprites to RGBA16
filesystem/gfx/sprites/%.sprite: resources/gfx/sprites/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) -f RGBA16 -o $(dir $@) $<

# sfx #
MP3S := $(wildcard resources/sfx/bgms/*.mp3)
BGMS := $(subst .mp3,.wav64,$(subst resources/,filesystem/,$(MP3S)))
filesystem/sfx/bgms/%.wav64: resources/sfx/bgms/%.mp3
	@mkdir -p $(dir $@)
	@echo "    [AUDIOCONV] $@"
	@$(N64_AUDIOCONV) --wav-compress 3 -o $(dir $@) $<

# code #
SRCS := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

-include $(DEPS)

$(ROM_NAME).z64: N64_ROM_TITLE = "TBD-64"

# Dependencies - let n64.mk handle the rules
$(BUILD_DIR)/$(ROM_NAME).elf: $(OBJS)
$(ROM_NAME).z64: $(BUILD_DIR)/$(ROM_NAME).dfs

# dfs #
$(BUILD_DIR)/$(ROM_NAME).dfs: $(SPRITES) $(BGMS)
	@mkdir -p ./filesystem/
	@echo `git rev-parse HEAD` > ./filesystem/hash
	@echo "    [DFS] $@"
	@$(N64_MKDFS) $@ ./filesystem/ >/dev/null

setup:		##    Create dev environment (docker image).
	@docker build --platform linux/amd64 -t build - < Dockerfile

resetup:	##  Force recreate the dev environment (docker image).
	@echo "Rebuilding dev environment in docker..."
	@docker build --platform linux/amd64 -t build --no-cache  - < Dockerfile

ares:		##    Start rom in Ares emulator.
	@echo "Starting ares..."
	$(ARES_BIN) $(ROM_NAME).z64

sd:			##    Flash rom to N64 EverDrive SD card.
	cp $(ROM_NAME).z64 /Volumes/N64/	

flashair: 	## Flash rom to EverDrive using a flashair SD card.
	curl -X POST -F 'file=@$(ROM_NAME).z64' http://flashair/upload.cgi

clean:		##    Cleanup temp files.
	@echo "Cleaning up temp files..."
	rm -rf $(BUILD_DIR) *.z64 *.elf src/*.o *.bin *.dfs filesystem/

help:		##     Show this help.
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/:.*##/:/' 
