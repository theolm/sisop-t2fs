
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apidisk.h"
#include "t2fs.h"

#ifndef __TYPEDEF_SUPERBLOCK__
#define __TYPEDEF_SUPERBLOCK__
typedef struct t2fs_superbloco SUPERBLOCK;
#endif

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

int main(int argc, char *argv[])
{
	SUPERBLOCK sb;
	memset(&sb, 0, sizeof(sb));
	int error = read_superblock(&sb);	
	if (error) {
		return 0;//error;
	}
	printf("ID:\t\t\t%.*s\nVersion:\t\t%04X\nSuperblock Size:\t%d\nDisk Size:\t\t%d Bytes\nNumber of Sectors:\t%d\nSectors per Cluster:\t%d\nFAT Sector Start:\t%d\nRoot Dir Cluster:\t%d\nData Sector Start:\t%d\n", 4, sb.id,
	       sb.version, sb.superblockSize, sb.DiskSize, sb.NofSectors, sb.SectorsPerCluster, sb.pFATSectorStart,
		   sb.RootDirCluster, sb.DataSectorStart);
    return 0;
}

	
	
