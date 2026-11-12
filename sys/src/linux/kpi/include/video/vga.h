/* Public domain. */

#ifndef _VIDEO_VGA_H
#define _VIDEO_VGA_H

#define VGA_MIS_W	0x3c2
#define VGA_SEQ_I	0x3c4
#define VGA_SEQ_D	0x3c5
#define VGA_MIS_R	0x3cc

#define VGA_SR01_SCREEN_OFF	(1 << 5)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#define VGA_MIS_COLOR	0x01

#define VGA_IS0_R	0x3c2	/* input status 0 */
#define VGA_MIS_ENB_MEM_ACCESS	0x02

#endif
