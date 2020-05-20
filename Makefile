PATH := $(DEVKITARM)/bin:$(PATH)

# --- Project details -------------------------------------------------

PROJ    := rpg-example
TARGET  := $(PROJ)

IMAGES := $(shell find images -name '*.png')
IMAGE_OBJS := $(patsubst %.png,%.png.o,$(IMAGES))
IMAGE_CFILES := $(patsubst %.png,%.png.c,$(IMAGES))

TILEMAPS := $(shell find tilemaps -name '*.csv')
TILEMAP_OBJS := $(patsubst %.csv,%.o,$(TILEMAPS))
TILEMAP_HEADERS := $(patsubst %.csv,%.h,$(TILEMAPS))

CFILES  := $(shell find src -type f -name '*.c') $(shell find lostgba/src -type f -name '*.c')
HFILES  := $(shell find -name '*.h')
OBJS    := $(patsubst %.c,%.o,$(CFILES)) $(IMAGE_OBJS) $(TILEMAP_OBJS)
DEPS    := $(patsubst %.c,%.d,$(CFILES))

# --- Build defines ---------------------------------------------------

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

CFLAGS_COMMON := -std=gnu11 -Wall -Wextra -fno-strict-aliasing -Werror=implicit-function-declaration -Wstrict-prototypes -Wwrite-strings -Wuninitialized -Werror=return-type

HOSTCC  := gcc
HOSTLD  := $(HOSTCC)

HOST_CFLAGS := $(CFLAGS_COMMON) -O3 -flto -g
HOST_LDFLAGS := $(HOSTCC_FLAGS) -lz -lm

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

INCLUDES := -I. -Ilostgba/include -Iinclude
CFLAGS  := $(ARCH) -O2 -flto -g $(CFLAGS_COMMON) $(INCLUDES)

PNGTOGBA := lostgba/tools/pngtogba/pngtogba

LDFLAGS := $(ARCH) $(SPECS) -flto -g -O2

default: build

lostgba/tools/%.o : lostgba/tools/%.c Makefile
	@echo [HOSTCC] $<
	@$(HOSTCC) $(HOST_CFLAGS) -MMD -MP -c $< -o $@

#### PNGTOGBA ####

PNGTOGBA_CFILES := $(shell find ./lostgba/tools/pngtogba -name '*.c')
PNGTOGBA_DEPS := $(patsubst %.c,%.d,$(PNGTOGBA_CFILES))
PNGTOGBA_OBJS := $(patsubst %.c,%.o,$(PNGTOGBA_CFILES))

$(PNGTOGBA): $(PNGTOGBA_OBJS)
	@echo [HOSTLD] $@
	@$(HOSTLD) $(HOST_LDFLAGS) -o $@ $(PNGTOGBA_OBJS)

#### END PNGTOGBA ####

.PHONY : build clean default docs dump gdb
.SUFFIXES:
.SUFFIXES: .c .o .s .h .png

gdb: $(TARGET).elf
	$(PREFIX)gdb $(TARGET).elf

docs: docs/html/index.html

docs/html/index.html: Makefile Doxyfile $(CFILES) $(HFILES)
	@echo [DOXYGEN]
	@-doxygen

dump: $(TARGET).dump

$(TARGET).dump: $(TARGET).elf Makefile
	@echo [OBJDUMP]
	@$(PREFIX)objdump -Sd $< > $@

.SECONDARY: $(TILEMAP_HEADERS) $(IMAGE_CFILES)

%.o : %.c
%.o : %.s

%.o : %.c Makefile $(TILEMAP_HEADERS)
	@echo [CC] $<
	@$(CC) -c $< $(CFLAGS) -o $@ -MMD -MP

%.o : %.s Makefile
	@echo [ASM] $<
	@$(CC) -c $< $(CFLAGS) -o $@

%.png.c: %.png.h %.png $(PNGTOGBA) Makefile
	@echo [PNGTOGBA] $<
	-@$(PNGTOGBA) $<

tilemaps/%.c tilemaps/%.h : tilemaps/%.csv Makefile
	@echo [TILEMAP] $<
	@(echo "#pragma once" && echo "extern int $(*F)Tilemap[];") > tilemaps/$*.h
	@(echo "int $(*F)Tilemap[] = {" && sed -e 's/$$/,/' "$<" && echo "};") > tilemaps/$*.c 

# --- Build -----------------------------------------------------------
# Build process starts here!
build: $(TARGET).gba

$(TARGET).gba : $(TARGET).elf
	@echo [OBJCOPY] $@
	@$(OBJCOPY) -v -O binary $< $@ > /dev/null
	@gbafix $@ > /dev/null

$(TARGET).elf : $(OBJS)
	@echo [LD] $@
	@$(LD) $^ $(LDFLAGS) -o $@

# --- Clean -----------------------------------------------------------

.PHONY: clean
clean :
	@rm -fv $(TARGET).gba $(TARGET).elf $(TARGET).dump
	@rm -fv $(OBJS) $(DEPS)
	@rm -fv images/*.c
	@rm -fv $(PNGTOGBA) $(PNGTOGBA_OBJS) $(PNGTOGBA_DEPS)

-include $(DEPS) $(PNGTOGBA_DEPS)