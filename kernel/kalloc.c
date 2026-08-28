// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;
  int refcnt[PHYSTOP / PGSIZE];
} ref;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&ref.lock, "ref");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    ref.refcnt[(uint64)p / PGSIZE] = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  uint64 p = (uint64)pa;

  if((p % PGSIZE) != 0 || (char*)pa < end ||
     p >= PHYSTOP)
    panic("kfree");

  acquire(&ref.lock);

  if(ref.refcnt[p / PGSIZE] <= 0)
    panic("kfree refcnt");

  ref.refcnt[p / PGSIZE]--;

  if(ref.refcnt[p / PGSIZE] > 0){
    release(&ref.lock);
    return;
  }

  release(&ref.lock);

  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

void
kref_inc(uint64 pa)
{
  if(pa % PGSIZE != 0 || pa >= PHYSTOP)
    panic("kref_inc");

  acquire(&ref.lock);
  ref.refcnt[pa / PGSIZE]++;
  release(&ref.lock);
}

int
kref_get(uint64 pa)
{
  int count;

  if(pa % PGSIZE != 0 || pa >= PHYSTOP)
    panic("kref_get");

  acquire(&ref.lock);
  count = ref.refcnt[pa / PGSIZE];
  release(&ref.lock);

  return count;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);

  r = kmem.freelist;

  if(r)
    kmem.freelist = r->next;

  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE);

    acquire(&ref.lock);
    ref.refcnt[(uint64)r / PGSIZE] = 1;
    release(&ref.lock);
  }

  return (void*)r;
}
