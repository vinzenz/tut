GCC=g++
CFLAGS=-I. -I../src/include -g -O2 -funwind-tables -finstrument-functions

TARGET_SIMPLE=simple
OBJS_SIMPLE=simple.o ../src/libunwind.o

TARGET_CYG=cyg
OBJS_CYG=cyg.o ../src/libunwind.o

all: $(TARGET_CYG) $(TARGET_SIMPLE)

$(TARGET_CYG): $(OBJS_CYG)
	$(GCC) $(CFLAGS) -o $(TARGET_CYG) $(OBJS_CYG)

$(TARGET_SIMPLE): $(OBJS_SIMPLE)
	$(GCC) $(CFLAGS) -o $(TARGET_SIMPLE) $(OBJS_SIMPLE)

../src/libunwind.o: ../src/
	cd ../src && $(MAKE)

%.o: *.c *.h
	$(GCC) $(CFLAGS) -c $*.c 

clean:
	-rm -f $(OBJS_SIMPLE) $(OBJS_CYG) $(TARGET_SIMPLE) $(TARGET_CYG)

.PHONY: all clean
