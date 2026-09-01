#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_mmap(void)
{
  uint64 addr;
  uint64 len;
  int prot;
  int flags;
  int fd;
  uint64 offset;

  struct proc *p = myproc();
  struct file *f;
  struct vma *vma = 0;

  //获取参数
  argaddr(0, &addr);
  argaddr(1, &len);
  argint(2, &prot);
  argint(3, &flags);
  argint(4, &fd);
  argaddr(5, &offset);

  //参数检查
  if(addr != 0)
    return -1;

  if(len == 0)
    return -1;

  if(offset != 0)
    return -1;

  if(flags != MAP_SHARED && flags != MAP_PRIVATE)
    return -1;

  if(prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC))
    return -1;

  if(fd < 0 || fd >= NOFILE)
    return -1;

  f = p->ofile[fd];

  if(f == 0)
    return -1;

  if((prot & PROT_READ) && !f->readable)
    return -1;

  if((prot & PROT_WRITE) &&
     flags == MAP_SHARED &&
     !f->writable)
    return -1;

  //找一个空闲VMA
  for(int i = 0; i < NVMA; i++){
    if(!p->vmas[i].used){
      vma = &p->vmas[i];
      break;
    }
  }

  if(vma == 0)
    return -1;

  //按页向上取整
  len = PGROUNDUP(len);

  uint64 base = 0xC0000000UL;

  for(int i = 0; i < NVMA; i++){
    if(p->vmas[i].used){
      uint64 end = p->vmas[i].addr + p->vmas[i].len;

      if(end > base)
        base = PGROUNDUP(end);
    }
  }

  if(base + len >= MAXVA)
    return -1;

  //填写VMA
  vma->addr = base;
  vma->len = len;
  vma->prot = prot;
  vma->flags = flags;
  vma->file = filedup(f);
  vma->offset = offset;
  vma->used = 1;

  return base;
}

int
mmap_writeback(struct proc *p, struct vma *vma,
               uint64 start, uint64 len)
{
  if(vma->flags != MAP_SHARED)
    return 0;

  struct file *f = vma->file;
  struct inode *ip = f->ip;

  for(uint64 va = start; va < start + len; va += PGSIZE){
    pte_t *pte = walk(p->pagetable, va, 0);

    if(pte == 0 || (*pte & PTE_V) == 0)
      continue;

    uint64 pa = PTE2PA(*pte);

    uint64 file_offset =
        vma->offset + (va - vma->addr);

    begin_op();
    ilock(ip);

    uint n = PGSIZE;

    if(file_offset >= ip->size){
      iunlock(ip);
      end_op();
      continue;
    }

    if(file_offset + n > ip->size)
      n = ip->size - file_offset;

    int ret = writei(ip, 0, pa, file_offset, n);

    iunlock(ip);
    end_op();

    if(ret < 0)
      return -1;
  }

  return 0;
}

uint64
sys_munmap(void)
{
  uint64 addr;
  uint64 len;

  struct proc *p = myproc();
  struct vma *vma = 0;

  argaddr(0, &addr);
  argaddr(1, &len);

  if(len == 0)
    return -1;

  if(addr % PGSIZE != 0)
    return -1;

  len = PGROUNDUP(len);

  for(int i = 0; i < NVMA; i++){
    if(p->vmas[i].used &&
       addr >= p->vmas[i].addr &&
       addr < p->vmas[i].addr + p->vmas[i].len){
      vma = &p->vmas[i];
      break;
    }
  }

  if(vma == 0)
    return -1;

  uint64 vma_end = vma->addr + vma->len;

  if(addr + len > vma_end)
    return -1;

  if(addr != vma->addr &&
     addr + len != vma_end)
    return -1;

  if(mmap_writeback(p, vma, addr, len) < 0)
    return -1;

  uvmunmap(p->pagetable, addr, len / PGSIZE, 1);

  if(addr == vma->addr && len == vma->len){
    fileclose(vma->file);
    memset(vma, 0, sizeof(*vma));
    return 0;
  }

  if(addr == vma->addr){
    vma->addr += len;
    vma->offset += len;
    vma->len -= len;
    return 0;
  }

  if(addr + len == vma_end){
    vma->len -= len;
    return 0;
  }

  return -1;
}