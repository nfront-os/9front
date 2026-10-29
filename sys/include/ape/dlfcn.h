/*
 * APE (GCC port): <dlfcn.h>. 9front has no dynamic loading: dlopen always
 * fails, dlerror says why (libap). binutils' ld calls dlopen
 * for -plugin even when built without plugins.
 */
#ifndef __APE_DLFCN_H
#define __APE_DLFCN_H
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL	4
#define RTLD_LOCAL	0
#ifdef __cplusplus
extern "C" {
#endif
extern void	*dlopen(const char *, int);
extern void	*dlsym(void *, const char *);
extern int	dlclose(void *);
extern char	*dlerror(void);
#ifdef __cplusplus
}
#endif
#endif
