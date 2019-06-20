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

	FILE2 handle = create2("/rui/home/documentos/foto.jpg");

	close2(handle);

	handle = open2("/rui/home/documentos/foto.jpg");

	close2(handle);


	FILE *fptr;

	if ((fptr = fopen("/home/felipe/Imagens/047.jpg","rb")) == NULL){
		printf("Erro ao abrir arquivo!");
		exit(1);
	}

    return 0;
}
