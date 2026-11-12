// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: pages.  Linux's pages are memory the device can
 * reach by physical address; here they come from a pool of #g segments
 * of pinned pages (sticky: they stay where they are) attached to the
 * file server, 8 MB each, made as needed; vmap of scattered pages makes
 * an alias segment of them.  Each page of a chunk has its
 * struct page, with where the server sees it and where the device does.
 * A run of 2^order pages must be contiguous both ways; pages are found
 * again by address (virt_to_page) and by frame number (pfn_to_page).
 */
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/bitmap.h>
#include <linux/vmalloc.h>
#include <linux/pagevec.h>
#include <linux/smp.h>
#include <kpi/p9.h>

#define CHUNK		(8UL << 20)
#define CHUNKPAGES	(CHUNK >> PAGE_SHIFT)
#define NHASH		4096

unsigned long	kpi_shrink(gfp_t gfp, unsigned long want);	/* shrinker.c */

struct chunk {
	struct chunk	*next;
	char	name[32];
	u8	*va;
	struct page	*pages;
	unsigned long	used[BITS_TO_LONGS(CHUNKPAGES)];
	unsigned long	nfree;
};

static u64 plock[4];		/* QLock */
static struct chunk *chunks;
static int nchunks;
static struct page *hash[NHASH];

static inline unsigned int hashpfn(unsigned long pfn) { return (pfn * 2654435761UL) % NHASH; }

static struct chunk *
newchunk(void)
{
	unsigned long long *pa;
	struct chunk *c;
	struct page *p;
	unsigned long i;
	unsigned int h;

	c = kzalloc(sizeof(*c), GFP_KERNEL);
	pa = kmalloc_array(CHUNKPAGES, sizeof(*pa), GFP_KERNEL);
	if (!c || !pa)
		goto err;
	c->pages = kcalloc(CHUNKPAGES, sizeof(struct page), GFP_KERNEL);
	if (!c->pages)
		goto err;
	snprintf(c->name, sizeof(c->name), "kpi.%d.%d", p9_getpid(), nchunks);
	/*
	 * physically contiguous and aligned if the kernel has such a run
	 * free (then every block of an order is naturally aligned, as
	 * Linux's buddy allocator gives them), else scattered pages
	 */
	c->va = p9_segalloc(c->name, CHUNK, pa, 1);
	if (!c->va) {
		snprintf(c->name, sizeof(c->name), "kpi.%d.%ds", p9_getpid(), nchunks);
		c->va = p9_segalloc(c->name, CHUNK, pa, 0);
	}
	if (!c->va)
		goto err;
	for (i = 0; i < CHUNKPAGES; i++) {
		p = &c->pages[i];
		p->virtual = c->va + i * PAGE_SIZE;
		p->phys_addr = pa[i];
		p->kpi_chunk = c;
		INIT_LIST_HEAD(&p->lru);
		h = hashpfn(pa[i] >> PAGE_SHIFT);
		p->kpi_hnext = hash[h];
		hash[h] = p;
	}
	c->nfree = CHUNKPAGES;
	kfree(pa);
	c->next = chunks;
	chunks = c;
	nchunks++;
	return c;
err:
	if (c)
		kfree(c->pages);
	kfree(pa);
	kfree(c);
	return NULL;
}

/*
 * a free run of n pages in c, physically contiguous and aligned to its
 * size, as Linux's alloc_pages(order) gives
 */
static long
findrun(struct chunk *c, unsigned long n)
{
	unsigned long i, k;

	for (i = 0; i + n <= CHUNKPAGES; i++) {
		if (c->pages[i].phys_addr & (n * PAGE_SIZE - 1))
			continue;
		if (_find_next_bit(c->used, i + n, i) < i + n)
			continue;
		for (k = 1; k < n; k++)
			if (c->pages[i + k].phys_addr != c->pages[i].phys_addr + k * PAGE_SIZE)
				break;
		if (k == n)
			return i;
	}
	return -1;
}

