/* Public domain. */

#ifndef _LINUX_EXPORT_H
#define _LINUX_EXPORT_H

#include <linux/kconfig.h>
#include <linux/compiler.h>
#include <linux/stringify.h>	/* 9front: as Linux's export.h */

#define EXPORT_SYMBOL(x)
#define EXPORT_SYMBOL_GPL(x)

#define THIS_MODULE	NULL

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define EXPORT_SYMBOL_NS(s, ns)
#define EXPORT_SYMBOL_NS_GPL(s, ns)
#define EXPORT_SYMBOL_IF_KUNIT(s)

#endif
