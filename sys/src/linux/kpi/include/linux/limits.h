/* Public domain. */

#ifndef _LINUX_LIMITS_H
#define _LINUX_LIMITS_H

#include <sys/stdint.h>
#include <linux/types.h>

#define S8_MAX		INT8_MAX
#define S16_MAX		INT16_MAX
#define S32_MAX		INT32_MAX
#define S64_MAX		INT64_MAX

#define U8_MAX		UINT8_MAX
#define U16_MAX		UINT16_MAX
#define U32_MAX		UINT32_MAX
#define U64_C(x)	UINT64_C(x)
#define U64_MAX		UINT64_MAX

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/types.h>
#define PHYS_ADDR_MAX	((phys_addr_t)~0ULL)
#define S8_MIN		((s8)(-S8_MAX - 1))
#define S16_MIN		((s16)(-S16_MAX - 1))
#define U32_MIN		((u32)0)
#define S32_MIN		((s32)(-S32_MAX - 1))
#define S64_MIN		((s64)(-S64_MAX - 1))
#define PATH_MAX	4096
#define NAME_MAX	255

#endif