struct page *
alloc_pages(gfp_t gfp, unsigned int order)
{
	unsigned long n = 1UL << order, k;
	struct chunk *c;
	struct page *p;
	long i = -1;
	int shrunk = 0;

	if (n > CHUNKPAGES)
		return NULL;
again:
	p9_qlock(plock);
	for (c = chunks; c; c = c->next)
		if (c->nfree >= n && (i = findrun(c, n)) >= 0)
			break;
	if (!c) {
		/* a new chunk; a higher order may still fail there, then the caller tries lower */
		c = newchunk();
		if (!c && !(gfp & __GFP_NORETRY) && !shrunk) {
			/* no more memory: what the shrinkers give back, then again */
			p9_qunlock(plock);
			shrunk = 1;
			kpi_shrink(gfp, 2 * n);
			goto again;
		}
		if (!c || (i = findrun(c, n)) < 0) {
			p9_qunlock(plock);
			return NULL;
		}
	}
	bitmap_set(c->used, i, n);
	c->nfree -= n;
	p9_qunlock(plock);
	p = &c->pages[i];
	for (k = 0; k < n; k++) {
		p[k].flags = 0;
		p[k].private = 0;
		p[k].mapping = NULL;
		p[k].index = 0;
		p[k].order = 0;
		p[k].kpi_head = k ? p : NULL;
		atomic_set(&p[k]._refcount, k ? 0 : 1);
		atomic_set(&p[k]._mapcount, -1);
	}
	p->order = order;
	if (gfp & __GFP_ZERO)
		memset(p->virtual, 0, n * PAGE_SIZE);
	return p;
}

struct folio *folio_alloc(gfp_t gfp, unsigned int order) { return (struct folio *)alloc_pages(gfp | __GFP_COMP, order); }

static void
freerun(struct page *p)
{
	struct chunk *c = p->kpi_chunk;
	unsigned long n = 1UL << p->order;

	p9_qlock(plock);
	bitmap_clear(c->used, p - c->pages, n);
	c->nfree += n;
	p9_qunlock(plock);
}

void
put_page(struct page *p)
{
	if (!p->kpi_chunk)
		return;		/* a page of the heap or a frame's (heappage, kpi_pfn_to_page): not the pool's */
	p = compound_head(p);
	if (atomic_dec_and_test(&p->_refcount))
		freerun(p);
}

void __free_pages(struct page *p, unsigned int order) { put_page(p); }

/*
 * 9front: a run of 2^order pages into as many pages of their own (TTM's
 * pool before it frees them one by one): each its reference, order 0
 */
void
split_page(struct page *p, unsigned int order)
{
	unsigned long n = 1UL << order, k;

	for (k = 0; k < n; k++) {
		p[k].order = 0;
		p[k].kpi_head = NULL;
		if (k)
			atomic_set(&p[k]._refcount, 1);
	}
}

unsigned long
__get_free_pages(gfp_t gfp, unsigned int order)
{
	struct page *p = alloc_pages(gfp, order);

	return p ? (unsigned long)p->virtual : 0;
}

unsigned long get_zeroed_page(gfp_t gfp) { return __get_free_pages(gfp | __GFP_ZERO, 0); }

void
free_pages(unsigned long addr, unsigned int order)
{
	if (addr)
		__free_pages(virt_to_page((void *)addr), order);
}

void
__folio_batch_release(struct folio_batch *fb)
{
	unsigned int i;

	for (i = 0; i < fb->nr; i++)
		folio_put(fb->folios[i]);
	fb->nr = 0;
}

/*
 * Linux gives every kernel address a struct page, kmalloc's too (the
 * error state puts kmalloc'd text in a scatter list).  A page of the
 * heap gets one when first asked for: its address for the processor,
 * no physical address (not for the device), never given back to the
 * pool; found again by its address
 */
static struct page *heappages[NHASH];

static struct page *
heappage(const u8 *a)
{
	unsigned long va = (unsigned long)a & PAGE_MASK;
	unsigned int h = hashpfn(va >> PAGE_SHIFT);
	struct page *p;

	p9_qlock(plock);
	for (p = heappages[h]; p; p = p->kpi_hnext)
		if ((unsigned long)p->virtual == va)
			break;
	if (!p) {
		p = p9_mallocz(sizeof(*p));
		if (p) {
			p->virtual = (void *)va;
			INIT_LIST_HEAD(&p->lru);
			atomic_set(&p->_refcount, 1);
			atomic_set(&p->_mapcount, -1);
			p->kpi_hnext = heappages[h];
			heappages[h] = p;
		}
	}
	p9_qunlock(plock);
	return p;
}

struct page *
kpi_virt_to_page(const void *v)
{
	const u8 *a = v;
	struct chunk *c;

	for (c = chunks; c; c = c->next)
		if (a >= c->va && a < c->va + CHUNK)
			return &c->pages[(a - c->va) >> PAGE_SHIFT];
	return heappage(a);
}

