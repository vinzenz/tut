CXX=g++
CXXOPTS=-c -pedantic -Wall -Iinclude -I.. -fPIC

LIB=libftrace
OBJS= $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: all
all: $(LIB)

$(LIB): $(OBJS)
	$(CXX) -shared -Wl,-soname,${LIB}.so -o ${LIB}.so ${OBJS}

$(OBJS):
	$(CXX) $(CXXOPTS) -c $*.cpp

.PHONY: debug
debug:

.PHONY: clean
clean:
	rm -f $(OBJS) $(LIB)*

