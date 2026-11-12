/* Public domain. */

#ifndef _LINUX_STDDEF_H
#define _LINUX_STDDEF_H

#define DECLARE_FLEX_ARRAY(t, n) \
	struct { struct{} n ## __unused; t n[]; }

#define offsetofend(s, e)	(offsetof(s, e) + sizeof((((s *)0)->e)))
#define sizeof_field(s, e)	sizeof((((s *)0)->e))

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/compiler_types.h>
#define sizeof_field(t, m)	sizeof(((t *)0)->m)
#define struct_group(name, ...)	union { struct { __VA_ARGS__ }; struct { __VA_ARGS__ } name; }
#define struct_group_tagged(tag, name, ...)	union { struct { __VA_ARGS__ }; struct tag { __VA_ARGS__ } name; }
#define struct_group_attr(name, attrs, ...)	union { struct { __VA_ARGS__ } attrs; struct { __VA_ARGS__ } attrs name; }

#endif
