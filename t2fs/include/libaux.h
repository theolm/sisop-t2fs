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

void formataParticao(int setoresPorBloco, Mbr *mbr);
