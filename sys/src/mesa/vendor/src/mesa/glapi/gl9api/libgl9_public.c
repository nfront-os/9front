/* Copyright (C) 2010 LunarG Inc.
 * SPDX-License-Identifier: MIT
 *
 * Authors:
 *    Chia-I Wu <olv@lunarg.com>
 *
 * 9front: es2api's libgles2_public.c for desktop GL's entry points (1.0
 * to 4.6, as glvnd's libOpenGL) and GLES 2's to 3.2, the gl* symbols of a
 * program linked with Mesa; there is no GLX or glvnd to give them.
 */

#include "glapi/glapi_priv.h"

#if defined(_GLAPI_ENTRY_ARCH_TLS_H)
#include _GLAPI_ENTRY_ARCH_TLS_H
#define MAPI_TMP_STUB_ASM_GCC_NO_HIDDEN
#else
/* C version of the public entries */
#define MAPI_TMP_DEFINES
#define MAPI_TMP_PUBLIC_ENTRIES_NO_HIDDEN
#endif /* defined(_GLAPI_ENTRY_ARCH_TLS_H) */

#include "gl9_glapi_mapi_tmp.h"
