#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/apidisk.h"
#include "../include/libaux.h"

#define SECTOR_SIZE 256

char *converteByteParaHex(BYTE valor) {
	char *hex = malloc(sizeof(char));
	sprintf(hex, "%x", valor);
	return hex;
}

int converteHexParaInt(char *hex) {
	return (int) strtol(hex, NULL, 16);
}

char *byteToStr(int i) {
	char *s = malloc(sizeof(char));
	sprintf(s, "%1d", i);
	return s;
}

void substring(char s[], char sub[], int p, int l) {
	int c = 0;
	while (c < l) {
		sub[c] = s[p + c - 1];
		c++;
	}
	sub[c] = '\0';
}

void converteByteParaBin(BYTE b, BYTE *bits) {
	int i;
	for (i = 0; i < 8; i++) {
		bits[7 - i] = (b >> i) & 1;
	}
}

int pot(int x, int n) {
	int i;
	int res = x;
	for (i = 1; i < n; i++) {
		res *= x;
	}
	return res;
}

int converteBinParaByte(BYTE *b) {
	int res = 0;
	int i;

	for (i = 0; i < 8; i++) {
		res += pot(2, i) * b[7 - i];
	}

	return res == 256 ? 255 : res;
}

char** split(char* str, const char delimitador) {
	char** result = 0;
	size_t count = 0;
	char* tmp = str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = delimitador;
	delim[1] = 0;

	while (*tmp) {
		if (delimitador == *tmp) {
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	count += last_comma < (str + strlen(str) - 1);
	count++;
	result = malloc(sizeof(char*) * count);

	if (result) {
		size_t idx = 0;
		char *token = strtok(str, delim);

		while (token) {
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		*(result + idx) = 0;
	}

	return result;
}

int getTamanhoBloco(Mbr *mbr) {
	return SECTOR_SIZE * mbr->setoresPorBloco;
}

void montaMbr(Mbr *mbr) {
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, buffer);
	char *s = strcat(converteByteParaHex(buffer[1]),
			converteByteParaHex(buffer[0]));
	char c[strlen(s)];
	strcpy(c, s);
	char d[3];
	substring(c, d, 1, 3);
	int ano = converteHexParaInt(d);
	char e[3];
	substring(c, e, 4, 1);
	int semestre = converteHexParaInt(e);

	mbr->versaoDisco = strcat(byteToStr(ano), byteToStr(semestre));
	mbr->particao = 0;
	mbr->tamanhoSetor = buffer[3] * SECTOR_SIZE + buffer[2];
	mbr->inicioTabelaParticoes = buffer[5] * SECTOR_SIZE + buffer[4];
	mbr->qteParticoes = buffer[7] * SECTOR_SIZE + buffer[6];
	mbr->arrayParticoes = malloc(sizeof(Particao) * mbr->qteParticoes);

	int deslocamento = mbr->inicioTabelaParticoes;
	int i;
	for (i = 0; i < mbr->qteParticoes; i++) {
		Particao particao;
		particao.setorInicial = buffer[deslocamento + 1] * 256
				+ buffer[deslocamento] + buffer[deslocamento + 3] * 16777216
				+ buffer[deslocamento + 2] * 65536;
		deslocamento += 4;
		particao.setorFinal = buffer[deslocamento + 1] * 256
				+ buffer[deslocamento] + buffer[deslocamento + 3] * 16777216
				+ buffer[deslocamento + 2] * 65536;
		deslocamento += 4;
		memcpy(particao.nome, buffer + deslocamento, 24);
		mbr->arrayParticoes[i] = particao;
		deslocamento += 24;
	}
	mbr->indiceHandler = 0;
}

void imprimeMbr(Mbr *mbr) {
	printf("Disco Formatado LSF\n");
	printf("Versão: %s\n", mbr->versaoDisco);
	printf("Tamanho do setor: %d\n", mbr->tamanhoSetor);
	printf("Byte inicio da tabela de partições: %d\n",
			mbr->inicioTabelaParticoes);
	printf("Quantidade de partições: %d\n", mbr->qteParticoes);
	int i;
	for (i = 0; i < mbr->qteParticoes; i++) {
		Particao particao = mbr->arrayParticoes[i];
		printf("---Partição #%d-------------------\n", i);
		printf("Nome: %s\n", particao.nome);
		printf("Setor inicial: %d\n", particao.setorInicial);
		printf("Setor final: %d\n", particao.setorFinal);
	}

}

int getSetorDoBloco(int numBloco, Mbr *mbr) {
	return mbr->arrayParticoes[mbr->particao].setorInicial
			+ numBloco * mbr->setoresPorBloco;
}

void salvaBloco(int numBloco, BYTE *bufferBloco, Mbr *mbr) {
	int i;
	int setor;
	for (setor = 0; setor < mbr->setoresPorBloco; setor++) {
		BYTE bufferSetor[SECTOR_SIZE];
		for (i = 0; i < SECTOR_SIZE; i++) {
			bufferSetor[i] = bufferBloco[setor * SECTOR_SIZE + i];
		}
		int setorFisico = getSetorDoBloco(numBloco, mbr) + setor;
		write_sector(setorFisico, bufferSetor);
	}
}

void carregaBloco(int numBloco, BYTE *bufferBloco, Mbr *mbr) {
	int i;
	int setor;
	for (setor = 0; setor < mbr->setoresPorBloco; setor++) {
		BYTE bufferSetor[SECTOR_SIZE];
		read_sector(getSetorDoBloco(numBloco, mbr) + setor, bufferSetor);
		for (i = 0; i < SECTOR_SIZE; i++) {
			bufferBloco[setor * SECTOR_SIZE + i] = bufferSetor[i];
		}
	}
}

int getBlocoLivreDoByte(BYTE *b) {
	int i = 0;
	int res = -1;
	int achei = 0;
	while (i < 8 && achei == 0) {
		if (b[i] == 0) {
			achei = 1;
			res = i;
		} else {
			i++;
		}
	}
	return res;
}

void commitaMapaBloco(Mbr *mbr) {
	int j;
	int tamanhoBloco = getTamanhoBloco(mbr);
	for (j = 0; j < mbr->numeroBlocosBitmap; j++) {
		BYTE buffer[tamanhoBloco];
		for (int i = 0; i < tamanhoBloco; i++) {
			buffer[i] = i < mbr->tamanhoBitmap ? mbr->mapaEspaco[j * SECTOR_SIZE + i] : 0;
		}
		salvaBloco(j + 1, buffer, mbr);
	}
}

int getBlocoLivreDoBitmap(Mbr *mbr) {
	int b = -1;
	int i = 0;
	int bloco = -1;

	do {
		b = mbr->mapaEspaco[i];
	} while (b == 255 && ++i < mbr->tamanhoBitmap);

	if (b != -1) {
		BYTE bin[8];
		converteByteParaBin(b, bin);
		bloco = getBlocoLivreDoByte(bin);
		bin[bloco] = 1;
		mbr->mapaEspaco[i] = converteBinParaByte(bin);
		bloco += 8 * i;
	}

	commitaMapaBloco(mbr);
	return bloco;
}

int ceil2(float f1){
   int n = (int) f1;
   float f2 = n;
   float dif = f1 - f2;

   if (dif > 0 ){
       n++;
   }

   return n;
}

void formataParticao(int setoresPorBloco, Mbr *mbr) {
	montaMbr(mbr);
	Particao p;
	mbr->setoresPorBloco = setoresPorBloco;
	p = mbr->arrayParticoes[mbr->particao];
	mbr->numeroSetores = p.setorFinal - p.setorInicial;
	mbr->numeroBlocos = mbr->numeroSetores / mbr->setoresPorBloco;

	mbr->tamanhoBitmap = ceil2(mbr->numeroBlocos / (float) 8);
	mbr->mapaEspaco = malloc(sizeof(BYTE) * mbr->tamanhoBitmap);
	mbr->numeroBlocosBitmap = ceil2(mbr->tamanhoBitmap / (mbr->setoresPorBloco * (float) SECTOR_SIZE));

	BYTE buffer[setoresPorBloco * SECTOR_SIZE];

	int i;
	for (i = 0; i < setoresPorBloco * SECTOR_SIZE; i++) {
		buffer[i] = 0;
	}

	for (i = 0; i < mbr->numeroBlocosBitmap; i++) {
		salvaBloco(1 + i, buffer, mbr);
	}

	buffer[0] = (BYTE) (setoresPorBloco << 24 & 0xFF);
	buffer[1] = (BYTE) (setoresPorBloco << 16 & 0xFF);
	buffer[2] = (BYTE) (setoresPorBloco << 8 & 0xFF);
	buffer[3] = (BYTE) (setoresPorBloco & 0xFF);

	salvaBloco(0, buffer, mbr);

	for (i = 0; i < 1 + mbr->numeroBlocosBitmap; i++) {
		getBlocoLivreDoBitmap(mbr);
	}
}

BYTE *copiaFaixa(BYTE *buffer, int inicio, int fim) {
	static BYTE b[41];
	int i;
	int deslocamento = 0;
	for (i = inicio; i < fim; i++) {
		b[deslocamento++] = buffer[i];
	}
	return b;
}

int getProximoBloco(BYTE *buffer, Mbr *mbr) {
	int tam = getTamanhoBloco(mbr);
	return buffer[tam - 4] * 16777216 + buffer[tam - 3] * 65536 + buffer[tam - 2] * 256 + buffer[tam - 1];
}

void setProximoBloco(BYTE *buffer, int proximoBloco, int tamanhoBloco) {
	buffer[tamanhoBloco - 4] = (BYTE) (proximoBloco << 24 & 0xFF);
	buffer[tamanhoBloco - 3] = (BYTE) (proximoBloco << 16 & 0xFF);
	buffer[tamanhoBloco - 2] = (BYTE) (proximoBloco << 8 & 0xFF);
	buffer[tamanhoBloco - 1] = (BYTE) (proximoBloco & 0xFF);
}

DIRENT2 inicializaDirEnt() {
	DIRENT2 dirEnt;
	strcpy(dirEnt.name, "");
	dirEnt.fileSize = 0;
	dirEnt.fileType = 0;
	dirEnt.bloco = 0;
	dirEnt.pai = 0;
	return dirEnt;
}

DIRENT2 montaDirEnt(BYTE *buffer) {
	DIRENT2 dirEnt;
	dirEnt.fileType = buffer[0];

	char c[32];
	int i;
	for (i = 0; i < 31; i++) {
		c[i] = (char) buffer[i + 1];
	}
	c[31] = 0;
	strcpy(dirEnt.name, c);
	dirEnt.fileSize = buffer[33] * 16777216 + buffer[34] * 65536 + buffer[35] * 256 + buffer[36];
	dirEnt.bloco = buffer[37] * 16777216 + buffer[38] * 65536 + buffer[39] * 256 + buffer[40];

	return dirEnt;
}

void desmontaDirEnt(DIRENT2 dirEnt, BYTE *byteDirEnt) {
	byteDirEnt[0] = dirEnt.fileType;

	int i;
	int fim = 0;
	for (i = 0; i < 31; i++) {
		if (dirEnt.name[i] == 0) {
			fim = 1;
		}
		byteDirEnt[i + 1] = fim ? 0 : dirEnt.name[i];
	}
	byteDirEnt[32] = 0;

	byteDirEnt[33] = (BYTE) (dirEnt.fileSize << 24 & 0xFF);
	byteDirEnt[34] = (BYTE) (dirEnt.fileSize << 16 & 0xFF);
	byteDirEnt[35] = (BYTE) (dirEnt.fileSize << 8 & 0xFF);
	byteDirEnt[36] = (BYTE) (dirEnt.fileSize & 0xFF);

	byteDirEnt[37] = (BYTE) (dirEnt.bloco << 24 & 0xFF);
	byteDirEnt[38] = (BYTE) (dirEnt.bloco << 16 & 0xFF);
	byteDirEnt[39] = (BYTE) (dirEnt.bloco << 8 & 0xFF);
	byteDirEnt[40] = (BYTE) (dirEnt.bloco & 0xFF);
}

DIRENT2 getEntradaDiretorio(int blocoDiretorio, char *fileName, Mbr *mbr) {
	DIRENT2 dirEntResult;
	dirEntResult = inicializaDirEnt();
	int tamanhoBloco = getTamanhoBloco(mbr);
	BYTE buffer[tamanhoBloco];
	int entradaDeDiretorios = (tamanhoBloco - 4) / 41;
	int terminou = 0;

	while (!terminou) {
		carregaBloco(blocoDiretorio, buffer, mbr);
		int i = 0;
		while (dirEntResult.name[0] == 0 && i < entradaDeDiretorios) {
			if (i != 0 || blocoDiretorio != 0) {
				BYTE *b;
				b = copiaFaixa(buffer, i * 41, i * 41 + 41);
				if (b[0] != 0) {
					DIRENT2 dirEnt;
					dirEnt = montaDirEnt(b);
					if (strcmp(fileName, dirEnt.name) == 0) {
						dirEntResult = dirEnt;
						dirEntResult.pai = blocoDiretorio;
						terminou = 1;
					}
				} else {
					terminou = 1;
				}
			}
			i++;
		}
	}

	if (terminou == -1 && dirEntResult.bloco == -1) {
		int proximoBloco = getProximoBloco(buffer, mbr);
		if (proximoBloco == 0) {
			terminou = 1;
		} else {
			blocoDiretorio = proximoBloco;
		}
	}

	return dirEntResult;
}

int existeEntradaDiretorio(char *fileName, DIRENT2 *dirEnt, Mbr *mbr) {
	int result = 1;
	char arquivo[strlen(fileName)];
	strcpy(arquivo, fileName);
	char** arrayPastas = split(arquivo, '/');
	DIRENT2 dirEntAux;
	dirEntAux = inicializaDirEnt();

	int i;
	int bloco = 0;
	for (i = 0; result && *(arrayPastas + i) ; i++) {
		char *p = *(arrayPastas + i);
		bloco = dirEntAux.bloco;
		dirEntAux = getEntradaDiretorio(bloco, p, mbr);
		result = dirEntAux.name[0] != 0 ? 1 : 0;
		free(p);
	}

	free(arrayPastas);
	if (result) {
		strcpy(dirEnt->name, dirEntAux.name);
		dirEnt->bloco = dirEntAux.bloco;
		dirEnt->fileType = dirEntAux.fileType;
		dirEnt->fileSize = dirEntAux.fileSize;
		dirEnt->pai = bloco;
	}
	return result;
}

int adicionaArquivoNoTAAD(DIRENT2 dirEnt, int tipo, Mbr *mbr) {
	Arquivo arquivo;
	arquivo.dirEnt = dirEnt;
	arquivo.cp = 0;
	arquivo.tipo = tipo;
	while (mbr->taad[mbr->indiceHandler].tipo != 0) {
		mbr->indiceHandler++;
		if (mbr->indiceHandler > 4096) {
			mbr->indiceHandler = 0;
		}
	}
	arquivo.handler = mbr->indiceHandler;
	mbr->taad[mbr->indiceHandler] = arquivo;
	return mbr->indiceHandler;
}

int removeArquivoDoTAAD(int handler, Mbr *mbr) {
	int result = -1;
	if (mbr->taad[handler].tipo != 0) {
		mbr->taad[handler].tipo = 0;
		result = 0;
	}
	return result;
}

DIRENT2 getDirEntDoTAAD(int handler, Mbr *mbr) {
	DIRENT2 dirEnt;
	dirEnt = inicializaDirEnt();
	if (mbr->taad[handler].tipo != 0) {
		dirEnt = mbr->taad[handler].dirEnt;
	}
	return dirEnt;
}

int getArrayEntradaDiretorios(int diretorio, DIRENT2 *arrayDirEnt, BYTE *bufferBloco, Mbr *mbr, int entradasDeDiretorios) {

	int i = 0;
	for (i = 0; i < entradasDeDiretorios; i++) {
		if (i != 0 || diretorio != 0) {
			BYTE *b;
			b = copiaFaixa(bufferBloco, i * 41, i * 41 + 41);
			arrayDirEnt[i] = montaDirEnt(b);
		}
	}

	return getProximoBloco(bufferBloco, mbr);

}

DIRENT2 localizaDirEnt(DIRENT2 *arrayDirEnt, char *nomeEntradaDiretorio, int entradasDeDiretorio) {
	DIRENT2 dirEnt;
	dirEnt = inicializaDirEnt();
	int achei = 0;
	int i = 0;
	while (!achei && i < entradasDeDiretorio) {
		DIRENT2 dirEntAux;
		dirEntAux = arrayDirEnt[i];
		if (dirEntAux.name[0] != 0 && strcmp(dirEntAux.name, nomeEntradaDiretorio) == 0) {
			dirEnt = dirEntAux;
			achei = 1;
		} else {
			i++;
		}
	}
	return dirEnt;
}

int buscaDirEnt(DIRENT2 *dirEnt, int bloco, char *s, BYTE *bufferBloco, Mbr *mbr, int tamanhoBloco) {
	int entradasDeDiretorios = (tamanhoBloco - 4) / 41;
	int terminei = 0;
	int proximoBloco;

	do {
		DIRENT2 arrayDirEnt[entradasDeDiretorios];
		proximoBloco = getArrayEntradaDiretorios(bloco, arrayDirEnt, bufferBloco, mbr, entradasDeDiretorios);
		DIRENT2 dirEntAux;
		dirEntAux = localizaDirEnt(arrayDirEnt, s, entradasDeDiretorios);
		if (dirEntAux.name[0] != 0) {
			strcpy(dirEnt->name, dirEntAux.name);
			dirEnt->fileType = dirEntAux.fileType;
			dirEnt->fileSize = dirEntAux.fileSize;
			dirEnt->bloco = dirEntAux.bloco;
			terminei = 1;
		}
		if (!terminei) {
			if (proximoBloco == 0) {
				terminei = 1;
			} else {
				bloco = proximoBloco;
				carregaBloco(bloco, bufferBloco, mbr);
			}
		}

	} while (!terminei);
	return proximoBloco;
}

void salvaEntDir(DIRENT2 dirEnt, BYTE *buffer, int blocoDiretorio, int posicao, Mbr *mbr) {
	BYTE byteDirEnt[41];
	desmontaDirEnt(dirEnt, byteDirEnt);
	int i;
	for (i = 0; i < 41; i++) {
		buffer[i + posicao * 41] = byteDirEnt[i];
	}
	salvaBloco(blocoDiretorio, buffer, mbr);
}

void addEntradaDiretorio(DIRENT2 dirEntAtualizada, char *fileName, int blocoDiretorio, Mbr *mbr) {
	int tamanhoBloco = getTamanhoBloco(mbr);
	BYTE bufferBloco[tamanhoBloco];
	int entradasDeDiretorios = (tamanhoBloco - 4) / 41;
	int achei = 0;

	while (!achei) {
		carregaBloco(blocoDiretorio, bufferBloco, mbr);
		int i = 0;
		while (!achei && i < entradasDeDiretorios) {
			if (i != 0 || blocoDiretorio != 0) {
				BYTE *b;
				b = copiaFaixa(bufferBloco, i * 41, i * 41 + 41);
				if (b[0] == 0) {
					DIRENT2 dirEnt;
					strcpy(dirEnt.name, dirEntAtualizada.name);
					dirEnt.fileType = dirEntAtualizada.fileType;
					dirEnt.fileSize = dirEntAtualizada.fileSize;
					dirEnt.bloco = dirEntAtualizada.bloco;
					salvaEntDir(dirEnt, bufferBloco, blocoDiretorio, i, mbr);
					achei = 1;
				}
			}
			i++;
		}

		if (!achei) {
			int proximoBloco = getProximoBloco(bufferBloco, mbr);
			if (proximoBloco == 0) {
				proximoBloco = getBlocoLivreDoBitmap(mbr);
				setProximoBloco(bufferBloco, proximoBloco, tamanhoBloco);
				salvaBloco(blocoDiretorio, bufferBloco, mbr);
			}
			blocoDiretorio = proximoBloco;
		}
	}

}

void setEntradaDiretorio(DIRENT2 dirEntAtualizada, char *fileName, int blocoDiretorio, Mbr *mbr) {

	int tamanhoBloco = getTamanhoBloco(mbr);
	BYTE bufferBloco[tamanhoBloco];
	int entradasDeDiretorios = (tamanhoBloco - 4) / 41;
	int terminou = 0;
	int achei = 0;

	while (!terminou) {
		carregaBloco(blocoDiretorio, bufferBloco, mbr);
		int i = 0;

		while (!terminou && i < entradasDeDiretorios) {
			if (i != 0 || blocoDiretorio != 0) {
				BYTE *b;
				b = copiaFaixa(bufferBloco, i * 41, i * 41 + 41);
				if (b[0] != 0) {
					DIRENT2 dirEnt;
					dirEnt = montaDirEnt(b);
					if (strcmp(dirEnt.name, fileName) == 0) {
						strcpy(dirEnt.name, dirEntAtualizada.name);
						dirEnt.fileType = dirEntAtualizada.fileType;
						dirEnt.fileSize = dirEntAtualizada.fileSize;
						dirEnt.bloco = dirEntAtualizada.bloco;
						salvaEntDir(dirEnt, bufferBloco, blocoDiretorio, i, mbr);
						terminou = 1;
						achei = 1;
					}
				}
			}
			i++;
		}

		if (!terminou && !achei) {
			int proximoBloco = getProximoBloco(bufferBloco, mbr);
			if (proximoBloco == 0) {
				terminou = 1;
			} else {
				blocoDiretorio = proximoBloco;
			}
		}
	}

}

DIRENT2 criaEntradaDiretorioEfetivo(char **arrayPastas, int tipo, Mbr *mbr) {
	DIRENT2 dirEnt;
	dirEnt = inicializaDirEnt();
	int bloco = 0;
	int proximoBloco;
	int tamanhoBloco = getTamanhoBloco(mbr);
	BYTE bufferBloco[tamanhoBloco];

	int i;
	for (i = 0; *(arrayPastas + i); i++) {

		char *s = *(arrayPastas + i);

		do {
			dirEnt = inicializaDirEnt();
			carregaBloco(bloco, bufferBloco, mbr);
			proximoBloco = buscaDirEnt(&dirEnt, bloco, s, bufferBloco, mbr, tamanhoBloco);
		} while (dirEnt.name[0] == 0 && proximoBloco > 0);

		if (dirEnt.name[0] == 0) {
			strcpy(dirEnt.name, s);
			dirEnt.fileType = *(arrayPastas + i + 1) ? 1 : tipo;
			dirEnt.bloco = *(arrayPastas + i + 1) ? getBlocoLivreDoBitmap(mbr) : 0;
			dirEnt.fileSize = 0;
			dirEnt.pai = bloco;
			addEntradaDiretorio(dirEnt, s, bloco, mbr);
			bloco = dirEnt.bloco;
		} else {
			if (dirEnt.bloco == 0) {
				dirEnt.bloco = getBlocoLivreDoBitmap(mbr);
				setEntradaDiretorio(dirEnt, dirEnt.name, bloco, mbr);
			}
			dirEnt.pai = bloco;
			bloco = dirEnt.bloco;
		}

	}

	return dirEnt;
}

int criaEntradaDiretorio(char *fileName, int tipo, Mbr *mbr) {
	char arquivo[strlen(fileName)];
	strcpy(arquivo, fileName);
	char **arrayPastas = split(arquivo, '/');

	DIRENT2 dirEnt = criaEntradaDiretorioEfetivo(arrayPastas, tipo, mbr);

	return adicionaArquivoNoTAAD(dirEnt, tipo, mbr);
}

void apagaCadeiaDeBlocos(int bloco, Mbr *mbr) {

}

int escreveBlocos(DIRENT2 dirEnt, char *buffer, int size, Mbr *mbr) {
	int result;
	if (dirEnt.fileType == 2) {
		if (dirEnt.bloco != 0) {
			apagaCadeiaDeBlocos(dirEnt.bloco, mbr);
		}
		dirEnt.bloco = getBlocoLivreDoBitmap(mbr);
		dirEnt.fileSize = size;
		setEntradaDiretorio(dirEnt, dirEnt.name, dirEnt.pai, mbr);
		int novoBloco = 0;
		int bloco = dirEnt.bloco;
		int tamanhoBloco = getTamanhoBloco(mbr);
		int j = 0;
		while (j < size) {
			BYTE bufferBloco[tamanhoBloco];
			int i;
			for (i = 0; i < tamanhoBloco - 4; i++) {
				bufferBloco[i] = j < size ? buffer[j++] : 0;
			}
			if (j < size) {
				novoBloco = getBlocoLivreDoBitmap(mbr);
				setProximoBloco(bufferBloco, novoBloco, tamanhoBloco);
			} else {
				setProximoBloco(bufferBloco, 0, tamanhoBloco);
			}
			salvaBloco(bloco, bufferBloco, mbr);
			bloco = novoBloco;
		}
		result = j;
	} else {
		result = -1;
	}
	return result;
}

int leBlocos(DIRENT2 dirEnt, char *buffer, int size, Mbr *mbr) {
	if (dirEnt.fileType == 2) {
		int bloco = dirEnt.bloco;
		int tamanhoBloco = getTamanhoBloco(mbr);
		int tamanhoArquivo = dirEnt.fileSize;
		int j = 0;
		while (j < tamanhoArquivo) {
			BYTE bufferBloco[tamanhoBloco];
			carregaBloco(bloco, bufferBloco, mbr);
			int i;
			for (i = 0; i < tamanhoBloco - 4; i++) {
				if (j < tamanhoArquivo) {
					buffer[j] = bufferBloco[i];
					printf("%d", buffer[i]);
					j++;
				}
			}
			if (j < tamanhoArquivo) {
				bloco = getProximoBloco(bufferBloco, mbr);
			}
		}
	}
	return 1;
}

int escreve(FILE2 handle, char *buffer, int size, Mbr *mbr) {
	DIRENT2 dirEnt = getDirEntDoTAAD(handle, mbr);
	return escreveBlocos(dirEnt, buffer, size, mbr);
}

int le(FILE2 handle, char *buffer, int size, Mbr *mbr) {
	DIRENT2 dirEnt = getDirEntDoTAAD(handle, mbr);
	return leBlocos(dirEnt, buffer, size, mbr);
}
