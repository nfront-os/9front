// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the library functions of Linux's lib/ that the
 * drivers use and Plan 9's libc lacks (ours: Linux's are GPL): strings,
 * number parsing, bitmaps, bit search, sorting, lock-free lists, uuid,
 * hex dumps, random numbers.
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/sort.h>
#include <linux/list.h>
#include <linux/llist.h>
#include <linux/seq_buf.h>
#include <linux/uuid.h>
#include <linux/printk.h>
#include <linux/random.h>
#include <linux/prandom.h>
#include <linux/crc32.h>
#include <linux/math.h>
#include <linux/prime_numbers.h>
#include <linux/fault-inject.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>
#include <linux/ratelimit.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include <kpi/p9.h>

/* strings */
size_t
strnlen(const char *s, size_t n)
{
	const char *p = s;

	while (n-- && *p)
		p++;
	return p - s;
}



/* 9front's libc has no strlcpy, strlcat */
size_t
strlcpy(char *d, const char *s, size_t n)
{
	size_t l = strlen(s);

	if (n) {
		n = min(l, n - 1);
		memcpy(d, s, n);
		d[n] = 0;
	}
	return l;
}

size_t
strlcat(char *d, const char *s, size_t n)
{
	size_t dl = strnlen(d, n);

	if (dl == n)
		return n + strlen(s);
	return dl + strlcpy(d + dl, s, n - dl);
}

int
strcasecmp(const char *a, const char *b)
{
	int c, d;

	do {
		c = tolower((unsigned char)*a++);
		d = tolower((unsigned char)*b++);
	} while (c == d && c);
	return c - d;
}

int
strncasecmp(const char *a, const char *b, size_t n)
{
	int c = 0, d = 0;

	while (n--) {
		c = tolower((unsigned char)*a++);
		d = tolower((unsigned char)*b++);
		if (c != d || !c)
			break;
	}
	return c - d;
}

char *
strchrnul(const char *s, int c)
{
	for (; *s != '\0' && *s != (char)c; s++)
		;
	return (char *)s;
}

char *
strnchr(const char *s, size_t n, int c)
{
	for (; n-- && *s; s++)
		if (*s == (char)c)
			return (char *)s;
	return NULL;
}

char *
strnstr(const char *s, const char *t, size_t n)
{
	size_t l = strlen(t);

	if (!l)
		return (char *)s;
	while (n >= l) {
		if (!memcmp(s, t, l))
			return (char *)s;
		if (!*s)
			break;
		n--;
		s++;
	}
	return NULL;
}

char *
strsep(char **sp, const char *delim)
{
	char *s = *sp, *e;

	if (!s)
		return NULL;
	e = strpbrk(s, delim);
	if (e)
		*e++ = 0;
	*sp = e;
	return s;
}

char *
skip_spaces(const char *s)
{
	while (isspace((unsigned char)*s))
		s++;
	return (char *)s;
}

char *
strim(char *s)
{
	size_t n = strlen(s);
	char *e;

	if (!n)
		return s;
	e = s + n - 1;
	while (e >= s && isspace((unsigned char)*e))
		e--;
	e[1] = 0;
	return skip_spaces(s);
}

char *
strreplace(char *s, char old, char new)
{
	for (; *s; s++)
		if (*s == old)
			*s = new;
	return s;
}


int
sysfs_streq(const char *a, const char *b)
{
	while (*a && *a == *b) {
		a++;
		b++;
	}
	if (*a == *b)
		return 1;
	if (!*a && *b == '\n' && !b[1])
		return 1;
	if (!*b && *a == '\n' && !a[1])
		return 1;
	return 0;
}


void *
memscan(void *addr, int c, size_t n)
{
	u8 *p = addr;

	while (n && *p != (u8)c) {
		p++;
		n--;
	}
	return p;
}

void *memset16(u16 *s, u16 v, size_t n) { u16 *p = s; while (n--) *p++ = v; return s; }



