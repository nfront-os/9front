// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: Linux's printf (ours: lib/vsprintf.c is GPL),
 * with the %p extensions the drivers use: %pe (error name), %ph (hex
 * bytes), %pV (nested format), %p4cc (fourcc), %pa (phys_addr_t),
 * %pR/%pr (resource), %pE (escaped), %pb/%pbl (bitmap), %pU (uuid),
 * %pS/%ps (code address).  printk writes to standard error, one line
 * at a time, with Linux's level prefix as "<n>".
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <asm/page.h>
#include <linux/ctype.h>
#include <linux/bitmap.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/printk.h>
#include <linux/bug.h>
#include <kpi/p9.h>

int oops_in_progress;

#define E(n)	{ n, #n }
static const struct { int n; const char *s; } errnames[] = {
	E(E2BIG),
	E(EACCES),
	E(EADDRINUSE),
	E(EAGAIN),
	E(EALREADY),
	E(EBADCOOKIE),
	E(EBADE),
	E(EBADF),
	E(EBADHANDLE),
	E(EBADMSG),
	E(EBADSLT),
	E(EBADTYPE),
	E(EBUSY),
	E(ECANCELED),
	E(ECHILD),
	E(ECHRNG),
	E(ECONNABORTED),
	E(ECONNREFUSED),
	E(ECONNRESET),
	E(EDEADLK),
	E(EDOM),
	E(EDQUOT),
	E(EEXIST),
	E(EFAULT),
	E(EFBIG),
	E(EHOSTDOWN),
	E(EHOSTUNREACH),
	E(EHWPOISON),
	E(EIDRM),
	E(EILSEQ),
	E(EINPROGRESS),
	E(EINTR),
	E(EINVAL),
	E(EIO),
	E(EIOCBQUEUED),
	E(EISCONN),
	E(EISDIR),
	E(EJUKEBOX),
	E(EKEYEXPIRED),
	E(ELOOP),
	E(EMFILE),
	E(EMLINK),
	E(EMSGSIZE),
	E(EMULTIHOP),
	E(ENAMETOOLONG),
	E(ENETDOWN),
	E(ENETUNREACH),
	E(ENFILE),
	E(ENOANO),
	E(ENOBUFS),
	E(ENODATA),
	E(ENODEV),
	E(ENOENT),
	E(ENOEXEC),
	E(ENOIOCTLCMD),
	E(ENOKEY),
	E(ENOLCK),
	E(ENOLINK),
	E(ENOMEDIUM),
	E(ENOMEM),
	E(ENOMSG),
	E(ENOPARAM),
	E(ENOPKG),
	E(ENOSPC),
	E(ENOSR),
	E(ENOSTR),
	E(ENOSYS),
	E(ENOTBLK),
	E(ENOTCONN),
	E(ENOTDIR),
	E(ENOTEMPTY),
	E(ENOTRECOVERABLE),
	E(ENOTSOCK),
	E(ENOTSUPP),
	E(ENOTSYNC),
	E(ENOTTY),
	E(ENXIO),
	E(EOPENSTALE),
	E(EOPNOTSUPP),
	E(EOVERFLOW),
	E(EOWNERDEAD),
	E(EPERM),
	E(EPIPE),
	E(EPROTO),
	E(EPROTONOSUPPORT),
	E(ERANGE),
	E(ERECALLCONFLICT),
	E(EREMOTE),
	E(EREMOTEIO),
	E(ERESTARTNOHAND),
	E(ERESTARTNOINTR),
	E(ERESTARTSYS),
	E(ERFKILL),
	E(EROFS),
	E(ESERVERFAULT),
	E(ESHUTDOWN),
	E(ESPIPE),
	E(ESRCH),
	E(ESTALE),
	E(ETIME),
	E(ETIMEDOUT),
	E(ETOOSMALL),
	E(ETXTBSY),
	E(EXDEV),
};
#undef E

struct out {
	char	*buf;
	size_t	size;
	size_t	n;	/* would be written */
};

static void
putch(struct out *o, char c)
{
	if (o->n + 1 < o->size)
		o->buf[o->n] = c;
	o->n++;
}

static void
puts_pad(struct out *o, const char *s, int len, int width, int left)
{
	int i;

	if (!left)
		for (i = len; i < width; i++)
			putch(o, ' ');
	for (i = 0; i < len; i++)
		putch(o, s[i]);
	if (left)
		for (i = len; i < width; i++)
			putch(o, ' ');
}

enum { LEFT = 1, PLUS = 2, SPACE = 4, SPECIAL = 8, ZERO = 16, SIGN = 32, UPPER = 64 };

