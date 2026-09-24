#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Una etapa de la criba: lee de la tubería izquierda, imprime el primer
// número (siempre primo) y reenvía a su hijo los que no son múltiplos.
void
sieve(int left)
{
  int prime, n, pid;
  int right[2];

  // Si no llega nada, los procesos anteriores terminaron: fin de la cadena
  if(read(left, &prime, sizeof(prime)) != sizeof(prime)){
    close(left);
    exit(0);
  }
  printf("prime %d\n", prime);

  if(pipe(right) < 0){
    fprintf(2, "primes: pipe falló\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    fprintf(2, "primes: fork falló\n");
    exit(1);
  }

  if(pid == 0){
    // Hijo (nieto de la cadena): solo necesita el extremo de lectura nuevo
    close(right[1]);
    close(left);
    sieve(right[0]);
  }

  // Proceso actual: filtra los múltiplos de su primo
  close(right[0]);
  while(read(left, &n, sizeof(n)) == sizeof(n)){
    if(n % prime != 0)
      write(right[1], &n, sizeof(n));
  }
  close(left);
  close(right[1]);  // el hijo recibe EOF y puede terminar
  wait(0);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int p[2], pid;

  if(pipe(p) < 0){
    fprintf(2, "primes: pipe falló\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    fprintf(2, "primes: fork falló\n");
    exit(1);
  }

  if(pid == 0){
    close(p[1]);
    sieve(p[0]);
  }

  // Proceso padre inicial: genera 2..35
  close(p[0]);
  for(int i = 2; i <= 35; i++)
    write(p[1], &i, sizeof(i));
  close(p[1]);
  wait(0);
  exit(0);
}
