/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/utsname.h (ours, not Linux's) */
#ifndef _KPI_LINUX_UTSNAME_H
#define _KPI_LINUX_UTSNAME_H

#include <linux/kernel.h>

struct new_utsname { char sysname[65]; char nodename[65]; char release[65]; char version[65]; char machine[65]; char domainname[65]; };
struct new_utsname	*init_utsname(void);
#define utsname()	init_utsname()

#endif
