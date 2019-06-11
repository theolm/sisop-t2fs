#include "t2fs.h"

typedef struct {
	int setorInicial;
	int setorFinal;
	char nome[TAMANHO_MAXIMO_NOME_PARTICAO];
} Particao;

typedef struct {
	char *versaoDisco;
	int tamanhoSetor;
	int inicioTabelaParticoes;
	int qteParticoes;
	Particao *arrayParticoes;
	int particao;
	int setoresPorBloco;
	int numeroSetores;
	int numeroBlocos;
	int tamanhoBitmap;
	int numeroBlocosBitmap;
	BYTE *mapaEspaco;
} Mbr;

char *converteByteParaHex(BYTE valor);

int converteHexParaInt(char *hex);

char *byteToStr(int i);

void substring(char s[], char sub[], int p, int l);

BYTE *converteByteParaBin(BYTE b);

int converteBinParaByte(BYTE *b);

void montaMbr();

void imprimeMbr();

int getSetorDoBloco(int numBloco, Mbr *mbr);

void salvaBloco(int numBloco, BYTE *bufferBloco, Mbr *mbr);

int getBlocoLivreDoByte(BYTE *b);

int getBlocoLivreDoBitmap();

void formataParticao(int setoresPorBloco, Mbr *mbr);
