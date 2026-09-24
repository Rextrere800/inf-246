#include "kernel/types.h"
#include "user/user.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winfinite-recursion"
void etapa(int izq){
    int prim, der[2], n;
    if(read(izq, &prim, sizeof(prim)) == 0){
            close(izq);
            exit(0);
    }
    printf("prime %d\n", prim);
    pipe(der);
    if(fork() == 0){
        close(der[1]);
        close(izq);
        etapa(der[0]);
    }
    close(der[0]); //padre no lee a la der
    while(read(izq, &n, sizeof(n)) == sizeof(n)){
        if(n % prim != 0){
            write(der[1], &n, sizeof(n));
        }

    }
    close(izq);
    close(der[1]);
    wait(0);
    exit(0);
}
#pragma GCC diagnostic pop

int main(void){

    int p[2];
    int pid, i;
    pipe(p);
    pid = fork();



    if(pid == 0){
        close(p[1]);// el hijo lee, por lo que puede hacer errores dejar la escritura abierta
        etapa(p[0]);
    }else{
        close(p[0]);//el padre solo escribira
        
        for(i = 2; i <= 35; i++){//ciclo para generar los numeros y escribirlos en la p[1]
            write(p[1], &i, sizeof(i));
        }
        close(p[1]);//en este punto ya termino de escirbir
        wait(0);//espera a que el hijo termine
        exit(0);

    }

    exit(0);
}