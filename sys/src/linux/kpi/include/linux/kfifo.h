/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/kfifo.h (ours, not Linux's) */
#ifndef _KPI_LINUX_KFIFO_H
#define _KPI_LINUX_KFIFO_H

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/log2.h>

/* a FIFO of fixed-size elements, a power of two of them (ours) */
struct __kfifo { unsigned int in; unsigned int out; unsigned int mask; unsigned int esize; void *data; };
#define DECLARE_KFIFO_PTR(f, t)		union { struct __kfifo kfifo; t *type; } f
#define DECLARE_KFIFO(f, t, s)		union { struct __kfifo kfifo; t *type; t buf[s]; } f
#define INIT_KFIFO(f)			do { (f).kfifo.in = (f).kfifo.out = 0; (f).kfifo.mask = ARRAY_SIZE((f).buf) - 1; (f).kfifo.esize = sizeof(*(f).type); (f).kfifo.data = (f).buf; } while (0)
#define kfifo_alloc(f, s, g)		({ unsigned int __n = roundup_pow_of_two(s); (f)->kfifo.in = (f)->kfifo.out = 0; (f)->kfifo.esize = sizeof(*(f)->type); \
	(f)->kfifo.mask = __n - 1; (f)->kfifo.data = kmalloc_array(__n, sizeof(*(f)->type), g); (f)->kfifo.data ? 0 : -ENOMEM; })
#define kfifo_free(f)			do { kfree((f)->kfifo.data); (f)->kfifo.data = NULL; } while (0)
#define kfifo_len(f)			((f)->kfifo.in - (f)->kfifo.out)
#define kfifo_is_empty(f)		((f)->kfifo.in == (f)->kfifo.out)
#define kfifo_is_full(f)		(kfifo_len(f) > (f)->kfifo.mask)
#define kfifo_reset(f)			((f)->kfifo.in = (f)->kfifo.out = 0)
#define kfifo_put(f, v)			({ int __r = !kfifo_is_full(f); if (__r) { ((typeof((f)->type))(f)->kfifo.data)[(f)->kfifo.in & (f)->kfifo.mask] = (v); (f)->kfifo.in++; } __r; })
#define kfifo_get(f, p)			({ int __r = !kfifo_is_empty(f); if (__r) { *(p) = ((typeof((f)->type))(f)->kfifo.data)[(f)->kfifo.out & (f)->kfifo.mask]; (f)->kfifo.out++; } __r; })

/*
 * 9front: struct kfifo, a FIFO of bytes, and the operations on runs of
 * elements (whatever the element size): in copies what fits, out what
 * is there, peek leaves it, skip_count drops it, out_linear_ptr names
 * the part that lies in one piece; the counts are of elements
 */
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
struct kfifo { union { struct __kfifo kfifo; unsigned char *type; const unsigned char *const_type; }; };

static inline unsigned int
__kpi_kfifo_copy(struct __kfifo *k, unsigned int at, void *buf, unsigned int n, int in)
{
	unsigned int size = k->mask + 1, off = at & k->mask, first = min(n, size - off);
	char *d = k->data;

	if (in) {
		memcpy(d + off * k->esize, buf, first * k->esize);
		memcpy(d, (char *)buf + first * k->esize, (n - first) * k->esize);
	} else {
		memcpy(buf, d + off * k->esize, first * k->esize);
		memcpy((char *)buf + first * k->esize, d, (n - first) * k->esize);
	}
	return n;
}

static inline unsigned int
__kpi_kfifo_in(struct __kfifo *k, const void *buf, unsigned int n)
{
	n = min(n, k->mask + 1 - (k->in - k->out));
	__kpi_kfifo_copy(k, k->in, (void *)buf, n, 1);
	k->in += n;
	return n;
}

static inline unsigned int
__kpi_kfifo_peek(struct __kfifo *k, void *buf, unsigned int n)
{
	n = min(n, k->in - k->out);
	return __kpi_kfifo_copy(k, k->out, buf, n, 0);
}

static inline unsigned int
__kpi_kfifo_out(struct __kfifo *k, void *buf, unsigned int n)
{
	n = __kpi_kfifo_peek(k, buf, n);
	k->out += n;
	return n;
}

static inline unsigned int
__kpi_kfifo_linear_ptr(struct __kfifo *k, void **p, unsigned int n)
{
	unsigned int off = k->out & k->mask;

	n = min(min(n, k->in - k->out), k->mask + 1 - off);
	*p = (char *)k->data + off * k->esize;
	return n;
}

#define kfifo_size(f)			((f)->kfifo.mask + 1)
#define kfifo_avail(f)			(kfifo_size(f) - kfifo_len(f))
#define kfifo_in(f, buf, n)		__kpi_kfifo_in(&(f)->kfifo, (buf), (n))
#define kfifo_out(f, buf, n)		__kpi_kfifo_out(&(f)->kfifo, (buf), (n))
#define kfifo_out_peek(f, buf, n)	__kpi_kfifo_peek(&(f)->kfifo, (buf), (n))
#define kfifo_skip_count(f, n)		((f)->kfifo.out += min((unsigned int)(n), kfifo_len(f)))
#define kfifo_skip(f)			kfifo_skip_count(f, 1)
#define kfifo_out_linear_ptr(f, p, n)	__kpi_kfifo_linear_ptr(&(f)->kfifo, (void **)(p), (n))
#define kfifo_in_spinlocked(f, buf, n, lock)	({ unsigned long __fl; unsigned int __r;	\
	spin_lock_irqsave(lock, __fl); __r = kfifo_in(f, buf, n); spin_unlock_irqrestore(lock, __fl); __r; })
#define kfifo_out_spinlocked(f, buf, n, lock)	({ unsigned long __fl; unsigned int __r;	\
	spin_lock_irqsave(lock, __fl); __r = kfifo_out(f, buf, n); spin_unlock_irqrestore(lock, __fl); __r; })
/* to a client's memory: what the fifo has, at most len bytes; *copied how much */
#define kfifo_to_user(f, to, len, copied)	({ void *__b = kmalloc((len), GFP_KERNEL); int __r = -ENOMEM;	\
	if (__b) { unsigned int __n = kfifo_out_peek(f, __b, (len) / (f)->kfifo.esize);			\
		__r = copy_to_user((to), __b, __n * (f)->kfifo.esize) ? -EFAULT : 0;			\
		if (!__r) kfifo_skip_count(f, __n); *(copied) = __r ? 0 : __n * (f)->kfifo.esize; kfree(__b); } __r; })

#endif
