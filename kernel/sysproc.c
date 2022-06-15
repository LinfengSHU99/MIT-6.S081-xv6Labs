#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "fcntl.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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

uint64 sys_mmap() {
  struct file *f;
  uint64 addr;
  argaddr(0, &addr);
  int length;
  argint(1, &length);
  int prot;
  argint(2, &prot);
  int flags;
  argint(3, &flags);
  int fd;
  argint(4, &fd);
  int offset;
  argint(5, &offset);
  
  if (addr != 0) panic("mmap: addr != 0\n");
 
  struct proc *p = myproc();
  f = p->ofile[fd];
  if (flags == MAP_SHARED) {
    if (f->writable == 0 && (prot & PROT_WRITE) != 0) return -1;  
  }
  filedup(f);
  uint64 ret_addr = p->sz;
  ret_addr = PGROUNDUP(ret_addr);
  printf("ori sz = %p  length = %p\n", myproc()->sz, length);
  p->sz += (length + ret_addr - p->sz);
  printf("sz = %p  ret_addr = %p\n", p->sz,ret_addr);
  p->vma[p->nvma].addr = ret_addr;
  p->vma[p->nvma].fd = fd;
  p->vma[p->nvma].length = length;
  p->vma[p->nvma].prot = prot;
  printf("prot = %d\n", p->vma[p->nvma].prot);
  p->vma[p->nvma].flags = flags;
  p->vma[p->nvma].offset = offset;
  p->vma[p->nvma].f = f;
  p->nvma++;
  // p->sz += length;
  return ret_addr;
  // return 0xffffffffffffffff;
}

uint64 sys_munmap() {
  uint64 addr;
  int length;
  argaddr(0, &addr);
  argint(1, &length);
  struct proc *p = myproc();
  int i = -1;
  for (i = 0; i < p->nvma; i++) {
    if (p->vma[i].addr <= addr && addr < p->vma[i].addr + p->vma[i].length) {
      break;
    }
  }
  if (i == -1) panic("munmap: vma not found\n");
  printf("unmap addr = %p  flags == %d\n", addr, p->vma[i].flags);
  struct file *f = p->vma[i].f;
  if (p->vma[i].flags== MAP_SHARED) {
    //TODO
    int n = length;
    int r = 0;
    // int off = p->vma[i].offset;
    int off = 0;
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 1024;
    int j = 0;
    while(j < n){
      int n1 = n - j;
      if(n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      if ((r = writei(f->ip, 1, addr + j, off, n1)) > 0)
        off += r;
      iunlock(f->ip);
      end_op();

      if(r != n1){
        // error from writei
        break;
      }
      j += r;
    }
    // ret = (i == n ? n : -1);
    // p->vma[i].offset += length;
  }
  uint64 npages = length / PGSIZE;
  if (length % PGSIZE != 0) panic("munmap: remainder != 0\n");
  uvmunmap(p->pagetable, addr, npages, 1);
  if (length == p->vma[i].length) {
    fileclose(p->vma[i].f);
    p->vma[i].addr = 0;
    p->vma[i].length = 0;
    // TODO maybe need a circular queue? 
  }
  else {
    p->vma[i].addr = addr + length;
    p->vma[i].length -= length;
  }
  return 0;
}