// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the processor (x86): its identity and features
 * from CPUID, the TSC frequency, cache line flushes.
 */
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/processor.h>
#include <asm/tsc.h>
#include <kpi/p9.h>

struct cpuinfo_x86 boot_cpu_data;
uint32_t cpu_sefflags_ebx;
unsigned int tsc_khz;
int kpi_ncpus = 1;

static void
cpuid(u32 leaf, u32 sub, u32 r[4])
{
	__asm__ volatile("cpuid" : "=a"(r[0]), "=b"(r[1]), "=c"(r[2]), "=d"(r[3]) : "a"(leaf), "c"(sub));
}

static u64
kpi_rdtsc(void)
{
	u32 lo, hi;

	__asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return (u64)hi << 32 | lo;
}

/* Linux's feature words: 0 CPUID 1 EDX, 4 CPUID 1 ECX, 9 CPUID 7 EBX */
static u32 words[10];

/* the cores of the package: AMD: its threads (CPUID 0x80000008) over the threads per core (0x8000001e); Intel: CPUID 4 */
static int
maxcores(u32 max)
{
	u32 r[4], ext, n;

	if (boot_cpu_data.x86_vendor == X86_VENDOR_AMD) {
		cpuid(0x80000000, 0, r);
		ext = r[0];
		if (ext < 0x80000008)
			return 1;
		cpuid(0x80000008, 0, r);
		n = (r[2] & 0xff) + 1;
		if (ext >= 0x8000001e) {
			cpuid(0x8000001e, 0, r);
			n /= ((r[1] >> 8) & 0xff) + 1;
		}
		return n ? n : 1;
	}
	if (max < 4)
		return 1;
	cpuid(4, 0, r);
	return (r[0] >> 26) + 1;
}

void
kpi_cpu_init(void)
{
	u32 r[4], max, fam, model;
	long long t0, t1;
	u64 c0, c1;

	kpi_ncpus = p9_ncpus();
	cpuid(0, 0, r);
	max = r[0];
	boot_cpu_data.x86_vendor = r[1] == 0x68747541 ? X86_VENDOR_AMD : X86_VENDOR_INTEL;	/* "Auth" */
	cpuid(1, 0, r);
	fam = (r[0] >> 8) & 0xf;
	model = (r[0] >> 4) & 0xf;
	if (fam == 0xf)
		fam += (r[0] >> 20) & 0xff;
	if (fam >= 6)
		model |= ((r[0] >> 16) & 0xf) << 4;
	boot_cpu_data.x86 = fam;
	boot_cpu_data.x86_model = model;
	boot_cpu_data.x86_stepping = r[0] & 0xf;
	boot_cpu_data.x86_clflush_size = ((r[1] >> 8) & 0xff) * 8;
	boot_cpu_data.x86_cache_alignment = boot_cpu_data.x86_clflush_size;
	words[0] = r[3];
	words[4] = r[2];
	if (max >= 7) {
		cpuid(7, 0, r);
		words[9] = r[1];
	}
	/* OpenBSD's names for them (machine/cpu.h) */
	cpu_feature = words[0];
	cpu_ecxfeature = words[4];
	cpu_sefflags_ebx = words[9];
	cpu_vendor_amd = boot_cpu_data.x86_vendor == X86_VENDOR_AMD;
	boot_cpu_data.x86_max_cores = maxcores(max);
	/* the TSC against the clock, over 20 ms */
	t0 = p9_nsec();
	c0 = kpi_rdtsc();
	p9_sleep(20);
	c1 = kpi_rdtsc();
	t1 = p9_nsec();
	if (t1 > t0)
		tsc_khz = (c1 - c0) * 1000000ULL / (t1 - t0);
}

int
kpi_boot_cpu_has(int feature)
{
	int w = feature / 32;

	if (!boot_cpu_data.x86_clflush_size)
		kpi_cpu_init();
	if (w < 0 || w >= (int)ARRAY_SIZE(words))
		return 0;
	return (words[w] >> (feature % 32)) & 1;
}

/* model specific registers: #P/msr, on the processor the proc runs on */
int
kpi_rdmsr(u32 msr, u64 *v)
{
	unsigned long long x;

	if (p9_rdmsr(msr, &x) < 0)
		return -EIO;
	*v = x;
	return 0;
}
