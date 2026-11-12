/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/kdev_t.h (ours, not Linux's) */
#ifndef _KPI_LINUX_KDEV_T_H
#define _KPI_LINUX_KDEV_T_H

#include <linux/types.h>

#define MINORBITS	20
#define MINORMASK	((1U << MINORBITS) - 1)
#define MAJOR(dev)	((unsigned int)((dev) >> MINORBITS))
#define MINOR(dev)	((unsigned int)((dev) & MINORMASK))
#define MKDEV(ma, mi)	(((ma) << MINORBITS) | (mi))
static inline u16 old_encode_dev(dev_t d) { return (MAJOR(d) << 8) | MINOR(d); }
static inline u32 new_encode_dev(dev_t d) { unsigned major = MAJOR(d), minor = MINOR(d); return (minor & 0xff) | (major << 8) | ((minor & ~0xff) << 12); }

#endif
