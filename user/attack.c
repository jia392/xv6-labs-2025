#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // Your code here.
   char *p;
  for(int i = 0; i < 100; i++){
    p = sbrk(PGSIZE);

    if(p == (char *)-1)
      exit(1);

    char *s = p + 32;
    int len = 0;
    while(len < 100 &&
          ((s[len] >= 'a' && s[len] <= 'z') ||
           (s[len] >= 'A' && s[len] <= 'Z') ||
           (s[len] >= '0' && s[len] <= '9'))){
      len++;
    }
    if(len >= 5){
      write(1, s, len);
      write(1, "\n", 1);
      exit(0);
    }
  }

  exit(1);
}
