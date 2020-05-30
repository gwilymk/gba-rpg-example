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

CMAIN := src/main.c
CFILES  := $(shell find src -type f -name '*.c' -not -name 'main.c') $(shell find lostgba/src -type f -name '*.c')
ASMFILES := $(shell find src -type f -name '*.s') $(shell find lostgba/src -type f -name '*.s')
ASMMACROFILES := $(shell find src -type f -name '*.i') $(shell find lostgba/src -type f -name '*.i')
HFILES  := $(shell find -name '*.h')
MAINOBJ := src/main.o
OBJS    := $(patsubst %.c,%.o,$(CFILES)) $(patsubst %.s,%.o,$(ASMFILES)) $(IMAGE_OBJS) $(TILEMAP_OBJS)
TESTCFILES := $(CFILES) $(shell find test -type f -name '*.c') $(shell find lostgba/test -type f -name '*.c')
TESTOBJS := $(patsubst %.c,%.to,$(TESTCFILES)) $(patsubst %.s,%.to,$(ASMFILES)) $(IMAGE_OBJS) $(TILEMAP_OBJS)
DEPS    := $(patsubst %.c,%.d,$(CFILES)) $(patsubst %.c,%.td,$(TESTCFILES)) src/main.d

# --- Build defines ---------------------------------------------------

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

CFLAGS_COMMON := -std=gnu11 -Wall -Wextra -fno-strict-aliasing -Werror=implicit-function-declaration -Wstrict-prototypes -Wwrite-strings -Wuninitialized -Werror=return-type

HOSTCC  := gcc
HOSTLD  := $(HOSTCC)

HOST_CFLAGS := $(CFLAGS_COMMON) -O3 -flto -g
HOST_LDFLAGS := $(HOST_CFLAGS) -lz -lm

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

INCLUDES := -I. -Ilostgba/include -Iinclude
OPTFLAGS := -O2 -flto

MGBA_DEFINES =
ifdef MGBA
  MGBA_DEFINES = -DLOSTGBA_MGBA_TARGET
endif

CFLAGS  := $(ARCH) -g $(CFLAGS_COMMON) $(INCLUDES) $(MGBA_DEFINES)

PNGTOGBA := lostgba/tools/pngtogba/pngtogba

LDFLAGS := $(ARCH) $(SPECS) -g

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

.PHONY : build test clean default docs dump gdb gdb-test dump dump-test
.SUFFIXES:
.SUFFIXES: .c .o .to .s .h .png .dump .gba .elf

gdb: $(TARGET).elf
	$(PREFIX)gdb $(TARGET).elf

gdb-test: $(TARGET)-test.elf
	$(PREFIX)gdb $(TARGET)-test.elf

docs: docs/html/index.html

docs/html/index.html: Makefile Doxyfile $(CFILES) $(HFILES)
	@echo [DOXYGEN]
	@-doxygen

dump: $(TARGET).dump

dump-test: $(TARGET)-test.dump

%.dump: %.elf Makefile
	@echo [OBJDUMP] $<
	@$(PREFIX)objdump -Sd $< > $@

.SECONDARY: $(TILEMAP_HEADERS) $(IMAGE_CFILES)

%.o : %.c
%.o : %.s

%.o : %.c Makefile $(TILEMAP_HEADERS)
	@echo [CC] $<
	@$(CC) -c $< $(CFLAGS) $(OPTFLAGS) -o $@ -MMD -MP

%.o : %.s Makefile $(ASMMACROFILES)
	@echo [ASM] $<
	@$(CC) -c $< $(CFLAGS) $(OPTFLAGS) -I$(*D) -o $@ -MMD -MP

%.to : %.c Makefile
	@echo [TESTCC] $<
	@$(CC) -c $< $(CFLAGS) -o $@ -MMD -MP -MF $*.td -DLOSTGBA_TEST

%.to : %.s Makefile $(ASMMACROFILES)
	@echo [TESTASM] $<
	@$(CC) -c $< $(CFLAGS) -I$(*D) -o $@ -MMD -MP -DLOSTGBA_TEST

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
test: $(TARGET)-test.gba

%.gba : %.elf
	@echo [OBJCOPY] $<
	@$(OBJCOPY) -O binary $< $@
	@echo [GBAFIX] $@
	@gbafix $@ > /dev/null

$(TARGET).elf : $(OBJS) $(MAINOBJ)
	@echo [LD] $@
	@$(LD) $^ $(LDFLAGS) $(OPTFLAGS) -o $@

$(TARGET)-test.elf : $(TESTOBJS)
	@echo [LD] $@
	@$(LD) $^ $(LDFLAGS) -o $@

# --- Clean -----------------------------------------------------------

.PHONY: clean
clean :
	@rm -fv $(TARGET).gba $(TARGET).elf $(TARGET).dump $(TARGET)-test.gba $(TARGET)-test.elf $(TARGET)-test.dump
	@rm -fv $(OBJS) $(MAINOBJ) $(DEPS) $(TESTOBJS)
	@rm -fv images/*.c
	@rm -fv $(PNGTOGBA) $(PNGTOGBA_OBJS) $(PNGTOGBA_DEPS)

-include $(DEPS) $(PNGTOGBA_DEPS)