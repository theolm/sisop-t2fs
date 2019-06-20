#include "t2fs.h"

typedef struct {
	int setorInicial;
	int setorFinal;
	char nome[TAMANHO_MAXIMO_NOME_PARTICAO];
} Particao;

typedef struct {
	int handler;
	DIRENT2 dirEnt;
	int tipo;
	int cp;
} Arquivo;

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
	Arquivo taad[4096];
	int indiceHandler;
	int hash;
	int validacao;
} Mbr;

int formataParticao(int setoresPorBloco, Mbr *mbr);
int existeEntradaDiretorio(char *fileName, DIRENT2 *dirEnt, Mbr *mbr);
int criaEntradaDiretorio(char *fileName, int tipo, Mbr *mbr);
int adicionaArquivoNoTAAD(DIRENT2 dirEnt, int tipo, Mbr *mbr);
int removeArquivoDoTAAD(int handler, Mbr *mbr);
int escreve(FILE2 handle, char *buffer, int size, Mbr *mbr);
int le(FILE2 handle, char *buffer, int size, Mbr *mbr);
int validaDisco(Mbr *mbr);
int carregaMbrDisco(Mbr *mbr);
