/* Public domain. */

#ifndef _LINUX_PRINTK_H
#define _LINUX_PRINTK_H

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/stdarg.h>

#include <linux/init.h>

#define KERN_CRIT	"\0012"
#define KERN_ERR	"\0013"
#define KERN_WARNING	"\0014"
#define KERN_NOTICE	"\0015"
#define KERN_INFO	"\0016"
#define KERN_DEBUG	"\0017"

#define KERN_CONT	"\001c"

#define HW_ERR		"HW_ERR: "

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#define printk_once(fmt, arg...) ({		\
	static int __warned;			\
	if (!__warned) {			\
		printk(fmt, ## arg);		\
		__warned = 1;			\
	}					\
})

#define printk_ratelimit()	1

int printk(const char *fmt, ...);

#define pr_warn(fmt, arg...)	printk(KERN_WARNING pr_fmt(fmt), ## arg)
#define pr_warn_ratelimited(fmt, arg...)	printk(KERN_WARNING pr_fmt(fmt), ## arg)
#define pr_warn_once(fmt, arg...)	printk_once(KERN_WARNING pr_fmt(fmt), ## arg)
#define pr_notice(fmt, arg...)	printk(KERN_NOTICE pr_fmt(fmt), ## arg)
#define pr_crit(fmt, arg...)	printk(KERN_CRIT pr_fmt(fmt), ## arg)
#define pr_err(fmt, arg...)	printk(KERN_ERR pr_fmt(fmt), ## arg)
#define pr_err_once(fmt, arg...)	printk_once(KERN_ERR pr_fmt(fmt), ## arg)
#define pr_cont(fmt, arg...)	printk(KERN_CONT pr_fmt(fmt), ## arg)

/* 9front: always to printk, whose log level decides (kpi/linux/printf.c); OpenBSD's only with DRMDEBUG */
#define pr_info(fmt, arg...)	printk(KERN_INFO pr_fmt(fmt), ## arg)
#define pr_info_ratelimited(fmt, arg...)	printk(KERN_INFO pr_fmt(fmt), ## arg)
#define pr_info_once(fmt, arg...)	printk_once(KERN_INFO pr_fmt(fmt), ## arg)
#define pr_debug(fmt, arg...)	printk(KERN_DEBUG pr_fmt(fmt), ## arg)

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

void print_hex_dump(const char *, const char *, int, int, int,
	 const void *, size_t, bool);

struct va_format {
	const char *fmt;
	va_list *va;
};

static inline int
_in_dbg_master(void)
{
#ifdef DDB
	return (db_active);
#endif
	return (0);
}

#define oops_in_progress _in_dbg_master()

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/stdarg.h>
#include <linux/types.h>
#include <linux/compiler.h>
#define KERN_SOH	"\001"
#define KERN_EMERG	KERN_SOH "0"
#define KERN_ALERT	KERN_SOH "1"
#define KERN_DEFAULT	""
#define LOGLEVEL_ERR	3
#define LOGLEVEL_DEBUG	7
int	vprintk(const char *fmt, va_list args);
#define pr_emerg(fmt, ...)	printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...)	printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_devel(fmt, ...)	pr_debug(fmt, ##__VA_ARGS__)
#define pr_notice_once(fmt, ...)	pr_notice(fmt, ##__VA_ARGS__)
#define pr_err_ratelimited(fmt, ...)	pr_err(fmt, ##__VA_ARGS__)
#define pr_debug_ratelimited(fmt, ...)	pr_debug(fmt, ##__VA_ARGS__)
#define printk_ratelimited(fmt, ...)	printk(fmt, ##__VA_ARGS__)
#define no_printk(fmt, ...)	({ if (0) printk(fmt, ##__VA_ARGS__); 0; })
int	hex_dump_to_buffer(const void *buf, size_t len, int rowsize, int groupsize, char *linebuf, size_t linebuflen, bool ascii);

#define print_hex_dump_debug(p, t, r, g, b, l, a)	do { } while (0)	/* 9front: no dynamic debug */

#endif