void *kpi_page_address(const struct page *p) { return p->virtual; }

/*
 * Linux has a struct page for every page frame (the memmap): a frame
 * that is no page of the pool gets one when first asked for, its
 * physical address only (no address for the processor), never the
 * pool's (put_page leaves it); found again by its number
 */
static struct page *framepages[NHASH];

/*
 * a frame of RAM: what we know to be is the pool's (other frames, of the
 * machine's memory or of a device, are not told apart here)
 */
int
page_is_ram(unsigned long pfn)
{
	struct page *p;

	for (p = hash[hashpfn(pfn)]; p; p = p->kpi_hnext)
		if ((p->phys_addr >> PAGE_SHIFT) == pfn)
			return 1;
	return 0;
}

struct page *
kpi_pfn_to_page(unsigned long pfn)
{
	unsigned int h = hashpfn(pfn);
	struct page *p;

	for (p = hash[h]; p; p = p->kpi_hnext)
		if ((p->phys_addr >> PAGE_SHIFT) == pfn)
			return p;
	p9_qlock(plock);
	for (p = framepages[h]; p; p = p->kpi_hnext)
		if ((p->phys_addr >> PAGE_SHIFT) == pfn)
			break;
	if (!p) {
		p = p9_mallocz(sizeof(*p));
		if (p) {
			p->phys_addr = (phys_addr_t)pfn << PAGE_SHIFT;
			INIT_LIST_HEAD(&p->lru);
			atomic_set(&p->_refcount, 1);
			atomic_set(&p->_mapcount, -1);
			p->kpi_hnext = framepages[h];
			framepages[h] = p;
		}
	}
	p9_qunlock(plock);
	return p;
}

/*
 * nth_page: the page n frames after p; p + n while that is in p's chunk
 * and the frame that follows (a chunk's frames may be scattered), else
 * found by its frame number (another chunk's, which lies right after it
 * in memory: page + n would run past the end of p's chunk's array)
 */
struct page *
kpi_nth_page(struct page *p, unsigned long n)
{
	struct chunk *c = p->kpi_chunk;
	unsigned long i;

	if (n == 0)
		return p;
	if (c) {
		i = p - c->pages;
		if (i + n < CHUNKPAGES && c->pages[i + n].phys_addr == p->phys_addr + ((phys_addr_t)n << PAGE_SHIFT))
			return p + n;
	}
	return kpi_pfn_to_page((p->phys_addr >> PAGE_SHIFT) + n);
}

/*
 * vmap: pages that lie one after the other in a chunk are mapped
 * already; others get an alias segment (segment(3)) made of them
 */
struct vmapping {
	struct list_head	node;
	void	*va;
	char	name[32];	/* the alias segment; "": the pages lie one after the other */
	struct page	**pages;
	unsigned int	count;
	unsigned long	flags;	/* VM_MAP_PUT_PAGES: vfree puts the pages, frees pages */
};
static LIST_HEAD(vmappings);
static int nvmaps;

/*
 * an alias segment (segment(3)) named name of pages of the pool, page i
 * at i*PAGE_SIZE: its address; attached here if attach, else only made
 * (an object for a client: the file server attaches none of them)
 */
void *
kpi_alias_pages(const char *name, struct page **pages, unsigned int count, bool attach)
{
	unsigned long long *off;
	const char **src;
	struct chunk *c;
	unsigned int i;
	void *va = NULL;

	src = kmalloc_array(count, sizeof(*src), GFP_KERNEL);
	off = kmalloc_array(count, sizeof(*off), GFP_KERNEL);
	if (!src || !off)
		goto out;
	for (i = 0; i < count; i++) {
		c = pages[i]->kpi_chunk;
		if (!c) {
			pr_warn("alias %s: page %u not of the pool\n", name, i);
			goto out;
		}
		src[i] = c->name;
		off[i] = (u8 *)pages[i]->virtual - c->va;
		/* the kernel takes whole pages of the chunk only (segment(3)) */
		if (off[i] & (PAGE_SIZE - 1) || off[i] >= CHUNK) {
			pr_warn("alias %s: page %u (%p) at %p: not a page of chunk %s (%p)\n",
			    name, i, pages[i], pages[i]->virtual, c->name, c->va);
			goto out;
		}
	}
	va = p9_segalias(name, count, src, off, attach);
out:
	kfree(src);
	kfree(off);
	return va;
}

