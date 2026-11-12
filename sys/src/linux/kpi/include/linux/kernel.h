/* Public domain. */

#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/stdarg.h>
#include <sys/malloc.h>

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/bitops.h>
#include <linux/log2.h>
#include <linux/linkage.h>
#include <linux/printk.h>
#include <linux/typecheck.h>
#include <linux/container_of.h>
#include <linux/stddef.h>
#include <linux/align.h>
#include <linux/math.h>
#include <linux/limits.h>
#include <asm/byteorder.h>
#include <linux/wordpart.h>
#include <linux/jump_label.h>	/* 9front: static keys, as Linux's headers bring them */

#define swap(a, b) \
	do { __typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while(0)

#define ARRAY_SIZE nitems

/* 9front: scnprintf is a function with Linux's result, what it wrote (kpi/linux/printf.c) */
int	scnprintf(char *, size_t, const char *, ...) __attribute__((__format__(__printf__, 3, 4)));

#define min_t(t, a, b) ({ \
	t __min_a = (a); \
	t __min_b = (b); \
	__min_a < __min_b ? __min_a : __min_b; })

#define max_t(t, a, b) ({ \
	t __max_a = (a); \
	t __max_b = (b); \
	__max_a > __max_b ? __max_a : __max_b; })

#define MIN_T(t, a, b) min_t(t, a, b)
#define MAX_T(t, a, b) max_t(t, a, b)

#define clamp_t(t, x, a, b) min_t(t, max_t(t, x, a), b)
#define clamp(x, a, b) clamp_t(__typeof(x), x, a, b)
#define clamp_val(x, a, b) clamp_t(__typeof(x), x, a, b)

#define min(a, b) MIN(a, b)
#define max(a, b) MAX(a, b)
#define min3(x, y, z) MIN(x, MIN(y, z))
#define max3(x, y, z) MAX(x, MAX(y, z))

#define min_not_zero(a, b) (a == 0) ? b : ((b == 0) ? a : min(a, b))

static inline char *
kvasprintf(int flags, const char *fmt, va_list ap)
{
	char *buf;
	size_t len;
	va_list vl;

	va_copy(vl, ap);
	len = vsnprintf(NULL, 0, fmt, vl);
	va_end(vl);

	buf = kpi_malloc(len + 1, M_DRM, flags);
	if (buf) {
		vsnprintf(buf, len + 1, fmt, ap);
	}

	return buf;
}

static inline char *
kasprintf(int flags, const char *fmt, ...)
{
	char *buf;
	va_list ap;

	va_start(ap, fmt);
	buf = kvasprintf(flags, fmt, ap);
	va_end(ap);

	return buf;
}

static inline int
vscnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
	int nc;

	nc = vsnprintf(buf, size, fmt, ap);
	if (nc > (size - 1))
		return (size - 1);
	else
		return nc;
}
/* 9front: from OpenBSD 6.18 */
#include <linux/array_size.h>
#include <linux/stringify.h>
#include <linux/sprintf.h>
#include <linux/minmax.h>
#include <linux/panic.h>

#define might_sleep()		assertwaitok()
#define might_sleep_if(x)	do {	\
	if (x)				\
		assertwaitok();		\
} while (0)
#define might_fault()

#define u64_to_user_ptr(x)	((void *)(uintptr_t)(x))

#define _RET_IP_		__builtin_return_address(0)
#define _THIS_IP_		0

#define STUB() do { printf("%s: stub\n", __func__); } while(0)

