/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pagevec.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PAGEVEC_H
#define _KPI_LINUX_PAGEVEC_H

#include <linux/mm.h>

#define PAGEVEC_SIZE		15
struct folio_batch { unsigned char nr; bool percpu_pvec_drained; struct folio *folios[PAGEVEC_SIZE]; };
static inline void folio_batch_init(struct folio_batch *fb) { fb->nr = 0; fb->percpu_pvec_drained = false; }
static inline unsigned int folio_batch_count(struct folio_batch *fb) { return fb->nr; }
static inline unsigned int folio_batch_space(struct folio_batch *fb) { return PAGEVEC_SIZE - fb->nr; }
static inline unsigned folio_batch_add(struct folio_batch *fb, struct folio *f) { fb->folios[fb->nr++] = f; return folio_batch_space(fb); }
void	__folio_batch_release(struct folio_batch *fb);
static inline void folio_batch_release(struct folio_batch *fb) { if (folio_batch_count(fb)) __folio_batch_release(fb); }

#endif
