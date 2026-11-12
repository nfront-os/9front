/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/mfd/intel_soc_pmic.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MFD_INTEL_SOC_PMIC_H
#define _KPI_LINUX_MFD_INTEL_SOC_PMIC_H

#include <linux/kernel.h>
#include <linux/errno.h>

struct intel_soc_pmic { int irq; void *regmap; };
#define intel_soc_pmic_exec_mipi_pmic_seq_element(a, r, v, m)	(-EOPNOTSUPP)

#endif
