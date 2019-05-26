
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apidisk.h"
#include "t2fs.h"

int main(int argc, char *argv[])
{
	char name[128];
	identify2(name, 64);
	printf(ANSI_COLOR_MAGENTA "%s\n\n" ANSI_COLOR_RESET, name);

	SUPERBLOCK sb;
	memset(&sb, 0, sizeof(sb));
	int error = read_superblock(&sb);	
	if (error) {
		return 0;//error;
	}
	printf(ANSI_COLOR_RED "Superblock Content:\n" ANSI_COLOR_RESET);
	printf("ID:\t\t\t%.*s\nVersion:\t\t%04X\nSuperblock Size:\t%d\nDisk Size:\t\t%d Bytes\nNumber of Sectors:\t%d\nSectors per Cluster:\t%d\nFAT Sector Start:\t%d\nRoot Dir Cluster:\t%d\nData Sector Start:\t%d\n", 4, sb.id,
	       sb.version, sb.superblockSize, sb.DiskSize, sb.NofSectors, sb.SectorsPerCluster, sb.pFATSectorStart,
		   sb.RootDirCluster, sb.DataSectorStart);

	
	printf(ANSI_COLOR_RED "\nRoot Dir Content:\n" ANSI_COLOR_RESET);
	struct t2fs_record root[MAX_DIR_ENTRIES];
	unsigned char buffer[SECTOR_SIZE];
	int nSector;
	for (nSector = 0; nSector < sb.SectorsPerCluster; nSector++)
	{
		error = read_sector (sb.RootDirCluster * sb.SectorsPerCluster + sb.DataSectorStart + nSector, buffer);
		if (error) {
			printf ("read_sector (%d) error = %d\n", sb.RootDirCluster * sb.SectorsPerCluster + sb.DataSectorStart + nSector, error);
		}
		else {
			int nIndex;
			for (nIndex = 0; nIndex < 4; nIndex++)
			{
				memcpy(&root[nIndex+nSector*4], &buffer[nIndex * sizeof(struct t2fs_record)], sizeof(struct t2fs_record));
				switch(root[nIndex+nSector*4].TypeVal)
				{
					case TYPEVAL_REGULAR:
						printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, root[nIndex+nSector*4].name);
					break;
					case TYPEVAL_DIRETORIO:
						printf(ANSI_COLOR_CYAN "%s\n" ANSI_COLOR_RESET, root[nIndex+nSector*4].name);
					break;
					case TYPEVAL_LINK:
						printf(ANSI_COLOR_YELLOW "%s\n" ANSI_COLOR_RESET, root[nIndex+nSector*4].name);
					break;
				}
			}
		}
	}
	return error;
	
}

	
	
