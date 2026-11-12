/* Public domain. */

#ifndef _LINUX_UUID_H
#define _LINUX_UUID_H

#include <linux/string.h>

#define UUID_STRING_LEN 36
#define UUID_SIZE	16

typedef struct {
	uint8_t guid[UUID_SIZE];
} guid_t;

static inline void
import_guid(guid_t *dst, const uint8_t *src)
{
	memcpy(&dst->guid, src, sizeof(dst->guid));
}

static inline void
export_guid(uint8_t *dst, const guid_t *src)
{
	memcpy(dst, &src->guid, sizeof(src->guid));
}

static inline void
guid_copy(guid_t *dst, const guid_t *src)
{
	memcpy(&dst->guid, &src->guid, sizeof(dst->guid));
}

static inline bool
guid_equal(const guid_t *a, const guid_t *b)
{
	return memcmp(&a->guid, &b->guid, sizeof(a->guid)) == 0;
}

static inline bool
guid_is_null(const guid_t *a)
{
	return memchr_inv(&a->guid, 0, sizeof(a->guid)) == NULL;
}

static inline void
guid_gen(guid_t *a)
{
	arc4random_buf(&a->guid, sizeof(a->guid));
	a->guid[6] = (a->guid[6] & 0x0f) | 0x40;
	a->guid[8] = (a->guid[8] & 0x3f) | 0x80;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
typedef struct { u8 b[16]; } uuid_t;
#define GUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7) ((guid_t){{ (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, \
	(b) & 0xff, ((b) >> 8) & 0xff, (c) & 0xff, ((c) >> 8) & 0xff, (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }})
#define UUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7) ((uuid_t){{ ((a) >> 24) & 0xff, ((a) >> 16) & 0xff, ((a) >> 8) & 0xff, (a) & 0xff, \
	((b) >> 8) & 0xff, (b) & 0xff, ((c) >> 8) & 0xff, (c) & 0xff, (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }})
static inline bool uuid_equal(const uuid_t *a, const uuid_t *b) { return memcmp(a, b, sizeof(uuid_t)) == 0; }
static inline void uuid_copy(uuid_t *d, const uuid_t *s) { memcpy(d, s, sizeof(uuid_t)); }
static inline bool uuid_is_null(const uuid_t *u) { return mem_is_zero(u, sizeof(*u)); }
static inline void import_uuid(uuid_t *d, const u8 *s) { memcpy(d, s, sizeof(uuid_t)); }
static inline void export_uuid(u8 *d, const uuid_t *s) { memcpy(d, s, sizeof(uuid_t)); }
bool	uuid_is_valid(const char *uuid);
void	uuid_gen(uuid_t *u);
int	uuid_parse(const char *in, uuid_t *u);
int	guid_parse(const char *in, guid_t *g);

#endif
