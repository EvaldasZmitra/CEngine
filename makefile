CC = gcc
CFLAGS = -Wall -g
LIB_DIR = "./lib"
SRC_DIR = ./src

all: main

linear_algebra.o: ${SRC_DIR}/math/linear_algebra.c ${SRC_DIR}/math/linear_algebra.h
	${CC} ${CFLAGS} -c ${SRC_DIR}/math/linear_algebra.c 

geometry.o: ${SRC_DIR}/math/geometry.c ${SRC_DIR}/math/geometry.h
	${CC} ${CFLAGS} -c ${SRC_DIR}/math/geometry.c 

rendering.o: ${SRC_DIR}/rendering/rendering.c ${SRC_DIR}/rendering/rendering.h
	${CC} ${CFLAGS} -c ${SRC_DIR}/rendering/rendering.c -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32

main: ${SRC_DIR}/main.c rendering.o
	${CC} ${CFLAGS} -o $@ $^ -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32