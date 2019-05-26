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
CFLAGS=-m32 -std=c99
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
BIN_TESTE=./exemplo
SRC_TESTE=./teste

all: $(LIB_DIR)/libt2fs.a

$(LIB_DIR)/libt2fs.a: $(SRC_DIR)/libt2fs.c
	$(CC) $(CFLAGS) -c -o $(LIB_DIR)/libt2fs.a -I$(INC_DIR) $(SRC_DIR)/libt2fs.c

teste: $(BIN_TESTE)/teste.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/libt2fs.a
	$(CC) $(CFLAGS) $(BIN_TESTE)/teste.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/libt2fs.a -o $(BIN_TESTE)/teste -lm

$(BIN_TESTE)/teste.o: $(SRC_TESTE)/teste.c
	$(CC) $(CFLAGS) -c -o $(BIN_TESTE)/teste.o -I$(INC_DIR)/ $(SRC_TESTE)/teste.c

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/* $(BIN_TESTE)/* $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

cleandisk:
	cp t2fs_disk_clean.dat t2fs_disk.dat


