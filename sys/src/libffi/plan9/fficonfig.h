/*
 * SPDX-License-Identifier: MIT
 *
 * libffi's configuration for 9front (configure's fficonfig.h, by hand):
 * amd64, APE with ../9front-gcc's cross gcc (the System V ABI, GNU as)
 */
#define STDC_HEADERS 1
#define HAVE_MEMCPY 1
#define HAVE_AS_CFI_PSEUDO_OP 1
#define HAVE_AS_X86_PCREL 1
#define HAVE_AS_X86_64_UNWIND_SECTION_TYPE 1
#define HAVE_HIDDEN_VISIBILITY_ATTRIBUTE 1
#define EH_FRAME_FLAGS "a"
#define SIZEOF_DOUBLE 8
#define SIZEOF_LONG_DOUBLE 16
#define SIZEOF_SIZE_T 8

#ifdef HAVE_HIDDEN_VISIBILITY_ATTRIBUTE
#ifdef LIBFFI_ASM
#define FFI_HIDDEN(name) .hidden name
#else
#define FFI_HIDDEN __attribute__ ((visibility ("hidden")))
#endif
#else
#ifdef LIBFFI_ASM
#define FFI_HIDDEN(name)
#else
#define FFI_HIDDEN
#endif
#endif
