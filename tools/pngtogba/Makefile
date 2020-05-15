CFILES := $(shell find -name '*.c')
OBJS := $(patsubst %.c,%.o,$(CFILES))
DEPFILES := $(patsubst %.c,%.d,$(CFILES))

CFLAGS := -std=c11 -O2 -flto \
	-Wall -Wextra -fno-strict-aliasing -Werror=implicit-function-declaration -Wstrict-prototypes -Wwrite-strings -Wuninitialized
LFLAGS := $(CFLAGS) -lz -lm

pngtogba: $(OBJS) Makefile
	$(CC) -o $@ $(OBJS) $(LFLAGS) 