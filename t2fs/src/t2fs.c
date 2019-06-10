#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/libaux.h"
#include "../include/apidisk.h"
#include "../include/t2fs.h"

#define SECTOR_SIZE 256


int particao = 0; // Partição selecionada - primeira por definição do T2
int setoresPorBloco;
int numeroSetores;
int numeroBlocos;
int tamanhoBitmap;
int numeroBlocosBitmap;
BYTE *mapaEspaco;
Mbr mbr;

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
    mbr.tamanhoSetor = buffer[3] * SECTOR_SIZE + buffer[2];
    mbr.inicioTabelaParticoes = buffer[5] * SECTOR_SIZE + buffer[4];
    mbr.qteParticoes = buffer[7] * SECTOR_SIZE + buffer[6];
    mbr.arrayParticoes = malloc(sizeof(Particao) * mbr.qteParticoes);

    int deslocamento = mbr.inicioTabelaParticoes;
    int i;
    for (i = 0; i < mbr.qteParticoes; i++) {
        Particao particao;
        particao.setorInicial =
                buffer[deslocamento + 1] * 256 + buffer[deslocamento] + buffer[deslocamento + 3] * 16777216 +
                buffer[deslocamento + 2] * 65536;
        deslocamento += 4;
        particao.setorFinal =
                buffer[deslocamento + 1] * 256 + buffer[deslocamento] + buffer[deslocamento + 3] * 16777216 +
                buffer[deslocamento + 2] * 65536;
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

int getSetorDoBloco(int numBloco) {
    return mbr.arrayParticoes[particao].setorInicial + numBloco * setoresPorBloco;
}

void salvaBloco(int numBloco, BYTE *bufferBloco) {
    int i;
    int setor;
    for (setor = 0; setor < setoresPorBloco; setor++) {
        BYTE bufferSetor[SECTOR_SIZE];
        for (i = 0; i < SECTOR_SIZE; i++) {
            bufferSetor[i] = bufferBloco[setor * SECTOR_SIZE + i];
        }
        int setorFisico = getSetorDoBloco(numBloco) + setor;
        write_sector(setorFisico, bufferSetor);
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


int getBlocoLivreDoBitmap() {
    int b = -1;
    int i = 0;
    int bloco = -1;

    do {
        b = mapaEspaco[i];
    } while (b == -1 && ++i < tamanhoBitmap);

    if (b != -1) {
        BYTE *bin = malloc(sizeof(BYTE) * 8);
        bin = converteByteParaBin(b);
        bloco = getBlocoLivreDoByte(bin);
        bin[bloco] = 1;
        mapaEspaco[i] = converteBinParaByte(bin);
        bloco += 8 * i;
    }

    return bloco;
}

/*-----------------------------------------------------------------------------
Função:	Informa a identificação dos desenvolvedores do T2FS.
-----------------------------------------------------------------------------*/
int identify2(char *name, int size) {
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
int format2(int sectors_per_block) {
    setoresPorBloco = sectors_per_block;
    Particao p = mbr.arrayParticoes[particao];
    numeroSetores = p.setorFinal - p.setorInicial;
    numeroBlocos = numeroSetores / setoresPorBloco;

    mapaEspaco = malloc(sizeof(BYTE) * ceil(tamanhoBitmap / setoresPorBloco * SECTOR_SIZE));

    BYTE buffer[setoresPorBloco * SECTOR_SIZE];

    int i;
    for (i = 0; i < setoresPorBloco * SECTOR_SIZE; i++) {
        buffer[i] = 0;
    }

    for (i = 0; i < numeroBlocosBitmap; i++) {
        salvaBloco(1 + i, buffer);
    }

    buffer[0] = (BYTE) (setoresPorBloco << 24 & 0xFF);
    buffer[1] = (BYTE) (setoresPorBloco << 16 & 0xFF);
    buffer[2] = (BYTE) (setoresPorBloco << 8 & 0xFF);
    buffer[3] = (BYTE) (setoresPorBloco & 0xFF);

    salvaBloco(0, buffer);

    for (i = 0; i < 1 + numeroBlocosBitmap; i++) {
        //getBlocoLivreDoBitmap();
    }

    return 0;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo arquivo no disco e abrí-lo,
		sendo, nesse último aspecto, equivalente a função open2.
		No entanto, diferentemente da open2, se filename referenciar um 
		arquivo já existente, o mesmo terá seu conteúdo removido e 
		assumirá um tamanho de zero bytes.
-----------------------------------------------------------------------------*/
FILE2 create2(char *filename) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco. 
-----------------------------------------------------------------------------*/
int delete2(char *filename) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2(char *filename) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2(FILE2 handle) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a leitura de uma certa quantidade
		de bytes (size) de um arquivo.
-----------------------------------------------------------------------------*/
int read2(FILE2 handle, char *buffer, int size) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a escrita de uma certa quantidade
		de bytes (size) de  um arquivo.
-----------------------------------------------------------------------------*/
int write2(FILE2 handle, char *buffer, int size) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para truncar um arquivo. Remove do arquivo 
		todos os bytes a partir da posição atual do contador de posição
		(current pointer), inclusive, até o seu final.
-----------------------------------------------------------------------------*/
int truncate2(FILE2 handle) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Altera o contador de posição (current pointer) do arquivo.
-----------------------------------------------------------------------------*/
int seek2(FILE2 handle, DWORD offset) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo diretório.
-----------------------------------------------------------------------------*/
int mkdir2(char *pathname) {
    getBlocoLivreDoBitmap();
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um diretório do disco.
-----------------------------------------------------------------------------*/
int rmdir2(char *pathname) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para alterar o CP (current path)
-----------------------------------------------------------------------------*/
int chdir2(char *pathname) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para obter o caminho do diretório corrente.
-----------------------------------------------------------------------------*/
int getcwd2(char *pathname, int size) {
    unsigned char buffer[SECTOR_SIZE];
    return read_sector(0, buffer);
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
DIR2 opendir2(char *pathname) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2(DIR2 handle, DIRENT2 *dentry) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2(DIR2 handle) {
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink) com
		o nome dado por linkname (relativo ou absoluto) para um 
		arquivo ou diretório fornecido por filename.
-----------------------------------------------------------------------------*/
int ln2(char *linkname, char *filename) {
    return -1;
}
