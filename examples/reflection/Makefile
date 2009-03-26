TGT=ref
OBJS=ref.o
CXX=g++
CXXOPTS=-g -c 
LNK=g++
LNKOPTS=-g -o
RM=rm -f

all: $(TGT)

clean: 
	$(RM) $(OBJS) $(TGT)

$(TGT): $(OBJS)
	$(LNK) $(LNKOPTS) $(TGT) $(OBJS) ../libreflection.a

ref.o : ref.cpp
	$(CXX) $(CXXOPTS)ref.cpp
