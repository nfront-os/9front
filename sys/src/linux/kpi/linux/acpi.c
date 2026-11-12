/* SPDX-License-Identifier: MIT AND ISC */
/*
 * LinuxKPI for 9front: ACPI.  The tables: the kernel gives them all,
 * one after the other, in #P/acpitbls (amdgpu finds the card's video
 * BIOS in VFCT).  The namespace: 9front's AML interpreter on the Plan 9
 * side (p9acpi.c); here Linux's calls on it.  acpi_get_handle,
 * acpi_get_name, acpi_evaluate_object and acpi_format_exception after
 * OpenBSD's drm_linux.c (notice below), for every type of value and
 * with ACPICA's buffers (one allocation, freed with one kfree).
 *
 * Copyright (c) 2013 Jonathan Gray <jsg@openbsd.org>
 * Copyright (c) 2015, 2016 Mark Kettenis <kettenis@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <linux/acpi.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <kpi/p9.h>

static DEFINE_MUTEX(tbllock);	/* reading them sleeps */
static u8 *tbls;
static long ntbls = -1;

acpi_status
acpi_get_table(const char *sig, u32 instance, struct acpi_table_header **t)
{
	struct acpi_table_header *h;
	void *data;
	long n, o;
	u32 i;

	mutex_lock(&tbllock);
	if (ntbls < 0) {
		if (p9_readfile("#P/acpitbls", &data, &n) < 0) {
			data = NULL;
			n = 0;
		}
		tbls = data;
		ntbls = n;
	}
	mutex_unlock(&tbllock);
	i = 0;
	for (o = 0; o + (long)sizeof(*h) <= ntbls; o += h->length) {
		h = (struct acpi_table_header *)(tbls + o);
		if (h->length < sizeof(*h) || o + h->length > ntbls)
			break;
		if (memcmp(h->signature, sig, 4) == 0 && ++i == instance) {
			*t = h;
			return AE_OK;
		}
	}
	return AE_NOT_FOUND;
}

/* the tables stay read */
void
acpi_put_table(struct acpi_table_header *t)
{
}

/*
 * the namespace
 */
int
kpi_acpi_enabled(void)
{
	static int on = -1;

	if (on < 0)
		on = p9_acpi_ok() == 0;
	return on;
}

/* Linux's object as ours (into v): arguments; 0 or -1 */
static int
tovals(const union acpi_object *o, struct p9_acpival *v, int depth)
{
	u32 i;

	memset(v, 0, sizeof(*v));
	if (depth > 8)
		return -1;
	switch (o->type) {
	case ACPI_TYPE_INTEGER:
		v->type = P9_ACPI_INTEGER;
		v->integer = o->integer.value;
		return 0;
	case ACPI_TYPE_STRING:
		v->type = P9_ACPI_STRING;
		v->len = o->string.length;
		v->ptr = o->string.pointer;
		return 0;
	case ACPI_TYPE_BUFFER:
		v->type = P9_ACPI_BUFFER;
		v->len = o->buffer.length;
		v->ptr = o->buffer.pointer;
		return 0;
	case ACPI_TYPE_PACKAGE:
		v->type = P9_ACPI_PACKAGE;
		v->len = o->package.count;
		v->elem = kcalloc(o->package.count ? o->package.count : 1, sizeof(*v->elem), GFP_KERNEL);
		if (!v->elem)
			return -1;
		for (i = 0; i < o->package.count; i++)
			if (tovals(&o->package.elements[i], &v->elem[i], depth + 1) < 0)
				return -1;
		return 0;
	case ACPI_TYPE_LOCAL_REFERENCE:
		v->type = P9_ACPI_REFERENCE;
		v->node = o->reference.handle;
		return 0;
	}
	return -1;
}

