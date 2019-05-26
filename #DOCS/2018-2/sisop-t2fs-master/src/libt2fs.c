
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "apidisk.h"
#include "t2fs.h"

static const char* NOMES = "Natanael Rodrigo Zimmer - 00252850, Jessica Lorencetti 00228342";
DIRHANDLE* dirHandles[MAX_HANDLES];
FILEHANDLE* fileHandles[MAX_HANDLES];
SUPERBLOCK* superBlock = NULL;
DIRHANDLE curDir;
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

	curDir.cluster = superBlock->RootDirCluster;
	curDir.dirEntrys = malloc(sizeof(FSRECORD) * superBlock->SectorsPerCluster * 4);
	getDirData(curDir.dirEntrys, curDir.cluster);
	curDir.entryNum = 0;


	for (int i = 0; i != MAX_HANDLES; i++)
	{
		dirHandles[i] = NULL;
		fileHandles[i] = NULL;
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

	char *name = getLastToken(filename);

	if (strlen(name) <= 51)
	{
		if (findEntry(filename, superBlock->RootDirCluster) != 1)
		{
			FSRECORD parent[superBlock->SectorsPerCluster * 4];
			getParentDirData(parent, filename, superBlock->RootDirCluster);

			int nParent = -1;
			for (int i =0; i < MAX_DIR_ENTRIES; i++)
			{
				if (parent[i].TypeVal == TYPEVAL_INVALIDO)
				{
					nParent = i;
					break;
				}
			}

			if (nParent >= 0)
			{
				FREECLUSTER clusterRec;
				int hasCluster = findFreeCluster(&clusterRec);
				if (hasCluster)
				{
					parent[nParent].TypeVal = TYPEVAL_REGULAR;
					parent[nParent].bytesFileSize = 0;
					parent[nParent].clustersFileSize = 1;
					parent[nParent].firstCluster = clusterRec.cluster;
					strncpy(parent[nParent].name, name, 51);
					free(name);
					updateClusterInFat(clusterRec.cluster, 0xFFFFFFFF);
					write_sector(parent[0].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + (nParent / 4),(unsigned char *)&parent[(nParent / 4)*4]);
					return open2(filename);
				}
				return -4;
			}
			return -3;
		}
		return -2;
	}
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

	char *nome = getLastToken(filename);
	FSRECORD parent[superBlock->SectorsPerCluster * 4];
	getParentDirData(parent, filename, superBlock->RootDirCluster);
	int nParent = -1;
	for (int i = 0; i < MAX_DIR_ENTRIES; i++)
	{
		if (strcmp(parent[i].name, nome) == 0)
		{
			nParent = i;
			break;
		}
	}
	if (nParent >= 0)
	{
		DWORD clusterToUpdate = parent[nParent].firstCluster;
		for (int i = 0; i < parent[nParent].clustersFileSize; i++)
		{
			DWORD nextclusterToUpdate = getClusterInFat(clusterToUpdate);
			updateClusterInFat(clusterToUpdate, 0x0);
			clusterToUpdate = nextclusterToUpdate;
		}
		parent[nParent].TypeVal = TYPEVAL_INVALIDO;
		parent[nParent].firstCluster = 0;
		parent[nParent].clustersFileSize = 0;
		write_sector(parent[0].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + (nParent / 4),(unsigned char *)&parent[(nParent / 4)*4]);
		return 0;
	}
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
	FILE2 handle = findFreeFileHandle();
	if (handle >= 0)
	{
		FSRECORD* rec = malloc(sizeof(FSRECORD));
		DWORD cluster = findFile(rec, filename, superBlock->RootDirCluster);
		if (cluster != 0)
		{
			FILEHANDLE *file = malloc(sizeof(FILEHANDLE));
			file->name = getLastToken(filename);
			file->file = rec;
			file->current_pointer = 0;
			file->dirCluster = cluster;
			fileHandles[handle] = file;
			return handle;			
		}
	}	
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
	if (fileHandles[handle] != NULL)
	{
		//free(dirHandles[handle]->pathName);
		free(fileHandles[handle]->file);
		free(fileHandles[handle]);
		fileHandles[handle] = NULL;
		return 0;
	}
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
	FILEHANDLE *fHandle = fileHandles[handle];
	if (fHandle != NULL)
	{
		if (fHandle->current_pointer < fHandle->file->bytesFileSize)
		{
			int nRead = 0;
			unsigned char data[superBlock->SectorsPerCluster * SECTOR_SIZE];
			if (fHandle->file->clustersFileSize == 1)
			{
				readCluster(data, fHandle->file->firstCluster);
				nRead = fHandle->file->bytesFileSize - fHandle->current_pointer;
				if (nRead > size)
					nRead = size;
				memcpy(buffer, data + fHandle->current_pointer, nRead);
				fHandle->current_pointer += nRead;
			}
			else
			{
				DWORD clusterToRead = fHandle->file->firstCluster;
				for (int nCluster = 0; nCluster < fHandle->file->clustersFileSize; nCluster++)
				{
					//avanço cluster até chegar no ponteiro
					while(fHandle->current_pointer >= (nCluster + 1) * superBlock->SectorsPerCluster * SECTOR_SIZE)
						{
							nCluster++;
							clusterToRead = getClusterInFat(clusterToRead);
						}
					readCluster(data, clusterToRead);
					int stepRead = fHandle->file->bytesFileSize - fHandle->current_pointer;
					if (stepRead > size)
						stepRead = size;
					if (stepRead > superBlock->SectorsPerCluster * SECTOR_SIZE)
						stepRead = superBlock->SectorsPerCluster * SECTOR_SIZE;

					memcpy(buffer + nRead, data + fHandle->current_pointer - (nCluster * superBlock->SectorsPerCluster * SECTOR_SIZE), stepRead);
					fHandle->current_pointer += stepRead;
					nRead += stepRead;
					clusterToRead = getClusterInFat(clusterToRead);
				}
			}
			return nRead;
		}
		return 0;
	}
	return -1;
}

