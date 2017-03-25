CC=gcc

CFLAGS=-lpthread -c -ggdb -O0 -std=gnu99
GTKFLAGS=`pkg-config --cflags --libs gtk+-3.0`

CLIENT=tmmo
SERVER=server

SERVER_INCFLAGS=-I./src/server
PRCSSR_INCFLAGS=-I./src/game_processing

SERVER_SRC=$(wildcard src/server/*.c)
PRCSSR_SRC=$(wildcard src/game_processing/*.c)
CLIENT_SRC=$(wildcard src/client/*.c)

CLIENT_OBJ=$(CLIENT_SRC:src/client/%.c=build/client/%.o)
SERVER_OBJ=$(SERVER_SRC:src/server/%.c=build/server/%.o)
PRCSSR_OBJ=$(PRCSSR_SRC:src/game_processing/%.c=build/server/%.o)

all: dirs processer server client

dirs:
	mkdir -p bin/client
	mkdir -p bin/server
	
	mkdir -p build/client
	mkdir -p build/server

processer: $(PRCSSR_OBJ)

server: $(SERVER_OBJ)
	$(CC) -o bin/server/$(SERVER) build/server/*.o -lpthread

client: $(CLIENT_OBJ)
	$(CC) -o bin/client/$(CLIENT) build/client/*.o $(GTKFLAGS)
	cp src/client/gui/main.glade bin/client/

build/server/%.o: src/server/%.c
	$(CC) $(CFLAGS) $(PRCSSR_INCFLAGS) -o $@ $<

build/server/%.o: src/game_processing/%.c
	$(CC) $(CFLAGS) $(SERVER_INCFLAGS) -o $@ $<

build/client/%.o: src/client/%.c
	$(CC) $(CFLAGS) $(GTKFLAGS) -o $@ $<

clean:
	rm -r build/*
	rm -r bin/*