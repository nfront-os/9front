/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/module.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MODULE_H
#define _KPI_LINUX_MODULE_H

#include <linux/types.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/error-injection.h>	/* 9front: as Linux's module.h */
#include <linux/compiler.h>
#include <linux/stringify.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>

struct module;
#define MODULE_LICENSE(s)
#define MODULE_AUTHOR(s)
#define MODULE_DESCRIPTION(s)
#define MODULE_VERSION(s)
#define MODULE_ALIAS(s)
#define MODULE_FIRMWARE(s)
#define MODULE_DEVICE_TABLE(t, n)
#define MODULE_SOFTDEP(s)
#define MODULE_IMPORT_NS(ns)
#define MODULE_INFO(t, s)
#define MODULE_PARM_DESC(p, d)
/*
 * module parameters: each registers itself by name (a constructor, run
 * by kpi_start: 9front's libc runs none) in kpi/linux/param.c, so that
 * the file server can set them as Linux's command line does
 * (aux/i915 -p name=value)
 */
enum kpi_param_type { KPI_PARAM_bool, KPI_PARAM_invbool, KPI_PARAM_int, KPI_PARAM_uint, KPI_PARAM_long,
	KPI_PARAM_ulong, KPI_PARAM_short, KPI_PARAM_ushort, KPI_PARAM_byte, KPI_PARAM_charp,
	KPI_PARAM_hexint, KPI_PARAM_ullong, KPI_PARAM_bint };	/* hexint: an uint; bint: an int set as a bool */
void	kpi_param_add(const char *name, void *var, int type);
#define __kpi_module_param(n, v, t) \
	static void __attribute__((constructor, used)) __kpi_param_##n(void) { kpi_param_add(#n, (void *)&(v), KPI_PARAM_##t); }
#define module_param(n, t, p)			__kpi_module_param(n, n, t)
#define module_param_named(n, v, t, p)		__kpi_module_param(n, v, t)
#define module_param_named_unsafe(n, v, t, p)	__kpi_module_param(n, v, t)
#define module_param_string(n, s, l, p)
#define module_param_unsafe(n, t, p)		__kpi_module_param(n, n, t)
#define module_param_cb(n, o, a, p)
#define module_param_cb_unsafe(n, o, a, p)
#define __module_param_call(...)
#define kpi_module_param_cb_placeholder
#define module_param_array(n, t, c, p)
#define __module_get(m)		do { } while (0)
#define try_module_get(m)	1
#define module_put(m)		do { } while (0)
#define module_is_live(m)	1
#define symbol_get(s)		((typeof(&(s)))(&(s)))
#define symbol_put(s)		do { } while (0)
#define request_module(...)	(-ENOENT)

#endif
