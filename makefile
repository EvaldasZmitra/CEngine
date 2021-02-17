CC = gcc
CFLAGS = -Wall -g
LIB_DIR = "./lib"
SRC_DIR = ./src

all: main

util.o: ${SRC_DIR}/util.c ${SRC_DIR}/util.h
	${CC} ${CFLAGS} -c ${SRC_DIR}/util.c -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32

main: ${SRC_DIR}/main.c util.o
	${CC} ${CFLAGS} -o $@ $^ -L${LIB_DIR} -lglfw3 -lglew64s -lgdi32 -lOpenGL32