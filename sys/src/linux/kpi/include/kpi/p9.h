/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: the narrow interface between the Linux side
 * (the files in kpi/linux, which see only kpi/include) and the Plan 9
 * side (the files in kpi/plan9, which see only Plan 9's headers).  Plain C types
 * only; the storage of Plan 9's Lock, QLock, RWLock and Rendez lives
 * in the Linux structs, sized there (spinlock_t, struct mutex, ...).
 */
#ifndef _KPI_P9_H
#define _KPI_P9_H

/* locks: pointers to the storage of a Lock, QLock or RWLock */
void	p9_lock(void *l);
void	p9_unlock(void *l);
int	p9_canlock(void *l);
int	p9_locked(void *l);
void	p9_qlock(void *q);
void	p9_qunlock(void *q);
int	p9_canqlock(void *q);
int	p9_qlocked(void *q);
void	p9_rlock(void *rw);
void	p9_runlock(void *rw);
void	p9_wlock(void *rw);
void	p9_wunlock(void *rw);
int	p9_canrlock(void *rw);
int	p9_canwlock(void *rw);
int	p9_rwlocked(void *rw);

/* sleep and wakeup: a Rendez with the QLock it sleeps under (held) */
void	p9_rendez_init(void *rendez, void *qlock);
void	p9_rsleep(void *rendez);
int	p9_rsleep_ms(void *rendez, long ms);	/* 0: timed out */
int	p9_rwakeup(void *rendez);
int	p9_rwakeupall(void *rendez);

/* procs */
int	p9_proccreate(void (*fn)(void *), void *arg, unsigned int stack);
int	p9_getpid(void);
void	**p9_procdata(void);	/* a pointer each proc has for itself */
void	p9_sleep(long ms);
void	p9_yield(void);
void	p9_exits(const char *why);

/* time: nanoseconds since some point (monotonic), and since the epoch */
long long	p9_nsec(void);
long long	p9_realnsec(void);
int	p9_ncpus(void);
void	p9_fpu_kernel(void);
void	p9_segclean(void);

/* memory of the file server */
void	*p9_malloc(unsigned long n);
void	*p9_mallocz(unsigned long n);
void	*p9_mallocalign(unsigned long n, unsigned long align);
void	*p9_realloc(void *p, unsigned long n);
void	p9_free(void *p);
unsigned long	p9_msize(void *p);

/*
 * memory for the device: a #g segment of pinned pages, attached to the
 * file server at va; pa[] gets each page's physical address
 */
void	*p9_segalloc(const char *name, unsigned long size, unsigned long long *pa, int contig);
/* contig: the pages physically contiguous, aligned to the size (up to 2 MB): segment(3)'s "sticky contig" */
void	p9_segfree(const char *name, void *va);
/* an alias segment (segment(3)): page i is at offset off[i] of segment src[i]; attached here if attach */
void	*p9_segalias(const char *name, int n, const char **src, unsigned long long *off, int attach);
/* a part of a physical segment (a BAR's) for a client, revoked by p9_segfree: its address */
void	*p9_segphys(const char *name, const char *phys, unsigned long long off, unsigned long long size);
/*
 * 9front-ng pager segments (segment(3)): a queue the driver reads its
 * pagers' faults from ("NAME PAGE" lines), a part of a physical segment
 * without pages whose faults go there, and the segment's ctl ("pages IDX
 * OFF...", "unmap", "nopage"); 0 or -1, the segment's address
 */
int	p9_pagerq(const char *name);
void	*p9_segpager(const char *name, const char *phys, unsigned long long size, const char *queue);
int	p9_segctl(const char *name, const char *fmt, ...);
/* the queue's faults for its driver: a descriptor, then whole "NAME PAGE" lines; 0 at its end */
int	p9_pagerqopen(const char *name);
long	p9_pagerqread(int fd, char *buf, long n);
/* unmap_mapping_range's: CPU mappings of [start, start+len) of the device's mmap offsets go (a TTM move) */
extern void	(*kpi_unmap_range)(unsigned long long start, unsigned long long len);
int	p9_meminfo(unsigned long long *total, unsigned long long *free);
int	p9_wbinvd(void);	/* write back all processors' caches */

/* the device (#G): its PCI address, BARs, configuration space, interrupt */
int	p9_devopen(const char *dir);	/* #G/n; 0 or -1 */
int	p9_pciaddr(int *bus, int *dev, int *fn);
int	p9_pcicfg(int write, int off, void *buf, int n);
void	*p9_bar(int bar, unsigned long long *pa, unsigned long long *size);	/* uncached */
void	*p9_barwc(int bar);	/* the same, write-combining */
int	p9_barseg(int bar, int wc, char *name, int n);	/* its segment's name, for a client; 0 or -1 */
void	*p9_memmap(unsigned long long pa, unsigned long size);	/* #G's "mem" segments: the OpRegion */
int	p9_barinfo(int bar, unsigned long long *pa, unsigned long long *size, int *io);
int	p9_pcicfgat(int bus, int dev, int fn, int write, int off, void *buf, int n);
int	p9_pcieach(int (*fn)(int bus, int dev, int fn, void *arg), void *arg);
int	p9_irqopen(void);	/* the device's interrupt enabled (#G/n/irq open) */
int	p9_irqwait(void);	/* blocks until the device interrupts; -1: gone */
void	p9_irqack(void);	/* unmask it again (INTx) */
int	p9_devid(unsigned int *vendor, unsigned int *device);

/* port I/O (#P) */
unsigned int	p9_in(int width, unsigned long port);
void	p9_out(int width, unsigned long port, unsigned int v);

/* files, output, randomness */
int	p9_readfile(const char *path, void **data, long *size);	/* p9_malloc'd */
void	p9_print(int fd, const char *s, long n);
void	p9_random(void *buf, long n);
void	p9_panic(const char *why) __attribute__((__noreturn__));	/* abort()s */
void	p9_poweroff(int reboot);	/* fshalt [-r], in a process of its own */
const char	*p9_sysname(void);
int	p9_rdmsr(unsigned int msr, unsigned long long *v);	/* #P/msr; 0 or -1 */
/* SMBIOS (#P/smbios, kpi/plan9/p9smbios.c): the first structure of a type */
int	p9_smbios_version(void);	/* major<<8 | minor, or 0: no tables */
const char	*p9_smbios_string(int type, int off);	/* the string its byte at off names, or NULL */
int	p9_smbios_bytes(int type, int off, void *buf, int n);	/* bytes from off; how many */

/*
 * ACPI through 9front's AML interpreter (libaml; kpi/plan9/p9acpi.c),
 * loaded at the first use.  A node is a name of the namespace (nil: the
 * root).  Values are a malloc'd tree, types as Linux's ACPI_TYPE_*.
 */
enum {
	P9_ACPI_ANY = 0, P9_ACPI_INTEGER = 1, P9_ACPI_STRING = 2,
	P9_ACPI_BUFFER = 3, P9_ACPI_PACKAGE = 4, P9_ACPI_REFERENCE = 0x14,
};
struct p9_acpival {
	int	type;
	unsigned long long	integer;	/* INTEGER */
	int	len;	/* STRING (without NUL), BUFFER bytes, PACKAGE elements */
	void	*ptr;	/* STRING, BUFFER */
	struct p9_acpival	*elem;	/* PACKAGE */
	void	*node;	/* REFERENCE */
};
int	p9_acpi_ok(void);	/* the tables loaded: 0, or -1 (no ACPI) */
void	*p9_acpi_walk(void *node, const char *path);	/* a relative name only below node; NULL if none */
void	*p9_acpi_parent(void *node);
int	p9_acpi_name(void *node, char *buf, int n);	/* its full path */
int	p9_acpi_eval(void *node, int nargs, const struct p9_acpival *args, struct p9_acpival **ret);	/* 0, -1 */
void	p9_acpi_free(struct p9_acpival *v);
void	*p9_acpi_pcidev(int bus, int dev, int fn);	/* the device's node (_ADR), or NULL */
int	p9_acpi_hid(void *node, char *buf, int n);	/* a device's _HID as text; 0 or -1 */
int	p9_acpi_devices(int (*fn)(void *node, void *arg), void *arg);	/* every device with a _HID until fn returns nonzero */

#endif
