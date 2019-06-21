#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/libaux.h"
#include "../include/apidisk.h"
#include "../include/t2fs.h"

#define SECTOR_SIZE 256

Mbr mbr;

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
	formataParticao(sectors_per_block, &mbr);
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
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}

    FILE2 handle = -1;
    DIRENT2 dirEnt;
    if (!existeEntradaDiretorio(filename, &dirEnt, &mbr)) {
    	handle = criaEntradaDiretorio(filename, 2, &mbr);
    } else {

    }
    return handle;
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
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}

	FILE2 handle = -1;
	DIRENT2 dirEnt;
	if (existeEntradaDiretorio(filename, &dirEnt, &mbr)) {
		handle = adicionaArquivoNoTAAD(dirEnt, 2, &mbr);
	}
	return handle;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2(FILE2 handle) {
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}

    return removeArquivoDoTAAD(handle, &mbr);
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a leitura de uma certa quantidade
		de bytes (size) de um arquivo.
-----------------------------------------------------------------------------*/
int read2(FILE2 handle, char *buffer, int size) {
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}

    return le(handle, buffer, size, &mbr);
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a escrita de uma certa quantidade
		de bytes (size) de  um arquivo.
-----------------------------------------------------------------------------*/
int write2(FILE2 handle, char *buffer, int size) {
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}
    return escreve(handle, buffer, size, &mbr);
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
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}

    FILE2 result = -1;
    DIRENT2 dirEnt;
    if (!existeEntradaDiretorio(pathname, &dirEnt, &mbr)) {
    	result = criaEntradaDiretorio(pathname, 1, &mbr);
    }
    return result;
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
    return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
DIR2 opendir2(char *pathname) {
	if (mbr.hash != 22222) {
		int erro = carregaMbrDisco(&mbr);
		if (erro == -1) {
			return -1;
		}
	}

	DIR2 dir;
    DIRENT2 dirEnt;
    if (!existeEntradaDiretorio(pathname, &dirEnt, &mbr)) {
    	dir = adicionaArquivoNoTAAD(dirEnt, 1, &mbr);
    }
    return dir;
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