/*
 * vmap: pages that lie one after the other in a chunk are mapped
 * already, others get an alias segment (segment(3)) made of them.  Each
 * mapping is recorded, so that vunmap and vfree find it, as Linux's
 * vmap areas: with VM_MAP_PUT_PAGES the mapping owns the pages and the
 * array, and vfree gives them back
 */
void *
vmap(struct page **pages, unsigned int count, unsigned long flags, pgprot_t prot)
{
	struct vmapping *m;
	unsigned int i;

	if (!count)
		return NULL;
	m = kzalloc(sizeof(*m), GFP_KERNEL);
	if (!m)
		return NULL;
	m->pages = pages;
	m->count = count;
	m->flags = flags;
	for (i = 1; i < count; i++)
		if ((u8 *)pages[i]->virtual != (u8 *)pages[0]->virtual + i * PAGE_SIZE)
			break;
	if (i == count)
		m->va = pages[0]->virtual;
	else {
		p9_qlock(plock);
		snprintf(m->name, sizeof(m->name), "kpi.%d.v%d", p9_getpid(), nvmaps++);
		p9_qunlock(plock);
		m->va = kpi_alias_pages(m->name, pages, count, true);
		if (!m->va) {
			pr_warn("vmap: %u pages: no alias segment\n", count);
			kfree(m);
			return NULL;
		}
	}
	p9_qlock(plock);
	list_add(&m->node, &vmappings);
	p9_qunlock(plock);
	return m->va;
}

/*
 * the mapping va is in, as Linux finds the vmap area of an address: a
 * caller may free a pointer into it (i915's selftest_lrc frees its
 * mapping of a context image past the first page); the mapping that
 * begins there first (two vmaps of the same pages begin at one place)
 */
static struct vmapping *
takevmap(const void *va)
{
	struct vmapping *m, *found = NULL;
	const u8 *p = va;

	p9_qlock(plock);
	list_for_each_entry(m, &vmappings, node)
		if (m->va == va) {
			found = m;
			break;
		}
	if (!found)
		list_for_each_entry(m, &vmappings, node)
			if (p > (u8 *)m->va && p < (u8 *)m->va + (size_t)m->count * PAGE_SIZE) {
				found = m;
				break;
			}
	if (found)
		list_del(&found->node);
	p9_qunlock(plock);
	return found;
}

static void
unmapvmap(struct vmapping *m)
{
	/* the alias segment is revoked in every proc that had it (segment(3)) */
	if (m->name[0])
		p9_segfree(m->name, m->va);
}

void
vunmap(const void *va)
{
	struct vmapping *m = takevmap(va);

	if (m) {
		unmapvmap(m);
		kfree(m);
	}
}

/* vmalloc'd memory (the heap), or a vmap mapping (Linux's vfree takes both) */
void
vfree(const void *va)
{
	struct vmapping *m;
	unsigned int i;

	if (!va)
		return;
	m = takevmap(va);
	if (!m) {
		kfree(va);
		return;
	}
	unmapvmap(m);
	if (m->flags & VM_MAP_PUT_PAGES) {
		for (i = 0; i < m->count; i++)
			put_page(m->pages[i]);
		kvfree(m->pages);
	}
	kfree(m);
}

static unsigned long long memtotal, memfree;

void
si_meminfo(struct sysinfo *i)
{
	if (p9_meminfo(&memtotal, &memfree) < 0) {
		memtotal = 1ULL << 32;
		memfree = memtotal / 2;
	}
	i->totalram = memtotal >> PAGE_SHIFT;
	i->freeram = memfree >> PAGE_SHIFT;
	i->totalhigh = 0;
	i->mem_unit = PAGE_SIZE;
}


int kpi_wbinvd(void) { return p9_wbinvd(); }

/* 9front: the one memory node (NODE_DATA) */
#include <linux/mmzone.h>
pg_data_t kpi_node0;

/*
 * unmap_mapping_range: the CPU's mappings of the device's mmap offsets
 * [start, start+len) go (TTM, before it moves an object); the driver
 * shows objects to clients as pager segments (segment(3)) and takes
 * their pages back here
 */
void	(*kpi_unmap_range)(unsigned long long start, unsigned long long len);

void
unmap_mapping_range(struct address_space *m, loff_t start, loff_t len, int even_cows)
{
	(void)m;
	(void)even_cows;
	if(kpi_unmap_range != NULL)
		kpi_unmap_range(start, len);
}
