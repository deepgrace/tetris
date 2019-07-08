# Makefile

CC = gcc -m64
BIN = tetris
LDLIBS = -l pthread
SOURCE = tetris.c
CFLAGS = -std=gnu17 -s -Wall -Os

${BIN}: ${SOURCE}
	${CC} ${CFLAGS} ${LDLIBS} -o $@ $^

clean:
	rm -f ${BIN}
