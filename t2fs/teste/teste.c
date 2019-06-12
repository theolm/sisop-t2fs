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
    printf("\n***TESTE FORMAT********************\n\n");
    resultado = format2(4);
    printf("\n***RESULTADO DO FORMAT: %s **********\n", resultado == 0 ? "OK" : "ERRO");

    return 0;
}
