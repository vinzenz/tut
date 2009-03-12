PLUGIN=distributed
TGT=simple
OBJS=simple.o
ARCH=x86
TUT=../../../..
CXX=g++
CXXOPTS=-g -finstrument-functions -Wall -c 
LNK=g++
LNKOPTS=-lrt -o 
RM=rm -f


all: $(TGT)

clean: 
	$(RM) $(OBJS) $(TGT)

$(TGT): $(OBJS)
	$(LNK) $(LNKOPTS) $(TGT) $(OBJS) $(TUT)/plugins/$(PLUGIN)/lib_tut_$(PLUGIN)_client_$(ARCH).a

simple.o : simple.cpp
	$(CXX) $(CXXOPTS)simple.cpp 
