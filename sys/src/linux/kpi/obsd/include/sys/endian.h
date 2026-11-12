/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/endian.h (little endian, gcc builtins) */
#ifndef _OBSD_SYS_ENDIAN_H
#define _OBSD_SYS_ENDIAN_H

#define LITTLE_ENDIAN	1234
#define BIG_ENDIAN	4321
#define BYTE_ORDER	LITTLE_ENDIAN
#define _LITTLE_ENDIAN	LITTLE_ENDIAN
#define _BIG_ENDIAN	BIG_ENDIAN
#define _BYTE_ORDER	BYTE_ORDER

#define swap16(x)	__builtin_bswap16(x)
#define swap32(x)	__builtin_bswap32(x)
#define swap64(x)	__builtin_bswap64(x)
#define htole16(x)	((uint16_t)(x))
#define htole32(x)	((uint32_t)(x))
#define htole64(x)	((uint64_t)(x))
#define letoh16(x)	((uint16_t)(x))
#define letoh32(x)	((uint32_t)(x))
#define letoh64(x)	((uint64_t)(x))
#define htobe16(x)	swap16(x)
#define htobe32(x)	swap32(x)
#define htobe64(x)	swap64(x)
#define betoh16(x)	swap16(x)
#define betoh32(x)	swap32(x)
#define betoh64(x)	swap64(x)
#define le16toh		letoh16
#define le32toh		letoh32
#define le64toh		letoh64
#define be16toh		betoh16
#define be32toh		betoh32
#define be64toh		betoh64

/* loads and stores of little-endian values in memory (the device's registers) */
#define lemtoh16(p)	(*(volatile uint16_t *)(p))
#define lemtoh32(p)	(*(volatile uint32_t *)(p))
#define lemtoh64(p)	(*(volatile uint64_t *)(p))
#define htolem16(p, v)	(*(volatile uint16_t *)(p) = (v))
#define htolem32(p, v)	(*(volatile uint32_t *)(p) = (v))
#define htolem64(p, v)	(*(volatile uint64_t *)(p) = (v))

#endif
