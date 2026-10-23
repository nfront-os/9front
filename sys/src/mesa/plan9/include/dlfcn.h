/*
 * SPDX-License-Identifier: MIT
 *
 * <dlfcn.h> for Mesa on 9front: the toolchain's (dlopen always fails)
 * and dladdr (lib/dl.c), which knows the one object, the program.
 */
#ifndef _PLAN9_DLFCN_H
#define _PLAN9_DLFCN_H
#include_next <dlfcn.h>

typedef struct {
	const char	*dli_fname;
	void	*dli_fbase;
	const char	*dli_sname;
	void	*dli_saddr;
} Dl_info;

#ifdef __cplusplus
extern "C" {
#endif
extern int	dladdr(const void*, Dl_info*);
#ifdef __cplusplus
}
#endif
#endif
