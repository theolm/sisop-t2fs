
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apidisk.h"
#include "t2fs.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static const char* NOMES = "Natanael Rodrigo Zimmer - 00252850, Jessica Lorencetti 00228342";
DIRHANDLE* dirHandles[MAX_HANDLES];
SUPERBLOCK* superBlock = NULL;
DIRHANDLE* curDir = NULL;
int MAX_DIR_ENTRIES = 0;
char* curPath = NULL;

int validate_superblock(SUPERBLOCK *bloco)
{
	if ((memcmp(bloco, &"T2FS", 4) != 0) | (bloco->version != 0x7E22) | (bloco->superblockSize != 1) |
		(bloco->pFATSectorStart != 1) | (bloco->NofSectors * SECTOR_SIZE != bloco->DiskSize)) {
		return -1;
	}
	else {
		return 0;
	}
}

int read_superblock(SUPERBLOCK *bloco)
{
	unsigned char buffer[SECTOR_SIZE];
	int error = read_sector (0, buffer);
	if (error) {
		printf ("read_sector (%d) error = %d\n", 0, error);
	}
	else {
		memcpy(bloco, &buffer, sizeof(*bloco));
		error = validate_superblock(bloco);
		if (error) {
			printf("invalid t2fs_superbloco\n");
		}
	}
	return error;
}

int initialize()
{
	superBlock = malloc(sizeof(SUPERBLOCK));
	if (superBlock == 0)
	{
		printf("Failed to allocate memory to read superblock\n");
		exit(0);
	}
	memset(superBlock, 0, sizeof(SUPERBLOCK));
	if (read_superblock(superBlock) != 0)
	{
		exit(0);
	}

	MAX_DIR_ENTRIES = superBlock->SectorsPerCluster * 4;

	curPath = malloc(sizeof(char)*2);
	curPath[0] = '/';
	curPath[1] = 0;
	for (int i = 0; i != MAX_HANDLES; i++)
	{
		dirHandles[i] = NULL;
	}
	return 1;
}

