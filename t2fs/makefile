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
TARGETS=./lib/ot2fs.o ./lib/libaux.o ./lib/libt2fs.a
CFLAGS = -Wall -m32

all: libaux ot2fs libt2fs teste

teste: ./teste/teste.c $(LIB_DIR)/libt2fs.a
	$(CC) -g -o ./teste.exe ./teste/teste.c $(LIB_DIR)/libt2fs.a $(CFLAGS)

libt2fs: $(LIB_DIR)/apidisk.o $(LIB_DIR)/ot2fs.o $(LIB_DIR)/libaux.o
	ar crs $(LIB_DIR)/libt2fs.a $(LIB_DIR)/apidisk.o $(LIB_DIR)/ot2fs.o $(LIB_DIR)/libaux.o

ot2fs: $(LIB_DIR)/libaux.o $(LIB_DIR)/apidisk.o $(SRC_DIR)/t2fs.c
	$(CC) -c -g $(SRC_DIR)/t2fs.c -o $(LIB_DIR)/ot2fs.o $(CFLAGS)

libaux: $(LIB_DIR)/apidisk.o $(SRC_DIR)/libaux.c
	$(CC) -c -g $(SRC_DIR)/libaux.c -o $(LIB_DIR)/libaux.o $(CFLAGS)


clean:
	rm -rf $(TARGETS) ./teste.exe


