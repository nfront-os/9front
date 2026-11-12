/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drivers/platform/x86/intel_ips.h (ours: Linux's is GPL) */
#ifndef _KPI_INTEL_IPS_H
#define _KPI_INTEL_IPS_H

/* intel_ips (turbo for Ironlake's GPU and CPU together): not on 9front */
/* i915 looks for it with symbol_get: weak, so it is not there */
void	ips_link_to_i915_driver(void) __attribute__((__weak__));

#endif
