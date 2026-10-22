/*
 * SPDX-License-Identifier: MIT
 *
 * Included before every Mesa source (-include): what meson's checks
 * get wrong for 9front's APE.
 */
#ifndef MESA_PLAN9_H
#define MESA_PLAN9_H

/*
 * gcc knows __builtin_posix_memalign, so meson finds the function,
 * but APE has none, and APE's free() cannot take an aligned pointer
 * inside a block: use Mesa's own fallback (os_memory_aligned.h).
 */
#undef HAVE_POSIX_MEMALIGN

#endif
