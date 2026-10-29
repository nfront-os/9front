#ifndef __IOCTL_H__
#define __IOCTL_H__

/* GCC port: a program that includes it wants it: _BSD_EXTENSION for
 * this header (bsd.h's prototypes stay off) */
#ifndef _BSD_EXTENSION
#define _BSD_EXTENSION
#define __GCC_BSD_SYS_IOCTL_H
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* FIONREAD: return number of bytes readable in *(long*)arg */
#define FIONREAD 1

int ioctl(int, unsigned long, void*);

#ifdef __cplusplus
}
#endif


#ifdef __GCC_BSD_SYS_IOCTL_H
#undef _BSD_EXTENSION
#undef __GCC_BSD_SYS_IOCTL_H
#endif
#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: a terminal's size, which Plan 9 does not tell (ioctl fails) */
#ifndef TIOCGWINSZ
#define TIOCGWINSZ	0x5413
struct winsize {
	unsigned short	ws_row;
	unsigned short	ws_col;
	unsigned short	ws_xpixel;
	unsigned short	ws_ypixel;
};
#endif
#endif
#endif /* !__IOCTL_H__ */
