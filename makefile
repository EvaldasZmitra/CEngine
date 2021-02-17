CC = gcc
CFLAGS = -Wall -g
LIB_DIR = "./lib"

all: main

util.o: util.c util.h
	${CC} ${CFLAGS} -c util.c -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32

main: main.c util.o
	${CC} ${CFLAGS} -o $@ $^ -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32