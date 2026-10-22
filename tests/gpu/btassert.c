/*
 * SPDX-License-Identifier: MIT
 *
 * APE's _assert with a backtrace (libgcc's unwinder), for debugging
 * tests/gpu/vktest: resolve the addresses with addr2line -e vktest.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unwind.h>

static _Unwind_Reason_Code
frame(struct _Unwind_Context *ctx, void *arg)
{
	int *n = arg;

	fprintf(stderr, " %#lx", (unsigned long)_Unwind_GetIP(ctx));
	return ++*n < 32 ? _URC_NO_REASON : _URC_END_OF_STACK;
}

void
_assert(char *f, unsigned line)
{
	int n = 0;

	fprintf(stderr, "assertion failed: file %s:%u\nbacktrace:", f, line);
	_Unwind_Backtrace(frame, &n);
	fprintf(stderr, "\n");
	abort();
}
