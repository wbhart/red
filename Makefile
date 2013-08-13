INC=-I/home/wbhart/gc/include
LIB=-L/home/wbhart/gc/lib
OBJS=backend.o types.o symbol.o environment.o
HEADERS=backend.h types.h symbol.h environment.h
CC=g++
RED_FLAGS=-O2 -g -std=c++11

red: red.c $(HEADERS) $(OBJS)
	$(CC) $(RED_FLAGS) red.c -o red $(INC) $(OBJS) $(LIB) -lgc

backend.o: backend.c $(HEADERS)
	$(CC) $(RED_FLAGS) -c backend.c -o backend.o $(INC)

types.o: types.c $(HEADERS)
	$(CC) $(RED_FLAGS) -c types.c -o types.o $(INC)

environment.o: environment.c $(HEADERS)
	$(CC) $(RED_FLAGS) -c environment.c -o environment.o $(INC)

symbol.o: symbol.c $(HEADERS)
	$(CC) $(RED_FLAGS) -c symbol.c -o symbol.o $(INC)

clean:
	rm -f *.o
	rm -f red

.PHONY: clean 
