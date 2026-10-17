#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/pci.h"
#include "riscv.h"

/*
 * the platform-level interrupt controller; device interrupts go
 * to the supervisor context of cpu0's hart (2*hart+1 on qemu's virt).
 */
enum {
	Nirq		= 128,

	Priority	= 0x000000,	/* per irq */
	Pending		= 0x001000,
	Enable		= 0x002000,	/* per context, 0x80 apart */
	Threshold	= 0x200000,	/* per context, 0x1000 apart */
	Claim		= 0x200004,
};

typedef struct Vctl Vctl;
struct Vctl {
	Vctl	*next;
	void	(*f)(Ureg*, void*);
	void	*a;
	int	irq;
	char	name[KNAMELEN];
};

static Lock vctllock;
static Vctl *vctl[Nirq];
static u32int *plic;

#define REG(o)	plic[(o)/4]

static void
plicenable(int irq, int on)
{
	u32int *e;
	int ctx;

	ctx = MACHP(0)->plicctx;
	e = &REG(Enable + ctx*0x80 + (irq/32)*4);
	if(on)
		*e |= 1<<(irq%32);
	else
		*e &= ~(1<<(irq%32));
	coherence();
}

void
intrinit(void)
{
	int i;

	if(plic == nil)
		plic = vmap(PHYSPLIC, 0x400000);
	m->plicctx = 2*m->hartid + 1;
	if(m->machno == 0)
		for(i = 1; i < Nirq; i++)
			REG(Priority + i*4) = 0;
	for(i = 0; i < Nirq/32; i++)
		REG(Enable + m->plicctx*0x80 + i*4) = 0;
	REG(Threshold + m->plicctx*0x1000) = 0;
	coherence();
	setsie(getsie() | Seie);
}

void
intrsoff(void)
{
	int i;

	for(i = 0; i < Nirq/32; i++)
		REG(Enable + m->plicctx*0x80 + i*4) = 0;
	coherence();
}

void
intrcpushutdown(void)
{
	setsie(getsie() & ~Seie);
}

int
irq(Ureg *ureg)
{
	Vctl *v;
	u32int id;

	m->intr++;
	for(;;){
		id = REG(Claim + m->plicctx*0x1000);
		if(id == 0 || id >= Nirq)
			break;
		for(v = vctl[id]; v != nil; v = v->next){
			coherence();
			v->f(ureg, v->a);
			coherence();
		}
		REG(Claim + m->plicctx*0x1000) = id;
	}
	return 0;
}

void
intrenable(int irq, void (*f)(Ureg*, void*), void *a, int tbdf, char *name)
{
	Vctl *v;

	if(BUSTYPE(tbdf) == BusPCI){
		pciintrenable(tbdf, f, a);
		return;
	}
	if(tbdf != BUSUNKNOWN)
		return;
	if(irq <= 0 || irq >= Nirq){
		print("intrenable: %s: bad irq %d\n", name, irq);
		return;
	}
	if((v = xalloc(sizeof(Vctl))) == nil)
		panic("intrenable: no mem");
	v->irq = irq;
	v->f = f;
	v->a = a;
	strncpy(v->name, name, KNAMELEN-1);

	ilock(&vctllock);
	v->next = vctl[irq];
	vctl[irq] = v;
	REG(Priority + irq*4) = 1;
	plicenable(irq, 1);
	iunlock(&vctllock);
}

void
intrdisable(int irq, void (*f)(Ureg*, void*), void *a, int tbdf, char*)
{
	Vctl **l, *v;

	if(BUSTYPE(tbdf) == BusPCI){
		pciintrdisable(tbdf, f, a);
		return;
	}
	if(irq <= 0 || irq >= Nirq)
		return;
	ilock(&vctllock);
	for(l = &vctl[irq]; (v = *l) != nil; l = &v->next)
		if(v->f == f && v->a == a){
			*l = v->next;
			break;
		}
	if(vctl[irq] == nil)
		plicenable(irq, 0);
	iunlock(&vctllock);
}