char *
kstrndup(const char *s, size_t max, gfp_t gfp)
{
	size_t n;
	char *d;

	if (!s)
		return NULL;
	n = strnlen(s, max);
	d = kmalloc(n + 1, gfp);
	if (d) {
		memcpy(d, s, n);
		d[n] = 0;
	}
	return d;
}


char *
kmemdup_nul(const char *s, size_t max, gfp_t gfp)
{
	char *d;

	if (!s)
		return NULL;
	d = kmalloc(max + 1, gfp);
	if (d) {
		memcpy(d, s, max);
		d[max] = 0;
	}
	return d;
}

/* the file server's requests are in its own memory: "user" is ours, within the proc's user window */
void *
memdup_user(const void __user *src, size_t len)
{
	void *p = kmalloc(len, GFP_KERNEL);

	if (!p)
		return ERR_PTR(-ENOMEM);
	if (copy_from_user(p, src, len)) {
		kfree(p);
		return ERR_PTR(-EFAULT);
	}
	return p;
}

void *
memdup_user_nul(const void __user *src, size_t len)
{
	char *p = kmalloc(len + 1, GFP_KERNEL);

	if (!p)
		return ERR_PTR(-ENOMEM);
	if (copy_from_user(p, src, len)) {
		kfree(p);
		return ERR_PTR(-EFAULT);
	}
	p[len] = 0;
	return p;
}

void *vmemdup_user(const void __user *src, size_t len) { return memdup_user(src, len); }

char **
argv_split(gfp_t gfp, const char *str, int *argcp)
{
	char *copy, *p, **argv;
	int n = 0, i;

	copy = kstrdup(str, gfp);
	if (!copy)
		return NULL;
	for (p = copy; *(p = skip_spaces(p)); n++)
		while (*p && !isspace((unsigned char)*p))
			p++;
	argv = kcalloc(n + 2, sizeof(*argv), gfp);
	if (!argv) {
		kfree(copy);
		return NULL;
	}
	/* argv[n + 1] keeps the copy for argv_free */
	argv[n + 1] = copy;
	for (p = copy, i = 0; *(p = skip_spaces(p)); i++) {
		argv[i] = p;
		while (*p && !isspace((unsigned char)*p))
			p++;
		if (*p)
			*p++ = 0;
	}
	argv[n] = NULL;
	if (argcp)
		*argcp = n;
	return argv;
}

void
argv_free(char **argv)
{
	char **p;

	if (!argv)
		return;
	for (p = argv; *p; p++)
		;
	kfree(p[1]);
	kfree(argv);
}

/* numbers */
static int
digit(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	c = tolower(c);
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 10;
	return 99;
}

static const char *
base_prefix(const char *s, unsigned int *base)
{
	if (*base == 0) {
		if (s[0] == '0') {
			if (tolower((unsigned char)s[1]) == 'x' && isxdigit((unsigned char)s[2]))
				*base = 16;
			else
				*base = 8;
		} else
			*base = 10;
	}
	if (*base == 16 && s[0] == '0' && tolower((unsigned char)s[1]) == 'x')
		s += 2;
	return s;
}

/* digits of s in base: the value, the end, -ERANGE on overflow */
static int
parse_ull(const char *s, unsigned int base, unsigned long long *res, const char **end)
{
	unsigned long long v = 0;
	int d, any = 0, over = 0;

	s = base_prefix(s, &base);
	while ((d = digit((unsigned char)*s)) < (int)base) {
		if (v > (ULLONG_MAX - d) / base)
			over = 1;
		v = v * base + d;
		s++;
		any = 1;
	}
	*end = s;
	*res = v;
	if (!any)
		return -EINVAL;
	return over ? -ERANGE : 0;
}

int
kstrtoull(const char *s, unsigned int base, unsigned long long *res)
{
	const char *e;
	int r;

	if (*s == '+')
		s++;
	r = parse_ull(s, base, res, &e);
	if (r)
		return r;
	if (*e == '\n')
		e++;
	return *e ? -EINVAL : 0;
}

