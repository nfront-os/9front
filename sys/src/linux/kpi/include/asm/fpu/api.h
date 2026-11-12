/* Public domain. */

#ifndef _ASM_FPU_API_H
#define _ASM_FPU_API_H

#include <linux/bottom_half.h>

#ifdef __i386__
#include <machine/npx.h>
#endif

#ifdef __amd64__
#include <machine/fpu.h>
#endif

#define kernel_fpu_begin()	fpu_kernel_enter()
#define kernel_fpu_end()	fpu_kernel_exit()

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <asm/processor.h>
#define irq_fpu_usable()	1

#endif
