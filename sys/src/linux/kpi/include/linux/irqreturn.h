/* Public domain. */

#ifndef _LINUX_IRQRETURN_H
#define _LINUX_IRQRETURN_H

typedef int irqreturn_t;
enum irqreturn {
	IRQ_NONE = 0,
	IRQ_HANDLED = 1,
	IRQ_WAKE_THREAD = 2	/* 9front: Linux's */
};

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define IRQ_RETVAL(x)	((x) ? IRQ_HANDLED : IRQ_NONE)

#endif
