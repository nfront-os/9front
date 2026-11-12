/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/moduleparam.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MODULEPARAM_H
#define _KPI_LINUX_MODULEPARAM_H

#include <linux/module.h>

/* module parameters: set at start-up from the file server's flags (kpi/param.c) */
struct kernel_param;
struct kernel_param_ops { unsigned int flags; int (*set)(const char *val, const struct kernel_param *kp); int (*get)(char *buf, const struct kernel_param *kp); void (*free)(void *arg); };
struct kernel_param { const char *name; const struct kernel_param_ops *ops; void *arg; };
extern const struct kernel_param_ops param_ops_int, param_ops_uint, param_ops_bool, param_ops_charp, param_ops_ulong;
int	param_set_int(const char *val, const struct kernel_param *kp);
int	param_get_int(char *buf, const struct kernel_param *kp);
int	param_set_bool(const char *val, const struct kernel_param *kp);
int	param_get_bool(char *buf, const struct kernel_param *kp);

#endif
