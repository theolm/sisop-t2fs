#include "../include/t2fs.h"
#include "../include/libaux.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    /*printf("\nTESTE de main\n");

    char members[100];

    if(identify2(members, 100) == 0) {
        printf("%s\n\n", members);
    } else {
        printf("cidentify erro!\n\n");
    }

    format2(4);

	mkdir2("cardozo");
	mkdir2("/rui/home/documentos");
	mkdir2("/root/home/downloads");
	mkdir2("/rui/zanza/kim");
	mkdir2("/rui/zanza/belinha");
	mkdir2("/rui/zanza/hanna");
	mkdir2("/rui/zanza/belinha/teste");

	//selecionar um arquivo real no teu disco
	//usar arquivo de teste com poucos kbytes ex 80kb
	char *nomeArquivoOrigem = "/home/felipe/Imagens/047.jpg";
	char *nomeArquivoDestino= "/home/felipe/Imagens/teste2.jpg";

	FILE2 handle = create2("/rui/home/documentos/foto.jpg");

	close2(handle);

	handle = open2("/rui/home/documentos/foto.jpg");

	int tamanhoArquivo = 86251;
	char arquivoOrigem[tamanhoArquivo];
	char arquivoDestino[tamanhoArquivo];

	FILE *fptr;
	if ((fptr = fopen(nomeArquivoOrigem,"rb")) == NULL){
		printf("Erro ao abrir arquivo!");
		exit(1);
	}
	int i;
	int num;
	for(i = 0; i < tamanhoArquivo; ++i) {
		fread(&num, sizeof(char), 1, fptr);
		arquivoOrigem[i] = num;
	}
	fclose(fptr);

	write2(handle, arquivoOrigem, tamanhoArquivo);

	close2(handle);

	handle = open2("/rui/home/documentos/foto.jpg");

	read2(handle, arquivoDestino, tamanhoArquivo);

	if ((fptr = fopen(nomeArquivoDestino,"wb")) == NULL){
		printf("Error! opening file");
		exit(1);
	}

	for(i = 0; i < tamanhoArquivo; ++i)	{
		num = arquivoDestino[i];
		fwrite(&num, sizeof(char), 1, fptr);
	}
	fclose(fptr);*/



	create2("/rui/zanza/belinha/teste/arquivo");



    return 0;
}
