CXX=g++
CXXOPTS=-c -Wall 
AR=ar
AROPTS=ru
RM=rm -f

LIB=libreflection
OBJS=reflection.o namespace.o structure.o member.o method.o errors.o

all: $(LIB)

clean: 
	$(RM) $(OBJS) $(PROGRAM) $(LIB).a

$(LIB): $(OBJS)
	$(AR) $(AROPTS) $(LIB).a $(OBJS)

reflection.o : reflection.cpp
	$(CXX) $(CXXOPTS)reflection.cpp 

namespace.o : namespace.cpp
	$(CXX) $(CXXOPTS)namespace.cpp 

structure.o : structure.cpp
	$(CXX) $(CXXOPTS)structure.cpp

member.o : member.cpp
	$(CXX) $(CXXOPTS)member.cpp 

method.o : method.cpp
	$(CXX) $(CXXOPTS)method.cpp 

errors.o : errors.cpp
	$(CXX) $(CXXOPTS)errors.cpp 

