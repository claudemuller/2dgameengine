CC=g++
FLAGS=-Wall -D_THREAD_SAFE
INCS=-I/opt/homebrew/include -I/opt/homebrew/include/SDL2
LIBS=-L/opt/homebrew/lib -lSDL2
SRC=./src/*.cpp
BIN=game

all: clean build run

build:
	$(CC) $(SRC) $(FLAGS) $(INCS) $(LIBS) -o $(BIN)

debug:
	$(CC) -g $(SRC) $(FLAGS) $(INCS) $(LIBS) -o debug

run:
	./$(BIN)

clean:
	rm -rf $(BIN)
