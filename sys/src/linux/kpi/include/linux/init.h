/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/init.h (ours, not Linux's) */
#ifndef _KPI_LINUX_INIT_H
#define _KPI_LINUX_INIT_H

#include <linux/compiler.h>
#include <linux/types.h>

#define module_init(f)		int kpi_init_##f(void) { return f(); }
#define module_exit(f)		void kpi_exit_##f(void) { f(); }
#define subsys_initcall(f)	module_init(f)
#define late_initcall(f)	module_init(f)
#define core_initcall(f)	module_init(f)
#define device_initcall(f)	module_init(f)
#define fs_initcall(f)		module_init(f)
#define arch_initcall(f)	module_init(f)
#define postcore_initcall(f)	module_init(f)
#define __setup(s, f)

#endif
