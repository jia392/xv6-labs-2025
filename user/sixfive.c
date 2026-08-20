#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "usage: sixfive file...\n");
        exit(1);
    }

    char c;
    char *separators = " -\r\t\n./,";
    
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }

        int num = 0;
        int in_number = 0;

        while (read(fd, &c, 1) == 1) {
            if (c >= '0' && c <= '9') {
                num = num * 10 + (c - '0');
                in_number = 1;
            } 
            else if (strchr(separators, c) != 0) {
                if (in_number) {
                    if (num % 5 == 0 || num % 6 == 0) {
                        printf("%d\n", num);
                    }
                    num = 0;
                    in_number = 0;
                }
            }
        }

        //文件结束
        if (in_number) {
            if (num % 5 == 0 || num % 6 == 0) {
                printf("%d\n", num);
            }
        }

        close(fd);
    }

    exit(0);
}