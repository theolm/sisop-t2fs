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
LIB_DIR=../lib
INC_DIR=../include
BIN_DIR=../bin
SRC_DIR=./
TARGETS= teste
CFLAGS = -Wall -m32

all: teste

teste: teste.c $(LIB_DIR)/libt2fs.a
	$(CC) -g -o teste teste.c $(LIB_DIR)/libt2fs.a $(CFLAGS)

clean:
	rm -rf $(TARGETS)


