/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/jump_label.h (ours, not Linux's) */
#ifndef _KPI_LINUX_JUMP_LABEL_H
#define _KPI_LINUX_JUMP_LABEL_H

#include <linux/atomic.h>

/* static keys: plain flags here */
struct static_key { atomic_t enabled; };
struct static_key_true { struct static_key key; };
struct static_key_false { struct static_key key; };
#define STATIC_KEY_INIT_TRUE		{ .enabled = ATOMIC_INIT(1) }
#define STATIC_KEY_INIT_FALSE		{ .enabled = ATOMIC_INIT(0) }
#define DEFINE_STATIC_KEY_TRUE(n)	struct static_key_true n = { .key = STATIC_KEY_INIT_TRUE }
#define DEFINE_STATIC_KEY_FALSE(n)	struct static_key_false n = { .key = STATIC_KEY_INIT_FALSE }
#define DECLARE_STATIC_KEY_TRUE(n)	extern struct static_key_true n
#define DECLARE_STATIC_KEY_FALSE(n)	extern struct static_key_false n
#define static_branch_likely(x)		(atomic_read(&(x)->key.enabled) > 0)
#define static_branch_unlikely(x)	(atomic_read(&(x)->key.enabled) > 0)
#define static_branch_enable(x)		atomic_set(&(x)->key.enabled, 1)
#define static_branch_disable(x)	atomic_set(&(x)->key.enabled, 0)
#define static_branch_inc(x)		atomic_inc(&(x)->key.enabled)
#define static_branch_dec(x)		atomic_dec(&(x)->key.enabled)
#define static_key_enabled(x)		(atomic_read(&(x)->enabled) > 0)

#endif
