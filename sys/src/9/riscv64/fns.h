#include "../port/portfns.h"

/* l.s */
extern int tas(void *);
extern int cmpswap(long*, long, long);
extern void coherence(void);
extern void idlehands(void);
extern uvlong vcycles(void);
#define cycles(ip) *(ip) = vcycles()
extern void touser(uintptr sp);
extern void forkret(void);
extern void noteret(void);
extern void fpon(void);
extern void fpoff(void);
extern void fpsaveregs(void*);
extern void fploadregs(void*);
extern void setsatp(uvlong);
extern void flushtlb(void);
extern void flushtlbva(uintptr);
extern void flushtlbasid(uintptr);
extern void fencei(void);
extern uvlong getsie(void);
extern void setsie(uvlong);
extern uvlong getsip(void);
extern void setsip(uvlong);
extern void setstvec(uintptr);
extern void setscounteren(uvlong);
extern vlong sbicall(uvlong, uvlong, uvlong, uvlong, uvlong, uvlong*);
extern void strap(void);
extern int peek(void*, void*, int);
extern void _peekend(void);

/* sbi */
extern void sbisettimer(uvlong);
extern void sbisendipi(int);
extern int sbihartstart(int, uintptr, uintptr);
extern void sbireset(int);
extern void sbihartstop(void);

/* mmu */
#define	getpgcolor(a)	0
extern uintptr paddr(void*);
#define PADDR(a) paddr((void*)(a))
extern uintptr cankaddr(uintptr);
extern void* kaddr(uintptr);
#define KADDR(a) kaddr(a)
extern void kmapinval(void);
#define	VA(k)	((uintptr)(k))
extern KMap *kmap(Page*);
extern void kunmap(KMap*);
extern void kmapram(uintptr, uintptr);
extern void* vmap(uvlong, vlong);
extern void vunmap(void*, vlong);
extern void mmu1init(void);
extern void putasid(Proc*);
extern void meminit(void);

/* clock */
extern void clockinit(void);
extern void synccycles(void);
extern void clockshutdown(void);
extern void clockintr(Ureg*);

/* fpu */
extern void fpuinit(void);
extern void fpuprocsetup(Proc*);
extern void fpuprocfork(Proc*);
extern void fpuprocsave(Proc*);
extern void fpuprocrestore(Proc*);
extern void fpukenter(Ureg*);
extern void fpukexit(Ureg*);
extern void mathtrap(Ureg*);

/* trap */
extern void trapinit(void);
extern int userureg(Ureg*);
extern void evenaddr(uintptr);
extern void setkernur(Ureg*, Proc*);
extern void procfork(Proc*);
extern void procsetup(Proc*);
extern void procsave(Proc*);
extern void procrestore(Proc *);
extern void trap(Ureg*);
extern void syscall(Ureg*);
extern void dumpstack(void);
extern void dumpregs(Ureg*);

/* plic */
extern void intrinit(void);
extern void intrcpushutdown(void);
extern void intrsoff(void);
extern void intrenable(int, void (*)(Ureg*, void*), void*, int, char*);
extern void intrdisable(int, void (*)(Ureg*, void*), void*, int, char*);
extern int irq(Ureg*);

/* uart */
extern void uartconsinit(void);

/* dma */
extern void dmaflush(int, void*, ulong);

/* main */
extern char *getconf(char *name);
extern void setconfenv(void);
extern void writeconf(void);

extern int isaconfig(char*, int, ISAConf*);
extern void links(void);

/* pcivirt */
extern int pcicfgrw8(int tbdf, int rno, int data, int read);
extern int pcicfgrw16(int tbdf, int rno, int data, int read);
extern int pcicfgrw32(int tbdf, int rno, int data, int read);
extern void pciintrenable(int tbdf, void (*f)(Ureg*, void*), void *a);
extern void pciintrdisable(int tbdf, void (*f)(Ureg*, void*), void *a);

/* bootargs */
extern void bootargsinit(void);
extern uintptr dtbpa;
