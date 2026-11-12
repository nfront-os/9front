// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: RCU, simply.  Readers count themselves in one
 * of two epochs (the outermost rcu_read_lock of a proc); a grace
 * period flips the epoch and waits until the old one has no readers,
 * twice, as SRCU does.  Readers never block.  call_rcu's callbacks
 * run on the rcu proc after a grace period.
 */
#include <linux/rcupdate.h>
#include <linux/srcu.h>
#include <linux/slab.h>
#include "kpi.h"

static atomic_t readers[2];
static int epoch;
static u64 gq[4];			/* QLock: one grace period at a time */
static u64 cq[4], cr[4], cdone[4];	/* callbacks: QLock, Rendez work, Rendez done */
static struct rcu_head *cbhead, **cbtail = &cbhead;
static unsigned long cbqueued, cbdone;
static int cstarted;

void
kpi_rcu_read_lock(void)
{
	struct task_struct *k = curproc;
	int e;

	if (k->rcu_nest++ == 0) {
		for (;;) {
			e = READ_ONCE(epoch);
			atomic_inc(&readers[e]);
			smp_mb();
			if (READ_ONCE(epoch) == e)
				break;
			/* flipped meanwhile: count in the new one */
			atomic_dec(&readers[e]);
		}
		k->rcu_epoch = e;
	}
	barrier();
}

void
kpi_rcu_read_unlock(void)
{
	struct task_struct *k = curproc;

	barrier();
	if (--k->rcu_nest == 0) {
		smp_mb();
		atomic_dec(&readers[k->rcu_epoch]);
	}
}

static void
flip_and_wait(void)
{
	int old = epoch;

	WRITE_ONCE(epoch, !old);
	smp_mb();
	while (atomic_read(&readers[old]) != 0)
		p9_sleep(1);
	smp_mb();
}

void
synchronize_rcu(void)
{
	if (curproc->rcu_nest)
		p9_panic("synchronize_rcu inside rcu_read_lock");
	p9_qlock(gq);
	flip_and_wait();
	flip_and_wait();
	p9_qunlock(gq);
}

static void
rcuproc(void *arg)
{
	struct rcu_head *list, *h;
	unsigned long n;

	strlcpy(curproc->comm, "rcu", sizeof(curproc->comm));
	p9_qlock(cq);
	for (;;) {
		while (!cbhead)
			p9_rsleep(cr);
		list = cbhead;
		cbhead = NULL;
		cbtail = &cbhead;
		n = cbqueued;
		p9_qunlock(cq);
		synchronize_rcu();
		while ((h = list) != NULL) {
			list = h->next;
			if ((uintptr_t)h->func < 4096)	/* kfree_rcu: the head's offset */
				kvfree((char *)h - (uintptr_t)h->func);
			else
				h->func(h);
		}
		p9_qlock(cq);
		cbdone = n;
		p9_rwakeupall(cdone);
	}
}

void
call_rcu(struct rcu_head *head, rcu_callback_t func)
{
	head->func = func;
	head->next = NULL;
	p9_qlock(cq);
	if (!cstarted) {
		cstarted = 1;
		p9_rendez_init(cr, cq);
		p9_rendez_init(cdone, cq);
		p9_proccreate(rcuproc, NULL, 64 * 1024);
	}
	*cbtail = head;
	cbtail = &head->next;
	cbqueued++;
	p9_rwakeup(cr);
	p9_qunlock(cq);
}

void
rcu_barrier(void)
{
	unsigned long n;

	p9_qlock(cq);
	n = cbqueued;
	while ((long)(cbdone - n) < 0)
		p9_rsleep(cdone);
	p9_qunlock(cq);
}

/* kfree_rcu and friends; without a head (mightsleep), wait here */
void
kpi_kvfree_call_rcu(struct rcu_head *head, void *ptr)
{
	if (!ptr)
		return;
	if (!head) {
		synchronize_rcu();
		kvfree(ptr);
		return;
	}
	call_rcu(head, (rcu_callback_t)((char *)head - (char *)ptr));
}
