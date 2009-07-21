ARCH=x86

GCC=gcc
CFLAGS=-DHAVE_CONFIG_H -I. -Iinclude -Iinclude/tdep-$(ARCH) -I. -D_GNU_SOURCE -DNDEBUG -g -O2 -fexceptions -Wall -Wsign-compare
AR=ar

TARGET=libunwind
OBJS=os-linux.o elf32.o\
     $(patsubst %.c,%.o,$(wildcard arch/$(ARCH)/*.c))\
     $(patsubst %.c,%.o,$(wildcard unwind/*.c))\
     $(patsubst %.c,%.o,$(wildcard dwarf/*.c))\

all: $(OBJS)
	$(AR) ru $(TARGET).o $(OBJS)

$(OBJS): *.c *.h
	$(GCC) $(CFLAGS) -c $*.c -o $*.o

clean:
	-rm -f $(OBJS) $(TARGET).o

.PHONY: all clean
