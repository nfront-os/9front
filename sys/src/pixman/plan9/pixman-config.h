/*
 * pixman's configuration for APE (meson's pixman-config.h): SSE2 (amd64 has it
 * always; SSSE3 and MMX left out: their files want flags of their own),
 * thread-local data by __thread with pthreads, the implementation chosen
 * on first use (no constructor: APE's start runs none)
 */
#define PACKAGE "pixman"
#define HAVE_PTHREADS 1
#define TLS __thread
#define USE_SSE2 1
#define HAVE_BUILTIN_CLZ 1
#define HAVE_GCC_VECTOR_EXTENSIONS 1
#define SIZEOF_LONG 8
