CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TARGETS=./lib/ot2fs.o ./lib/libaux.o ./lib/libt2fs.a
CFLAGS = -Wall -m32

teste: ./t2shell.c ../$(LIB_DIR)/libt2fs.a
	$(CC) -g -o ../t2shell.exe ./t2shell.c ../$(LIB_DIR)/libt2fs.a $(CFLAGS)
