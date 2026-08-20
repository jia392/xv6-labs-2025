#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

void find(char *path, char *target, int use_exec, char *command, int command_argc, char *command_args[])
{
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512];
    char *p;

    fd = open(path, 0);
    if(fd < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    //普通文件
    if(st.type == T_FILE){
        char *name = path + strlen(path);

        while(name >= path && *name != '/')
            name--;
        name++;

        //判断文件名是否匹配
        if(strcmp(name, target) == 0){
            if(!use_exec){
                printf("%s\n", path);
            } else {
                int pid = fork();
                if(pid < 0){
                    fprintf(2, "find: fork failed\n");
                    close(fd);
                    return;
                }
                if(pid == 0){
                    char *argv[MAXARG];
                    int i;

                    if(command_argc + 3 > MAXARG){
                        fprintf(2, "find: too many arguments for exec\n");
                        exit(1);
                    }

                    argv[0] = command;
                    for(i = 0; i < command_argc; i++){
                        argv[i + 1] = command_args[i];
                    }
                    argv[command_argc + 1] = path; 
                    argv[command_argc + 2] = 0;    

                    exec(command, argv);
                    fprintf(2, "find: exec %s failed\n", command);
                    exit(1);
                }

                wait(0); //父进程等待子进程
            }
        }

        close(fd);
        return;
    }

    if(st.type != T_DIR){
        close(fd);
        return;
    }

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;

        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        int i;
        for(i = 0; i < DIRSIZ && de.name[i] != '\0'; i++){
            p[i] = de.name[i];
        }
        p[i] = '\0';

        //递归查找
        find(buf, target, use_exec, command, command_argc, command_args);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc == 3){
        find(argv[1], argv[2], 0, 0, 0, 0);
        exit(0);
    }
    //-exec用法
    if(argc >= 5 && strcmp(argv[3], "-exec") == 0){
        char *command = argv[4];
        int command_argc = argc - 5;
        char **command_args = &argv[5];
        find(argv[1], argv[2], 1, command, command_argc, command_args);
        exit(0);
    }

    fprintf(2, "usage: find path name [-exec command [args...]]\n");
    exit(1);
}