int readCluster(unsigned char * buffer, DWORD cluster)
{
	int err;
	for (int nSector = 0; nSector < superBlock->SectorsPerCluster; nSector++)
	{
		err = read_sector(cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, buffer + nSector*256);
		if (err) {
			printf ("read_sector (%d) error = %d\n", cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, err);
			return 0;
		}
	}
	return 1;
}

int writeCluster(unsigned char * buffer, DWORD cluster)
{
	int err;
	for (int nSector = 0; nSector < superBlock->SectorsPerCluster; nSector++)
	{
		err = write_sector(cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, buffer + nSector*256);
		if (err) {
			printf ("write_sector (%d) error = %d\n", cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, err);
			return 0;
		}
	}
	return 1;
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
	FILEHANDLE *fHandle = fileHandles[handle];
	if (fHandle != NULL)
	{
		int nWrite = 0;
		unsigned char data[superBlock->SectorsPerCluster * SECTOR_SIZE];
		FREECLUSTER clusterRec;
		int nCluster = 0;
		DWORD clusterToWrite = fHandle->file->firstCluster;
		//avanço cluster até chegar no ponteiro
		while(fHandle->current_pointer >= (nCluster + 1) * superBlock->SectorsPerCluster * SECTOR_SIZE)
			{
				nCluster++;
				clusterToWrite = getClusterInFat(clusterToWrite);
			}
		while (nWrite < size)
		{
			readCluster(data, clusterToWrite);
			int stepWrite = ((nCluster + 1) * superBlock->SectorsPerCluster * SECTOR_SIZE) - fHandle->current_pointer;
			if (stepWrite > size)
				stepWrite = size - nWrite;
			if (stepWrite > superBlock->SectorsPerCluster * SECTOR_SIZE)
				stepWrite = superBlock->SectorsPerCluster * SECTOR_SIZE;

			memcpy(data + fHandle->current_pointer - (nCluster * superBlock->SectorsPerCluster * SECTOR_SIZE), buffer + nWrite, stepWrite);
			writeCluster(data, clusterToWrite);
			fHandle->current_pointer += stepWrite;
			nWrite += stepWrite;
			if (nWrite < size)
			{
				int hasCluster = findFreeCluster(&clusterRec);
				if (!hasCluster)
					return nWrite;
				updateClusterInFat(clusterToWrite, clusterRec.cluster);
				clusterToWrite = clusterRec.cluster;
				nCluster++;
			}
			else
			{
				updateClusterInFat(clusterToWrite, 0xFFFFFFFF);
			}
		}
		fHandle->file->clustersFileSize = nCluster + 1;
		fHandle->file->bytesFileSize = fHandle->current_pointer;
		updateFileInDir(handle);
		return nWrite;
	}
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
	if (fileHandles[handle] != NULL)
	{
		fileHandles[handle]->file->bytesFileSize = fileHandles[handle]->current_pointer;
		fileHandles[handle]->file->clustersFileSize = ceil((float)fileHandles[handle]->current_pointer / (superBlock->SectorsPerCluster * SECTOR_SIZE));
		updateFileInDir(handle);
		updateFileInFat(handle);
		return 0;
	}
	return -1;
}

