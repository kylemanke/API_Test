LIB = lib
UTILS = $(LIB)/Utils

BIN = bin

LIB_FLAGS = -I $(LIB)
DEBUG_FLAGS = -g -Wall $(LIB_FLAGS)

socket_deque: $(UTILS)/socket_deque.cpp $(UTILS)/socket_deque.h
	g++ $(DEBUG_FLAGS) -c $(UTILS)/socket_deque.cpp -o $(BIN)/socket_deque.o

clean:
	rm -rf bin/*