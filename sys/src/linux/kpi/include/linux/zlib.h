/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/zlib.h (ours, not Linux's) */
#ifndef _KPI_LINUX_ZLIB_H
#define _KPI_LINUX_ZLIB_H

#include <linux/types.h>

/* no compressed error states (CONFIG_DRM_I915_COMPRESS_ERROR off): the types it names */
struct z_stream_s { const u8 *next_in; unsigned int avail_in; unsigned long total_in; u8 *next_out; unsigned int avail_out; unsigned long total_out; void *workspace; };
typedef struct z_stream_s z_stream;
#define MAX_WBITS		15
#define MAX_MEM_LEVEL		8
#define Z_OK			0
#define Z_STREAM_END		1
#define Z_NO_FLUSH		0
#define Z_SYNC_FLUSH		2
#define Z_FINISH		4
#define Z_DEFLATED		8
#define Z_DEFAULT_STRATEGY	0
#define Z_BEST_SPEED		1

#endif
