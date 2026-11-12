/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024-2026 The FreeBSD Foundation
 *
 * This software was developed by Björn Zeeb under sponsorship from
 * the FreeBSD Foundation.
 */
/*
 * LinuxKPI for 9front: FreeBSD's linux/cleanup.h (freebsd/, 584f44221f45),
 * changed where marked 9front: the names Linux's drivers write themselves
 * (class_<n>_t, _constructor, _destructor: ttm_bo.h and gpu_scheduler.h
 * define classes of their own), DEFINE_CLASS under that name, unique
 * names per use, no_free_ptr.
 */

#ifndef	_LINUXKPI_LINUX_CLEANUP_H
#define	_LINUXKPI_LINUX_CLEANUP_H

#include <linux/compiler_types.h>
#include <linux/err.h>

/* 9front: the drivers' own classes use these names */
#define	CLEANUP_NAME(_n, _s)	class_##_n##_s

/* 9front: a name of its own for each use (__COUNTER__ expanded first) */
#define	_CLEANUP_PASTE(_a, _b)	_a##_b
#define	_CLEANUP_UNIQUE(_a, _b)	_CLEANUP_PASTE(_a, _b)
#define	CLEANUP_UNIQUE(_p)	_CLEANUP_UNIQUE(_p, __COUNTER__)

#define	DECLARE(_n, _x)							\
    CLEANUP_NAME(_n, _t) _x __cleanup(CLEANUP_NAME(_n, _destructor)) =	\
	CLEANUP_NAME(_n, _constructor)

/*
 * Note: "_T" are special as they are exposed into common code for
 * statements.  Extra care should be taken when changing the code.
 */
#define	DEFINE_CLASS(_name, _type, _exit, _init, _init_args...)		\
    typedef _type class_##_name##_t;					\
									\
    static inline __maybe_unused _type					\
    class_##_name##_constructor(_init_args)				\
    {									\
	_type v = _init;						\
	return (v);							\
    }									\
									\
    static inline __maybe_unused void					\
    class_##_name##_destructor(_type *p)				\
    {									\
	_type _T = *p;							\
	_exit;								\
    }

#define	CLASS(_name, _var)						\
    class_##_name##_t _var __cleanup(class_##_name##_destructor) =	\
	class_##_name##_constructor

/*
 * Note: "_T" are special as they are exposed into common code for
 * statements.  Extra care should be taken when changing the code.
 */
#define	DEFINE_GUARD(_n, _dt, _lock, _unlock)				\
									\
    typedef _dt CLEANUP_NAME(_n, _t);					\
									\
    static inline __maybe_unused _dt					\
    CLEANUP_NAME(_n, _constructor)( _dt _T)				\
    {									\
	_dt c;								\
									\
	c = ({ _lock; _T; });						\
	return (c);							\
    }									\
									\
    static inline __maybe_unused void					\
    CLEANUP_NAME(_n, _destructor)(_dt *t)				\
    {									\
	_dt _T;								\
									\
	_T = *t;							\
	if (_T) { _unlock; };						\
    }

/* We need to keep these calls unique. */
#define	guard(_n)							\
    DECLARE(_n, CLEANUP_UNIQUE(guard_##_n##_))

#define	DEFINE_FREE(_n, _t, _f)						\
    static inline __maybe_unused void					\
    __free_ ## _n(void *p)						\
    {									\
	_t _T;								\
									\
	_T = *(_t *)p;							\
	_f;								\
    }

#define	__free(_n)		__cleanup(__free_##_n)

/* 9front: the value, and NULL left in its place (no free at the end) */
#define	no_free_ptr(_p)							\
    ((__typeof__(_p))({ void *_v = (void *)(_p); (_p) = NULL; _v; }))

/*
 * Our initial version go broken up.  Some simplifications like using
 * "bool" for the lock had to be changed to a more general type.
 * _T is still special and, like other bits, may not always be used,
 * so tag with __unused (or better the LinuxKPI __maybe_unused).
 */
#define	_DEFINE_LOCK_GUARD_0(_n, _lock)					\
    static inline __maybe_unused CLEANUP_NAME(_n, _t)			\
    CLEANUP_NAME(_n, _constructor)(void)				\
    {									\
	CLEANUP_NAME(_n, _t) _tmp;					\
	CLEANUP_NAME(_n, _t) *_T __maybe_unused;			\
									\
	_tmp.lock = (void *)1;						\
	_T = &_tmp;							\
	_lock;								\
	return (_tmp);							\
    }

#define	_DEFINE_LOCK_GUARD_1(_n, _type, _lock)				\
    static inline __maybe_unused CLEANUP_NAME(_n, _t)			\
    CLEANUP_NAME(_n, _constructor)(_type *l)				\
    {									\
	CLEANUP_NAME(_n, _t) _tmp;					\
	CLEANUP_NAME(_n, _t) *_T __maybe_unused;			\
									\
	_tmp.lock = l;							\
	_T = &_tmp;							\
	_lock;								\
	return (_tmp);							\
    }

#define	_GUARD_IS_ERR(_v)						\
    ({									\
	uintptr_t x = (uintptr_t)(void *)(_v);				\
	IS_ERR_VALUE(x);						\
    })

#define	_DEFINE_UNLOCK_GUARD(_n, _type, _unlock, ...)			\
    typedef struct {							\
	_type *lock;							\
	__VA_ARGS__;							\
    } CLEANUP_NAME(_n, _t);	    					\
									\
    static inline __maybe_unused void					\
    CLEANUP_NAME(_n, _destructor)(CLEANUP_NAME(_n, _t) *_T)		\
    {									\
	if (!_GUARD_IS_ERR(_T->lock)) {					\
	    _unlock;							\
	}								\
    }

#define	DEFINE_LOCK_GUARD_0(_n, _lock, _unlock, ...)			\
    _DEFINE_UNLOCK_GUARD(_n, void, _unlock, __VA_ARGS__)		\
    _DEFINE_LOCK_GUARD_0(_n, _lock)

/* This allows the type to be set. */
#define	DEFINE_LOCK_GUARD_1(_n, _t, _lock, _unlock, ...)		\
    _DEFINE_UNLOCK_GUARD(_n, _t, _unlock, __VA_ARGS__)			\
    _DEFINE_LOCK_GUARD_1(_n, _t, _lock)

/* 9front: no conditional guards are used, the block runs once */
#define	_scoped_guard(_n, _l, ...)					\
    for (DECLARE(_n, _scoped)(__VA_ARGS__);				\
	1;								\
	({ goto _l; }))							\
		if (0) {						\
_l:									\
			break;						\
		} else

#define	scoped_guard(_n, ...)						\
    _scoped_guard(_n, CLEANUP_UNIQUE(___label_), ##__VA_ARGS__)

#endif	/* _LINUXKPI_LINUX_CLEANUP_H */