void updateFileInDir(FILE2 handle)
{
	if (fileHandles[handle] != NULL)
	{
		FSRECORD parent[superBlock->SectorsPerCluster * 4];
		readCluster((unsigned char *)&parent, fileHandles[handle]->dirCluster);
		for (int i = 0; i < MAX_DIR_ENTRIES; i++)
		{
			if (strcmp(parent[i].name, fileHandles[handle]->name) == 0)
			{
				parent[i].bytesFileSize = fileHandles[handle]->file->bytesFileSize;
				parent[i].clustersFileSize = fileHandles[handle]->file->clustersFileSize;
				break;
			}
		}
		writeCluster((unsigned char *)&parent, fileHandles[handle]->dirCluster);
	}
}

void updateFileInFat(FILE2 handle)
{
	if (fileHandles[handle] != NULL)
	{
		DWORD clusterToUpdate = fileHandles[handle]->file->firstCluster;
		for (int i = 0; i < fileHandles[handle]->file->clustersFileSize; i++)
		{
			clusterToUpdate = getClusterInFat(clusterToUpdate);
		}
		updateClusterInFat(clusterToUpdate, 0xFFFFFFFF);
		while ((clusterToUpdate >= 0x2) && (clusterToUpdate <= 0xFFFFFFFD))
		{
			updateClusterInFat(clusterToUpdate,0);
			clusterToUpdate = getClusterInFat(clusterToUpdate);
		}
	}
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
	if (fileHandles[handle] != NULL)
	{
		if ((offset == -1) || (offset > fileHandles[handle]->file->bytesFileSize))
		{
			fileHandles[handle]->current_pointer = fileHandles[handle]->file->bytesFileSize;
			return 0;
		}
		fileHandles[handle]->current_pointer = offset;
		return 0;
	}
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

	char *name = getLastToken(pathname);

	if (strlen(name) <= 51)
	{
		if (findEntry(pathname, superBlock->RootDirCluster) != 1)
		{
			FSRECORD parent[superBlock->SectorsPerCluster * 4];
			getParentDirData(parent, pathname, superBlock->RootDirCluster);

			int nParent = -1;
			for (int i =0; i < MAX_DIR_ENTRIES; i++)
			{
				if (parent[i].TypeVal == TYPEVAL_INVALIDO)
				{
					nParent = i;
					break;
				}
			}

			if (nParent >= 0)
			{
				FREECLUSTER clusterRec;
				int hasCluster = findFreeCluster(&clusterRec);
				if (hasCluster)
				{
					parent[nParent].TypeVal = TYPEVAL_DIRETORIO;
					parent[nParent].bytesFileSize = 1024;
					parent[nParent].clustersFileSize = 1;
					parent[nParent].firstCluster = clusterRec.cluster;
					strncpy(parent[nParent].name, name, 51);
					free(name);
					FSRECORD newdir[superBlock->SectorsPerCluster * 4];
					memset(newdir, 0, superBlock->SectorsPerCluster * 4 * sizeof(FSRECORD));
					newdir[0].TypeVal = TYPEVAL_DIRETORIO;
					strcpy(newdir[0].name,".");
					newdir[0].bytesFileSize = 1024;
					newdir[0].clustersFileSize = 1;
					newdir[0].firstCluster = clusterRec.cluster;
					newdir[1].TypeVal = TYPEVAL_DIRETORIO;
					strcpy(newdir[1].name,"..");
					newdir[1].bytesFileSize = 1024;
					newdir[1].clustersFileSize = 1;
					newdir[1].firstCluster = parent[0].firstCluster;
					//clusterRec.clusters[clusterRec.fatPos] = 0xFFFFFFFF;
					updateClusterInFat(clusterRec.cluster, 0xFFFFFFFF);
					write_sector(parent[0].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + (nParent / 4),(unsigned char *)&parent[(nParent / 4)*4]);
					write_sector(clusterRec.cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart,(unsigned char *)&newdir[0]);
					return 0;
				}
				return -4;
			}
			return -3;
		}
		return -2;
	}
	return -1;
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

	DWORD cluster = findDir(pathname, superBlock->RootDirCluster);
	if (cluster > 0)
	{
		//Existe
		char *nome = getLastToken(pathname);
		FSRECORD deldir[superBlock->SectorsPerCluster * 4];
		getDirData(deldir, cluster);
		int nContent = 0;
		for (int i = 0; i < MAX_DIR_ENTRIES; i++)
		{
			if (deldir[i].TypeVal != TYPEVAL_INVALIDO)
				nContent++;
		}
		if (nContent == 2)
		{
			//Vazio
			FSRECORD parent[superBlock->SectorsPerCluster * 4];
			getDirData(parent, deldir[1].firstCluster);
			int nParent = -1;
			for (int i = 0; i < MAX_DIR_ENTRIES; i++)
			{
				if (strcmp(parent[i].name, nome) == 0)
				{
					parent[i].TypeVal = TYPEVAL_INVALIDO;
					nParent = i;
					break;
				}
			}
			updateClusterInFat(parent[nParent].firstCluster, 0x0);
			write_sector(parent[0].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + (nParent / 4),(unsigned char *)&parent[(nParent / 4)*4]);
			return 0;
		}
		return -2;
	}
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

	DWORD cluster = findDir(pathname, superBlock->RootDirCluster);
	if (cluster > 0)
	{
		if (cluster == superBlock->RootDirCluster)
		{
			free(curPath);
			curPath = malloc(sizeof(char)*2);
			curPath[0] = '/';
			curPath[1] = 0;
			return 0;
		}

		curDir.cluster = cluster;
		getDirData(curDir.dirEntrys, cluster);
		curDir.entryNum = 0;
		DIRHANDLE parent;
		parent.dirEntrys = malloc(sizeof(FSRECORD) * superBlock->SectorsPerCluster * 4);
		free(curPath);
		curPath = malloc(2);
		curPath[0] = '/';
		curPath[1] = 0;
		DWORD cluster = curDir.dirEntrys[1].firstCluster;
		DWORD cluster2 = curDir.cluster;

		while (getDirData(parent.dirEntrys, cluster))
		{
			for (int i = 0; i < MAX_DIR_ENTRIES; i++)
			{
				if (parent.dirEntrys[i].firstCluster == cluster2)
				{
					char *temp = malloc(strlen(parent.dirEntrys[i].name) + strlen(curPath) + 2);
					strcpy(temp, "/");
					strcat(temp, parent.dirEntrys[i].name);
					strcat(temp, curPath);
					free(curPath);
					curPath = temp;
					break;
				}
			}
			if (cluster == superBlock->RootDirCluster)
				break;
			cluster = parent.dirEntrys[1].firstCluster;
			cluster2 = parent.dirEntrys[0].firstCluster;
		}
		return 0;
	}
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
		DWORD cluster = findDir(pathname, superBlock->RootDirCluster);
		if (cluster > 0)
		{
			DIRHANDLE *dir = malloc(sizeof(DIRHANDLE));
			dir->cluster = cluster;
			dir->dirEntrys = malloc(sizeof(FSRECORD) * superBlock->SectorsPerCluster * 4);
			getDirData(dir->dirEntrys, cluster);
			dir->entryNum = 0;
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

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
int tokenize(char *in, char *delim, char ***intokens)
{
    int count = 0;
    char *ptr = in;
    ptr++;
    while((ptr = strpbrk(ptr, delim)) != NULL) 
    {
        count++;
        ptr++;
    }

    char **tokens = malloc(count * sizeof(char *));
    char *copy = malloc(strlen(in)+1);
    strcpy(copy, in);
    char *token = strtok(copy,delim);
    int tokenNumber = 0;
    while (token != NULL)
    {
        //tokens = (char **)realloc(tokens, (tokenNumber+1) * sizeof(char *));
        tokens[tokenNumber] = malloc(strlen(token)+1);
        strcpy(tokens[tokenNumber++],token);
        token = strtok(NULL, delim);
    }
    (*intokens) = tokens;
    free(copy);
    return count;
}

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
void freeTokens(char ***tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        free((*tokens)[i]);
    }
    free((*tokens));
}

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
void str_replace(char *target, const char *needle, const char *replacement)
{
    char buffer[4096] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        tmp = p + needle_len;
    }

    strcpy(target, buffer);
	if (strstr(target, needle) != NULL)
		str_replace(target, needle, replacement);
}

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
DWORD findDir(char* pathname, DWORD cluster)
{
	char **tokens;
	char *findPath;
	if (pathname[0] != '/')
	{
		findPath = malloc(strlen(curPath) + strlen(pathname) + 2);
		strcpy(findPath, curPath);
		strcat(findPath, pathname);
		strcat(findPath, "/");
	}
	else
	{
		findPath = malloc(strlen(pathname) + 2);
		strcpy(findPath, pathname);
		strcat(findPath, "/");
	}
	str_replace(findPath,"//", "/");
	int count = tokenize(findPath, "/", &tokens);
	if (count == 0)
	{
		return cluster;
	}
	FSRECORD dir[4* superBlock->SectorsPerCluster];
	int res = getParentDirData(dir, findPath, cluster);
	if (res == 0)
		return 0;
	for (int nEntry = 0; nEntry < MAX_DIR_ENTRIES; nEntry++)
	{
		if (strcmp(dir[nEntry].name, tokens[count-1]) == 0)
		{
			switch(dir[nEntry].TypeVal)
			{
				case TYPEVAL_INVALIDO: 
				case TYPEVAL_REGULAR:
					{
						freeTokens(&tokens, count);
						return 0;
					}
				case TYPEVAL_DIRETORIO:
					{
						DWORD result = dir[nEntry].firstCluster;
						freeTokens(&tokens, count);
						return result;
					}
				case TYPEVAL_LINK:
					{
						freeTokens(&tokens, count);
						char linkDst[255];
						read_sector(dir[nEntry].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart, (unsigned char *)linkDst);
						return findDir(linkDst, superBlock->DataSectorStart);
					}
			}
		}
	}
	return 0;
}