static void
number(struct out *o, unsigned long long v, int base, int width, int prec, int flags)
{
	const char *digits = (flags & UPPER) ? "0123456789ABCDEF" : "0123456789abcdef";
	char tmp[66], sign = 0;
	int n = 0, len, pad, i;

	if (flags & SIGN) {
		if ((long long)v < 0) {
			sign = '-';
			v = -(long long)v;
		} else if (flags & PLUS)
			sign = '+';
		else if (flags & SPACE)
			sign = ' ';
	}
	if (v == 0 && prec != 0)
		tmp[n++] = '0';
	while (v) {
		tmp[n++] = digits[v % base];
		v /= base;
	}
	if (prec < n)
		prec = n;
	len = prec + (sign != 0);
	if ((flags & SPECIAL) && base == 16)
		len += 2;
	else if ((flags & SPECIAL) && base == 8 && tmp[n - 1] != '0')
		len++, prec++;
	pad = width > len ? width - len : 0;
	if (!(flags & (LEFT | ZERO)))
		for (i = 0; i < pad; i++)
			putch(o, ' ');
	if (sign)
		putch(o, sign);
	if ((flags & SPECIAL) && base == 16) {
		putch(o, '0');
		putch(o, (flags & UPPER) ? 'X' : 'x');
	}
	if ((flags & ZERO) && !(flags & LEFT))
		for (i = 0; i < pad; i++)
			putch(o, '0');
	for (i = n; i < prec; i++)
		putch(o, '0');
	while (n > 0)
		putch(o, tmp[--n]);
	if (flags & LEFT)
		for (i = 0; i < pad; i++)
			putch(o, ' ');
}

static void
hexbytes(struct out *o, const u8 *p, int n, char sep)
{
	int i;

	if (n < 0)
		n = 1;
	if (n > 64)
		n = 64;
	for (i = 0; i < n; i++) {
		if (i && sep)
			putch(o, sep);
		putch(o, hex_asc_hi(p[i]));
		putch(o, hex_asc_lo(p[i]));
	}
}

static int vformat(struct out *o, const char *fmt, va_list ap);

