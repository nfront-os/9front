/*	$OpenBSD: uaccess.h,v 1.9 2026/08/18 00:53:37 jsg Exp $	*/
/*
 * Copyright (c) 2015 Mark Kettenis
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

#ifndef _LINUX_UACCESS_H
#define _LINUX_UACCESS_H

#include <sys/param.h>
#include <sys/systm.h>
#include <uvm/uvm_extern.h>

#include <linux/sched.h>
#include <linux/nospec.h>

static inline unsigned long
__copy_to_user(void *to, const void *from, unsigned long len)
{
	if (copyout(from, to, len))
		return len;
	return 0;
}

static inline unsigned long
copy_to_user(void *to, const void *from, unsigned long len)
{
	return __copy_to_user(to, from, len);
}

static inline unsigned long
__copy_from_user(void *to, const void *from, unsigned long len)
{
	if (copyin(from, to, len))
		return len;
	return 0;
}

static inline unsigned long
copy_from_user(void *to, const void *from, unsigned long len)
{
	return __copy_from_user(to, from, len);
}

#define get_user(x, ptr)	-copyin(ptr, &(x), sizeof(x))
#define put_user(x, ptr) ({				\
	__typeof((x)) __tmp = (x);			\
	-copyout(&(__tmp), ptr, sizeof(__tmp));		\
})
#define __get_user(x, ptr)	get_user((x), (ptr))
#define __put_user(x, ptr)	put_user((x), (ptr))

#define unsafe_put_user(x, ptr, err) ({				\
	__typeof((x)) __tmp = (x);				\
	if (copyout(&(__tmp), ptr, sizeof(__tmp)) != 0)		\
		goto err;					\
})

int	kpi_user_ok(const void *uaddr, size_t len);	/* 9front: inside the proc's user window (kpi/obsd/systm.c) */
void	kpi_user_window(void *start, unsigned long len);
void	kpi_user_privileged(int priv);	/* 9front: the client's capable() (kpi/obsd/systm.c) */

static inline int
access_ok(const void *addr, unsigned long size)
{
	vaddr_t startva = (vaddr_t)addr;
	vaddr_t endva = ((vaddr_t)addr) + size;
	return (startva >= VM_MIN_ADDRESS && endva >= VM_MIN_ADDRESS) &&
	    (startva <= VM_MAXUSER_ADDRESS && endva <= VM_MAXUSER_ADDRESS) &&
	    kpi_user_ok(addr, size);
}

#define user_access_begin(addr, size)	access_ok(addr, size)
#define user_access_end()

#define user_write_access_begin(addr, size)	access_ok(addr, size)
#define user_write_access_end()

#if defined(__i386__) || defined(__amd64__)

static inline void
pagefault_disable(void)
{
	curcpu()->ci_inatomic++;
	KASSERT(curcpu()->ci_inatomic > 0);
}

static inline void
pagefault_enable(void)
{
	KASSERT(curcpu()->ci_inatomic > 0);
	curcpu()->ci_inatomic--;
}

static inline int
pagefault_disabled(void)
{
	return curcpu()->ci_inatomic;
}

static inline unsigned long
__copy_to_user_inatomic(void *to, const void *from, unsigned long len)
{
	struct cpu_info *ci = curcpu();
	int inatomic = ci->ci_inatomic;
	int error;

	ci->ci_inatomic = 1;
	error = copyout(from, to, len);
	ci->ci_inatomic = inatomic;

	return (error ? len : 0);
}

static inline unsigned long
__copy_from_user_inatomic(void *to, const void *from, unsigned long len)
{
	struct cpu_info *ci = curcpu();
	int inatomic = ci->ci_inatomic;
	int error;

	ci->ci_inatomic = 1;
	error = copyin(from, to, len);
	ci->ci_inatomic = inatomic;

	return (error ? len : 0);
}

static inline unsigned long
copy_from_user_inatomic_nontemporal(void *to, const void *from,
    unsigned long len)
{
	return __copy_from_user_inatomic(to, from, len);
}

#endif /* defined(__i386__) || defined(__amd64__) */

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#define user_read_access_begin(p, s)	1
#define user_read_access_end()		do { } while (0)
#define copy_from_user_inatomic(t, f, n)	copy_from_user(t, f, n)
#define unsafe_get_user(x, p, e)	do { if (get_user((x), (p))) goto e; } while (0)	/* 9front: within the user window */
#define unsafe_copy_to_user(d, s, n, e)	do { memcpy((void *)(d), s, n); } while (0)
#define clear_user(p, n)		({ memset((void *)(p), 0, n); 0UL; })
/* 9front: as many bytes as the user window holds, at most n; -EFAULT outside it */
static inline long
strncpy_from_user(char *d, const char __user *s, long n)
{
	long i;

	for (i = 0; i < n; i++) {
		if (!kpi_user_ok(s + i, 1))
			return -EFAULT;
		if ((d[i] = s[i]) == 0)
			return i;
	}
	return n;
}
#define faultin_page_range(a, b)	0
#define fault_in_readable(p, s)		0
#define fault_in_writeable(p, s)	0
static inline int check_zeroed_user(const void __user *from, size_t size) { const u8 *p = (const void *)from; size_t i; for (i = 0; i < size; i++) if (p[i]) return 0; return 1; }
static inline int copy_struct_from_user(void *dst, size_t ksize, const void __user *src, size_t usize)
{ size_t s = min(ksize, usize); if (usize > ksize && !check_zeroed_user((const char *)src + ksize, usize - ksize)) return -E2BIG;
  memcpy(dst, (const void *)src, s); if (ksize > usize) memset((char *)dst + usize, 0, ksize - usize); return 0; }

#endif
