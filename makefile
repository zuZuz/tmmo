CC=gcc

NAME=client

GTKFLAGS=`pkg-config --cflags --libs gtk+-3.0`
CFLAGS=

SRCS=$(wildcard src/client/*.c)

main: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(GTKFLAGS) -o $(NAME)

