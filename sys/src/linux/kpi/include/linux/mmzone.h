/* Public domain. */

#ifndef _LINUX_MMZONE_H
#define _LINUX_MMZONE_H

#include <linux/mm_types.h>
#include <linux/nodemask.h>
#include <linux/notifier.h> /* via memory_hotplug.h */

#define MAX_PAGE_ORDER	10
#define NR_PAGE_ORDERS	(MAX_PAGE_ORDER + 1)
#define pfn_to_nid(x)	0

/* 9front: one memory node, nothing per node kept */
#define MAX_NR_ZONES	1
struct zone { unsigned long kpi_unused; };
typedef struct pglist_data { int node_id; struct zone node_zones[MAX_NR_ZONES]; } pg_data_t;
/* the zone's pages: all memory (TTM's pool sizes itself by it) */
#define zone_managed_pages(z)	((unsigned long)totalram_pages())
extern pg_data_t kpi_node0;
#define NODE_DATA(nid)	(&kpi_node0)

#endif
