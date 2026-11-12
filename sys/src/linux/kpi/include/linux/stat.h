/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/stat.h (ours, not Linux's) */
#ifndef _KPI_LINUX_STAT_H
#define _KPI_LINUX_STAT_H

#include <linux/types.h>

#define S_IRUGO		00444
#define S_IWUSR		00200
#define S_IRUSR		00400
#define S_IWUGO		00222
#define S_IXUGO		00111
#define S_IRWXUGO	00777
#define S_IFREG		0100000
#define S_IRWXU		00700
#define S_IXUSR		00100
#define S_IRWXG		00070
#define S_IRGRP		00040
#define S_IWGRP		00020
#define S_IXGRP		00010
#define S_IRWXO		00007
#define S_IROTH		00004
#define S_IWOTH		00002
#define S_IXOTH		00001
#define S_IFCHR		0020000
#define S_IFDIR		0040000

#endif
