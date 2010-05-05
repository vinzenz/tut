TGT=reflection

.PHONY: all
all:
	g++ -g -Wall -I../include/ -o $(TGT) $(TGT).cpp -L.. -lcppreflection

run:
	LD_LIBRARY_PATH=.. ./$(TGT)

.PHONY: clean
clean:
	rm -f $(TGT)

