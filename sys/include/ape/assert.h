#pragma lib "/$M/lib/ape/libap.a"

#undef assert
#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#ifdef __cplusplus
extern "C" {
#endif

extern void _assert(char *, unsigned);

#ifdef __cplusplus
}
#endif
#define assert(e) ((e) ? (void)0 : _assert(__FILE__, __LINE__))
#endif /* NDEBUG */

/* C11: <assert.h> defines static_assert (kencc has no _Static_assert) */
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#ifndef static_assert
#define static_assert _Static_assert
#endif
#endif
