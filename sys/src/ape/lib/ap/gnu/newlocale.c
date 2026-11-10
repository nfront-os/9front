/*
 * POSIX.1-2008's locale objects for APE, which knows only the C locale
 * (setlocale): newlocale makes one of "C", "POSIX" or "" (the same),
 * uselocale switches to it (a no-op: every locale is C), freelocale and
 * duplocale.  fontconfig formats and reads numbers in the C locale so.
 */
#include <locale.h>
#include <string.h>
#include <errno.h>

struct __locale {
	int	mask;
};

static struct __locale clocale = { LC_ALL_MASK };

static int
iscname(const char *name)
{
	return name != NULL && (name[0] == 0 || strcmp(name, "C") == 0 || strcmp(name, "POSIX") == 0);
}

locale_t
newlocale(int mask, const char *name, locale_t base)
{
	(void)base;
	if((mask & ~LC_ALL_MASK) != 0){
		errno = EINVAL;
		return (locale_t)0;
	}
	if(!iscname(name)){
		errno = ENOENT;
		return (locale_t)0;
	}
	return &clocale;
}

locale_t
duplocale(locale_t l)
{
	if(l == (locale_t)0){
		errno = EINVAL;
		return (locale_t)0;
	}
	return &clocale;
}

void
freelocale(locale_t l)
{
	(void)l;
}

locale_t
uselocale(locale_t l)
{
	(void)l;
	return LC_GLOBAL_LOCALE;
}
