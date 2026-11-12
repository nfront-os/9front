/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: our own declarations of selftests/lib_sw_fence.c
 * (MIT; Linux's header is GPL): software fences for the selftests, on
 * the stack, signalled by a timer, or on the heap.
 */
#ifndef _LIB_SW_FENCE_H_
#define _LIB_SW_FENCE_H_

#include <linux/timer.h>
#include "i915_sw_fence.h"

/* no lockdep here: no lock class for the fence's wait queue */
#define onstack_fence_init(fence)	__onstack_fence_init((fence), NULL, NULL)
void	__onstack_fence_init(struct i915_sw_fence *fence, const char *name, struct lock_class_key *key);
void	onstack_fence_fini(struct i915_sw_fence *fence);

struct timed_fence {
	struct i915_sw_fence	fence;
	struct timer_list	timer;
};
void	timed_fence_init(struct timed_fence *tf, unsigned long expires);
void	timed_fence_fini(struct timed_fence *tf);

struct i915_sw_fence	*heap_fence_create(gfp_t gfp);
void	heap_fence_put(struct i915_sw_fence *fence);

#endif
