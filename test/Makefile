CXX = g++
CXXFLAGS = -Wall -O
LD = g++
OBJS = test-reversi-output.o board.o util.o

all: test-reversi-output

test-reversi-output: $(OBJS)
	$(LD) $^ -o $@

clean:
	rm -f test-reversi-output $(OBJS)

board.o: board.cpp board.h cell.h move.h
test-reversi-output.o: test-reversi-output.cpp util.h move.h cell.h \
 board.h
util.o: util.cpp util.h move.h cell.h
