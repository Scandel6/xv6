#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();

  begin_op();

  if((ip = namei(path)) == 0){
    end_op();
    cprintf("exec: fail\n");
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  //Lee la ip , dirección del elf, desde 0 hasta el final 
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

// Crea una nueva tabla de páginas
  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  // Lee cada una de las secciones de readelf
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD || ph.memsz == 0)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    // En la memoria física allocuvm pide marcos libres 
    // segun el tamaño del proceso
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
      // En esa misma tabla de páginas coge el inicio del programa
      // Y carga el fichero en los marcos libres reservados previamente
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary. 
  // Make the first inaccessible.  Use the second as the user stack.

  // Reserva memoria desde la posición actual y guarda 2 más
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  // De la primera página de estas ultimas dos paginas y cambia el bit u = 0
  // Para crear páginas de guarda para que no haya stack overflow
  /*
  | pila del programa |
  | pagina de guarda u = 0  |
  | ...
  | páginas reservadas y almacenadas por allocuvm y loaduvm |
  
  */

  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz; // sp (puntero de pila) guarda el final del tamaño

  // Push argument strings, prepare rest of stack in ustack.
  // Se guardan los parámetros de argv y argc en la pila
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

// Como del main no se vuelve
  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc); // Cambia la tabla de páginas del proceso padre por la del proceso hijo
  freevm(oldpgdir, 1); // Libera la página antigua y todos sus marcos físicos
  return 0; // Salida del kernel

 bad:
  if(pgdir)
    freevm(pgdir, 1);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
