#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Cada proceso lee de "izq", imprime el primer numero (es primo)
// y le pasa a su hijo por "der" los que no son multiplos de el.
void
sieve(int izq)
{
  int primo, n, pid;
  int der[2];

  for (;;) {
    // si no llega nada, la cadena termino
    if (read(izq, &primo, sizeof(primo)) != sizeof(primo)) {
      close(izq);
      exit(0);
    }
    printf("prime %d\n", primo);

    if (pipe(der) < 0) {
      fprintf(2, "primes: error en pipe\n");
      exit(1);
    }
    pid = fork();
    if (pid < 0) {
      fprintf(2, "primes: error en fork\n");
      exit(1);
    }

    if (pid == 0) {
      // el hijo pasa a leer de la nueva tuberia
      close(der[1]);
      close(izq);
      izq = der[0];
      continue;
    }

    // el padre filtra y envia al hijo
    close(der[0]);
    while (read(izq, &n, sizeof(n)) == sizeof(n)) {
      if (n % primo != 0)
        write(der[1], &n, sizeof(n));
    }
    close(izq);
    close(der[1]);
    wait(0);
    exit(0);
  }
}

int
main(void)
{
  int p[2], pid;

  if (pipe(p) < 0) {
    fprintf(2, "primes: error en pipe\n");
    exit(1);
  }
  pid = fork();
  if (pid < 0) {
    fprintf(2, "primes: error en fork\n");
    exit(1);
  }

  if (pid == 0) {
    close(p[1]);
    sieve(p[0]);
  }

  // el primer proceso manda del 2 al 35
  close(p[0]);
  for (int i = 2; i <= 35; i++)
    write(p[1], &i, sizeof(i));
  close(p[1]);
  wait(0);
  exit(0);
}
