#include <t2fs.h>

char *converteByteParaHex(BYTE valor);

int converteHexParaInt(char *hex);

char *byteToStr(int i);

void substring(char s[], char sub[], int p, int l);

BYTE *converteByteParaBin(BYTE b);

int converteBinParaByte(BYTE *b);

void montaMbr();

void imprimeMbr();

int getSetorDoBloco(int numBloco);

void salvaBloco(int numBloco, BYTE *bufferBloco);

int getBlocoLivreDoByte(BYTE *b);

int getBlocoLivreDoBitmap();
