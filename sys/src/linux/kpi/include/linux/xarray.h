/* Public domain. */

#ifndef _LINUX_XARRAY_H
#define _LINUX_XARRAY_H

#include <linux/gfp.h>

#include <sys/tree.h>

#define XA_FLAGS_ALLOC		(1 << 0)
#define XA_FLAGS_ALLOC1		(1 << 1)
#define XA_FLAGS_LOCK_IRQ	(1 << 2)

/*
 * lower bits of pointer are tagged:
 * 00: pointer
 * 01: value
 * 10: internal
 */
struct xarray_entry {
	SPLAY_ENTRY(xarray_entry) entry;
	int id;
	void *ptr;
};

struct xarray {
	gfp_t		xa_flags;
	struct spinlock	xa_lock;
	SPLAY_HEAD(xarray_tree, xarray_entry) xa_tree;
};

#define DEFINE_XARRAY_ALLOC(name)				\
	struct xarray name = {					\
		.xa_flags = XA_FLAGS_ALLOC,			\
		.xa_lock = MUTEX_INITIALIZER(IPL_NONE),		\
		.xa_tree = SPLAY_INITIALIZER(&name.xa_tree)	\
	}

struct xarray_range {
	uint32_t start;
	uint32_t end;
};

#define XA_LIMIT(_start, _end)	(struct xarray_range){ _start, _end }
#define xa_limit_32b		XA_LIMIT(0, UINT_MAX)

void xa_init_flags(struct xarray *, gfp_t);
void xa_destroy(struct xarray *);
int __xa_alloc(struct xarray *, u32 *, void *, struct xarray_range, gfp_t);
int __xa_alloc_cyclic(struct xarray *, u32 *, void *, struct xarray_range,
    u32 *, gfp_t);
void *__xa_load(struct xarray *, unsigned long);
void *__xa_store(struct xarray *, unsigned long, void *, gfp_t);
void *__xa_erase(struct xarray *, unsigned long);
void *xa_get_next(struct xarray *, unsigned long *);

#define xa_for_each(xa, index, entry) \
	for (index = 0; ((entry) = xa_get_next(xa, &(index))) != NULL; index++)

#define xa_lock(_xa) do {				\
		mtx_enter(&(_xa)->xa_lock);		\
	} while (0)

#define xa_unlock(_xa) do {				\
		mtx_leave(&(_xa)->xa_lock);		\
	} while (0)

#define xa_lock_irq(_xa) do {				\
		mtx_enter(&(_xa)->xa_lock);		\
	} while (0)

#define xa_unlock_irq(_xa) do {				\
		mtx_leave(&(_xa)->xa_lock);		\
	} while (0)

#define xa_lock_irqsave(_xa, _flags) do {		\
		_flags = 0;				\
		mtx_enter(&(_xa)->xa_lock);		\
	} while (0)

#define xa_unlock_irqrestore(_xa, _flags) do {		\
		(void)(_flags);				\
		mtx_leave(&(_xa)->xa_lock);		\
	} while (0)

static inline void *
xa_mk_value(unsigned long v)
{
	unsigned long r = (v << 1) | 1;
	return (void *)r;
}

static inline bool
xa_is_value(const void *e)
{
	unsigned long v = (unsigned long)e;
	return v & 1;
}

static inline unsigned long
xa_to_value(const void *e)
{
	unsigned long v = (unsigned long)e;
	return v >> 1;
}

#define XA_ERROR(x)	((struct xa_node *)(((unsigned long)x << 2) | 2))

static inline int
xa_err(const void *e)
{
	long v = (long)e;
	/* not tagged internal, not an errno */
	if ((v & 3) != 2)
		return 0;
	v >>= 2;
	if (v >= -ELAST)
		return v;
	return 0;
}

static inline bool
xa_is_err(const void *e)
{
	return xa_err(e) != 0;
}

static inline int
xa_alloc(struct xarray *xa, u32 *id, void *entry, struct xarray_range xr,
    gfp_t gfp)
{
	int r;
	mtx_enter(&xa->xa_lock);
	r = __xa_alloc(xa, id, entry, xr, gfp);
	mtx_leave(&xa->xa_lock);
	return r;
}

static inline void *
xa_load(struct xarray *xa, unsigned long index)
{
	void *r;
	r = __xa_load(xa, index);
	return r;
}


static inline void *
xa_store(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	void *r;
	mtx_enter(&xa->xa_lock);
	r = __xa_store(xa, index, entry, gfp);
	mtx_leave(&xa->xa_lock);
	return r;
}

static inline void *
xa_erase(struct xarray *xa, unsigned long index)
{
	void *r;
	mtx_enter(&xa->xa_lock);
	r = __xa_erase(xa, index);
	mtx_leave(&xa->xa_lock);
	return r;
}

static inline void *
xa_store_irq(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	void *r;
	mtx_enter(&xa->xa_lock);
	r = __xa_store(xa, index, entry, gfp);
	mtx_leave(&xa->xa_lock);
	return r;
}

static inline void *
xa_erase_irq(struct xarray *xa, unsigned long index)
{
	void *r;
	mtx_enter(&xa->xa_lock);
	r = __xa_erase(xa, index);
	mtx_leave(&xa->xa_lock);
	return r;
}

