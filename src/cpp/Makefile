# Makefile

CXX = g++ -m64
BIN = tetris
LDLIBS = -l pthread
SOURCE = tetris.cpp
CXXFLAGS = -std=c++2a -s -Wall -Os

${BIN}: ${SOURCE}
	${CXX} ${CXXFLAGS} ${LDLIBS} -o $@ $^

clean:
	rm -f ${BIN}