/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
int findEntry(char* pathname, DWORD cluster)
{
	char **tokens;
	char *findPath;
	if (pathname[0] != '/')
	{
		findPath = malloc(strlen(curPath) + strlen(pathname) + 2);
		strcpy(findPath, curPath);
		strcat(findPath, pathname);
		strcat(findPath, "/");
	}
	else
	{
		findPath = malloc(strlen(pathname) + 2);
		strcpy(findPath, pathname);
		strcat(findPath, "/");
	}
	str_replace(findPath,"//", "/");
	int count = tokenize(findPath, "/", &tokens);
	if (count == 0)
	{
		return 1;
	}
	FSRECORD dir[4* superBlock->SectorsPerCluster];
	int res = getParentDirData(dir, findPath, cluster);
	free(findPath);
	if (res == 0)
		return 0;
	for (int nEntry = 0; nEntry < MAX_DIR_ENTRIES; nEntry++)
	{
		if (strcmp(dir[nEntry].name, tokens[count-1]) == 0)
		{
			switch(dir[nEntry].TypeVal)
			{
				case TYPEVAL_INVALIDO: 
					{
						freeTokens(&tokens, count);
						return 0;
					}
				case TYPEVAL_DIRETORIO:
				case TYPEVAL_REGULAR:
				case TYPEVAL_LINK:
					{
						freeTokens(&tokens, count);
						return 1;
					}
			}
		}
	}
	return 0;
}

