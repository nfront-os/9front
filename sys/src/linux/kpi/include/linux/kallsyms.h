/* Public domain. */

#ifndef _LINUX_KALLSYMS_H
#define _LINUX_KALLSYMS_H

#include <linux/module.h>

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define KSYM_NAME_LEN		512
#define KSYM_SYMBOL_LEN		(sizeof("%s+%#lx/%#lx [%s %s]") + KSYM_NAME_LEN + 2 * (64 * 3 / 10) + 64 + 1)

#endif
