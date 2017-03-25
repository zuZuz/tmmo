CC=gcc
CFLAGS= -lpthread -c -ggdb -O0 -std=gnu99
GTKFLAGS=`pkg-config --cflags --libs gtk+-3.0`

CLIENT=tmmo_client
SERVER=tmmo_server

SERVER_INCFLAGS=-I./src/server
PRCSSR_INCFLAGS=-I./src/game_processing

SERVER_SRC=$(wildcard src/server/*.c)
PRCSSR_SRC=$(wildcard src/game_processing/*.c)
CLIENT_SRC=$(wildcard src/client/*.c)

CLIENT_OBJ=$(CLIENT_SRC)
SERVER_OBJ=$(SERVER_SRC:src/server/%.c=build/%.o)
PRCSSR_OBJ=$(PRCSSR_SRC:src/game_processing/%.c=build/%.o)

all: dirs processer server client

dirs:
	mkdir -p bin
	mkdir -p build

client:
	$(CC) -o bin/$(CLIENT) $(GTKFLAGS) build/*.o

server: $(SERVER_OBJ)
	$(CC) -o bin/$(SERVER) build/*.o -lpthread

processer: $(PRCSSR_OBJ)

build/%.o: src/server/%.c
	$(CC) $(CFLAGS) $(PRCSSR_INCFLAGS) -o $@ $<

build/%.o: src/game_processing/%.c
	$(CC) $(CFLAGS) $(SERVER_INCFLAGS) -o $@ $<

build/%.o: src/client/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm build/*
	rm bin/*