DWORD findFile(FSRECORD* rec, char* pathname, DWORD cluster)
{
	char **tokens;
	char *findPath;
	if (pathname[0] != '/')
	{
		findPath = malloc(strlen(curPath) + strlen(pathname) + 2);
		strcpy(findPath, curPath);
		strcat(findPath, pathname);
		strcat(findPath, "/");
	}
	else
	{
		findPath = malloc(strlen(pathname) + 2);
		strcpy(findPath, pathname);
		strcat(findPath, "/");
	}
	str_replace(findPath,"//", "/");
	int count = tokenize(findPath, "/", &tokens);
	if (count == 0)
	{
		return 0;
	}
	FSRECORD dir[4* superBlock->SectorsPerCluster];
	int res = getParentDirData(dir, findPath, cluster);
	free(findPath);
	if (res == 0)
		return 0;
	for (int nEntry = 0; nEntry < MAX_DIR_ENTRIES; nEntry++)
	{
		if (strcmp(dir[nEntry].name, tokens[count-1]) == 0)
		{
			switch(dir[nEntry].TypeVal)
			{
				case TYPEVAL_DIRETORIO:
				case TYPEVAL_INVALIDO: 
					{
						freeTokens(&tokens, count);
						return 0;
					}
				case TYPEVAL_LINK:
					{
						freeTokens(&tokens, count);
						char linkDst[255];
						read_sector(dir[nEntry].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart, (unsigned char *)linkDst);
						return findFile(rec, linkDst, superBlock->RootDirCluster);
					}
				case TYPEVAL_REGULAR:
					{
						freeTokens(&tokens, count);
						//FSRECORD* res = malloc(sizeof(FSRECORD));
						rec->bytesFileSize = dir[nEntry].bytesFileSize;
						rec->clustersFileSize = dir[nEntry].clustersFileSize;
						rec->firstCluster = dir[nEntry].firstCluster;
						rec->TypeVal = dir[nEntry].TypeVal;
						strcpy(rec->name, dir[nEntry].name);
						return dir[0].firstCluster;
					}
			}
		}
	}
	return 0;
}

