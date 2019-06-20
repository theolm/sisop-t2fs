#include "../include/t2fs.h"
#include "../include/libaux.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("\nTESTE de main\n");

    char members[100];

    if(identify2(members, 100) == 0) {
        printf("%s\n\n", members);
    } else {
        printf("cidentify erro!\n\n");
    }

    format2(4);

	mkdir2("/rui/felipe/cardozo");
	mkdir2("/rui/home/documentos");
	mkdir2("/root/home/downloads");
	mkdir2("/rui/zanza/kim");
	mkdir2("/rui/zanza/belinha");
	mkdir2("/rui/zanza/hanna");
	mkdir2("/rui/zanza/belinha/teste");

	//selecionar um arquivo real no teu disco
	//usar arquivo de teste com poucos kbytes ex 80kb
	char *nomeArquivoOrigem = "/home/felipe/Imagens/047.jpg";

	FILE2 handle = create2("/rui/home/documentos/foto.jpg");

	close2(handle);

	handle = open2("/rui/home/documentos/foto.jpg");

	int tamanhoArquivo = 86251;
	char arquivoOrigem[tamanhoArquivo];

	FILE *fptr;
	if ((fptr = fopen(nomeArquivoOrigem,"rb")) == NULL){
		printf("Erro ao abrir arquivo!");
		exit(1);
	}
	int n;
	int num;
	for(n = 0; n < tamanhoArquivo; ++n)
	{
		fread(&num, sizeof(char), 1, fptr);
		arquivoOrigem[n] = num;
	}
	fclose(fptr);

	write2(handle, arquivoOrigem, tamanhoArquivo);

    return 0;
}
