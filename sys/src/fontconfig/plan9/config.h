/*
 * fontconfig's configuration for APE (meson's config.h): fonts in
 * /lib/font/ttf, its configuration in /lib/fontconfig (fonts.conf, conf.d),
 * the system's cache in /lib/fontconfig/cache (else the user's in
 * $HOME/.cache/fontconfig).  XML by expat, threads by APE's pthreads, no
 * mmap of caches (read into memory).  The generated headers are the
 * host's gperf's and the release's scripts' (scripts/ape/vendor.sh:
 * VENDOR.json's gen).
 */
#define FC_VERSION_MAJOR 2
#define FC_VERSION_MINOR 18
#define FC_VERSION_MICRO 3
#define PACKAGE_NAME "fontconfig"
#define PACKAGE_TARNAME "fontconfig"
#define PACKAGE_VERSION "2.18.3"
#define PACKAGE_STRING "fontconfig 2.18.3"
#define PACKAGE_BUGREPORT "https://gitlab.freedesktop.org/fontconfig/fontconfig/issues/new"
#define PACKAGE_URL ""
#define GETTEXT_PACKAGE "fontconfig"
#define ENABLE_FREETYPE 1
#define FREETYPE_PCF_LONG_FAMILY_NAMES 0
#define WORDS_BIGENDIAN 0
#define EXEEXT ""
#define USE_ICONV 0

#define FC_DEFAULT_FONTS "\t<dir>/lib/font/ttf</dir>\n"
#define FC_FONTPATH ""
#define CONFIGDIR "/lib/fontconfig/conf.d"
#define FC_CACHEDIR "/lib/fontconfig/cache"
#define FC_TEMPLATEDIR "/lib/fontconfig/conf.avail"
#define FONTCONFIG_PATH "/lib/fontconfig"

#define HAVE_DIRENT_H 1
#define HAVE_FCNTL_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_STATVFS_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_TIME_H 1
#define HAVE_WCHAR_H 1
#define HAVE_SCHED_H 1
#define HAVE_SCHED_YIELD 1

#define HAVE_LINK 1
#define HAVE_SYMLINK 1
#define HAVE_MKSTEMP 1
#define HAVE_GETOPT 1
#define HAVE_RAND 1
#define HAVE_READLINK 1
#define HAVE_FSTATVFS 1
#define HAVE_LSTAT 1
#define HAVE_STRERROR 1
#define HAVE_VASPRINTF 1
#define HAVE_VPRINTF 1
#define HAVE_VSNPRINTF 1
#define HAVE_VSPRINTF 1
#define HAVE_GETPAGESIZE 1
#define HAVE_GETPID 1
#define HAVE_LOCALTIME_R 1
#define HAVE_STRDUP 1
#define HAVE_USELOCALE 1	/* APE: ap/gnu/newlocale.c, only C */

#define HAVE_FT_GET_BDF_PROPERTY 1
#define HAVE_FT_GET_PS_FONT_INFO 1
#define HAVE_FT_HAS_PS_GLYPH_NAMES 1
#define HAVE_FT_GET_X11_FONT_FORMAT 1
#define HAVE_FT_DONE_MM_VAR 1

#define SIZEOF_VOID_P 8
#define ALIGNOF_VOID_P 8
#define ALIGNOF_DOUBLE 8
#define FLEXIBLE_ARRAY_MEMBER /**/
#define HAVE_INTEL_ATOMIC_PRIMITIVES 1
#define HAVE_PTHREAD 1
#define HAVE_C99_VSNPRINTF 1
#define FC_GPERF_SIZE_T size_t
#define _GNU_SOURCE 1

#include "config-fixups.h"
