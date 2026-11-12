/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/ascii85.h (ours, not Linux's) */
#ifndef _KPI_LINUX_ASCII85_H
#define _KPI_LINUX_ASCII85_H

#include <linux/kernel.h>

/* ascii85 for the error state (ours: Linux's is GPL); 0 is "z" */
#define ASCII85_BUFSZ		6
static inline long ascii85_encode_len(long len) { return DIV_ROUND_UP(len, 4); }
static inline const char *ascii85_encode(u32 in, char *out)
{ int i; if (in == 0) return "z"; out[5] = 0; for (i = 5; i--;) { out[i] = '!' + in % 85; in /= 85; } return out; }

#endif
