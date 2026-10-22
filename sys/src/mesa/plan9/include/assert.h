/* SPDX-License-Identifier: MIT */
/* C11's static_assert, which APE's <assert.h> lacks */
#include_next <assert.h>

#if !defined(__cplusplus) && !defined(static_assert)
#define static_assert _Static_assert
#endif
