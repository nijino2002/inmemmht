CC_FLAGS = -g
CC = gcc $(CC_FLAGS)
OBJ = defs.o mhtdefs.o sha256.o mht.o dataelem.o
LIBS = -lm

all : test_1
.PHONY : all

test_1 : test_1.o $(OBJ)
	$(CC) -o test_1 test_1.o $(OBJ) $(LIBS)

$(OBJ) : defs.h mhtdefs.h sha256.h dataelem.h mht.h

.PHONY : clean
clean : 
	rm -rf test_1 test_1.o $(OBJ)