/*-----------------------------------------------------------------------------
Função: Usada para identificar os desenvolvedores do T2FS.
	Essa função copia um string de identificação para o ponteiro indicado por "name".
	Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
	O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por ‘\0’.
	O string deve conter o nome e número do cartão dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identificação.
	size -> tamanho do buffer "name" (número máximo de bytes a serem copiados).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size)
{
	memcpy(name, NOMES, MIN(size, 64));
	return 0;
}

/*-----------------------------------------------------------------------------
Função: Criar um novo arquivo.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	O contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	Caso já exista um arquivo ou diretório com o mesmo nome, a função deverá retornar um erro de criação.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo).
	Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Apagar um arquivo do disco.
	O nome do arquivo a ser apagado é aquele informado pelo parâmetro "filename".

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Abre um arquivo existente no disco.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	Ao abrir um arquivo, o contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.
	Todos os arquivos abertos por esta chamada são abertos em leitura e em escrita.
	O ponto em que a leitura, ou escrita, será realizada é fornecido pelo valor current_pointer (ver função seek2).

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo)
	Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Fecha o arquivo identificado pelo parâmetro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
	Os bytes lidos são colocados na área apontada por "buffer".
	Após a leitura, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último lido.

Entra:	handle -> identificador do arquivo a ser lido
	buffer -> buffer onde colocar os bytes lidos do arquivo
	size -> número de bytes a serem lidos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes lidos.
	Se o valor retornado for menor do que "size", então o contador de posição atingiu o final do arquivo.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
	Os bytes a serem escritos estão na área apontada por "buffer".
	Após a escrita, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último escrito.

Entra:	handle -> identificador do arquivo a ser escrito
	buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
	size -> número de bytes a serem escritos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes efetivamente escritos.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para truncar um arquivo.
	Remove do arquivo todos os bytes a partir da posição atual do contador de posição (CP)
	Todos os bytes a partir da posição CP (inclusive) serão removidos do arquivo.
	Após a operação, o arquivo deverá contar com CP bytes e o ponteiro estará no final do arquivo

Entra:	handle -> identificador do arquivo a ser truncado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Reposiciona o contador de posições (current pointer) do arquivo identificado por "handle".
	A nova posição é determinada pelo parâmetro "offset".
	O parâmetro "offset" corresponde ao deslocamento, em bytes, contados a partir do início do arquivo.
	Se o valor de "offset" for "-1", o current_pointer deverá ser posicionado no byte seguinte ao final do arquivo,
		Isso é útil para permitir que novos dados sejam adicionados no final de um arquivo já existente.

Entra:	handle -> identificador do arquivo a ser escrito
	offset -> deslocamento, em bytes, onde posicionar o "current pointer".

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Criar um novo diretório.
	O caminho desse novo diretório é aquele informado pelo parâmetro "pathname".
		O caminho pode ser ser absoluto ou relativo.
	São considerados erros de criação quaisquer situações em que o diretório não possa ser criado.
		Isso inclui a existência de um arquivo ou diretório com o mesmo "pathname".

Entra:	pathname -> caminho do diretório a ser criado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname)
{
	if (superBlock == NULL) { initialize(); }

	//if (pathExists(pathname, superBlock->RootDirCluster) > 0)
	{
		return -1;
	}
}

/*-----------------------------------------------------------------------------
Função:	Apagar um subdiretório do disco.
	O caminho do diretório a ser apagado é aquele informado pelo parâmetro "pathname".
	São considerados erros quaisquer situações que impeçam a operação.
		Isso inclui:
			(a) o diretório a ser removido não está vazio;
			(b) "pathname" não existente;
			(c) algum dos componentes do "pathname" não existe (caminho inválido);
			(d) o "pathname" indicado não é um diretório;

Entra:	pathname -> caminho do diretório a ser removido

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Altera o diretório atual de trabalho (working directory).
		O caminho desse diretório é informado no parâmetro "pathname".
		São considerados erros:
			(a) qualquer situação que impeça a realização da operação
			(b) não existência do "pathname" informado.

Entra:	pathname -> caminho do novo diretório de trabalho.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Informa o diretório atual de trabalho.
		O "pathname" do diretório de trabalho deve ser copiado para o buffer indicado por "pathname".
			Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
		São considerados erros:
			(a) quaisquer situações que impeçam a realização da operação
			(b) espaço insuficiente no buffer "pathname", cujo tamanho está informado por "size".

Entra:	pathname -> buffer para onde copiar o pathname do diretório de trabalho
		size -> tamanho do buffer pathname

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size)
{
	if (superBlock == NULL) { initialize(); }
	strncpy(pathname, curPath, size);
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Abre um diretório existente no disco.
	O caminho desse diretório é aquele informado pelo parâmetro "pathname".
	Se a operação foi realizada com sucesso, a função:
		(a) deve retornar o identificador (handle) do diretório
		(b) deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório "pathname".
	O handle retornado será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do diretório.

Entra:	pathname -> caminho do diretório a ser aberto

Saída:	Se a operação foi realizada com sucesso, a função retorna o identificador do diretório (handle).
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname)
{
	if (superBlock == NULL) { initialize(); }
	DIR2 handle = findFreeDirHandle();
	if (handle >= 0)
	{
		char *normPath = normalizePath(pathname);
		DWORD cluster = findDir(normPath, superBlock->RootDirCluster);
		if (cluster > 0)
		{
			DIRHANDLE *dir = malloc(sizeof(DIRHANDLE));
			dir->cluster = cluster;
			dir->dirEntrys = getDirData(cluster);
			dir->entryNum = 0;
			dir->pathName = malloc(strlen(normPath) + 1);
			strcpy(dir->pathName, normPath);
			free(normPath);
			while(dir->dirEntrys[dir->entryNum].TypeVal == TYPEVAL_INVALIDO)
			{
				dir->entryNum++;
				if (dir->entryNum >= MAX_DIR_ENTRIES)
				{
					dir->entryNum = 0;
					break;
				}
			}
			dirHandles[handle] = dir;
			return handle;			
		}
	}	
	return -1;
}

char *normalizePath(char *path)
{
	char *newpath = malloc(strlen(curPath)+1);
	if (path[0] != '/') 
	{
		newpath = realloc(newpath, strlen(curPath) + strlen(curPath)+1);
		strcpy(newpath, curPath);
	}
	strcat(newpath, path);
	return newpath;
}

int findPath(char *pathname, FSREC* record)
{
	char* temp = malloc(strlen(pathname) + 1);
	strcpy(temp, pathname);
	char* token = strtok(temp,'/');

	DWORD curCluster = superBlock->RootDirCluster;
	while(token != NULL)
	{
		char* newtoken = strtok(NULL, '/');
		DWORD lastCluster = curCluster;
		FSREC* curReadingDir = getDirData(curCluster);

		for (int i = 0; i < MAX_DIR_ENTRIES; i++)
		{
			if (strcmp(curReadingDir[i].name, token) == 0)
			{
				curCluster = curReadingDir[i].firstCluster;
				break;
			}
		}
		free(curReadingDir);
		if (lastCluster == curCluster)
			break;
	}
	return 0;
}

FSREC* getDirData(DWORD cluster)
{
	FSREC* dirReading = malloc(sizeof(FSREC) * superBlock->SectorsPerCluster * 4);
	int err;
	for (int nSector = 0; nSector < superBlock->SectorsPerCluster; nSector++)
	{
		err = read_sector(cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, (unsigned char *)&dirReading[nSector*4]);
		if (err) {
			printf ("read_sector (%d) error = %d\n", cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, err);
			free(dirReading);
			return NULL;
		}
	}
	return dirReading;
}


int findFreeDirHandle()
{
	for (int i = 0; i != MAX_HANDLES; i++)
	{
		if (dirHandles[i] == NULL)
		{
			return i;
		}
	}
	return -1;
}

DWORD findFreeCluster()
{
	int FAT_PER_SECTOR = SECTOR_SIZE/4;
	DWORD buffer[FAT_PER_SECTOR];
	int err;
	for (int nSector = superBlock->pFATSectorStart; nSector < superBlock->DataSectorStart; nSector++)
	{
		err = read_sector(nSector, (unsigned char*)&buffer);
		if (err) {
			printf ("read_sector (%d) error = %d\n", nSector, err);
			return 0;
		}
		for (int nCluster = 0; nCluster < FAT_PER_SECTOR; nCluster++)
		{
			if (buffer[nCluster] == 0)
			{
				return nCluster + nSector * FAT_PER_SECTOR;
				break;
			}
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Realiza a leitura das entradas do diretório identificado por "handle".
	A cada chamada da função é lida a entrada seguinte do diretório representado pelo identificador "handle".
	Algumas das informações dessas entradas serão colocadas no parâmetro "dentry".
	Após realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a próxima entrada válida, seguinte à última lida.
	São considerados erros:
		(a) qualquer situação que impeça a realização da operação
		(b) término das entradas válidas do diretório identificado por "handle".

Entra:	handle -> identificador do diretório cujas entradas deseja-se ler.
	dentry -> estrutura de dados onde a função coloca as informações da entrada lida.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero ( e "dentry" não será válido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry)
{
	if (superBlock == NULL) { initialize(); }
	
	DIRHANDLE *dir = dirHandles[handle];
	if (dir != NULL)
	{
		if (dir->entryNum >= 0)
		{
			strcpy(dentry->name, dir->dirEntrys[dir->entryNum].name);
			dentry->fileType = dir->dirEntrys[dir->entryNum].TypeVal;
			dentry->fileSize = dir->dirEntrys[dir->entryNum].bytesFileSize;
	
			dir->entryNum++;
			while(dir->dirEntrys[dir->entryNum].TypeVal == TYPEVAL_INVALIDO)
			{
				dir->entryNum++;
				if (dir->entryNum >= MAX_DIR_ENTRIES)
				{
					dir->entryNum = -1;
					break;
				}
			}
			return 0;
		}
	}
	dentry->fileType = TYPEVAL_INVALIDO;
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Fecha o diretório identificado pelo parâmetro "handle".

Entra:	handle -> identificador do diretório que se deseja fechar (encerrar a operação).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle)
{
	if (superBlock == NULL) { initialize(); }
	if (dirHandles[handle] != NULL)
	{
		free(dirHandles[handle]->pathName);
		free(dirHandles[handle]->dirEntrys);
		free(dirHandles[handle]);
		return 0;
	}
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink) com o nome dado por linkname (relativo ou absoluto) para um arquivo ou diretório fornecido por filename.

Entra:	linkname -> nome do link a ser criado
	filename -> nome do arquivo ou diretório apontado pelo link

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int ln2(char *linkname, char *filename)
{
	if (superBlock == NULL) { initialize(); }
	return -1;
}

	
	
