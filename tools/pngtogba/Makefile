CFILES := $(shell find -name '*.c')
OBJS := $(patsubst %.c,%.o,$(CFILES))
DEPFILES := $(patsubst %.c,%.d,$(CFILES))

CFLAGS := -std=c11 -g -Og -flto \
	-Wall -Wextra -fno-strict-aliasing -Werror=implicit-function-declaration -Wstrict-prototypes -Wwrite-strings -Wuninitialized -Werror=return-type \
	-MMD -MP
LFLAGS := $(CFLAGS) -lz -lm

.PHONY: default
default: pngtogba

.PHONY: clean
clean:
	rm -vf $(OBJS) $(DEPFILES)

pngtogba: $(OBJS) Makefile
	$(CC) -o $@ $(OBJS) $(LFLAGS) 

-include $(DEPFILES)