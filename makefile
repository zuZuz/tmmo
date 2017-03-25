CC=gcc

CFLAGS= -lpthread -c -ggdb -O0 -std=gnu99

CLIENT=tmmo_game
SERVER=tmmo_server

SERVER_SRC=$(wildcard src/server/*.c)
PRCSSR_SRC=$(wildcard src/game_processing/*.c)

SERVER_OBJ=$(SERVER_SRC:src/server/%.c=build/%.o)
PRCSSR_OBJ=$(PRCSSR_SRC:src/game_processing/%.c=build/%.o)

all: dirs server processer
	$(CC) -o bin/$(SERVER) build/*.o -lpthread -O0 -ggdb

dirs:
	mkdir -p bin
	mkdir -p build

server: $(SERVER_OBJ)

build/%.o: src/server/%.c
	$(CC) $(CFLAGS) -o $@ $<

processer: $(PRCSSR_OBJ)

build/%.o: src/game_processing/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm build/*
	rm bin/*
