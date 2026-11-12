/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, machine/fpu.h.  A proc of the
 * file server has its own FPU state always (Plan 9 saves it on each
 * switch); what kernel_fpu_begin must give is Linux's floating point
 * environment: every exception masked (MXCSR 0x1f80), so that a division
 * by zero makes an infinity, not a note (DCN 3.2's DML divides by
 * DSCSlices 0 for HDMI without DSC, Linux 7.3).  Plan 9 starts a process
 * with the traps on; kpi_start masks them before other procs are made
 * (a fork copies the state), this for procs made elsewhere.
 */
#ifndef _OBSD_MACHINE_FPU_H
#define _OBSD_MACHINE_FPU_H
void	p9_fpu_kernel(void);
static inline void fpu_kernel_enter(void) { p9_fpu_kernel(); }
static inline void fpu_kernel_exit(void) { }
#endif
