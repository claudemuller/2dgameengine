CC=g++
CFLAGS=-Wall -Wfatal-errors -std=c++17
INCS=-I/opt/homebrew/include -I./libs/ -I./libs/lua/
LIBS=-L/opt/homebrew/lib
LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua
SRC=./src/*.cpp ./src/**/*.cpp
BIN=gameengine

all: clean build run

build:
	$(CC) $(SRC) $(CFLAGS) $(INCS) $(LIBS) $(LFLAGS) -o $(BIN)

debug:
	$(CC) -g $(SRC) $(CFLAGS) $(INCS) $(LIBS) $(LFLAGS) -o debug

run:
	./$(BIN)

clean:
	rm -rf $(BIN) debug
