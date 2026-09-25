#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char *path, char *name) {
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512];
    char *p;
    // Abrimos el path actual y cerramos ejecución en caso de fallo
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    // Obtenemos la metainformación (tipo de archivo, tamaño, etc..) y cerramos ejecución en caso de fallo
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Comprobamos en caso que no sea directorio el path actual para cerrar la ejecución
    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }

    // Armamos el prefijo del path en buf
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // Recorremos el directorio
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        // Se omite porque es ranura libre (si es que inum es igual a 0)
        if (de.inum == 0)
            continue;
        // Se omite si es que estamos trabajando con el mismo archivo del inicio o con el padre del mismo
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // Pegamos el nombre de ESTA entrada después del prefijo
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        // Obtenemos metainformación y cerramos en caso de falla
        if (stat(buf, &st) < 0) {
            printf("stat failed for %s\n", buf);
            continue;
        }
        // Si es directorio el archivo actual, volvemos a ejecutar find en el nuevo path
        if (st.type == T_DIR) {
            find(buf, name);   // buf YA tiene el path completo del subdirectorio
        // En caso que sea un archivo y el nombre coincida, se imprime en pantalla el path actual
        } else if (st.type == T_FILE && strcmp(p, name) == 0) {
            printf("%s\n", buf);
        }
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
// Si solo se ingresa un argumento, se asume que parte del path .
  if (argc == 2) {
    find(".",argv[1]);
    exit(0);
  }
  // Si no tiene ningun argumento, se solicitan mas y no ejecuta nada
  else if (argc < 2){
    printf("find: more arguments needed\n");
    exit(0);
  }
  // Si tiene exactamente 2 argumentos, se ejecuta el find con path y name
  else if (argc == 3){
    find(argv[1], argv[2]);
    exit(0);
  // Si tienen mas argumentos, se finaliza la ejecución dejando un aviso
  }else{
    printf("find: there is more arguments\n");
    exit(0);
  }
  
  
}