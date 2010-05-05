.SILENT:
all:
	#g++ -c  libunwind.c #-finstrument-functions -funwind-tables
	#x86_64-pc-linux-gnu-gcc -c -g libunwind.c #-finstrument-functions -funwind-tables
	#g++ -o simple simple.c libunwind.o -g -finstrument-functions -funwind-tables
	#x86_64-pc-linux-gnu-gcc -o simple simple.c libunwind.o -g -finstrument-functions -funwind-tables
	#g++ -o my.g++ my.c libunwind.o -lelf -ldw -finstrument-functions  -funwind-tables -g -fno-inline
	#g++ -Wall -o my.g++ my.c -lelf -ldw -finstrument-functions  -funwind-tables -g -fno-inline
	g++ -o my.g++ my.c -lelf -ldw -finstrument-functions  -funwind-tables -g -fno-inline

clean:
	rm -f *.o simple