static inline bool
xa_empty(const struct xarray *xa)
{
	return SPLAY_EMPTY(&xa->xa_tree);
}

static inline void
xa_init(struct xarray *xa)
{
	xa_init_flags(xa, 0);
}

static inline int
xa_alloc_cyclic_irq(struct xarray *xa, u32 *id, void *entry,
    struct xarray_range xr, u32 *next, gfp_t gfp)    
{
	int r;
	mtx_enter(&xa->xa_lock);
	r = __xa_alloc_cyclic(xa, id, entry, xr, next, gfp);
	mtx_leave(&xa->xa_lock);
	return r;
}
/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/limits.h>
typedef unsigned int xa_mark_t;
#define XA_MARK_0		0U
#define XA_MARK_1		1U
#define XA_MARK_2		2U
#define XA_PRESENT		8U
#define XA_FREE_MARK		XA_MARK_0
#define XA_FLAGS_LOCK_BH	2U
#define XA_FLAGS_TRACK_FREE	4U
#define XA_FLAGS_ZERO_BUSY	8U
#define XA_FLAGS_ALLOC_WRAPPED	16U
struct xa_limit { u32 max; u32 min; };
#define xa_limit_31b		XA_LIMIT(0, INT_MAX)
#define xa_limit_16b		XA_LIMIT(0, USHRT_MAX)
#define XA_ZERO_ENTRY		((void *)((257UL << 2) | 2))
#define DEFINE_XARRAY_FLAGS(n, f)	struct xarray n = { .xa_flags = (f) }
#define DEFINE_XARRAY(n)		DEFINE_XARRAY_FLAGS(n, 0)
#define DEFINE_XARRAY_ALLOC1(n)		DEFINE_XARRAY_FLAGS(n, XA_FLAGS_ALLOC1)
void	*xa_cmpxchg(struct xarray *xa, unsigned long index, void *old, void *entry, gfp_t gfp);
void	*__xa_cmpxchg(struct xarray *xa, unsigned long index, void *old, void *entry, gfp_t gfp);
int	xa_insert(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp);
int	__xa_insert(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp);
int	xa_alloc_cyclic(struct xarray *xa, u32 *id, void *entry, struct xa_limit limit, u32 *next, gfp_t gfp);
void	*xa_find(struct xarray *xa, unsigned long *index, unsigned long max, xa_mark_t mark);
void	*xa_find_after(struct xarray *xa, unsigned long *index, unsigned long max, xa_mark_t mark);
bool	xa_get_mark(struct xarray *xa, unsigned long index, xa_mark_t mark);
void	xa_set_mark(struct xarray *xa, unsigned long index, xa_mark_t mark);
void	xa_clear_mark(struct xarray *xa, unsigned long index, xa_mark_t mark);
int	xa_reserve(struct xarray *xa, unsigned long index, gfp_t gfp);
void	xa_release(struct xarray *xa, unsigned long index);
#define xa_lock_bh(xa)			spin_lock(&(xa)->xa_lock)
#define xa_unlock_bh(xa)		spin_unlock(&(xa)->xa_lock)
#define xa_alloc_irq(xa, id, e, l, g)	xa_alloc(xa, id, e, l, g)
#define xa_for_each_range(xa, i, e, from, to) \
	for (i = (from), e = xa_find(xa, &i, to, XA_PRESENT); e != NULL; e = xa_find_after(xa, &i, to, XA_PRESENT))
#define xa_for_each_start(xa, index, entry, start)	xa_for_each_range(xa, index, entry, start, ULONG_MAX)
#define xa_for_each_marked(xa, i, e, mark) \
	for (i = 0, e = xa_find(xa, &i, ULONG_MAX, mark); e != NULL; e = xa_find_after(xa, &i, ULONG_MAX, mark))

#define xa_insert_irq(xa, i, e, gfp)	xa_insert(xa, i, e, gfp)	/* 9front: one lock for all */

/*
 * 9front: the advanced API, as far as amdgpu uses it: an XA_STATE at an
 * index, xas_find_marked with XA_FREE_MARK moves it to the first free
 * index (amdgpu's user queue fences count their drivers so)
 */
#ifndef XA_FREE_MARK
#define XA_FREE_MARK	0
#endif
struct xa_state {
	struct xarray	*xa;
	unsigned long	xa_index;
};
#define XA_STATE(name, array, index)	struct xa_state name = { .xa = (array), .xa_index = (index) }
static inline void *
xas_find_marked(struct xa_state *xas, unsigned long max, int mark)
{
	if (mark != XA_FREE_MARK)
		return NULL;
	while (xas->xa_index <= max && xa_load(xas->xa, xas->xa_index) != NULL)
		xas->xa_index++;
	return NULL;
}
#define xas_retry(xas, entry)	false

/* up to n entries from start to max into dst: how many (Linux's xa_extract, XA_PRESENT) */
static inline unsigned int
xa_extract(struct xarray *xa, void **dst, unsigned long start, unsigned long max, unsigned int n, xa_mark_t filter)
{
	unsigned long i = start;
	unsigned int k = 0;
	void *e;

	for (e = xa_find(xa, &i, max, filter); e != NULL && k < n; e = xa_find_after(xa, &i, max, filter))
		dst[k++] = e;
	return k;
}

#endif