/* what tovals allocated (the package arrays; the data stays the caller's) */
static void
freeargs(struct p9_acpival *v)
{
	int i;

	if (v->type != P9_ACPI_PACKAGE || !v->elem)
		return;
	for (i = 0; i < v->len; i++)
		freeargs(&v->elem[i]);
	kfree(v->elem);
}

/* the bytes after the object itself that ours needs as Linux's */
static size_t
extra(const struct p9_acpival *v)
{
	size_t n;
	int i;

	switch (v->type) {
	case P9_ACPI_STRING:
		return v->len + 1;
	case P9_ACPI_BUFFER:
		return v->len;
	case P9_ACPI_PACKAGE:
		n = ALIGN((size_t)v->len * sizeof(union acpi_object), 8);
		for (i = 0; i < v->len; i++)
			n += ALIGN(extra(&v->elem[i]), 8);
		return n;
	}
	return 0;
}

/* ours as Linux's in o, its data from *cur on */
static void
fill(union acpi_object *o, const struct p9_acpival *v, u8 **cur)
{
	union acpi_object *e;
	int i;

	memset(o, 0, sizeof(*o));
	switch (v->type) {
	case P9_ACPI_INTEGER:
		o->type = ACPI_TYPE_INTEGER;
		o->integer.value = v->integer;
		break;
	case P9_ACPI_STRING:
		o->type = ACPI_TYPE_STRING;
		o->string.length = v->len;
		o->string.pointer = (char *)*cur;
		memcpy(*cur, v->ptr, v->len);
		(*cur)[v->len] = 0;
		*cur += ALIGN(v->len + 1, 8);
		break;
	case P9_ACPI_BUFFER:
		o->type = ACPI_TYPE_BUFFER;
		o->buffer.length = v->len;
		o->buffer.pointer = *cur;
		memcpy(*cur, v->ptr, v->len);
		*cur += ALIGN(v->len, 8);
		break;
	case P9_ACPI_PACKAGE:
		o->type = ACPI_TYPE_PACKAGE;
		o->package.count = v->len;
		e = (union acpi_object *)*cur;
		o->package.elements = e;
		*cur += ALIGN((size_t)v->len * sizeof(*e), 8);
		for (i = 0; i < v->len; i++)
			fill(&e[i], &v->elem[i], cur);
		break;
	case P9_ACPI_REFERENCE:
		o->type = ACPI_TYPE_LOCAL_REFERENCE;
		o->reference.actual_type = ACPI_TYPE_ANY;
		o->reference.handle = v->node;
		break;
	default:
		o->type = ACPI_TYPE_ANY;
		break;
	}
}

acpi_status
acpi_get_handle(acpi_handle parent, const char *name, acpi_handle *ret)
{
	void *x;

	if (!name || !ret || (!parent && name[0] != '\\'))
		return AE_BAD_PARAMETER;
	if (!kpi_acpi_enabled())
		return AE_NOT_FOUND;
	if ((x = p9_acpi_walk(parent, name)) == NULL)
		return AE_NOT_FOUND;
	*ret = x;
	return AE_OK;
}

acpi_status
acpi_get_name(acpi_handle h, u32 type, struct acpi_buffer *b)
{
	char name[256];

	if (!kpi_acpi_enabled() || p9_acpi_name(h, name, sizeof(name)) < 0)
		return AE_NOT_FOUND;
	if (b->length == ACPI_ALLOCATE_BUFFER) {
		if ((b->pointer = kstrdup(name, GFP_KERNEL)) == NULL)
			return AE_ERROR;
		b->length = strlen(name) + 1;
		return AE_OK;
	}
	if (b->length < strlen(name) + 1) {
		b->length = strlen(name) + 1;
		return AE_BUFFER_OVERFLOW;
	}
	strscpy(b->pointer, name, b->length);
	return AE_OK;
}

acpi_status
acpi_evaluate_object(acpi_handle h, const char *path, struct acpi_object_list *args,
	struct acpi_buffer *ret)
{
	struct p9_acpival a[7], *v;
	size_t n;
	u8 *cur;
	void *x;
	u32 i, na;
	int r;

