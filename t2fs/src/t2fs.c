
/**
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#define SECTOR_SIZE 256


int particao = 0; // Partição selecionada - primeira por definição do T2
int setoresPorBloco;
int numeroSetores;
int numeroBlocos;
int tamanhoBitmap;
int numeroBlocosBitmap;
int mapaEspaco[];
Mbr mbr;

char *converteByteParaHex(BYTE valor) {
	char *hex = malloc(sizeof(char));
	sprintf(hex, "%x", valor);
	return hex;
}

int converteHexParaInt(char *hex) {
	return (int)strtol(hex, NULL, 16);
}

char *byteToStr(int i) {
	char *s = malloc(sizeof(char));
	sprintf(s, "%1d", i);
	return s;
}

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

void montaMbr() {
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, buffer);
	char *s = strcat(converteByteParaHex(buffer[1]), converteByteParaHex(buffer[0]));
	char c[strlen(s)];
	strcpy(c, s);
	char d[3];
	substring(c, d, 1, 3);
	int ano = converteHexParaInt(d);
	char e[3];
	substring(c, e, 4, 1);
	int semestre = converteHexParaInt(e);

	mbr.versaoDisco = strcat(byteToStr(ano), byteToStr(semestre));
	mbr.tamanhoSetor = buffer[3] * 256 + buffer[2];
	mbr.inicioTabelaParticoes = buffer[5] * 256 + buffer[4];
	mbr.qteParticoes = buffer[7] * 256 + buffer[6];
	mbr.arrayParticoes = malloc(sizeof(Particao) * mbr.qteParticoes);

	int deslocamento = mbr.inicioTabelaParticoes;
	int i;
	for (i = 0; i < mbr.qteParticoes; i++) {
		Particao particao;
		particao.setorInicial = buffer[deslocamento + 1] * 256 + buffer[deslocamento] + buffer[deslocamento + 3] * 16777216 + buffer[deslocamento + 2] * 65536;
		deslocamento += 4;
		particao.setorFinal = buffer[deslocamento + 1] * 256 + buffer[deslocamento]	+ buffer[deslocamento + 3] * 16777216 + buffer[deslocamento + 2] * 65536;
		deslocamento += 4;
		memcpy(particao.nome, buffer + deslocamento, 24);
		mbr.arrayParticoes[i] = particao;
		deslocamento += 24;
	}
}

void imprimeMbr() {
	printf("Disco Formatado LSF\n");
	printf("Versão: %s\n", mbr.versaoDisco);
	printf("Tamanho do setor: %d\n", mbr.tamanhoSetor);
	printf("Byte inicio da tabela de partições: %d\n", mbr.inicioTabelaParticoes);
	printf("Quantidade de partições: %d\n", mbr.qteParticoes);
	int i;
	for (i = 0; i < mbr.qteParticoes; i++) {
		Particao particao = mbr.arrayParticoes[i];
		printf("---Partição #%d-------------------\n", i);
		printf("Nome: %s\n", particao.nome);
		printf("Setor inicial: %d\n", particao.setorInicial);
		printf("Setor final: %d\n", particao.setorFinal);
	}

}

/*-----------------------------------------------------------------------------
Função:	Informa a identificação dos desenvolvedores do T2FS.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
    char members[] = "Theodoro Loureiro Mota - 173230\nRui Cardozo - 120211\nFabio Petkowicz - 118627";
    int members_size = sizeof(members) / sizeof(members[0]);

    if (size < members_size) {
        return -1;
    } else {
        strncpy(name, members, size);
        return 0;
    }
}

/*-----------------------------------------------------------------------------
Função:	Formata logicamente o disco virtual t2fs_disk.dat para o sistema de
		arquivos T2FS definido usando blocos de dados de tamanho 
		corresponde a um múltiplo de setores dados por sectors_per_block.
-----------------------------------------------------------------------------*/
int format2 (int sectors_per_block) {
	setoresPorBloco = sectors_per_block;

	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo arquivo no disco e abrí-lo,
		sendo, nesse último aspecto, equivalente a função open2.
		No entanto, diferentemente da open2, se filename referenciar um 
		arquivo já existente, o mesmo terá seu conteúdo removido e 
		assumirá um tamanho de zero bytes.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco. 
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a leitura de uma certa quantidade
		de bytes (size) de um arquivo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a escrita de uma certa quantidade
		de bytes (size) de  um arquivo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para truncar um arquivo. Remove do arquivo 
		todos os bytes a partir da posição atual do contador de posição
		(current pointer), inclusive, até o seu final.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Altera o contador de posição (current pointer) do arquivo.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo diretório.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um diretório do disco.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para alterar o CP (current path)
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para obter o caminho do diretório corrente.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size) {
    unsigned char buffer[SECTOR_SIZE];
    return read_sector(0, buffer);
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink) com
		o nome dado por linkname (relativo ou absoluto) para um 
		arquivo ou diretório fornecido por filename.
-----------------------------------------------------------------------------*/
int ln2 (char *linkname, char *filename) {
	return -1;
}



