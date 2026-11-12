// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: module parameters by name (linux/module.h), as
 * Linux's command line sets them ("i915.enable_guc=0"): the file server
 * passes name=value.  A name not known as it is may be the end of one
 * after "__" (i915's selftest switches: igt__19__live_gt_lrc as
 * live_gt_lrc).
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

struct param {
	struct param	*next;
	const char	*name;
	void	*var;
	int	type;
};

static struct param *params;

void
kpi_param_add(const char *name, void *var, int type)
{
	struct param *p = kzalloc(sizeof(*p), GFP_KERNEL);

	if (!p)
		return;
	p->name = name;
	p->var = var;
	p->type = type;
	p->next = params;
	params = p;
}

/* 9front's libc calls no constructors: the parameters' are run here, once */
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);

void
kpi_constructors(void)
{
	static bool done;
	void (**f)(void);

	if (done)
		return;
	done = true;
	for (f = __init_array_start; f < __init_array_end; f++)
		(*f)();
}

static struct param *
find(const char *name)
{
	struct param *p, *found = NULL;
	size_t n = strlen(name), l;
	int nfound = 0;

	for (p = params; p; p = p->next)
		if (strcmp(p->name, name) == 0)
			return p;
	for (p = params; p; p = p->next) {
		l = strlen(p->name);
		if (l > n + 2 && strcmp(p->name + l - n, name) == 0 && p->name[l - n - 1] == '_' && p->name[l - n - 2] == '_') {
			found = p;
			nfound++;
		}
	}
	return nfound == 1 ? found : NULL;
}

/* "name=value"; 0, or -ENOENT (no such parameter), -EINVAL (bad value) */
int
kpi_param_set(const char *s)
{
	char name[96], *e;
	const char *val;
	struct param *p;
	long long v = 0;
	bool b = false;
	size_t n;

	val = strchr(s, '=');
	n = val ? (size_t)(val - s) : strlen(s);
	if (n >= sizeof(name))
		return -EINVAL;
	memcpy(name, s, n);
	name[n] = 0;
	val = val ? val + 1 : NULL;
	p = find(name);
	if (!p) {
		pr_err("parameter %s: no such parameter\n", name);
		return -ENOENT;
	}
	/* as Linux: only a bool may go without a value ("name" sets it) */
	if (!val) {
		if (p->type != KPI_PARAM_bool && p->type != KPI_PARAM_invbool && p->type != KPI_PARAM_bint)
			return -EINVAL;
		val = "1";
	}
	if (p->type == KPI_PARAM_charp) {
		*(char **)p->var = kstrdup(val, GFP_KERNEL);
		return 0;
	}
	if (p->type == KPI_PARAM_bool || p->type == KPI_PARAM_invbool) {
		if (kstrtobool(val, &b))
			return -EINVAL;
		*(bool *)p->var = p->type == KPI_PARAM_bool ? b : !b;
		return 0;
	}
	if (p->type == KPI_PARAM_bint) {
		if (kstrtobool(val, &b))
			return -EINVAL;
		*(int *)p->var = b;
		return 0;
	}
	if (p->type == KPI_PARAM_ullong) {
		unsigned long long u = simple_strtoull(val, &e, 0);

		if (e == val || *e)
			return -EINVAL;
		*(unsigned long long *)p->var = u;
		return 0;
	}
	v = simple_strtoll(val, &e, 0);
	if (e == val || *e)
		return -EINVAL;
	switch (p->type) {
	case KPI_PARAM_int:	*(int *)p->var = v; break;
	case KPI_PARAM_uint:
	case KPI_PARAM_hexint:	*(unsigned int *)p->var = v; break;
	case KPI_PARAM_long:	*(long *)p->var = v; break;
	case KPI_PARAM_ulong:	*(unsigned long *)p->var = v; break;
	case KPI_PARAM_short:	*(short *)p->var = v; break;
	case KPI_PARAM_ushort:	*(unsigned short *)p->var = v; break;
	case KPI_PARAM_byte:	*(unsigned char *)p->var = v; break;
	}
	return 0;
}
