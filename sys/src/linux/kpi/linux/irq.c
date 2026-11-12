// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: interrupts, tasklets, irq_work.
 *
 * The device's interrupt (#G/n/irq) is waited for by an irq proc,
 * which calls the handlers requested for it (hard irq context), then
 * wakes the threaded handlers' procs and acknowledges.  Tasklets and
 * irq_work run on one softirq proc, as Linux runs them after the
 * interrupt.  Further irq numbers (irq_alloc_desc) are run through
 * generic_handle_irq by a driver's own handler.
 */
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irq_work.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include "kpi.h"

#define NIRQ	64

struct irqaction {
	struct irqaction	*next;
	irq_handler_t	handler;
	irq_handler_t	thread_fn;
	void	*dev;
	const char	*name;
	int	wake;		/* thread_fn to run */
	u64	q[4], r[4];	/* thread's QLock, Rendez */
};

struct irq_desc {
	struct irqaction	*action;
	const struct irq_chip	*chip;
	struct irq_data	data;
	irq_flow_handler_t	flow;
	int	used;
	int	depth;		/* disable_irq count */
	int	pending;	/* came while disabled */
	int	inhandler;
};

static struct irq_desc descs[NIRQ];
static irqreturn_t kpi_irq_default_primary(int irq, void *dev);
static u64 iq[4], ir[4];		/* QLock, Rendez: irq state changes */
static int irqproc_started;

static void
irq_thread(void *arg)
{
	struct irqaction *a = arg;

	p9_qlock(a->q);
	for (;;) {
		while (!a->wake)
			p9_rsleep(a->r);
		a->wake = 0;
		p9_qunlock(a->q);
		a->thread_fn(0, a->dev);
		p9_qlock(a->q);
	}
}

/* the irqs the device delivered, and those a handler took (kpi_irq_counts) */
static unsigned long ncalls[NIRQ], nhandled[NIRQ];

void
kpi_irq_counts(unsigned int irq, unsigned long *calls, unsigned long *handled)
{
	*calls = irq < NIRQ ? ncalls[irq] : 0;
	*handled = irq < NIRQ ? nhandled[irq] : 0;
}

/* run the handlers of irq; with iq held, which it drops while they run */
static irqreturn_t
run_handlers(unsigned int irq)
{
	struct irq_desc *d = &descs[irq];
	struct irqaction *a;
	irqreturn_t ret = IRQ_NONE, r;

	ncalls[irq]++;
	if (d->depth > 0) {
		d->pending = 1;
		return IRQ_NONE;
	}
	d->inhandler++;
	p9_qunlock(iq);
	for (a = d->action; a; a = a->next) {
		r = a->handler(irq, a->dev);
		if (r == IRQ_WAKE_THREAD && a->thread_fn) {
			p9_qlock(a->q);
			a->wake = 1;
			p9_rwakeup(a->r);
			p9_qunlock(a->q);
			r = IRQ_HANDLED;
		}
		ret |= r;
	}
	p9_qlock(iq);
	d->inhandler--;
	if (ret != IRQ_NONE)
		nhandled[irq]++;
	p9_rwakeupall(ir);
	return ret;
}

static void
irqproc(void *arg)
{
	unsigned int irq = (uintptr_t)arg;

	strlcpy(curproc->comm, "irq", sizeof(curproc->comm));
	for (;;) {
		if (p9_irqwait() < 0) {
			pr_err("kpi: the irq proc ends: no more interrupts\n");
			break;
		}
		p9_qlock(iq);
		run_handlers(irq);
		p9_qunlock(iq);
		p9_irqack();
	}
	p9_exits(NULL);
}

int
request_threaded_irq(unsigned int irq, irq_handler_t h, irq_handler_t t,
	unsigned long flags, const char *name, void *dev)
{
	struct irqaction *a, **l;

	if (irq >= NIRQ)
		return -EINVAL;
	if (!h && !t)
		return -EINVAL;
	a = kzalloc(sizeof(*a), GFP_KERNEL);
	if (!a)
		return -ENOMEM;
	a->handler = h;
	a->thread_fn = t;
	a->dev = dev;
	a->name = name;
	p9_rendez_init(a->r, a->q);
	if (t && p9_proccreate(irq_thread, a, 64 * 1024) < 0) {
		kfree(a);
		return -ENOMEM;
	}
	if (!a->handler)
		a->handler = kpi_irq_default_primary;
	p9_qlock(iq);
	p9_rendez_init(ir, iq);
	for (l = &descs[irq].action; *l; l = &(*l)->next)
		;
	*l = a;
	descs[irq].used = 1;
	/* the device's own interrupt: the first irq requested below 32 */
	if (irq < 32 && !irqproc_started) {
		irqproc_started = 1;
		/* enabled now, not when the proc first runs: none is lost before */
		if (p9_irqopen() < 0)
			pr_err("kpi: request_irq %u: no interrupt: %s\n", irq, "#G irq file");
		p9_proccreate(irqproc, (void *)(uintptr_t)irq, 64 * 1024);
	}
	p9_qunlock(iq);
	return 0;
}

static irqreturn_t
kpi_irq_default_primary(int irq, void *dev)
{
	return IRQ_WAKE_THREAD;
}

