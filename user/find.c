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

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }

    // armamos el prefijo del path en buf UNA vez, antes del loop
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // pegamos el nombre de ESTA entrada después del prefijo
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(buf, &st) < 0) {
            printf("stat failed for %s\n", buf);
            continue;
        }

        if (st.type == T_DIR) {
            find(buf, name);   // buf YA tiene el path completo del subdirectorio
        } else if (st.type == T_FILE && strcmp(p, name) == 0) {
            printf("%s\n", buf);
        }
    }

    close(fd);
}

int
main(int argc, char *argv[])
{

  if (argc == 2) {
    find(".",argv[1]);
    exit(0);
  }
  else if (argc < 2){
    printf("find: more arguments needed\n");
    exit(0);
  }
  else if (argc == 3){
    find(argv[1], argv[2]);
    exit(0);
  }else{
    printf("find: there is more arguments\n");
    exit(0);
  }
  
  
}