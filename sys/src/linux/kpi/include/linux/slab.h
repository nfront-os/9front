/* Public domain. */

#ifndef _LINUX_SLAB_H
#define _LINUX_SLAB_H

#include <sys/types.h>
#include <sys/malloc.h>

#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/gfp.h>
#include <linux/cleanup.h>

#include <linux/processor.h>	/* for CACHELINESIZE */

#define ARCH_KMALLOC_MINALIGN CACHELINESIZE

#define ZERO_SIZE_PTR		NULL
#define ZERO_OR_NULL_PTR(x)	((x) == NULL)

static inline void *
kmalloc(size_t size, int flags)
{
	return kpi_malloc(size, M_DRM, flags);
}

static inline void *
kmalloc_array(size_t n, size_t size, int flags)
{
	if (n != 0 && SIZE_MAX / n < size)
		return NULL;
	return kpi_malloc(n * size, M_DRM, flags);
}

static inline void *
kcalloc(size_t n, size_t size, int flags)
{
	if (n != 0 && SIZE_MAX / n < size)
		return NULL;
	return kpi_malloc(n * size, M_DRM, flags | M_ZERO);
}

static inline void *
kzalloc(size_t size, int flags)
{
	return kpi_malloc(size, M_DRM, flags | M_ZERO);
}

static inline void
kfree(const void *objp)
{
	kpi_free((void *)objp, M_DRM, 0);
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/overflow.h>
void	*krealloc(const void *p, size_t size, gfp_t gfp);
void	kfree_sensitive(const void *p);
size_t	ksize(const void *p);
size_t	kmalloc_size_roundup(size_t size);
void	*kvrealloc(const void *p, size_t size, gfp_t gfp);
#define kzalloc_node(s, g, n)		kzalloc(s, g)
#define kmalloc_node(s, g, n)		kmalloc(s, g)
#define kvmalloc_node(s, g, n)		kmalloc(s, g)
#define kvzalloc_node(s, g, n)		kzalloc(s, g)
#define krealloc_array(p, n, s, g)	krealloc(p, size_mul(n, s), g)
#define kmalloc_track_caller(s, g)	kmalloc(s, g)
struct rcu_head;
void	kpi_kvfree_call_rcu(struct rcu_head *head, void *ptr);
#define __kpi_kvfree_rcu_2(p, f)	kpi_kvfree_call_rcu(&(p)->f, (void *)(p))
#define __kpi_kvfree_rcu_1(p)		kpi_kvfree_call_rcu(NULL, (void *)(p))
#define __kpi_kvfree_rcu_n(_1, _2, n, ...)	n
#define kvfree_rcu(...)			__kpi_kvfree_rcu_n(__VA_ARGS__, __kpi_kvfree_rcu_2, __kpi_kvfree_rcu_1)(__VA_ARGS__)
#define kfree_rcu_mightsleep(p)		kpi_kvfree_call_rcu(NULL, (void *)(p))
#define kvfree_rcu_mightsleep(p)	kpi_kvfree_call_rcu(NULL, (void *)(p))
#define KMALLOC_MAX_SIZE		(1UL << 22)
#define SLAB_HWCACHE_ALIGN		0x00002000U
#define SLAB_RECLAIM_ACCOUNT		0x00020000U
#define SLAB_TYPESAFE_BY_RCU		0x00080000U
#define SLAB_PANIC			0x00040000U
#define SLAB_ACCOUNT			0x04000000U
#define SLAB_POISON			0x00000800U
struct kmem_cache	*kmem_cache_create(const char *name, unsigned int size, unsigned int align, slab_flags_t flags, void (*ctor)(void *));
void	kmem_cache_destroy(struct kmem_cache *c);
void	*kmem_cache_alloc(struct kmem_cache *c, gfp_t gfp);
void	*kmem_cache_zalloc(struct kmem_cache *c, gfp_t gfp);
void	kmem_cache_free(struct kmem_cache *c, void *p);
void	kmem_cache_shrink(struct kmem_cache *c);
#define KMEM_CACHE(s, f)		kmem_cache_create(#s, sizeof(struct s), __alignof__(struct s), (f), NULL)

/*
 * 9front: Linux 7's typed allocations: kmalloc_obj(x) one object of x's
 * type (x a variable or a type), _objs(x, n) n of them (the size checked
 * for overflow), _flex(x, fam, n) one with n elements of its flexible
 * array fam, the array's __counted_by counter set; the flags optional
 */
#include <linux/overflow.h>
#define __kpi_alloc_objs(KMALLOC, GFP, TYPE, COUNT)	\
	((TYPE *)KMALLOC(size_mul(sizeof(TYPE), (COUNT)), (GFP)))
#define __kpi_alloc_flex(KMALLOC, GFP, TYPE, FAM, COUNT)	\
({									\
	const size_t __n = (COUNT);					\
	TYPE *__p = KMALLOC(struct_size_t(TYPE, FAM, __n), (GFP));	\
	if (__p)							\
		__set_flex_counter(__p->FAM, __n);			\
	__p;								\
})
#define kmalloc_obj(P, ...)		__kpi_alloc_objs(kmalloc, default_gfp(__VA_ARGS__), typeof(P), 1)
#define kmalloc_objs(P, N, ...)		__kpi_alloc_objs(kmalloc, default_gfp(__VA_ARGS__), typeof(P), N)
#define kmalloc_flex(P, F, N, ...)	__kpi_alloc_flex(kmalloc, default_gfp(__VA_ARGS__), typeof(P), F, N)
#define kzalloc_obj(P, ...)		__kpi_alloc_objs(kzalloc, default_gfp(__VA_ARGS__), typeof(P), 1)
#define kzalloc_objs(P, N, ...)		__kpi_alloc_objs(kzalloc, default_gfp(__VA_ARGS__), typeof(P), N)
#define kzalloc_flex(P, F, N, ...)	__kpi_alloc_flex(kzalloc, default_gfp(__VA_ARGS__), typeof(P), F, N)
#define kvmalloc_obj(P, ...)		__kpi_alloc_objs(kvmalloc, default_gfp(__VA_ARGS__), typeof(P), 1)
#define kvmalloc_objs(P, N, ...)	__kpi_alloc_objs(kvmalloc, default_gfp(__VA_ARGS__), typeof(P), N)
#define kvmalloc_flex(P, F, N, ...)	__kpi_alloc_flex(kvmalloc, default_gfp(__VA_ARGS__), typeof(P), F, N)
#define kvzalloc_obj(P, ...)		__kpi_alloc_objs(kvzalloc, default_gfp(__VA_ARGS__), typeof(P), 1)
#define kvzalloc_objs(P, N, ...)	__kpi_alloc_objs(kvzalloc, default_gfp(__VA_ARGS__), typeof(P), N)
#define kvzalloc_flex(P, F, N, ...)	__kpi_alloc_flex(kvzalloc, default_gfp(__VA_ARGS__), typeof(P), F, N)

/* 9front: __free(kfree) (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_FREE(kfree, void *, if (!IS_ERR_OR_NULL(_T)) kfree(_T))
DEFINE_FREE(kvfree, void *, if (!IS_ERR_OR_NULL(_T)) kvfree(_T))

#endif