char *getLastToken(char *pathname)
{
	char **tokens;
	char *findPath;
	if (pathname[0] != '/')
	{
		findPath = malloc(strlen(curPath) + strlen(pathname) + 2);
		strcpy(findPath, curPath);
		strcat(findPath, pathname);
		strcat(findPath, "/");
	}
	else
	{
		findPath = malloc(strlen(pathname) + 2);
		strcpy(findPath, pathname);
		strcat(findPath, "/");
	}
	str_replace(findPath,"//", "/");
	int count = tokenize(findPath, "/", &tokens);
	char * result = malloc(strlen(tokens[count-1])+1);
	strcpy(result,tokens[count-1]);
	freeTokens(&tokens, count);
	free(findPath);
	return result;
}

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
int getParentDirData(FSRECORD* dir, char* pathname, DWORD cluster)
{
	char **tokens;
	char *findPath;
	if (pathname[0] != '/')
	{
		findPath = malloc(strlen(curPath) + strlen(pathname) + 2);
		strcpy(findPath, curPath);
		strcat(findPath, pathname);
		strcat(findPath, "/");
	}
	else
	{
		findPath = malloc(strlen(pathname) + 2);
		strcpy(findPath, pathname);
		strcat(findPath, "/");
	}
	str_replace(findPath,"//", "/");

	DWORD curCluster = cluster;
	int count = tokenize(findPath, "/", &tokens);
	int res = getDirData(dir, curCluster);
	for (int nPath = 0; nPath < count-1; nPath ++)
	{
		curCluster = 0;
		for (int nEntry = 0; nEntry < MAX_DIR_ENTRIES; nEntry++)
		{
			if (strcmp(dir[nEntry].name, tokens[nPath]) == 0)
			{
				switch(dir[nEntry].TypeVal)
				{
					case TYPEVAL_INVALIDO: 
					case TYPEVAL_REGULAR:
						{
							freeTokens(&tokens, count);
							return 0;
						}
					case TYPEVAL_LINK:
						{
							freeTokens(&tokens, count);
							char linkDst[255];
							read_sector(dir[nEntry].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart, (unsigned char *)linkDst);
							return getParentDirData(dir, linkDst, superBlock->RootDirCluster);
						}
					case TYPEVAL_DIRETORIO:
						{
							curCluster = dir[nEntry].firstCluster;
							res = getDirData(dir, curCluster);
							break;
						}
				}
			}
		}
		if (curCluster == 0)
			return 0;
	}
	if (res == 0)
		res = getDirData(dir, cluster);
	return res;
}

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
int getDirData(FSRECORD* dir, DWORD cluster)
{
	int err;
	for (int nSector = 0; nSector < superBlock->SectorsPerCluster; nSector++)
	{
		err = read_sector(cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, (unsigned char *)&dir[nSector*4]);
		if (err) {
			printf ("read_sector (%d) error = %d\n", cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + nSector, err);
			return 0;
		}
	}
	return 1;
}