int
request_irq(unsigned int irq, irq_handler_t h, unsigned long flags, const char *name, void *dev)
{
	return request_threaded_irq(irq, h, NULL, flags, name, dev);
}

void
free_irq(unsigned int irq, void *dev)
{
	struct irqaction **l, *a;

	if (irq >= NIRQ)
		return;
	p9_qlock(iq);
	for (l = &descs[irq].action; (a = *l) != NULL; l = &a->next)
		if (a->dev == dev) {
			*l = a->next;
			break;
		}
	while (descs[irq].inhandler)
		p9_rsleep(ir);
	p9_qunlock(iq);
	/* a thread proc may still use a: it stays */
}

void
disable_irq_nosync(unsigned int irq)
{
	if (irq >= NIRQ)
		return;
	p9_qlock(iq);
	descs[irq].depth++;
	p9_qunlock(iq);
}

void
synchronize_irq(unsigned int irq)
{
	if (irq >= NIRQ)
		return;
	p9_qlock(iq);
	while (descs[irq].inhandler)
		p9_rsleep(ir);
	p9_qunlock(iq);
}

void
disable_irq(unsigned int irq)
{
	disable_irq_nosync(irq);
	synchronize_irq(irq);
}

void
enable_irq(unsigned int irq)
{
	struct irq_desc *d;

	if (irq >= NIRQ)
		return;
	d = &descs[irq];
	p9_qlock(iq);
	if (d->depth > 0 && --d->depth == 0 && d->pending) {
		d->pending = 0;
		run_handlers(irq);
	}
	p9_qunlock(iq);
}

/* generic irqs: numbers a driver hands out and runs itself */
int
irq_alloc_desc(int node)
{
	int i;

	p9_qlock(iq);
	for (i = 32; i < NIRQ; i++)
		if (!descs[i].used) {
			memset(&descs[i], 0, sizeof(descs[i]));
			descs[i].used = 1;
			descs[i].data.irq = i;
			p9_qunlock(iq);
			return i;
		}
	p9_qunlock(iq);
	return -ENOSPC;
}

void
irq_free_desc(unsigned int irq)
{
	if (irq >= 32 && irq < NIRQ) {
		p9_qlock(iq);
		descs[irq].used = 0;
		p9_qunlock(iq);
	}
}

void
irq_set_chip_and_handler_name(unsigned int irq, const struct irq_chip *chip, irq_flow_handler_t h, const char *name)
{
	if (irq < NIRQ) {
		descs[irq].chip = chip;
		descs[irq].flow = h;
	}
}

int
irq_set_chip_data(unsigned int irq, void *data)
{
	if (irq >= NIRQ)
		return -EINVAL;
	descs[irq].data.chip_data = data;
	return 0;
}

void
handle_simple_irq(struct irq_desc *d)
{
	unsigned int irq = d - descs;

	p9_qlock(iq);
	run_handlers(irq);
	p9_qunlock(iq);
}

int
generic_handle_irq(unsigned int irq)
{
	struct irq_desc *d;

	if (irq >= NIRQ || !descs[irq].used)
		return -EINVAL;
	d = &descs[irq];
	if (d->flow)
		d->flow(d);
	else
		handle_simple_irq(d);
	return 0;
}

/* linear irq domains: a generic irq per hardware number, made when first mapped */
struct irq_domain *
irq_domain_add_linear(struct device_node *of, unsigned int size, const struct irq_domain_ops *ops, void *host_data)
{
	struct irq_domain *d;

	d = kzalloc(sizeof(*d), GFP_KERNEL);
	if (d == NULL)
		return NULL;
	d->irqs = kcalloc(size, sizeof(d->irqs[0]), GFP_KERNEL);
	if (d->irqs == NULL) {
		kfree(d);
		return NULL;
	}
	d->ops = ops;
	d->host_data = host_data;
	d->size = size;
	return d;
}

void
irq_domain_remove(struct irq_domain *d)
{
	unsigned int hw;

	for (hw = 0; hw < d->size; hw++)
		if (d->irqs[hw] != 0) {
			if (d->ops->unmap)
				d->ops->unmap(d, d->irqs[hw]);
			irq_free_desc(d->irqs[hw]);
		}
	kfree(d->irqs);
	kfree(d);
}

/* the irq of hardware number hw, 0 if there is none (as Linux) */
unsigned int
irq_create_mapping(struct irq_domain *d, irq_hw_number_t hw)
{
	int irq;

	if (hw >= d->size)
		return 0;
	if (d->irqs[hw] != 0)
		return d->irqs[hw];
	irq = irq_alloc_desc(0);
	if (irq < 0)
		return 0;
	if (d->ops->map && d->ops->map(d, irq, hw) != 0) {
		irq_free_desc(irq);
		return 0;
	}
	d->irqs[hw] = irq;
	return irq;
}

unsigned int
irq_find_mapping(struct irq_domain *d, irq_hw_number_t hw)
{
	return hw < d->size ? d->irqs[hw] : 0;
}

int
generic_handle_domain_irq(struct irq_domain *d, irq_hw_number_t hw)
{
	unsigned int irq = irq_find_mapping(d, hw);

	return irq != 0 ? generic_handle_irq(irq) : -EINVAL;
}

/* tasklets and irq_work: OpenBSD's (linux/interrupt.h, linux/irq_work.h: task queues) */
