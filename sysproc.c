#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

// Pone a 0 el lugar de memoria al que apunta pid
  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_date(void)
{
  struct rtcdate *r;
  // (void **) casting a tipo void**
  if(argptr(0, (void **)&r, sizeof(struct rtcdate)) < 0)
    return -1;

  cmostime(r); // No puede fallar, rellena r con el tiempo del cmos

  return 0;
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

// Recibe un entero
int
sys_sbrk(void)
{
  int addr;
  int n;
// Cogemos un entero de la pila
  if(argint(0, &n) < 0)
    return -1;
// Guarda el valor de sz
  addr = myproc()->sz += n;
  if (n < 0 && growproc(n) < 0)
    return -1;
// Ahora sale un error, hay que solucionarlo en trap.c
// Aumente o disminuye el tamaño del proceso
  // if(growproc(n) < 0)
  //   return -1;
  return addr;
}

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
