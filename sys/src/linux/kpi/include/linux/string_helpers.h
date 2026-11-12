/* Public domain. */

#ifndef _LINUX_STRING_HELPERS_H
#define _LINUX_STRING_HELPERS_H

#include <linux/string_choices.h>


/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/string.h>
#include <linux/ctype.h>
static inline const char *str_true_false(bool v) { return v ? "true" : "false"; }
static inline const char *str_high_low(bool v) { return v ? "high" : "low"; }
static inline const char *str_up_down(bool v) { return v ? "up" : "down"; }
#define str_disable_enable(v)	str_enable_disable((v) == 0)
#define str_disabled_enabled(v)	str_enabled_disabled((v) == 0)
#define str_off_on(v)		str_on_off((v) == 0)
#define str_no_yes(v)		str_yes_no((v) == 0)
enum string_size_units { STRING_UNITS_10, STRING_UNITS_2 };
void	string_get_size(u64 size, u64 blk_size, enum string_size_units units, char *buf, int len);
#define ESCAPE_ANY_NP		0
int	string_escape_mem(const char *src, size_t isz, char *dst, size_t osz, unsigned int flags, const char *only);
char	*kstrdup_quotable(const char *src, gfp_t gfp);

#endif
