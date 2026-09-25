#include "kernel/types.h"   // Tipos básicos de xv6
#include "kernel/stat.h"    // Estructuras relacionadas con archivos
#include "user/user.h"      // Funciones de usuario: fork, pipe, read, write, etc.

// Recibe números por una tubería y va filtrando los múltiplos
void sieve(int izq) {
  int primo, n, pid;        // Variables para el primo, números y PID
  int der[2];               // Nueva tubería hacia el siguiente proceso

  for (;;) {                // Se repite para continuar la cadena de procesos

    // Lee el primer número recibido, que será primo
    if (read(izq, &primo, sizeof(primo)) != sizeof(primo)) {
      close(izq);           // Cierra la tubería si ya no quedan datos
      exit(0);              // Termina el proceso
    }

    printf("prime %d\n", primo); // Muestra el número primo encontrado

    // Crea una nueva tubería para el siguiente proceso
    if (pipe(der) < 0) {
      fprintf(2, "primes: error en pipe\n"); // Muestra error
      exit(1);                               // Termina con error
    }

    pid = fork();            // Crea un nuevo proceso hijo

    if (pid < 0) {
      fprintf(2, "primes: error en fork\n"); // Muestra error
      exit(1);                               // Termina con error
    }

    if (pid == 0) {          // Código que ejecuta el proceso hijo
      close(der[1]);         // El hijo no escribirá en la nueva tubería
      close(izq);            // Cierra la tubería anterior
      izq = der[0];          // Ahora leerá desde la nueva tubería
      continue;              // Vuelve al inicio del ciclo
    }

    close(der[0]);           // El padre no necesita leer de la nueva tubería

    // Lee los números restantes de la tubería anterior
    while (read(izq, &n, sizeof(n)) == sizeof(n)) {

      // Solo envía números que no sean múltiplos del primo
      if (n % primo != 0)
        write(der[1], &n, sizeof(n)); // Envía el número al proceso hijo
    }

    close(izq);              // Cierra la tubería anterior
    close(der[1]);           // Cierra la escritura hacia el hijo
    wait(0);                 // Espera a que termine el hijo
    exit(0);                 // Termina el proceso actual
  }
}

int main(void) {
  int p[2], pid;             // Tubería inicial y PID del hijo

  // Crea la tubería inicial
  if (pipe(p) < 0) {
    fprintf(2, "primes: error en pipe\n"); // Muestra error
    exit(1);                               // Termina con error
  }

  pid = fork();              // Crea el primer proceso hijo

  if (pid < 0) {
    fprintf(2, "primes: error en fork\n"); // Muestra error
    exit(1);                               // Termina con error
  }

  if (pid == 0) {            // Código del proceso hijo
    close(p[1]);             // El hijo no escribe en la tubería inicial
    sieve(p[0]);             // Comienza la criba leyendo la tubería
  }

  close(p[0]);               // El padre no necesita leer

  // Envía los números desde 2 hasta 35 al primer hijo
  for (int i = 2; i <= 35; i++)
    write(p[1], &i, sizeof(i)); // Escribe cada número en la tubería

  close(p[1]);               // Indica que no se enviarán más números
  wait(0);                   // Espera a que termine el hijo
  exit(0);                   // Termina el programa
}