	if (!kpi_acpi_enabled())
		return AE_NOT_FOUND;
	if (path)
		x = p9_acpi_walk(h, path);
	else
		x = h;
	if (!x)
		return AE_NOT_FOUND;
	na = args ? args->count : 0;
	if (na > ARRAY_SIZE(a))
		return AE_BAD_PARAMETER;
	memset(a, 0, sizeof(a));
	for (i = 0; i < na; i++)
		if (tovals(&args->pointer[i], &a[i], 0) < 0) {
			while (i-- > 0)
				freeargs(&a[i]);
			return AE_BAD_PARAMETER;
		}
	v = NULL;
	r = p9_acpi_eval(x, na, a, ret ? &v : NULL);
	for (i = 0; i < na; i++)
		freeargs(&a[i]);
	if (r < 0) {
		pr_debug("acpi: %s: evaluation failed\n", path ? path : "(the handle)");
		return AE_ERROR;
	}
	if (!ret) {
		p9_acpi_free(v);
		return AE_OK;
	}
	if (!v || v->type == P9_ACPI_ANY) {
		/* no value: nothing to hand back */
		p9_acpi_free(v);
		if (ret->length == ACPI_ALLOCATE_BUFFER)
			ret->pointer = NULL;
		ret->length = 0;
		return AE_OK;
	}
	n = ALIGN(sizeof(union acpi_object), 8) + extra(v);
	if (ret->length == ACPI_ALLOCATE_BUFFER) {
		if ((ret->pointer = kzalloc(n, GFP_KERNEL)) == NULL) {
			p9_acpi_free(v);
			return AE_ERROR;
		}
	} else if (ret->length < n) {
		ret->length = n;
		p9_acpi_free(v);
		return AE_BUFFER_OVERFLOW;
	}
	ret->length = n;
	cur = (u8 *)ret->pointer + ALIGN(sizeof(union acpi_object), 8);
	fill(ret->pointer, v, &cur);
	p9_acpi_free(v);
	return AE_OK;
}

acpi_status
acpi_evaluate_integer(acpi_handle h, const char *path, struct acpi_object_list *args,
	unsigned long long *val)
{
	struct acpi_buffer b = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *o;
	acpi_status s;

	if ((s = acpi_evaluate_object(h, path, args, &b)) != AE_OK)
		return s;
	o = b.pointer;
	if (!o || o->type != ACPI_TYPE_INTEGER) {
		kfree(o);
		return AE_TYPE;
	}
	*val = o->integer.value;
	kfree(o);
	return AE_OK;
}

/* _DSM(UUID, revision, function, arguments): the device-specific method */
union acpi_object *
acpi_evaluate_dsm_typed(acpi_handle h, const guid_t *guid, u64 rev, u64 func,
	union acpi_object *argv4, u32 type)
{
	union acpi_object a[4], empty = { .package = { ACPI_TYPE_PACKAGE, 0, NULL } };
	struct acpi_object_list l = { 4, a };
	struct acpi_buffer b = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *o;

	if (!h || !guid)
		return NULL;
	a[0].buffer.type = ACPI_TYPE_BUFFER;
	a[0].buffer.length = sizeof(guid->guid);
	a[0].buffer.pointer = (u8 *)guid->guid;
	a[1].integer.type = ACPI_TYPE_INTEGER;
	a[1].integer.value = rev;
	a[2].integer.type = ACPI_TYPE_INTEGER;
	a[2].integer.value = func;
	a[3] = argv4 ? *argv4 : empty;
	if (acpi_evaluate_object(h, "_DSM", &l, &b) != AE_OK)
		return NULL;
	o = b.pointer;
	if (o && type != ACPI_TYPE_ANY && o->type != type) {
		kfree(o);
		return NULL;
	}
	return o;
}

