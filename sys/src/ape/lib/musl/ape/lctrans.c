/*
 * APE's (GCC port): musl's message translation for its regerror
 * (src/regex/regerror.c: LCTRANS_CUR): APE has only the C locale, the
 * message stays as it is.
 */
#include "locale_impl.h"

const char *
__lctrans_cur(const char *msg)
{
	return msg;
}
