#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  // pte_t *pte = (pte_t*)0x0000000087f41018;
  uint64 addr;
  argaddr(0, &addr);
  int page_num;
  argint(1, &page_num);
  uint64 user_addr;
  argaddr(2, &user_addr);
  if (page_num > 64) {
    return -1;
  }
  uint64 ret_mask = 0;
  struct proc *p = myproc();
  
  // printf("%p\n", *pte);
  // if (*(pte = walk(p->pagetable, addr, 0)) & PTE_A){
    // ret_mask |= 1;
  // }
  pte_t *pteptr = walk(p->pagetable, addr, 0);
  // printf("%p\n", pte);
  // printf("%p\n", *pte);
  pagetable_t start = pteptr;
  for (int i = 0; i < page_num; i++) {
    // printf("%p\n", start[i]);
    if ((start[i] & PTE_A) && (start[i] & PTE_V)) {
      ret_mask |= (1L << i);
      start[i] -= PTE_A;
    }
    // else if ((start[i] & PTE_A) == 0) {
      // start[i] |= PTE_A;
    // }
  }
  copyout(p->pagetable, user_addr, (char*)&ret_mask, sizeof(uint64));
  return 0;
}
#endif

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
