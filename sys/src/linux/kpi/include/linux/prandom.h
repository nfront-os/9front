/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/prandom.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PRANDOM_H
#define _KPI_LINUX_PRANDOM_H

#include <linux/types.h>
#include <sys/types.h>

struct rnd_state { u32 s1, s2, s3, s4; };
u32	prandom_u32_state(struct rnd_state *s);
void	prandom_seed_state(struct rnd_state *s, u64 seed);
void	prandom_bytes_state(struct rnd_state *s, void *buf, size_t n);
/* the linear congruential generator of Numerical Recipes, as Linux's */
static inline u32 next_pseudo_random32(u32 seed) { return seed * 1664525 + 1013904223; }

#endif
