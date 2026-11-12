/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: the rest of OpenBSD's sys/systm.h, sys/time.h,
 * uvm and machine/cpu.h that OpenBSD's LinuxKPI headers call: output
 * (printk's, kpi/linux/printf.c), delays, randomness, copies between
 * "user" and "kernel" (one address space here), the clocks, the pages
 * of an address (kpi/linux/page.c), each proc's cpu_info, firmware.
 */
#include "obsd.h"
#include <sys/time.h>
#include <sys/device.h>
#include <sys/reboot.h>
#include <machine/cpu.h>
#include <uvm/uvm_extern.h>
#include <linux/printk.h>
#include <linux/mm.h>
#include <asm/processor.h>

int cold = 1;		/* until kpi_start is done */
int ncpus = 1, ncpusonline = 1;
uint32_t cpu_ecxfeature, cpu_feature;
int cpu_vendor_amd;
unsigned long physmem;
struct uvmexp uvmexp;
int hw_power = 1;	/* on mains (power_supply_is_system_supplied): no battery is watched */

int
vprintf(const char *fmt, va_list ap)
{
	return vprintk(fmt, ap);
}

int
printf(const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vprintk(fmt, ap);
	va_end(ap);
	return n;
}

/* gcc makes some printfs these */
int
puts(const char *s)
{
	return printf("%s\n", s);
}

int
putchar(int c)
{
	return printf("%c", c);
}

void
kpi_assert(const char *t, const char *f, int l, const char *e)
{
	panic("%sassertion \"%s\" failed: file \"%s\", line %d", t, e, f, l);
}

void
db_enter(void)
{
	p9_panic("db_enter");
}

void
assertwaitok(void)
{
	if (curcpu()->ci_inatomic)
		printk(KERN_WARNING "might_sleep in an atomic section\n");
}

/* a busy wait of us microseconds, on the clock */
void
delay(int us)
{
	long long end = p9_nsec() + (long long)us * 1000;

	while (p9_nsec() < end)
		CPU_BUSY_CYCLE();
}

uint32_t
arc4random(void)
{
	uint32_t v;

	p9_random(&v, sizeof(v));
	return v;
}

void
arc4random_buf(void *buf, size_t n)
{
	p9_random(buf, n);
}

uint32_t
arc4random_uniform(uint32_t bound)
{
	uint32_t r, min;

	if (bound < 2)
		return 0;
	min = -bound % bound;	/* 2**32 % bound: reject below it, no bias */
	do
		r = arc4random();
	while (r < min);
	return r % bound;
}

/*
 * a client's ioctls (aux/amdgpu: through its arena) reach only the
 * memory of the window set for the proc that runs them, as Linux's
 * access_ok keeps a process to its own; without a window (the drivers'
 * own calls, with structures on their stacks) everything
 */
void
kpi_user_window(void *start, unsigned long len)
{
	curproc->uwin = (unsigned long)start;
	curproc->uwinlen = len;
}

/*
 * whether the client whose ioctl the proc runs next is privileged (the
 * hostowner): capable() as for it; reset by kpi_user_privileged(1)
 */
void
kpi_user_privileged(int priv)
{
	curproc->unpriv = !priv;
}

int
kpi_user_ok(const void *uaddr, size_t len)
{
	struct task_struct *p = curproc;
	unsigned long a = (unsigned long)uaddr;

	if (p->uwinlen == 0)
		return 1;
	return a >= p->uwin && len <= p->uwinlen && a - p->uwin <= p->uwinlen - len;
}

int
copyin(const void *uaddr, void *kaddr, size_t len)
{
	if (!kpi_user_ok(uaddr, len))
		return EFAULT;
	memcpy(kaddr, uaddr, len);
	return 0;
}

int
copyout(const void *kaddr, void *uaddr, size_t len)
{
	if (!kpi_user_ok(uaddr, len))
		return EFAULT;
	memcpy(uaddr, kaddr, len);
	return 0;
}

int
kcopy(const void *from, void *to, size_t len)
{
	memmove(to, from, len);
	return 0;
}

uint64_t
nsecuptime(void)
{
	return p9_nsec();
}

uint64_t
getnsecuptime(void)
{
	return p9_nsec();
}

void
nanouptime(struct timespec *ts)
{
	NSEC_TO_TIMESPEC(p9_nsec(), ts);
}

void
getnanouptime(struct timespec *ts)
{
	nanouptime(ts);
}

void
microuptime(struct timeval *tv)
{
	NSEC_TO_TIMEVAL(p9_nsec(), tv);
}

void
nanotime(struct timespec *ts)
{
	NSEC_TO_TIMESPEC(p9_realnsec(), ts);
}

void
getnanotime(struct timespec *ts)
{
	nanotime(ts);
}

void
microtime(struct timeval *tv)
{
	NSEC_TO_TIMEVAL(p9_realnsec(), tv);
}

time_t
gettime(void)
{
	return p9_realnsec() / 1000000000LL;
}

time_t
getuptime(void)
{
	return p9_nsec() / 1000000000LL;
}

/* each proc its own cpu_info: the atomic depth is the proc's */
struct cpu_info *
kpi_curcpu(void)
{
	struct task_struct *p = curproc;
	struct cpu_info *ci = p->p_cpu;

	if (ci == NULL) {
		ci = p9_mallocz(sizeof(*ci));
		if (ci == NULL)
			p9_panic("kpi: no memory for a cpu_info");
		ci->ci_feature_flags = cpu_feature;
		ci->ci_feature_sefflags_ebx = boot_cpu_data.x86_clflush_size ? cpu_sefflags_ebx : 0;
		ci->ci_cflushsz = boot_cpu_data.x86_clflush_size;
		p->p_cpu = ci;
	}
	return ci;
}

/* the pages of the file server's memory (kpi/linux/page.c) */
struct page *
uvm_atopg(vaddr_t va)
{
	return kpi_virt_to_page((void *)va);
}

struct page *
PHYS_TO_VM_PAGE(paddr_t pa)
{
	return kpi_pfn_to_page(pa >> PAGE_SHIFT);
}

void
pmap_flush_cache(vaddr_t va, vsize_t len)
{
	vaddr_t end = va + len, line = boot_cpu_data.x86_clflush_size;

	if (line == 0)
		line = 64;
	__asm volatile("mfence" ::: "memory");
	for (va &= ~(line - 1); va < end; va += line)
		__asm volatile("clflush (%0)" :: "r"(va) : "memory");
	__asm volatile("mfence" ::: "memory");
}

/* firmware(9): /lib/firmware/name */
int
loadfirmware(const char *name, unsigned char **bufp, size_t *buflen)
{
	char path[256];
	void *data;
	long size;

	snprintf(path, sizeof(path), "/lib/firmware/%s", name);
	if (p9_readfile(path, &data, &size) < 0)
		return ENOENT;
	*bufp = data;
	*buflen = size;
	return 0;
}

/*
 * orderly_poweroff (a critical temperature), emergency_restart: as
 * Linux, the machine goes down (fshalt, kpi/plan9/p9.c); the driver
 * keeps running the card (its fans, its clocks) until the power is off
 */
__dead void
reboot(int howto)
{
	printk(KERN_EMERG "kpi: the driver asks to %s the machine (%#x)\n",
	    (howto & RB_POWERDOWN) ? "power off" : "reboot", howto);
	p9_poweroff(!(howto & (RB_HALT | RB_POWERDOWN)));
	for (;;)
		p9_sleep(1000);
}
