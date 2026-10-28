/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: what the Plan 9 side calls on the Linux side (linux/i915.c);
 * plain C types only, both sides include it.
 */
int	i915_start(void);	/* i915's module_init: probes the device; 0 or -errno */
