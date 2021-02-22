CC = gcc
CFLAGS = -Wall -g
LIB_DIR = "./lib"
SRC_DIR = ./src

main: ${SRC_DIR}/main.c graphics.o util.o cmath.o
	${CC} ${CFLAGS} -o $@ $^ -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32

util.o: ${SRC_DIR}/util.c
	${CC} ${CFLAGS} -c ${SRC_DIR}/util.c

cmath.o: ${SRC_DIR}/cmath.c
	${CC} ${CFLAGS} -c ${SRC_DIR}/cmath.c

graphics.o: ${SRC_DIR}/graphics.c ${SRC_DIR}/graphics.h
	${CC} ${CFLAGS} -c ${SRC_DIR}/graphics.c -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32

