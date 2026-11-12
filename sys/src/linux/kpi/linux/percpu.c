/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: per-CPU variables are per proc (linux/percpu.h):
 * each proc's copies hang off its cpu_info, found by the variable's
 * address.
 */
#include <linux/percpu.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <machine/cpu.h>

struct kpi_percpu {
	struct kpi_percpu	*next;
	void	*var;
	char	data[];
};

void *
kpi_percpu(void *var, size_t size)
{
	struct cpu_info *ci = curcpu();
	struct kpi_percpu *p;

	for (p = ci->ci_percpu; p != NULL; p = p->next)
		if (p->var == var)
			return p->data;
	p = kmalloc(sizeof(*p) + size, GFP_ATOMIC);
	if (p == NULL)
		panic("kpi: no memory for a per-cpu variable");
	p->var = var;
	memcpy(p->data, var, size);
	p->next = ci->ci_percpu;
	ci->ci_percpu = p;
	return p->data;
}
