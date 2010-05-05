CXX=g++ -Wall -g

all:
	$(CXX) -I /usr/include/elfutils/ -I. -c nm.cpp
	$(CXX) -o nm nm.o -ldl -lelf -ldw -lebl