int
kstrtoll(const char *s, unsigned int base, long long *res)
{
	unsigned long long v;
	int r;

	if (*s == '-') {
		r = kstrtoull(s + 1, base, &v);
		if (r)
			return r;
		if ((long long)-v > 0)
			return -ERANGE;
		*res = -v;
		return 0;
	}
	r = kstrtoull(s, base, &v);
	if (r)
		return r;
	if ((long long)v < 0)
		return -ERANGE;
	*res = v;
	return 0;
}

#define KSTRTO_U(name, type, max) \
int name(const char *s, unsigned int base, type *res) \
{ unsigned long long v; int r = kstrtoull(s, base, &v); if (r) return r; if (v > (max)) return -ERANGE; *res = v; return 0; }
#define KSTRTO_S(name, type, min, max) \
int name(const char *s, unsigned int base, type *res) \
{ long long v; int r = kstrtoll(s, base, &v); if (r) return r; if (v < (min) || v > (max)) return -ERANGE; *res = v; return 0; }
KSTRTO_U(kstrtoul, unsigned long, ULONG_MAX)
KSTRTO_U(kstrtouint, unsigned int, UINT_MAX)
KSTRTO_U(kstrtou32, u32, U32_MAX)
KSTRTO_U(kstrtou16, u16, U16_MAX)
KSTRTO_U(kstrtou8, u8, U8_MAX)
KSTRTO_S(kstrtol, long, LONG_MIN, LONG_MAX)
KSTRTO_S(kstrtoint, int, INT_MIN, INT_MAX)
KSTRTO_S(kstrtos32, s32, S32_MIN, S32_MAX)

int
kstrtobool(const char *s, bool *res)
{
	if (!s)
		return -EINVAL;
	switch (s[0]) {
	case 'y': case 'Y': case 't': case 'T': case '1':
		*res = true;
		return 0;
	case 'n': case 'N': case 'f': case 'F': case '0':
		*res = false;
		return 0;
	case 'o': case 'O':
		if (tolower((unsigned char)s[1]) == 'n') {
			*res = true;
			return 0;
		}
		if (tolower((unsigned char)s[1]) == 'f') {
			*res = false;
			return 0;
		}
	}
	return -EINVAL;
}

