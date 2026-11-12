/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/topology.h (ours) */
#ifndef _KPI_LINUX_TOPOLOGY_H
#define _KPI_LINUX_TOPOLOGY_H
#include <asm/processor.h>
#include <linux/numa.h>
#define topology_num_cores_per_package()	(boot_cpu_data.x86_max_cores)
#endif
