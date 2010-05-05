CXX=g++
CXXOPTS=-c -Wall -Iinclude -fPIC

LIB=libcppreflection
OBJS= $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: all
all: $(LIB)

$(LIB): $(OBJS)
	$(CXX) -shared -Wl,-soname,${LIB}.so -o ${LIB}.so ${OBJS}

$(OBJS):
	$(CXX) $(CXXOPTS) -c $*.cpp

.PHONY: clean
clean:
	rm -f $(OBJS) $(LIB)*

