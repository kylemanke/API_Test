CC := g++
FLAGS := -Wall -Werror -Wextra -g

BIN := bin
SRC := src
LIB := lib

SOCKET := $(SRC)/socket
HTTP := $(SRC)/http

OBJECT_FILES := socket.o http_status_codes.o http_socket.o http_maps.o
OBJECT_FILES := $(patsubst %.o, $(BIN)/%.o, $(OBJECT_FILES))

U_HEADERS := types.h exception/exception.h
U_HEADERS := $(patsubst %, $(SRC)/%, $(U_HEADERS))

INCL_FLAGS := -I./$(SRC)/ -I./$(LIB)/

all: server

server:	$(SRC)/main.cpp $(OBJECT_FILES) $(HTTP)/* $(SOCKET)/*
	$(CC) $(FLAGS) $(INCL_FLAGS) $< $(OBJECT_FILES) -o $@

$(BIN)/socket.o: $(SOCKET)/socket.cpp $(SOCKET)/socket.h $(U_HEADERS)
	$(CC) $(FLAGS) $(INCL_FLAGS) -c $< -o $@

$(BIN)/http_socket.o: $(HTTP)/http_socket.cpp $(HTTP)/http_socket.h $(HTTP)/http_defines.h $(HTTP)/http_status_codes.h $(U_HEADERS)
	$(CC) $(FLAGS) $(INCL_FLAGS) -c $< -o $@

$(HTTP)/http_defines.h: $(LIB)/robin_hood.h

$(BIN)/%.o: $(HTTP)/%.cpp $(HTTP)/%.h
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf bin/*.o server