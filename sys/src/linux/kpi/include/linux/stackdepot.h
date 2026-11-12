/* Public domain. */

#ifndef _LINUX_STACKDEPOT_H
#define _LINUX_STACKDEPOT_H

typedef uint32_t depot_stack_handle_t;

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
static inline int stack_depot_snprint(depot_stack_handle_t h, char *buf, size_t size, int spaces) { if (size) buf[0] = 0; return 0; }
static inline depot_stack_handle_t stack_depot_save(unsigned long *e, unsigned int n, gfp_t f) { return 0; }
static inline void stack_depot_init(void) { }
static inline unsigned int stack_depot_fetch(depot_stack_handle_t h, unsigned long **e) { *e = NULL; return 0; }
static inline void stack_depot_print(depot_stack_handle_t h) { }

#endif