/* %p and its extensions; fmt points after the 'p'; returns the chars used */
static int
pointer(struct out *o, const char *fmt, void *p, int width, int prec, int flags)
{
	const char *f = fmt;
	char tmp[32];
	int i, n;

	switch (*f) {
	case 'e': {
		long e = PTR_ERR(p);

		if (IS_ERR(p))
			for (i = 0; i < (int)ARRAY_SIZE(errnames); i++)
				if (errnames[i].n == -e) {
					putch(o, '-');
					puts_pad(o, errnames[i].s, strlen(errnames[i].s), 0, 0);
					return 1;
				}
		if (IS_ERR(p))
			number(o, e, 10, width, -1, flags | SIGN);
		else
			number(o, (uintptr_t)p, 16, 2 + 2 * sizeof(void *), -1, SPECIAL | ZERO);
		return 1;
	}
	case 'h': {
		char sep = ' ';

		f++;
		if (*f == 'C')
			sep = ':', f++;
		else if (*f == 'D')
			sep = '-', f++;
		else if (*f == 'N')
			sep = 0, f++;
		hexbytes(o, p, width > 0 ? width : 1, sep);
		return f - fmt;
	}
	case 'V': {
		struct va_format *vf = p;
		va_list ap;

		va_copy(ap, *vf->va);
		vformat(o, vf->fmt, ap);
		va_end(ap);
		return 1;
	}
	case '4':
		if (f[1] == 'c' && f[2] == 'c') {
			u32 v = *(const u32 *)p;

			for (i = 0; i < 4; i++) {
				char c = (v >> (8 * i)) & 0x7f;

				putch(o, isascii(c) && isprint(c) ? c : '.');
			}
			n = snprintf(tmp, sizeof(tmp), " %s-endian (0x%08x)", (v & (1U << 31)) ? "big" : "little", v);
			puts_pad(o, tmp, n, 0, 0);
			return 3;
		}
		break;
	case 'a': {
		unsigned long long v;

		if (f[1] == 'd') {
			v = *(const dma_addr_t *)p;
			number(o, v, 16, 2 + 2 * sizeof(dma_addr_t), -1, SPECIAL | ZERO);
			return 2;
		}
		v = *(const phys_addr_t *)p;
		number(o, v, 16, 2 + 2 * sizeof(phys_addr_t), -1, SPECIAL | ZERO);
		return f[1] == 'p' ? 2 : 1;
	}
	case 'R':
	case 'r': {
		const struct resource *r = p;

		n = snprintf(tmp, sizeof(tmp), "[%s 0x%llx-0x%llx]",
			(r->flags & IORESOURCE_IO) ? "io" : "mem",
			(unsigned long long)r->start, (unsigned long long)r->end);
		puts_pad(o, tmp, n, 0, 0);
		return 1;
	}
	case 'E': {
		const u8 *s = p;

		f++;
		while (*f && isalpha(*f))
			f++;
		n = width > 0 ? width : 0;
		for (i = 0; i < n; i++)
			if (isprint(s[i]) && s[i] != '\\')
				putch(o, s[i]);
			else {
				int k = snprintf(tmp, sizeof(tmp), "\\x%02x", s[i]);

				puts_pad(o, tmp, k, 0, 0);
			}
		return f - fmt;
	}
	case 'b': {
		const unsigned long *m = p;
		int nbits = width > 0 ? width : 0, b, first = 1, end;

		if (f[1] == 'l') {
			for (b = find_first_bit(m, nbits); b < nbits; b = find_next_bit(m, nbits, end + 1)) {
				end = b;
				while (end + 1 < nbits && test_bit(end + 1, m))
					end++;
				n = end > b ? snprintf(tmp, sizeof(tmp), "%s%d-%d", first ? "" : ",", b, end)
					: snprintf(tmp, sizeof(tmp), "%s%d", first ? "" : ",", b);
				puts_pad(o, tmp, n, 0, 0);
				first = 0;
			}
			return 2;
		}
		for (i = DIV_ROUND_UP(nbits, 32); i-- > 0;) {
			n = snprintf(tmp, sizeof(tmp), "%s%08x", i == DIV_ROUND_UP(nbits, 32) - 1 ? "" : ",", ((const u32 *)m)[i]);
			puts_pad(o, tmp, n, 0, 0);
		}
		return 1;
	}
	case 'U': {
		const u8 *u = p;
		static const int be[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		static const int le[16] = { 3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15 };
		const int *idx = (f[1] == 'l' || f[1] == 'L') ? le : be;

		for (i = 0; i < 16; i++) {
			if (i == 4 || i == 6 || i == 8 || i == 10)
				putch(o, '-');
			n = snprintf(tmp, sizeof(tmp), (f[1] == 'B' || f[1] == 'L') ? "%02X" : "%02x", u[idx[i]]);
			puts_pad(o, tmp, n, 0, 0);
		}
		return 2;
	}
	case 'S':
	case 's':
	case 'B':
		/* no symbol table: the address */
		number(o, (uintptr_t)p, 16, 0, -1, SPECIAL);
		return 1;
	case 'O':
		puts_pad(o, "(of_node)", 9, 0, 0);
		f++;
		while (*f && isalpha(*f))
			f++;
		return f - fmt;
	case 'K':
	case 'x':
		number(o, (uintptr_t)p, 16, 2 * sizeof(void *), -1, ZERO);
		return 1;
	}
	if (!p) {
		puts_pad(o, "(null)", 6, width, flags & LEFT);
		return 0;
	}
	number(o, (uintptr_t)p, 16, 2 * sizeof(void *), -1, ZERO);
	return 0;
}

static int
vformat(struct out *o, const char *fmt, va_list ap)
{
	int flags, width, prec, qual, base;
	unsigned long long v;
	const char *s;
	int len;

	for (; *fmt; fmt++) {
		if (*fmt != '%') {
			putch(o, *fmt);
			continue;
		}
		flags = 0;
		for (;;) {
			fmt++;
			if (*fmt == '-') flags |= LEFT;
			else if (*fmt == '+') flags |= PLUS;
			else if (*fmt == ' ') flags |= SPACE;
			else if (*fmt == '#') flags |= SPECIAL;
			else if (*fmt == '0') flags |= ZERO;
			else break;
		}
		width = -1;
		if (isdigit(*fmt)) {
			width = 0;
			while (isdigit(*fmt))
				width = width * 10 + *fmt++ - '0';
		} else if (*fmt == '*') {
			width = va_arg(ap, int);
			if (width < 0) {
				width = -width;
				flags |= LEFT;
			}
			fmt++;
		}
		prec = -1;
		if (*fmt == '.') {
			fmt++;
			prec = 0;
			if (isdigit(*fmt))
				while (isdigit(*fmt))
					prec = prec * 10 + *fmt++ - '0';
			else if (*fmt == '*') {
				prec = va_arg(ap, int);
				fmt++;
			}
			if (prec < 0)
				prec = 0;
		}
		qual = 0;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'z' || *fmt == 'Z' || *fmt == 't' || *fmt == 'j') {
			qual = *fmt++;
			if (qual == 'l' && *fmt == 'l') {
				qual = 'L';
				fmt++;
			} else if (qual == 'h' && *fmt == 'h') {
				qual = 'H';
				fmt++;
			}
		}
		base = 10;
		switch (*fmt) {
		case 'c':
			{
				char c = va_arg(ap, int);

				puts_pad(o, &c, 1, width, flags & LEFT);
			}
			continue;
		case 's':
			s = va_arg(ap, const char *);
			/* as Linux: a pointer that cannot be one prints, not faults
			 * (i915's selftest_tlb prints a NULL region's name array) */
			if (!s)
				s = "(null)";
			else if ((uintptr_t)s < PAGE_SIZE || IS_ERR(s))
				s = "(efault)";
			len = strnlen(s, prec < 0 ? (size_t)-1 : (size_t)prec);
			puts_pad(o, s, len, width, flags & LEFT);
			continue;
		case 'p':
			fmt += pointer(o, fmt + 1, va_arg(ap, void *), width, prec, flags);
			continue;
		case '%':
			putch(o, '%');
			continue;
		case 'n':
			(void)va_arg(ap, void *);
			continue;
		case 'o':
			base = 8;
			break;
		case 'X':
			flags |= UPPER;
			/* fall through */
		case 'x':
			base = 16;
			break;
		case 'd':
		case 'i':
			flags |= SIGN;
			break;
		case 'u':
			break;
		default:
			putch(o, '%');
			if (*fmt)
				putch(o, *fmt);
			else
				fmt--;
			continue;
		}
		switch (qual) {
		case 'L':
		case 'j':
			v = va_arg(ap, unsigned long long);
			break;
		case 'l':
		case 'z':
		case 'Z':
		case 't':
			v = va_arg(ap, unsigned long);
			if (flags & SIGN)
				v = (long)v;
			break;
		case 'h':
			v = (unsigned short)va_arg(ap, int);
			if (flags & SIGN)
				v = (short)v;
			break;
		case 'H':
			v = (unsigned char)va_arg(ap, int);
			if (flags & SIGN)
				v = (signed char)v;
			break;
		default:
			v = va_arg(ap, unsigned int);
			if (flags & SIGN)
				v = (int)v;
		}
		number(o, v, base, width, prec, flags);
	}
	return o->n;
}

