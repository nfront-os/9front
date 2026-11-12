/* Public domain. */

#ifndef _LINUX_SEQLOCK_H
#define _LINUX_SEQLOCK_H

#include <sys/types.h>
#include <sys/mutex.h>
#include <sys/atomic.h>
#include <linux/lockdep.h>
#include <linux/processor.h>
#include <linux/preempt.h>
#include <linux/compiler.h>

typedef struct {
	unsigned int sequence;
} seqcount_t;

static inline void
__seqcount_init(seqcount_t *s, const char *name,
    struct lock_class_key *key)
{
	s->sequence = 0;
}

static inline void
seqcount_init(seqcount_t *s)
{
	__seqcount_init(s, NULL, NULL);
}

static inline unsigned int
__read_seqcount_begin(const seqcount_t *s)
{
	unsigned int r;
	for (;;) {
		r = s->sequence;
		if ((r & 1) == 0)
			break;
		cpu_relax();
	}
	return r;
}

static inline unsigned int
__obsd_read_seqcount_begin(const seqcount_t *s)
{
	unsigned int r = __read_seqcount_begin(s);
	membar_consumer();
	return r;
}

static inline int
__read_seqcount_retry(const seqcount_t *s, unsigned start)
{
	return (s->sequence != start);
}

static inline int
__obsd_read_seqcount_retry(const seqcount_t *s, unsigned start)
{
	membar_consumer();
	return __read_seqcount_retry(s, start);
}

static inline void
__obsd_write_seqcount_begin(seqcount_t *s)
{
	s->sequence++;
	membar_producer();
}

static inline void
__obsd_write_seqcount_end(seqcount_t *s)
{
	membar_producer();
	s->sequence++;
}

static inline unsigned int
__obsd_raw_read_seqcount(const seqcount_t *s)
{
	unsigned int r = s->sequence;
	membar_consumer();
	return r;
}

typedef struct {
	unsigned int seq;
	struct spinlock lock;
} seqlock_t;

/* 9front: Linux's one argument */
static inline void
seqlock_init(seqlock_t *sl)
{
	sl->seq = 0;
	mtx_init(&sl->lock, IPL_NONE);
}

static inline void
write_seqlock(seqlock_t *sl)
{
	mtx_enter(&sl->lock);
	sl->seq++;
	membar_producer();
}

static inline void
__write_seqlock_irqsave(seqlock_t *sl)
{
	mtx_enter(&sl->lock);
	sl->seq++;
	membar_producer();
}
#define write_seqlock_irqsave(_sl, _flags) do {			\
		_flags = 0;					\
		__write_seqlock_irqsave(_sl);			\
	} while (0)

static inline void
write_sequnlock(seqlock_t *sl)
{
	membar_producer();
	sl->seq++;
	mtx_leave(&sl->lock);
}

static inline void
__write_sequnlock_irqrestore(seqlock_t *sl)
{
	membar_producer();
	sl->seq++;
	mtx_leave(&sl->lock);
}
#define write_sequnlock_irqrestore(_sl, _flags) do {		\
		(void)(_flags);					\
		__write_sequnlock_irqrestore(_sl);		\
	} while (0)

static inline unsigned int
read_seqbegin(seqlock_t *sl)
{
	return READ_ONCE(sl->seq);
}

static inline unsigned int
read_seqretry(seqlock_t *sl, unsigned int pos)
{
	return sl->seq != pos;
}

typedef struct {
	seqcount_t seq;
	struct mutex *lock;	/* 9front: Linux's, a pointer to the mutex */
} seqcount_mutex_t;

#define seqcount_mutex_init(s, l)	do { seqcount_init(&(s)->seq); (s)->lock = (l); } while (0)

/*
 * 9front: the Linux code passes a seqcount_t or a seqcount_mutex_t to
 * the same operations (Linux's _Generic); OpenBSD's take the seqcount_t
 */
#define __seqcount_ptr(s)	_Generic((s),				\
	seqcount_t *: (seqcount_t *)(s),				\
	const seqcount_t *: (const seqcount_t *)(s),			\
	seqcount_mutex_t *: &((seqcount_mutex_t *)(s))->seq,		\
	const seqcount_mutex_t *: &((const seqcount_mutex_t *)(s))->seq)
#define read_seqcount_begin(s)		__obsd_read_seqcount_begin(__seqcount_ptr(s))
#define read_seqcount_retry(s, n)	__obsd_read_seqcount_retry(__seqcount_ptr(s), n)
#define write_seqcount_begin(s)		__obsd_write_seqcount_begin((seqcount_t *)__seqcount_ptr(s))
#define write_seqcount_end(s)		__obsd_write_seqcount_end((seqcount_t *)__seqcount_ptr(s))
#define raw_read_seqcount(s)		__obsd_raw_read_seqcount(__seqcount_ptr(s))

static inline unsigned int
seqprop_sequence(const seqcount_mutex_t *sm)
{
	return READ_ONCE(sm->seq.sequence);
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/atomic.h>
typedef seqcount_t seqcount_spinlock_t;
#define SEQCNT_ZERO(n)			{ 0 }
#define seqcount_spinlock_init(s, l)	seqcount_init(s)
#define raw_read_seqcount_begin(s)	read_seqcount_begin(s)
#define raw_write_seqcount_begin(s)	write_seqcount_begin(s)
#define raw_write_seqcount_end(s)	write_seqcount_end(s)
#define write_seqcount_invalidate(s)	do { ((seqcount_t *)__seqcount_ptr(s))->sequence += 2; membar_producer(); } while (0)
#define DEFINE_SEQLOCK(n)		seqlock_t n = { 0, MUTEX_INITIALIZER(IPL_NONE) }
#define write_seqlock_irq(l)			write_seqlock(l)
#define write_sequnlock_irq(l)			write_sequnlock(l)

#endif
