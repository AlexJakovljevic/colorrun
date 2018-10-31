PROGRAM   = colorrun
CC        = gcc
CFLAGS    = -g -Wall -std=c99
LDFLAGS   = -lGL -lGLU -lglut -lm -lSOIL

$(PROGRAM): main.o
	$(CC) -o $(PROGRAM) main.o  $(LDFLAGS) $(CFLAGS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM)

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)