int
vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
	struct out o = { buf, size, 0 };

	vformat(&o, fmt, ap);
	if (size)
		buf[min(o.n, size - 1)] = 0;
	return o.n;
}


int
snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(buf, size, fmt, ap);
	va_end(ap);
	return n;
}

int
scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vscnprintf(buf, size, fmt, ap);
	va_end(ap);
	return n;
}

int
sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(buf, INT_MAX, fmt, ap);
	va_end(ap);
	return n;
}


const char *kvasprintf_const(gfp_t gfp, const char *fmt, va_list ap) { return kvasprintf(gfp, fmt, ap); }


/*
 * sscanf: the conversions the drivers use (%d %i %u %x %o %s %c %n,
 * with h/hh/l/ll), whitespace and literal characters
 */
int
sscanf(const char *buf, const char *fmt, ...)
{
	const char *s = buf;
	unsigned long long v;
	int n = 0, qual, width, base, neg;
	char *e;
	va_list ap;

	va_start(ap, fmt);
	for (; *fmt; fmt++) {
		if (isspace(*fmt)) {
			s = skip_spaces(s);
			continue;
		}
		if (*fmt != '%' || fmt[1] == '%') {
			if (*fmt == '%')
				fmt++;
			if (*s != *fmt)
				break;
			s++;
			continue;
		}
		fmt++;
		width = 0;
		while (isdigit(*fmt))
			width = width * 10 + *fmt++ - '0';
		qual = 0;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'z') {
			qual = *fmt++;
			if (*fmt == qual) {
				qual = qual == 'h' ? 'H' : 'L';
				fmt++;
			}
		}
		if (*fmt == 'n') {
			*va_arg(ap, int *) = s - buf;
			continue;
		}
		if (*fmt == 'c') {
			if (!*s)
				break;
			*va_arg(ap, char *) = *s++;
			n++;
			continue;
		}
		s = skip_spaces(s);
		if (!*s)
			break;
		if (*fmt == 's') {
			char *d = va_arg(ap, char *);

			while (*s && !isspace(*s) && (!width || width-- > 0))
				*d++ = *s++;
			*d = 0;
			n++;
			continue;
		}
		switch (*fmt) {
		case 'd': case 'u': base = 10; break;
		case 'x': case 'X': base = 16; break;
		case 'o': base = 8; break;
		case 'i': base = 0; break;
		default: goto out;
		}
		neg = 0;
		if (*s == '-' || *s == '+')
			neg = *s++ == '-';
		if (!isxdigit(*s) && !(base == 10 && isdigit(*s)))
			goto out;
		v = simple_strtoull(s, &e, base);
		if (e == s)
			goto out;
		s = e;
		if (neg)
			v = -v;
		switch (qual) {
		case 'H': *va_arg(ap, char *) = v; break;
		case 'h': *va_arg(ap, short *) = v; break;
		case 'l': case 'z': *va_arg(ap, long *) = v; break;
		case 'L': *va_arg(ap, long long *) = v; break;
		default: *va_arg(ap, int *) = v; break;
		}
		n++;
	}
