/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: what the Plan 9 side of a file server calls on
 * the Linux side (the other way from kpi/p9.h).  Plain C types only:
 * both sides include it.
 */
#ifndef _KPI_LINUX_H
#define _KPI_LINUX_H

/* the KPI started and the device of #G (gdir: "#G/0") made its PCI device; 0 or -errno */
int	kpi_start(const char *gdir);
/* a module parameter, "name=value" as on Linux's command line; 0 or -errno (kpi_start first) */
int	kpi_param_set(const char *s);
/* DRM's core and helpers initialised, as Linux's module_init does; 0 or -errno */
int	kpi_drm_start(void);

/*
 * kpi/plan9: a fault on a segment the KPI attached in another proc
 * attaches it in this one: 1 if so (the note handler goes on), else 0
 */
int	p9_fault(char *note);

#endif
