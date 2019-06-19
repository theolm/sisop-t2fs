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

    int resultado;
    resultado = format2(4);

	mkdir2("/rui/felipe/cardozo");
	mkdir2("/rui/home/documentos");
	mkdir2("/root/home/downloads");

	FILE2 handle = create2("/rui/home/documentos/foto.jpg");

	close2(handle);

	handle = open2("/rui/home/documentos/foto.jpg");

	close2(handle);


	FILE *fptr;

	if ((fptr = fopen("/home/felipe/Imagens/047.jpg","rb")) == NULL){
		printf("Erro ao abrir arquivo!");
		exit(1);
	}



//	int n;
//	int num;
//	int tamanhoArquivo = 86251;
//	char arquivo[tamanhoArquivo];
//	for(n = 0; n < tamanhoArquivo; ++n)
//	{
//		fread(&num, sizeof(char), 1, fptr);
//		arquivo[n] = num;
//	}
//	fclose(fptr);
//
//
//
//
//	if ((fptr = fopen("/home/felipe/Imagens/teste.jpg","wb")) == NULL){
//		printf("Error! opening file");
//		exit(1);
//	}
//
//	for(n = 0; n < tamanhoArquivo; ++n)
//	{
//		num = arquivo[n];
//		fwrite(&num, sizeof(char), 1, fptr);
//	}
//	fclose(fptr);

//	FILE2 file = create2("/rui/home/documento/arquivo.dat");

//	write2(file, arquivo, 5);
//    printf("\n***TESTE CREATE********************\n\n");
//	mkdir2("/root/home/documentos/teste.txt");
//	mkdir2("/root/home/downloads/arquivo.jpg");
//	mkdir2("/root/home/downloads/outroteste.dat");



    return 0;
}