/* function 0 of a _DSM: which functions there are (bit 0: any) */
bool
acpi_check_dsm(acpi_handle h, const guid_t *guid, u64 rev, u64 funcs)
{
	union acpi_object *o;
	u64 mask;
	u32 i;

	if (!h || !guid)
		return false;
	if ((o = acpi_evaluate_dsm(h, guid, rev, 0, NULL)) == NULL)
		return false;
	mask = 0;
	if (o->type == ACPI_TYPE_INTEGER)
		mask = o->integer.value;
	else if (o->type == ACPI_TYPE_BUFFER)
		for (i = 0; i < o->buffer.length && i < 8; i++)
			mask |= (u64)o->buffer.pointer[i] << (8 * i);
	kfree(o);
	return (mask & 1) && (mask & funcs) == funcs;
}

bool
acpi_has_method(acpi_handle h, const char *name)
{
	return kpi_acpi_enabled() && p9_acpi_walk(h, name) != NULL;
}

const char *
acpi_format_exception(acpi_status s)
{
	switch (s) {
	case AE_OK:
		return "AE_OK";
	case AE_NOT_FOUND:
		return "not found";
	case AE_BAD_PARAMETER:
		return "bad parameter";
	case AE_TYPE:
		return "wrong type";
	case AE_BUFFER_OVERFLOW:
		return "buffer overflow";
	default:
		return "unknown";
	}
}

/* devices by _HID */
struct hidwalk {
	const char	*hid;
	acpi_status	(*fn)(acpi_handle, u32, void *, void **);
	void	*ctx, **ret;
	bool	found;
};

static int
hidone(void *node, void *arg)
{
	struct hidwalk *w = arg;
	char id[16];

	if (p9_acpi_hid(node, id, sizeof(id)) < 0 || strcmp(id, w->hid) != 0)
		return 0;
	w->found = true;
	if (w->fn && w->fn(node, 1, w->ctx, w->ret) == AE_CTRL_TERMINATE)
		return 1;
	return 0;
}

acpi_status
acpi_get_devices(const char *hid, acpi_status (*fn)(acpi_handle, u32, void *, void **),
	void *ctx, void **ret)
{
	struct hidwalk w = { hid, fn, ctx, ret, false };

	if (!kpi_acpi_enabled())
		return AE_NOT_FOUND;
	p9_acpi_devices(hidone, &w);
	return AE_OK;
}

bool
acpi_dev_present(const char *hid, const char *uid, s64 hrv)
{
	struct hidwalk w = { hid, NULL, NULL, NULL, false };

	if (!kpi_acpi_enabled())
		return false;
	/* uid, hrv: not compared (no caller here gives them) */
	p9_acpi_devices(hidone, &w);
	return w.found;
}

/* a device's node: a PCI device's by _ADR, looked up once */
static DEFINE_MUTEX(companionlock);

struct acpi_device *
kpi_acpi_companion(struct device *d)
{
	struct pci_dev *p;
	struct acpi_device *a;
	void *x;

	if (!d)
		return NULL;
	mutex_lock(&companionlock);
	if (d->acpi == KPI_ACPI_PCI) {
		p = to_pci_dev(d);
		x = NULL;
		if (kpi_acpi_enabled())
			x = p9_acpi_pcidev(p->bus->number, PCI_SLOT(p->devfn), PCI_FUNC(p->devfn));
		a = NULL;
		if (x && (a = kzalloc(sizeof(*a), GFP_KERNEL)) != NULL)
			a->handle = x;
		d->acpi = a;
		if (a) {
			char name[128];

			if (p9_acpi_name(x, name, sizeof(name)) == 0)
				pr_info("%s: ACPI node %s\n", dev_name(d), name);
		}
	}
	a = d->acpi;
	mutex_unlock(&companionlock);
	return a;
}

acpi_handle
kpi_acpi_handle(struct device *d)
{
	struct acpi_device *a = kpi_acpi_companion(d);

	return a ? a->handle : NULL;
}
