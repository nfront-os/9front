/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/security.h (ours): no LSMs, nothing locked down */
#ifndef _KPI_LINUX_SECURITY_H
#define _KPI_LINUX_SECURITY_H
enum lockdown_reason { LOCKDOWN_NONE, LOCKDOWN_PCI_ACCESS, LOCKDOWN_DEBUGFS, LOCKDOWN_INTEGRITY_MAX, LOCKDOWN_CONFIDENTIALITY_MAX };
#define security_locked_down(what)	0
#endif