#define FROM_USER(name, call, ...) \
{ char buf[24]; size_t n = min(count, sizeof(buf) - 1); memcpy(buf, (const void *)s, n); buf[n] = 0; return call(buf, ##__VA_ARGS__); }
int kstrtoint_from_user(const char __user *s, size_t count, unsigned int base, int *res) FROM_USER(x, kstrtoint, base, res)
int kstrtouint_from_user(const char __user *s, size_t count, unsigned int base, unsigned int *res) FROM_USER(x, kstrtouint, base, res)
int kstrtoull_from_user(const char __user *s, size_t count, unsigned int base, unsigned long long *res) FROM_USER(x, kstrtoull, base, res)
int kstrtobool_from_user(const char __user *s, size_t count, bool *res) FROM_USER(x, kstrtobool, res)

unsigned long long
simple_strtoull(const char *s, char **endp, unsigned int base)
{
	unsigned long long v;
	const char *e;

	parse_ull(s, base, &v, &e);
	if (endp)
		*endp = (char *)e;
	return v;
}

unsigned long simple_strtoul(const char *s, char **endp, unsigned int base) { return simple_strtoull(s, endp, base); }

long long
simple_strtoll(const char *s, char **endp, unsigned int base)
{
	if (*s == '-')
		return -(long long)simple_strtoull(s + 1, endp, base);
	return simple_strtoull(s, endp, base);
}

long
simple_strtol(const char *s, char **endp, unsigned int base)
{
	if (*s == '-')
		return -(long)simple_strtoull(s + 1, endp, base);
	return simple_strtoull(s, endp, base);
}

int
num_to_str(char *buf, int size, unsigned long long num, unsigned int width)
{
	char tmp[24];
	int n = 0, i;

	do {
		tmp[n++] = '0' + num % 10;
		num /= 10;
	} while (num);
	while ((unsigned int)n < width && n < (int)sizeof(tmp))
		tmp[n++] = ' ';
	if (n > size)
		return 0;
	for (i = 0; i < n; i++)
		buf[i] = tmp[n - 1 - i];
	return n;
}

/* bit search */
unsigned long
_find_next_bit(const unsigned long *a, unsigned long size, unsigned long off)
{
	unsigned long w;

	if (off >= size)
		return size;
	w = a[off / BITS_PER_LONG] & (~0UL << (off % BITS_PER_LONG));
	off &= ~(BITS_PER_LONG - 1UL);
	while (!w) {
		off += BITS_PER_LONG;
		if (off >= size)
			return size;
		w = a[off / BITS_PER_LONG];
	}
	return min(off + __builtin_ctzl(w), size);
}

unsigned long
_find_next_zero_bit(const unsigned long *a, unsigned long size, unsigned long off)
{
	unsigned long w;

	if (off >= size)
		return size;
	w = ~a[off / BITS_PER_LONG] & (~0UL << (off % BITS_PER_LONG));
	off &= ~(BITS_PER_LONG - 1UL);
	while (!w) {
		off += BITS_PER_LONG;
		if (off >= size)
			return size;
		w = ~a[off / BITS_PER_LONG];
	}
	return min(off + __builtin_ctzl(w), size);
}

unsigned long _find_first_bit(const unsigned long *a, unsigned long size) { return _find_next_bit(a, size, 0); }
unsigned long _find_first_zero_bit(const unsigned long *a, unsigned long size) { return _find_next_zero_bit(a, size, 0); }

unsigned long
_find_last_bit(const unsigned long *a, unsigned long size)
{
	unsigned long i;

	for (i = size; i-- > 0;)
		if (test_bit(i, a))
			return i;
	return size;
}

/* bitmaps */



bool bitmap_full(const unsigned long *m, unsigned int n) { return _find_first_zero_bit(m, n) >= n; }

#define EACHWORD(i, n)	for (i = 0; i < BITS_TO_LONGS(n); i++)
#define LASTMASK(i, n)	((i) == BITS_TO_LONGS(n) - 1 ? BITMAP_LAST_WORD_MASK(n) : ~0UL)

bool
bitmap_equal(const unsigned long *a, const unsigned long *b, unsigned int n)
{
	unsigned int i;

	EACHWORD(i, n)
		if ((a[i] ^ b[i]) & LASTMASK(i, n))
			return false;
	return true;
}


bool
bitmap_subset(const unsigned long *a, const unsigned long *b, unsigned int n)
{
	unsigned int i;

	EACHWORD(i, n)
		if (a[i] & ~b[i] & LASTMASK(i, n))
			return false;
	return true;
}


void
bitmap_xor(unsigned long *d, const unsigned long *a, const unsigned long *b, unsigned int n)
{
	unsigned int i;

	EACHWORD(i, n)
		d[i] = a[i] ^ b[i];
}


bool
bitmap_and(unsigned long *d, const unsigned long *a, const unsigned long *b, unsigned int n)
{
	unsigned long r = 0;
	unsigned int i;

	EACHWORD(i, n)
		r |= (d[i] = a[i] & b[i]) & LASTMASK(i, n);
	return r != 0;
}


void
bitmap_shift_right(unsigned long *d, const unsigned long *s, unsigned int shift, unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; i++)
		if (i + shift < n && test_bit(i + shift, s))
			__set_bit(i, d);
		else
			__clear_bit(i, d);
}

void
bitmap_shift_left(unsigned long *d, const unsigned long *s, unsigned int shift, unsigned int n)
{
	unsigned int i;

	for (i = n; i-- > 0;)
		if (i >= shift && test_bit(i - shift, s))
			__set_bit(i, d);
		else
			__clear_bit(i, d);
}


unsigned long *bitmap_alloc(unsigned int n, gfp_t gfp) { return kmalloc_array(BITS_TO_LONGS(n), sizeof(long), gfp); }

/* sort, sort_r, list_sort: OpenBSD's (linux_sort.c, linux_sort_r.c, linux_list_sort.c) */

/* lock-free lists */

struct llist_node *
llist_reverse_order(struct llist_node *h)
{
	struct llist_node *r = NULL, *n;

	while (h) {
		n = h->next;
		h->next = r;
		r = h;
		h = n;
	}
	return r;
}

/* hex */
const char hex_asc[17] = "0123456789abcdef";
const char hex_asc_upper[17] = "0123456789ABCDEF";

int
hex_to_bin(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	c = tolower(c);
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return -1;
}

int
hex2bin(u8 *dst, const char *src, size_t count)
{
	int hi, lo;

	while (count--) {
		hi = hex_to_bin(*src++);
		if (hi < 0)
			return -EINVAL;
		lo = hex_to_bin(*src++);
		if (lo < 0)
			return -EINVAL;
		*dst++ = hi << 4 | lo;
	}
	return 0;
}

char *
bin2hex(char *dst, const void *src, size_t count)
{
	const u8 *p = src;

	while (count--)
		dst = hex_byte_pack(dst, *p++);
	return dst;
}

/* uuid */
bool
uuid_is_valid(const char *u)
{
	int i;

	/* 8-4-4-4-12 hex digits */
	for (i = 0; i < 36; i++)
		switch (i) {
		case 8: case 13: case 18: case 23:
			if (u[i] != '-')
				return false;
			break;
		default:
			if (!isxdigit((unsigned char)u[i]))
				return false;
		}
	return true;
}

/* one line of a hex dump, as Linux formats it */
int
hex_dump_to_buffer(const void *buf, size_t len, int rowsize, int groupsize, char *linebuf, size_t linebuflen, bool ascii)
{
	const u8 *p = buf;
	size_t i, j, n = 0, ngroups;
	int k;
	u64 v;

	if (rowsize != 16 && rowsize != 32)
		rowsize = 16;
	if (len > (size_t)rowsize)
		len = rowsize;
	if (groupsize != 1 && groupsize != 2 && groupsize != 4 && groupsize != 8)
		groupsize = 1;
	if (len % groupsize)
		groupsize = 1;
	ngroups = len / (size_t)groupsize;
	if (!linebuflen)
		return 0;
	linebuf[0] = 0;
	for (i = 0; i < ngroups; i++) {
		v = 0;
		for (k = groupsize - 1; k >= 0; k--)
			v = v << 8 | p[i * groupsize + k];
		n += snprintf(linebuf + min(n, linebuflen), n < linebuflen ? linebuflen - n : 0,
			"%s%0*llx", i ? " " : "", groupsize * 2, (unsigned long long)v);
	}
	if (ascii) {
		j = rowsize * 2 + rowsize / groupsize + 1;
		while (n < j) {
			if (n + 1 < linebuflen)
				linebuf[n] = ' ';
			n++;
		}
		for (i = 0; i < len; i++, n++)
			if (n + 1 < linebuflen)
				linebuf[n] = isprint(p[i]) ? p[i] : '.';
		if (linebuflen)
			linebuf[min(n, linebuflen - 1)] = 0;
	}
	return n;
}

/* random numbers from the kernel's (truerand) */



/*
 * prandom: the Tausworthe generator Linux's prandom uses (P. L'Ecuyer,
 * "Tables of maximally equidistributed combined LFSR generators", 1999:
 * LFSR113), with a state per user; the seed makes the four components
 * valid (each above its minimum)
 */
static u32
taus(u32 s, int a, int b, u32 mask, int c)
{
	return ((s & mask) << c) ^ (((s << a) ^ s) >> b);
}

u32
prandom_u32_state(struct rnd_state *st)
{
	st->s1 = taus(st->s1, 6, 13, 4294967294U, 18);
	st->s2 = taus(st->s2, 2, 27, 4294967288U, 2);
	st->s3 = taus(st->s3, 13, 21, 4294967280U, 7);
	st->s4 = taus(st->s4, 3, 12, 4294967168U, 13);
	return st->s1 ^ st->s2 ^ st->s3 ^ st->s4;
}

void
prandom_bytes_state(struct rnd_state *st, void *buf, size_t n)
{
	u8 *p = buf;
	u32 r;

	while (n >= 4) {
		r = prandom_u32_state(st);
		memcpy(p, &r, 4);
		p += 4;
		n -= 4;
	}
	if (n) {
		r = prandom_u32_state(st);
		memcpy(p, &r, n);
	}
}

static u32
seedpart(u32 x, u32 min)
{
	return x < min ? x + min : x;
}

void
prandom_seed_state(struct rnd_state *st, u64 seed)
{
	u32 i = (u32)(seed ^ seed >> 32 ^ seed << 10);	/* the low 32 bits */
	int k;

	st->s1 = seedpart(i, 2U);
	st->s2 = seedpart(i, 8U);
	st->s3 = seedpart(i, 16U);
	st->s4 = seedpart(i, 128U);
	/* away from the seed's simple pattern */
	for (k = 0; k < 10; k++)
		prandom_u32_state(st);
}

/* CRC-32 (IEEE 802.3), reflected, bit by bit */
u32
crc32_le(u32 crc, const void *p, size_t len)
{
	const u8 *b = p;
	int k;

	while (len--) {
		crc ^= *b++;
		for (k = 0; k < 8; k++)
			crc = (crc >> 1) ^ (0xEDB88320U & -(crc & 1));
	}
	return crc;
}

/* primes, by trial division */
bool
is_prime_number(unsigned long x)
{
	unsigned long d;

	if (x < 2)
		return false;
	if (x < 4)
		return true;
	if (!(x & 1))
		return false;
	for (d = 3; d <= x / d; d += 2)
		if (x % d == 0)
			return false;
	return true;
}

unsigned long
next_prime_number(unsigned long x)
{
	if (x < 2)
		return 2;
	for (x++; !is_prime_number(x); x++)
		;
	return x;
}

/* fault injection for the selftests (linux/fault-inject.h) */
bool
should_fail(struct fault_attr *a, ssize_t size)
{
	if (atomic_read(&a->times) == 0)
		return false;
	if (atomic_read(&a->space) > size) {
		atomic_sub(size, &a->space);
		return false;
	}
	if (a->interval > 1 && ++a->count % a->interval)
		return false;
	if (a->probability <= get_random_u32_below(100))
		return false;
	if (atomic_read(&a->times) != -1)
		atomic_dec(&a->times);
	return true;
}

/* the integer square root, rounded down (bit by bit) */
unsigned long
int_sqrt(unsigned long x)
{
	unsigned long r = 0, b = 1UL << (BITS_PER_LONG - 2);

	while (b > x)
		b >>= 2;
	while (b) {
		if (x >= r + b) {
			x -= r + b;
			r = (r >> 1) + b;
		} else
			r >>= 1;
		b >>= 2;
	}
	return r;
}


/* 1 if one more may go in this interval (at most burst), else 0; interval 0: no limit */
int
__ratelimit(struct ratelimit_state *r)
{
	unsigned long flags;
	int ok;

	if (r->interval == 0)
		return 1;
	raw_spin_lock_irqsave(&r->lock, flags);
	if (r->begin == 0 || time_after(jiffies, r->begin + r->interval)) {
		r->begin = jiffies;
		r->printed = 0;
		r->missed = 0;
	}
	ok = r->burst == 0 || r->printed < r->burst;
	if (ok)
		r->printed++;
	else
		r->missed++;
	raw_spin_unlock_irqrestore(&r->lock, flags);
	return ok;
}

/* a seq_buf: text appended while it fits; overflowed once it did not */
void
seq_buf_printf(struct seq_buf *s, const char *fmt, ...)
{
	va_list ap;
	int n;

	if (s->pos >= s->size) {
		s->overflowed = true;
		return;
	}
	va_start(ap, fmt);
	n = vsnprintf(s->buf + s->pos, s->size - s->pos, fmt, ap);
	va_end(ap);
	if (n < 0 || s->pos + n >= s->size) {
		s->pos = s->size - 1;
		s->overflowed = true;
	} else
		s->pos += n;
}

void
seq_buf_puts(struct seq_buf *s, const char *str)
{
	seq_buf_printf(s, "%s", str);
}
