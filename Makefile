LIB = lib
UTILS = $(LIB)/Utils

BIN = bin

LIB_FLAGS = -I $(LIB)
DEBUG_FLAGS = -g -Wall $(LIB_FLAGS)

clean:
	rm -rf bin/*