/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
int findFreeFileHandle()
{
	for (int i = 0; i != MAX_HANDLES; i++)
	{
		if (fileHandles[i] == NULL)
		{
			return i;
		}
	}
	return -1;
}

/*-------------------------------------------------------------------------------
-------------------------------------------------------------------------------*/
int findFreeCluster(FREECLUSTER* rec)
{
	int FAT_PER_SECTOR = SECTOR_SIZE/4;
	DWORD buffer[FAT_PER_SECTOR];
	int err;
	for (int nSector = superBlock->pFATSectorStart; nSector < superBlock->DataSectorStart; nSector++)
	{
		err = read_sector(nSector, (unsigned char*)buffer);
		if (err) {
			printf ("read_sector (%d) error = %d\n", nSector, err);
			return 0;
		}
		for (int nCluster = 0; nCluster < FAT_PER_SECTOR; nCluster++)
		{
			if (buffer[nCluster] == 0)
			{
				rec->cluster = nCluster;
				rec->fatPos = nCluster;
				rec->sector = nSector;
				printf("%s:\n\tcluster: %x\n\tfatpos: %x\n\tsector: %x\n",__FUNCTION__,rec->cluster,rec->fatPos,rec->sector);
				return 1;
			}
		}
	}
	return 0;
}

void updateClusterInFat(DWORD cluster, DWORD value)
{
	int FAT_PER_SECTOR = SECTOR_SIZE/4;
	int err;
	DWORD buffer[FAT_PER_SECTOR];
	int nSector = superBlock->pFATSectorStart + (cluster/64)*64;

	err = read_sector(nSector, (unsigned char*)buffer);
	if (err) {
		printf ("read_sector (%d) error = %d\n", nSector, err);
		return;
	}
	buffer[cluster % 64] = value;
	write_sector(nSector, (unsigned char *)buffer);
}

DWORD getClusterInFat(DWORD cluster)
{
	int FAT_PER_SECTOR = SECTOR_SIZE/4;
	int err;
	DWORD buffer[FAT_PER_SECTOR];
	int nSector = superBlock->pFATSectorStart + (cluster/64)*64;

	err = read_sector(nSector, (unsigned char*)buffer);
	if (err) {
		printf ("read_sector (%d) error = %d\n", nSector, err);
		return 0;
	}
	return buffer[cluster % 64];
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
		//free(dirHandles[handle]->pathName);
		free(dirHandles[handle]->dirEntrys);
		free(dirHandles[handle]);
		dirHandles[handle] = NULL;
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

	char *name = getLastToken(linkname);

	if ((strlen(name) <= 51) && (strlen(filename) <= MAX_FILE_NAME_SIZE))
	{
		FSRECORD frec;
		if ((findEntry(linkname, superBlock->RootDirCluster) != 1) && (findFile(&frec, filename, superBlock->RootDirCluster) != 0))
		{
			FSRECORD parent[superBlock->SectorsPerCluster * 4];
			getParentDirData(parent, linkname, superBlock->RootDirCluster);

			int nParent = -1;
			for (int i =0; i < MAX_DIR_ENTRIES; i++)
			{
				if (parent[i].TypeVal == TYPEVAL_INVALIDO)
				{
					nParent = i;
					break;
				}
			}

			if (nParent >= 0)
			{
				FREECLUSTER clusterRec;
				int hasCluster = findFreeCluster(&clusterRec);
				if (hasCluster)
				{
					parent[nParent].TypeVal = TYPEVAL_LINK;
					parent[nParent].bytesFileSize = 1024;
					parent[nParent].clustersFileSize = 1;
					parent[nParent].firstCluster = clusterRec.cluster;
					strncpy(parent[nParent].name, name, 51);
					free(name);
					char buffer[256];
					memset(buffer, 0, 256);
					memcpy(buffer, filename, strlen(filename));
					updateClusterInFat(clusterRec.cluster, 0xFFFFFFFF);
					write_sector(parent[0].firstCluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart + (nParent / 4),(unsigned char *)&parent[(nParent / 4)*4]);
					write_sector(clusterRec.cluster * superBlock->SectorsPerCluster + superBlock->DataSectorStart,(unsigned char *)buffer);
					return 0;
				}
				return -4;
			}
			return -3;
		}
		return -2;
	}
	return -1;
}

	
	
