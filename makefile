CC = gcc
CFLAGS = -Wall -g
LIB_DIR = "./lib"

all: main

util.o: util.c util.h
	${CC} ${CFLAGS} -c util.c

graphics.o: graphics.c graphics.h
	${CC} ${CFLAGS} -c graphics.c -L${LIB_DIR} -lglfw3 -lglew32s -lgdi32 -lOpenGL32

# main: main.c util.o graphics.o
# 	${CC} ${CFLAGS} -o $@ $^ -L${LIB_DIR} -lglfw3 -lglew32s -lgdi32 -lOpenGL32

main: main.c util.o graphics.o
	${CC} ${CFLAGS} -o $@ $^ -L${LIB_DIR} -lglfw3 -lglew32s -lgdi32 -lOpenGL32