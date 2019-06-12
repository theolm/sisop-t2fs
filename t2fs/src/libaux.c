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

BYTE *converteByteParaBin(BYTE b) {
	BYTE *bits = malloc(sizeof(BYTE) * 8);

	int i;
	for (i = 0; i < 8; i++) {
		bits[7 - i] = (b >> i) & 1;
	}

	return bits;
}

int converteBinParaByte(BYTE *b) {
	int res = 0;
	int i;

	for (i = 0; i < 8; i++) {
		res += 2 ^ i * b[7 - i];
	}

	return res;
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

int getBlocoLivreDoBitmap(Mbr *mbr) {
	int b = -1;
	int i = 0;
	int bloco = -1;

	do {
		b = mbr->mapaEspaco[i];
	} while (b == -1 && ++i < mbr->tamanhoBitmap);

	if (b != -1) {
		BYTE *bin = malloc(sizeof(BYTE) * 8);
		bin = converteByteParaBin(b);
		bloco = getBlocoLivreDoByte(bin);
		bin[bloco] = 1;
		mbr->mapaEspaco[i] = converteBinParaByte(bin);
		bloco += 8 * i;
	}

	return bloco;
}

void formataParticao(int setoresPorBloco, Mbr *mbr) {
	montaMbr(mbr);
	Particao p;
	mbr->setoresPorBloco = setoresPorBloco;
	p = mbr->arrayParticoes[mbr->particao];
	mbr->numeroSetores = p.setorFinal - p.setorInicial;
	mbr->numeroBlocos = mbr->numeroSetores / mbr->setoresPorBloco;

	mbr->mapaEspaco = malloc(sizeof(BYTE) * ceil(mbr->tamanhoBitmap	/ mbr->setoresPorBloco* SECTOR_SIZE));

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

	imprimeMbr(mbr);
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

DIRENT2 getBlocoDiretorio(int blocoDiretorio, char *fileName, Mbr *mbr) {
	DIRENT2 dirEntResult;
	int tamanhoBloco = getTamanhoBloco(mbr);
	BYTE buffer[tamanhoBloco];
	int entradaDeDiretorios = (tamanhoBloco - 4) / 41;
	int terminou = -1;

	while (terminou == -1) {
		carregaBloco(blocoDiretorio, buffer, mbr);
		int i = 0;
		while (dirEntResult.bloco == -1 && i < entradaDeDiretorios) {
			if (i != 0 || blocoDiretorio != 0) {
				BYTE *b;
				b = copiaFaixa(buffer, i * 41, i * 41 + 41);
				if (b[0] != 0) {
					DIRENT2 dirEnt;
					if (strcmp(fileName, dirEnt.name) == 0) {
						dirEntResult = dirEnt;
						terminou = 1;
					}
				}
			} else {
				terminou = 1;
			}
		}
		i++;
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

int existeEntradaDiretorio(char *fileName, Mbr *mbr) {
	DIRENT2 dirEnt;
	char arquivo[(int) strlen(fileName)];
	strcpy(arquivo, fileName);
	char** arrayPastas = split(arquivo, '/');

	if (arrayPastas) {
		int i;
		for (i = 0; *(arrayPastas + i); i++) {
			char *p = *(arrayPastas + i);
			dirEnt = getBlocoDiretorio(dirEnt.bloco, p, mbr);
			free(p);
		}

	} else {
		dirEnt.bloco = 0;
	}

	free(arrayPastas);
	return dirEnt.bloco != -1 ? 0 : -1;
}
