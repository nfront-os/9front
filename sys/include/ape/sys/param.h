#ifndef __PARAM_H__
#define __PARAM_H__

/* GCC port: a program that includes it wants it: _BSD_EXTENSION for
 * this header (bsd.h's prototypes stay off) */
#ifndef _BSD_EXTENSION
#define _BSD_EXTENSION
#define __GCC_BSD_SYS_PARAM_H
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NOFILES_MAX 100

#ifdef __cplusplus
}
#endif

#ifdef __GCC_BSD_SYS_PARAM_H
#undef _BSD_EXTENSION
#undef __GCC_BSD_SYS_PARAM_H
#endif
#endif /* !__PARAM_H__ */