out:
	va_end(ap);
	return n;
}

/*
 * printk: a line at a time to standard error; KERN_CONT continues the
 * line; levels above kpi_loglevel are dropped
 */
int kpi_loglevel = LOGLEVEL_DEBUG - 1;
static u64 plock[4];		/* QLock */
static char pline[1024];
static int plen;

static void
flushline(void)
{
	if (plen) {
		if (pline[plen - 1] != '\n' && plen < (int)sizeof(pline))
			pline[plen++] = '\n';
		p9_print(2, pline, plen);
		plen = 0;
	}
}

int
vprintk(const char *fmt, va_list ap)
{
	int level = 4, cont = 0, n;

	while (fmt[0] == KERN_SOH[0] && fmt[1]) {
		if (fmt[1] == 'c')
			cont = 1;
		else if (fmt[1] >= '0' && fmt[1] <= '7')
			level = fmt[1] - '0';
		fmt += 2;
	}
	if (level > kpi_loglevel)
		return 0;
	p9_qlock(plock);
	if (!cont)
		flushline();
	if (!cont)
		plen = snprintf(pline, sizeof(pline), "<%d>", level);
	n = vsnprintf(pline + plen, sizeof(pline) - plen, fmt, ap);
	plen = min(plen + n, (int)sizeof(pline) - 1);
	if (plen && pline[plen - 1] == '\n')
		flushline();
	p9_qunlock(plock);
	return n;
}

int
printk(const char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vprintk(fmt, ap);
	va_end(ap);
	return n;
}

void
kpi_dev_printk(const char *level, const struct device *d, const char *fmt, ...)
{
	struct va_format vaf;
	char lfmt[32];
	va_list ap;

	va_start(ap, fmt);
	vaf.fmt = fmt;
	vaf.va = &ap;
	/* the level (KERN_ERR, ...) leads the format, where vprintk looks for it */
	snprintf(lfmt, sizeof(lfmt), "%s%%s %%s: %%pV", level);
	printk(lfmt, d && d->driver ? d->driver->name : "", d ? dev_name(d) : "(NULL device *)", &vaf);
	va_end(ap);
}

/*
 * the caller's chain of return addresses along the frame pointers (all
 * is built with -fno-omit-frame-pointer); n at most; the number found
 */
int
kpi_backtrace(uintptr_t *pc, int n)
{
	uintptr_t *bp = __builtin_frame_address(0), *next;
	int i;

	for (i = 0; i < n && bp != NULL; i++) {
		pc[i] = bp[1];
		next = (uintptr_t *)bp[0];
		/* a frame further up the same stack, or the end */
		if (next <= bp || (uintptr_t)next - (uintptr_t)bp > 1024 * 1024)
			{ i++; break; }
		bp = next;
	}
	return i;
}

/* as the trap handler's: addr2line -e the unstripped binary */
void
dump_stack(void)
{
	uintptr_t pc[16];
	char buf[16 * 19 + 8], *e = buf + sizeof(buf), *q = buf;
	int i, n;

	n = kpi_backtrace(pc, ARRAY_SIZE(pc));
	for (i = 1; i < n; i++)	/* past dump_stack itself */
		q += snprintf(q, e - q, " %#lx", (unsigned long)pc[i]);
	*q = 0;
	printk(KERN_WARNING "chain%s\n", buf);
}

void
kpi_warn(const char *file, int line, const char *cond)
{
	printk(KERN_WARNING "WARNING: %s:%d: %s\n", file, line, cond);
}

void
kpi_warn_msg(const char *fmt, ...)
{
	va_list ap;

	if (!fmt[0])
		return;
	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);
}

void
panic(const char *fmt, ...)
{
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	p9_panic(buf);
	for (;;)
		;
}

void
kpi_bug(const char *file, int line, const char *cond)
{
	char buf[256];

	snprintf(buf, sizeof(buf), "BUG: %s:%d: %s", file, line, cond);
	p9_panic(buf);
	for (;;)
		;
}
