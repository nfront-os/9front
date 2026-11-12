// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: kmalloc and kmem_cache over the file server's
 * malloc.  As Linux's, kmalloc of a power of two up to a page is aligned
 * to its size.  Caches with SLAB_TYPESAFE_BY_RCU keep their freed
 * objects for themselves: a reader under RCU may still look at one, so
 * its memory must stay an object of that type.  Their free list link
 * lies behind the object, which keeps its contents (as Linux's).
 */
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/log2.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/list.h>
#include <kpi/p9.h>

static unsigned long
kalign(size_t size)
{
	if (size >= 16 && size <= PAGE_SIZE && is_power_of_2(size))
		return size;
	if (size > PAGE_SIZE)
		return PAGE_SIZE;
	return 16;
}

static void *
kalloc(size_t size, gfp_t gfp, unsigned long align)
{
	void *p;

	if (size == 0)
		return ZERO_SIZE_PTR;
	if (align < 16)
		align = 16;
	p = align > 16 ? p9_mallocalign(size, align) : p9_malloc(size);
	if (p && (gfp & __GFP_ZERO))
		memset(p, 0, size);
	return p;
}




void
kfree_sensitive(const void *p)
{
	if (!ZERO_OR_NULL_PTR(p)) {
		memzero_explicit((void *)p, p9_msize((void *)p));
		p9_free((void *)p);
	}
}

size_t
ksize(const void *p)
{
	return ZERO_OR_NULL_PTR(p) ? 0 : p9_msize((void *)p);
}

size_t
kmalloc_size_roundup(size_t size)
{
	if (size <= 16)
		return 16;
	return size <= PAGE_SIZE ? roundup_pow_of_two(size) : PAGE_ALIGN(size);
}

void *
krealloc(const void *p, size_t size, gfp_t gfp)
{
	size_t old;
	void *n;

	if (ZERO_OR_NULL_PTR(p))
		return kmalloc(size, gfp);
	if (size == 0) {
		kfree(p);
		return ZERO_SIZE_PTR;
	}
	old = ksize(p);
	if (size <= old)
		return (void *)p;
	n = kmalloc(size, gfp);
	if (!n)
		return NULL;
	memcpy(n, p, old);
	if (gfp & __GFP_ZERO)
		memset((char *)n + old, 0, size - old);
	kfree(p);
	return n;
}

/* 9front: Linux 6.12's kvrealloc, without the old size (ksize knows it: kvmalloc is kmalloc here) */
void *
kvrealloc(const void *p, size_t size, gfp_t gfp)
{
	size_t old = p ? ksize(p) : 0;
	void *n;

	if (size <= old && p)
		return (void *)p;
	n = kvmalloc(size, gfp);
	if (!n)
		return NULL;
	if (p)
		memcpy(n, p, min(old, size));
	kvfree(p);
	return n;
}

void *vmalloc(unsigned long size) { return kmalloc(size, GFP_KERNEL); }
void *vzalloc(unsigned long size) { return kzalloc(size, GFP_KERNEL); }

/* caches */
/*
 * the objects of the typesafe-by-RCU caches, a set: they belong to
 * their cache until kmem_cache_destroy, a kfree of one (kpi_free) says
 * so with its chain instead of handing it to the pool, from which the
 * cache would free it again later (D2B in kmem_cache_destroy)
 */
enum { Nslabset = 1 << 16 };
/*
 * the set and the chains of the frees cost a lock in every kfree and a
 * walk of the stack in every free of such a cache: only when debugging
 * (kpi_slabdebug, aux/i915 -T sets it); the free list's mark is always
 * kept (a word, under the cache's lock)
 */
int kpi_slabdebug;
static void *slabset[Nslabset];		/* NULL empty, Gone a removed one */
static unsigned int nslabset;
static u64 slabsetlock[4];		/* QLock */
#define Gone	((void *)1)

static unsigned int
slabhash(const void *p)
{
	return ((uintptr_t)p >> 6) * 2654435761u % Nslabset;
}

static void
slabset_add(void *p)
{
	unsigned int h, i;

	static int full;

	if (!kpi_slabdebug)
		return;
	p9_qlock(slabsetlock);
	if (nslabset >= Nslabset / 2 && !full) {
		full = 1;
		printk(KERN_WARNING "slab debugging: more than %d objects, the rest unchecked\n", Nslabset / 2);
	}
	if (nslabset < Nslabset / 2) {
		for (h = slabhash(p), i = 0; i < Nslabset; i++, h = (h + 1) % Nslabset)
			if (slabset[h] == NULL || slabset[h] == Gone) {
				slabset[h] = p;
				nslabset++;
				break;
			}
	}
	p9_qunlock(slabsetlock);
}

static int
slabset_find(const void *p, int remove)
{
	unsigned int h, i;
	int r = 0;

	if (nslabset == 0)
		return 0;
	p9_qlock(slabsetlock);
	for (h = slabhash(p), i = 0; i < Nslabset && slabset[h] != NULL; i++, h = (h + 1) % Nslabset)
		if (slabset[h] == p) {
			r = 1;
			if (remove) {
				slabset[h] = Gone;
				nslabset--;
			}
			break;
		}
	p9_qunlock(slabsetlock);
	return r;
}

/* kpi_free: p is an object of a typesafe-by-RCU cache, not the pool's to have */
int
kpi_slab_owned(const void *p)
{
	if (!kpi_slabdebug || nslabset == 0 || !slabset_find(p, 0))
		return 0;
	printk(KERN_ERR "kfree of %p, an object of a typesafe-by-RCU cache\n", p);
	dump_stack();
	return 1;
}

struct kmem_cache {
	const char	*name;
	unsigned int	size, align;
	unsigned int	link;		/* TYPESAFE_BY_RCU: offset of the free list link */
	slab_flags_t	flags;
	void	(*ctor)(void *);
	u64	lock[4];	/* QLock */
	void	*free;		/* TYPESAFE_BY_RCU: freed objects */
	unsigned long	nfree;
	atomic_long_t	nout;	/* objects allocated and not yet freed */
};

#define LINK(c, p)	(*(void **)((char *)(p) + (c)->link))
/*
 * TYPESAFE_BY_RCU: past the link a mark while the object is on the free
 * list and the chain of the free that put it there; a second free says
 * so with both chains (a fence released twice: st-dma-fence-chain)
 * instead of breaking the list
 */
enum { Freedmark = 0x5ab1f4ee, Seenmark = 0x5ab15ee4, Nfreepc = 8 };
#define MARK(c, p)	(((uintptr_t *)((char *)(p) + (c)->link))[1])
#define FREEPC(c, p)	(&((uintptr_t *)((char *)(p) + (c)->link))[2])
#define TAIL		((2 + Nfreepc) * sizeof(uintptr_t))
int	kpi_backtrace(uintptr_t *pc, int n);

struct kmem_cache *
kmem_cache_create(const char *name, unsigned int size, unsigned int align, slab_flags_t flags, void (*ctor)(void *))
{
	struct kmem_cache *c = kzalloc(sizeof(*c), GFP_KERNEL);

	if (!c) {
		if (flags & SLAB_PANIC)
			panic("kmem_cache_create %s: no memory", name);
		return NULL;
	}
	c->name = name;
	c->size = max_t(unsigned int, size, sizeof(void *));
	c->align = align ? align : 16;
	if (flags & SLAB_HWCACHE_ALIGN)
		c->align = max_t(unsigned int, c->align, 64);
	c->flags = flags;
	c->ctor = ctor;
	c->link = ALIGN(c->size, sizeof(void *));
	return c;
}

/*
 * the pool's header of a malloc'd object p (9front: Bhdr magic, size,
 * then malloc's two tag words; ALIGN_MAGIC words before an aligned
 * one): ALLOC_MAGIC unless it was freed or written over
 */
static int
poolheaderok(void *p)
{
	u32 *u = (u32 *)p - 2;	/* past malloc's tags */

	while (u[-1] == 0xA1F1D1C1)	/* ALIGN_MAGIC */
		u--;
	return u[-2] == 0x0A110C09;	/* ALLOC_MAGIC */
}

static void
badheader(struct kmem_cache *c, void *p)
{
	int i;

	printk(KERN_ERR "kmem_cache_destroy %s: %p's pool header is gone (freed elsewhere or written over); our free:", c->name, p);
	for (i = 0; i < Nfreepc && FREEPC(c, p)[i]; i++)
		printk(KERN_CONT " %#lx", (unsigned long)FREEPC(c, p)[i]);
	printk(KERN_CONT "\n");
	print_hex_dump(KERN_ERR, "before ", DUMP_PREFIX_OFFSET, 16, 4, (char *)p - 32, 32, false);
	print_hex_dump(KERN_ERR, "object ", DUMP_PREFIX_OFFSET, 16, 8, p, 64, false);
}

void
kmem_cache_destroy(struct kmem_cache *c)
{
	void *p, *prev;
	int i;

	if (!c)
		return;
	/*
	 * as Linux: frees by RCU still to come first; a cache with objects
	 * still out is reported and left as it is, for their frees that
	 * come later (st-dma-fence-chain: a chain's callback puts its
	 * fences a tick later, from a timeout; the last mock fence came
	 * back while the suite's end freed the cache's list, unlocked: the
	 * list broke, an object was freed twice)
	 */
	if (c->flags & SLAB_TYPESAFE_BY_RCU)
		rcu_barrier();
	p9_qlock(c->lock);
	if (atomic_long_read(&c->nout) != 0) {
		p9_qunlock(c->lock);
		printk(KERN_ERR "kmem_cache_destroy %s: %ld objects still out, the cache stays\n",
		    c->name, atomic_long_read(&c->nout));
		return;
	}
	p9_qunlock(c->lock);
	/*
	 * first the whole list: each element once (marked as seen); one met
	 * twice (an object on the list twice, or a cycle) is reported with
	 * the one before it, and the list cut there
	 */
	{
		unsigned long n = 0;
		void *q, *before = NULL;

		for (q = c->free; q != NULL; before = q, q = LINK(c, q), n++) {
			if (((uintptr_t)q & (c->align - 1)) || MARK(c, q) != Freedmark) {
				if (MARK(c, q) == Seenmark)
					printk(KERN_ERR "kmem_cache_destroy %s: %p twice on the free list, again after %p (element %lu of %lu)\n",
					    c->name, q, before, n, c->nfree);
				if (before)
					LINK(c, before) = NULL;
				else
					c->free = NULL;
				break;
			}
			MARK(c, q) = Seenmark;
		}
		for (q = c->free; q != NULL; q = LINK(c, q))
			MARK(c, q) = Freedmark;
	}
	prev = NULL;
	while ((p = c->free) != NULL) {
		if (((uintptr_t)p & (c->align - 1)) || MARK(c, p) != Freedmark) {
			/* not one we put there: the list was written over */
			printk(KERN_ERR "kmem_cache_destroy %s: %p on the free list is not a freed object (mark %#lx), after %p, freed at",
			    c->name, p, ((uintptr_t)p & (c->align - 1)) ? 0UL : (unsigned long)MARK(c, p), prev);
			for (i = 0; prev && i < Nfreepc && FREEPC(c, prev)[i]; i++)
				printk(KERN_CONT " %#lx", (unsigned long)FREEPC(c, prev)[i]);
			printk(KERN_CONT "\n");
			if (prev)
				print_hex_dump(KERN_ERR, "prev ", DUMP_PREFIX_OFFSET, 16, 8, prev, c->link + TAIL, false);
			break;
		}
		c->free = LINK(c, p);
		if (prev) {
			slabset_find(prev, 1);
			if (poolheaderok(prev))
				p9_free(prev);
			else
				badheader(c, prev);
		}
		prev = p;
	}
	if (prev) {
		slabset_find(prev, 1);
		if (poolheaderok(prev))
			p9_free(prev);
		else
			badheader(c, prev);
	}
	kfree(c);
}

void
kmem_cache_shrink(struct kmem_cache *c)
{
}

void *
kmem_cache_alloc(struct kmem_cache *c, gfp_t gfp)
{
	void *p = NULL;

	if (c->flags & SLAB_TYPESAFE_BY_RCU) {
		p9_qlock(c->lock);
		if ((p = c->free) != NULL) {
			c->free = LINK(c, p);
			c->nfree--;
			MARK(c, p) = 0;
		}
		p9_qunlock(c->lock);
		if (p) {
			atomic_long_inc(&c->nout);
			/* constructed when first made */
			if (gfp & __GFP_ZERO)
				memset(p, 0, c->size);
			return p;
		}
	}
	p = kalloc((c->flags & SLAB_TYPESAFE_BY_RCU) ? c->link + TAIL : c->size, gfp & ~__GFP_ZERO, c->align);
	if (p && (c->flags & SLAB_TYPESAFE_BY_RCU)) {
		MARK(c, p) = 0;
		slabset_add(p);
	}
	if (!p)
		return NULL;
	atomic_long_inc(&c->nout);
	if (c->ctor)
		c->ctor(p);
	else if (gfp & __GFP_ZERO)
		memset(p, 0, c->size);
	return p;
}

void *kmem_cache_zalloc(struct kmem_cache *c, gfp_t gfp) { return kmem_cache_alloc(c, gfp | __GFP_ZERO); }

void
kmem_cache_free(struct kmem_cache *c, void *p)
{
	if (!p)
		return;
	if (c->flags & SLAB_TYPESAFE_BY_RCU) {
		uintptr_t pc[Nfreepc];
		int i, n;

		n = kpi_slabdebug ? kpi_backtrace(pc, Nfreepc) : 0;
		if ((uintptr_t)p & (c->align - 1)) {
			printk(KERN_ERR "kmem_cache_free %s: %p is none of its objects (alignment %u)\n", c->name, p, c->align);
			dump_stack();
			return;
		}
		p9_qlock(c->lock);
		if (MARK(c, p) == Freedmark) {
			p9_qunlock(c->lock);
			printk(KERN_ERR "kmem_cache_free %s: %p freed twice; the first free:", c->name, p);
			for (i = 0; i < Nfreepc && FREEPC(c, p)[i]; i++)
				printk(KERN_CONT " %#lx", (unsigned long)FREEPC(c, p)[i]);
			printk(KERN_CONT "\n");
			dump_stack();
			return;
		}
		MARK(c, p) = Freedmark;
		for (i = 0; i < Nfreepc; i++)
			FREEPC(c, p)[i] = i < n ? pc[i] : 0;
		LINK(c, p) = c->free;
		c->free = p;
		c->nfree++;
		atomic_long_dec(&c->nout);
		p9_qunlock(c->lock);
		return;
	}
	atomic_long_dec(&c->nout);
	p9_free(p);
}
