/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/debugobjects.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DEBUGOBJECTS_H
#define _KPI_LINUX_DEBUGOBJECTS_H

#include <linux/kernel.h>

struct debug_obj_descr { const char *name; };
#define debug_object_init(a, d)	do { } while (0)
#define debug_object_activate(a, d)	0
#define debug_object_deactivate(a, d)	do { } while (0)
#define debug_object_destroy(a, d)	do { } while (0)
#define debug_object_free(a, d)	do { } while (0)
#define debug_object_assert_init(a, d)	do { } while (0)
#define debug_object_init_on_stack(a, d)	do { } while (0)

#endif
