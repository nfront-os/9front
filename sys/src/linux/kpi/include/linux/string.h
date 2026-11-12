/* Public domain. */

#ifndef _LINUX_STRING_H
#define _LINUX_STRING_H

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/stdint.h>
#include <sys/errno.h>

#include <linux/compiler.h>
#include <linux/err.h>
#include <linux/overflow.h>

void *memchr_inv(const void *, int, size_t);

static inline bool
mem_is_zero(const void *b, size_t len)
{
	return (memchr_inv(b, 0, len) == NULL);
}

static inline void *
memset32(uint32_t *b, uint32_t c, size_t len)
{
	uint32_t *dst = b;
	while (len--)
		*dst++ = c;
	return b;
}

static inline void *
memset64(uint64_t *b, uint64_t c, size_t len)
{
	uint64_t *dst = b;
	while (len--)
		*dst++ = c;
	return b;
}

static inline void *
memset_p(void **p, void *v, size_t n)
{
#ifdef __LP64__
	return memset64((uint64_t *)p, (uintptr_t)v, n);
#else
	return memset32((uint32_t *)p, (uintptr_t)v, n);
#endif
}

#define memset_after(type, c, member) ({ \
	memset(((uint8_t *)(type)) + offsetofend(typeof(*(type)), member), c, \
	    sizeof(*(type)) - offsetofend(typeof(*(type)), member)); \
})

static inline void *
kmemdup(const void *src, size_t len, int flags)
{
	void *p = kpi_malloc(len, M_DRM, flags);
	if (p)
		memcpy(p, src, len);
	return (p);
}

static inline void *
kmemdup_array(const void *src, size_t nemb, size_t size, int flags)
{
	void *p = mallocarray(nemb, size, M_DRM, flags);
	if (p)
		memcpy(p, src, nemb * size);
	return (p);
}

static inline void *
kstrdup(const char *str, int flags)
{
	size_t len;
	char *p;

	if (str == NULL)
		return NULL;

	len = strlen(str) + 1;
	p = kpi_malloc(len, M_DRM, flags);
	if (p)
		memcpy(p, str, len);
	return (p);
}

static inline const char *
kstrdup_const(const char *str, int flags)
{
	return kstrdup(str, flags);
}

void kfree_const(const void *);

static inline int
match_string(const char * const *array,  size_t n, const char *str)
{
	int i;

	for (i = 0; i < n; i++) {
		if (array[i] == NULL)
			break;
		if (!strcmp(array[i], str))	
			return i;
	}

	return -EINVAL;
}

static inline ssize_t
sized_strscpy(char *dst, const char *src, size_t dstsize)
{
	ssize_t r;
	r = strlcpy(dst, src, dstsize);
	if (dstsize == 0 || r >= dstsize)
		return -E2BIG;
	return r;
}

static inline ssize_t
sized_strscpy_pad(char *dst, const char *src, size_t dstsize)
{
	memset(dst, 0, dstsize);
	return sized_strscpy(dst, src, dstsize);
}

/* 9front: Linux 6.8's strscpy: the size optional, an array destination's own without it */
#include <linux/args.h>
#define __strscpy0(dst, src, ...)	sized_strscpy(dst, src, sizeof(dst) + __must_be_array(dst))
#define __strscpy1(dst, src, size)	sized_strscpy(dst, src, size)
#define __strscpy_pad0(dst, src, ...)	sized_strscpy_pad(dst, src, sizeof(dst) + __must_be_array(dst))
#define __strscpy_pad1(dst, src, size)	sized_strscpy_pad(dst, src, size)
#define strscpy(dst, src, ...)		CONCATENATE(__strscpy, COUNT_ARGS(__VA_ARGS__))(dst, src, __VA_ARGS__)
#define strscpy_pad(dst, src, ...)	CONCATENATE(__strscpy_pad, COUNT_ARGS(__VA_ARGS__))(dst, src, __VA_ARGS__)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/overflow.h>
#include <linux/stddef.h>
void	*memset16(u16 *s, u16 v, size_t n);
void	*memscan(void *addr, int c, size_t size);
char	*strchrnul(const char *s, int c);
char	*strnchr(const char *s, size_t n, int c);
char	*strnstr(const char *s, const char *t, size_t n);
char	*strpbrk(const char *s, const char *a);
size_t	strspn(const char *s, const char *a);
size_t	strcspn(const char *s, const char *r);
char	*strim(char *s);
char	*skip_spaces(const char *s);
char	*strreplace(char *s, char old, char new);
int	sysfs_streq(const char *a, const char *b);
char	*kstrndup(const char *s, size_t max, gfp_t gfp);
char	*kmemdup_nul(const char *s, size_t max, gfp_t gfp);
void	*memdup_user(const void __user *src, size_t len);
void	*memdup_user_nul(const void __user *src, size_t len);
void	*vmemdup_user(const void __user *src, size_t len);
char	**argv_split(gfp_t gfp, const char *str, int *argcp);
void	argv_free(char **argv);
#define memset_after(obj, v, member)	memset((u8 *)&(obj)->member + sizeof((obj)->member), v, sizeof(*(obj)) - offsetofend(typeof(*(obj)), member))
#define memset_startat(obj, v, member)	memset((u8 *)&(obj)->member, v, sizeof(*(obj)) - offsetof(typeof(*(obj)), member))
#define memcpy_and_pad(d, dl, s, c, p)	do { size_t __c = min_t(size_t, c, dl); memcpy(d, s, __c); memset((u8 *)(d) + __c, p, (dl) - __c); } while (0)
#define unsafe_memcpy(d, s, n, j)	memcpy(d, s, n)
#define str_has_prefix(s, p)		({ size_t __l = strlen(p); strncmp(s, p, __l) == 0 ? __l : 0; })
static inline void memzero_explicit(void *s, size_t n) { memset(s, 0, n); barrier_data(s); }
/* 9front: Linux 6.7's array forms, on ours (the client's arena: kpi_user_window) */
static inline void *
memdup_array_user(const void __user *src, size_t n, size_t size)
{
	size_t len;

	if (check_mul_overflow(n, size, &len))
		return ERR_PTR(-EOVERFLOW);
	return memdup_user(src, len);
}

static inline void *
vmemdup_array_user(const void __user *src, size_t n, size_t size)
{
	size_t len;

	if (check_mul_overflow(n, size, &len))
		return ERR_PTR(-EOVERFLOW);
	return vmemdup_user(src, len);
}

/* 9front: src into the array dest, the rest pad (no NUL needed: Linux 6.0) */
#define strtomem_pad(dest, src, pad)	do { size_t __n = strnlen((src), sizeof(dest)); memcpy((dest), (src), __n); memset((char *)(dest) + __n, (pad), sizeof(dest) - __n); } while (0)

#endif
