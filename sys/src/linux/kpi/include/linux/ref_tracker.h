/* Public domain. */

#ifndef _LINUX_REF_TRACKER_H
#define _LINUX_REF_TRACKER_H

#include <linux/types.h>

struct ref_tracker;
struct ref_tracker_dir {
};

/*
 * 9front: references are not tracked (Linux without CONFIG_REF_TRACKER:
 * i915's wakeref tracking since 6.9 goes through these)
 */
static inline void ref_tracker_dir_init(struct ref_tracker_dir *dir, unsigned int quarantine, const char *name) { }
static inline void ref_tracker_dir_debugfs(struct ref_tracker_dir *dir) { }
static inline void ref_tracker_dir_exit(struct ref_tracker_dir *dir) { }
static inline void ref_tracker_dir_print_locked(struct ref_tracker_dir *dir, unsigned int limit) { }
static inline void ref_tracker_dir_print(struct ref_tracker_dir *dir, unsigned int limit) { }
static inline int ref_tracker_dir_snprint(struct ref_tracker_dir *dir, char *buf, size_t size) { return 0; }
static inline int ref_tracker_alloc(struct ref_tracker_dir *dir, struct ref_tracker **trackerp, gfp_t gfp) { return 0; }
static inline int ref_tracker_free(struct ref_tracker_dir *dir, struct ref_tracker **trackerp) { return 0; }

#endif
