/* Public domain. */

#ifndef _LINUX_NODEMASK_H
#define _LINUX_NODEMASK_H

#include <linux/random.h>

#define num_possible_nodes()	1
#define num_online_nodes()	1

#ifndef for_each_node
#define for_each_node(n)	for ((n) = 0; (n) < 1; (n)++)	/* 9front: one node */
#endif

#endif
