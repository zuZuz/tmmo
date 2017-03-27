
PROG=data_base

all: $(PROG)

clean:
	rm $(PROG)

$(PROG): src/data_base/src/main.c
	cc -Ofast -I sglib-1.0.4/ -o $(PROG) src/data_base/src/main.c -lc -luv
