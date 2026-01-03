ARES_BIN := /Applications/ares.app/Contents/MacOS/ares

BUILD_DIR := build
SOURCE_DIR := src
ROM_NAME := TBD-64
N64_ROM_TITLE := "TBD-64"

N64_MK_PATH := $(N64_INST)/include/n64.mk
ifneq (,$(wildcard $(N64_MK_PATH)))
include $(N64_MK_PATH)
endif

N64_CFLAGS += -Iinclude
N64_ASFLAGS += -Iinclude

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
	@docker run --user $(shell id -u):$(shell id -g) -v ${CURDIR}:/game build make $(ROM_NAME).z64

rebuild: clean build	##  Erase temp files and create the rom.

# gfx #
PNGS := $(wildcard resources/gfx/sprites/*.png) $(wildcard resources/gfx/sprites/*/*.png)
SPRITES := $(subst .png,.sprite,$(subst resources/,filesystem/,$(PNGS)))
filesystem/gfx/sprites/%.sprite: resources/gfx/sprites/%.png
	@mkdir -p `echo $@ | xargs dirname`
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) -o $(dir $@) $<

# sfx #
MP3S := $(wildcard resources/sfx/bgms/*.mp3)
BGMS := $(subst .mp3,.wav64,$(subst resources/,filesystem/,$(MP3S)))
filesystem/sfx/bgms/%.wav64: resources/sfx/bgms/%.mp3
	@mkdir -p `echo $@ | xargs dirname`
	@echo "    [AUDIOCONV] $@"
	$(N64_AUDIOCONV) -d --wav-compress 3 --wav-loop false -o $(dir $@) $<

# code #
SRCS := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(ROM_NAME).elf: $(OBJS) $(N64_LIBDIR)/libdragon.a $(N64_LIBDIR)/libdragonsys.a $(N64_LIBDIR)/n64.ld
	@mkdir -p $(dir $@)
	@echo "    [LD] $@"
	$(N64_CXX) -o $@ $(filter %.o, $^) $(filter-out $(N64_LIBDIR)/libdragon.a $(N64_LIBDIR)/libdragonsys.a, $(filter %.a, $^)) \
		-lc -mabi=o64 $(patsubst %,-Wl$(COMMA)%,$(LDFLAGS)) -Wl,-Map=$(BUILD_DIR)/$(ROM_NAME).map
	$(N64_SIZE) -G $@

$(ROM_NAME).z64: $(BUILD_DIR)/$(ROM_NAME).elf
$(ROM_NAME).z64: $(ROM_NAME).dfs

# dfs #
$(ROM_NAME).dfs: $(SPRITES) $(BGMS)
	@mkdir -p ./filesystem/
	@echo `git rev-parse HEAD` > ./filesystem/hash
	$(N64_MKDFS) $@ ./filesystem/ >/dev/null

setup:		##    Create dev environment (docker image).
	@docker build --platform linux/amd64 -t build - < Dockerfile

resetup:	##  Force recreate the dev environment (docker image).
	@echo "Rebuilding dev environment in docker..."
	@docker build --platform linux/amd64 -t build --no-cache  - < Dockerfile

ares:		##    Start rom in Ares emulator.
	@echo "Starting ares..."
	$(ARES_BIN) $(ROM_NAME).z64

flashair: 	## Flash rom to EverDrive using a flashair SD card.
	curl -X POST -F 'file=@$(ROM_NAME).z64' http://flashair/upload.cgi

clean:		##    Cleanup temp files.
	@echo "Cleaning up temp files..."
	rm -rf $(BUILD_DIR) *.z64 *.elf src/*.o *.bin *.dfs filesystem/

help:		##     Show this help.
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/:.*##/:/' 
