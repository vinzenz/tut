TGT=tutgen
CXX=g++
CXXOPTS=-Wall -c 
LNK=g++
LNKOPTS=-o 
RM=rm -f

OBJS=entry.o parser.o config.o tutgen.o

all: $(TGT)

clean: 
	$(RM) $(OBJS) $(TGT)

$(TGT): $(OBJS)
	$(LNK) $(LNKOPTS) $(TGT) $(OBJS)

config.o : config.cpp
	$(CXX) $(CXXOPTS)config.cpp 

entry.o : entry.cpp
	$(CXX) -g $(CXXOPTS)entry.cpp 

parser.o : parser.cpp
	$(CXX) $(CXXOPTS)parser.cpp

tutgen.o : tutgen.cpp
	$(CXX) $(CXXOPTS)tutgen.cpp