#define PTR_IF(c, p)		((c) ? (p) : NULL)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/stdarg.h>
#include <linux/bits.h>
#include <linux/build_bug.h>
#include <linux/stringify.h>
#include <linux/bug.h>
#include <linux/err.h>
#include <linux/math64.h>
#include <linux/string.h>
#include <linux/swab.h>
#include <linux/string_helpers.h>
#include <linux/ratelimit.h>
#define __ALIGN_KERNEL(x, a)	__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, m)	(((x) + (m)) & ~(m))
#define PTR_ALIGN_DOWN(p, a)	((__typeof__(p))ALIGN_DOWN((uintptr_t)(p), (a)))
#define abs(x)			({ typeof(x) __x = (x); __x < 0 ? -__x : __x; })
#define abs_diff(a, b)		({ typeof(a) __a = (a); typeof(b) __b = (b); __a > __b ? __a - __b : __b - __a; })
#define upper_16_bits(n)	((u16)(((n) >> 16) & 0xffff))
#define lower_16_bits(n)	((u16)((n) & 0xffffU))
#define cant_sleep()		do { } while (0)
#define sched_annotate_sleep()	do { } while (0)
#define in_softirq()		0
#define preemptible()		1
#define typecheck_fn(type, f)	({ typeof(type) __tmp = f; (void)__tmp; })
#define SIZE_ADD(a, b)		((a) + (b))
#define STACK_MAGIC		0xdeadbeef
#define REPEAT_BYTE(x)		((unsigned long)0x0101010101010101ULL * (x))	/* 9front: x in each byte of a long */
#define swap_ptr(a, b)		swap(a, b)
int	kstrtoint(const char *s, unsigned int base, int *res);
int	kstrtouint(const char *s, unsigned int base, unsigned int *res);
int	kstrtol(const char *s, unsigned int base, long *res);
int	kstrtoul(const char *s, unsigned int base, unsigned long *res);
int	kstrtoll(const char *s, unsigned int base, long long *res);
int	kstrtoull(const char *s, unsigned int base, unsigned long long *res);
int	kstrtobool(const char *s, bool *res);
int	kstrtoint_from_user(const char __user *s, size_t count, unsigned int base, int *res);
int	kstrtobool_from_user(const char __user *s, size_t count, bool *res);
int	kstrtouint_from_user(const char __user *s, size_t count, unsigned int base, unsigned int *res);
int	kstrtoull_from_user(const char __user *s, size_t count, unsigned int base, unsigned long long *res);
int	kstrtou8(const char *s, unsigned int base, u8 *res);
int	kstrtou16(const char *s, unsigned int base, u16 *res);
int	kstrtou32(const char *s, unsigned int base, u32 *res);
#define kstrtou64(s, b, r)	kstrtoull(s, b, (unsigned long long *)(r))
int	kstrtos32(const char *s, unsigned int base, s32 *res);
unsigned long	simple_strtoul(const char *s, char **end, unsigned int base);
long	simple_strtol(const char *s, char **end, unsigned int base);
long long	simple_strtoll(const char *s, char **end, unsigned int base);
unsigned long long	simple_strtoull(const char *s, char **end, unsigned int base);
int	sprintf(char *buf, const char *fmt, ...) __printf(2, 3);
const char	*kvasprintf_const(gfp_t gfp, const char *fmt, va_list args);
int	sscanf(const char *buf, const char *fmt, ...);
int	num_to_str(char *s, int n, unsigned long long v, unsigned int width);
void	dump_stack(void);
#define test_taint(f)		0
#define TAINT_DIE		7
extern const char hex_asc[], hex_asc_upper[];
#define hex_asc_lo(x)		hex_asc[(x) & 15]
#define hex_asc_hi(x)		hex_asc[((x) >> 4) & 15]
#define hex_asc_upper_lo(x)	hex_asc_upper[(x) & 15]
#define hex_asc_upper_hi(x)	hex_asc_upper[((x) >> 4) & 15]
static inline char *hex_byte_pack(char *buf, u8 b) { *buf++ = hex_asc_hi(b); *buf++ = hex_asc_lo(b); return buf; }
int	hex_to_bin(unsigned char ch);
int	hex2bin(u8 *dst, const char *src, size_t count);
char	*bin2hex(char *dst, const void *src, size_t count);

/* 9front: the system is always up and running for the drivers */
enum system_states { SYSTEM_BOOTING, SYSTEM_SCHEDULING, SYSTEM_FREEING_INITMEM, SYSTEM_RUNNING, SYSTEM_HALT, SYSTEM_POWER_OFF, SYSTEM_RESTART, SYSTEM_SUSPEND };
#define system_state	SYSTEM_RUNNING

#endif
