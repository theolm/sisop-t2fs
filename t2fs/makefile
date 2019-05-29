#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TARGETS= ./lib/t2fs.o ./lib/libt2fs.a
CFLAGS = -Wall -m32

all: ot2fs libt2fs

libt2fs: $(LIB_DIR)/apidisk.o $(LIB_DIR)/ot2fs.o
	ar crs $(LIB_DIR)/libt2fs.a $(LIB_DIR)/apidisk.o $(LIB_DIR)/ot2fs.o

ot2fs: $(LIB_DIR)/apidisk.o $(SRC_DIR)/t2fs.c
	$(CC) -c -g $(SRC_DIR)/t2fs.c -o $(LIB_DIR)/ot2fs.o $(CFLAGS)

clean:
	rm -rf $(TARGETS)


