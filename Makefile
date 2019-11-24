CC=g++ -std=c++11
INC=inc

all: clean build run

clean:
	rm -rf main *.o

build: 
	$(CC) -I$(INC) main.cpp -o main -lpng -lz -lpthread -lX11 

run:
	./main