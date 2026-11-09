/*
 * libxkbcommon's configuration for APE (meson's config.h): keymaps come
 * from the compositor as text (wlsys: kbdfs' tables), no XKB data on the
 * system yet; its paths where such data would go.  The parser's code is
 * the host's bison's (scripts/ape/vendor.sh: VENDOR.json's gen).
 */
#define EXIT_INVALID_USAGE 2
#define LIBXKBCOMMON_VERSION "1.13.2"
#define LIBXKBCOMMON_TOOL_PATH "/amd64/lib/xkbcommon"
#define DFLT_XKB_LEGACY_ROOT "/lib/X11/xkb"
#define DFLT_XKB_CONFIG_ROOT "/lib/xkb"
#define DFLT_XKB_CONFIG_EXTRA_PATH "/lib/xkb/extra"
#define XLOCALEDIR "/lib/X11/locale"
#define DEFAULT_XKB_RULES "evdev"
#define DEFAULT_XKB_MODEL "pc105"
#define DEFAULT_XKB_LAYOUT "us"
#define DEFAULT_XKB_VARIANT NULL
#define DEFAULT_XKB_OPTIONS NULL
#define HAVE_UNISTD_H 1
#define HAVE_DIRENT_H 1
#define HAVE_XKB_EXTENSIONS_DIRECTORIES 1
#define HAVE___BUILTIN_EXPECT 1
#define HAVE_MMAP 1
#define HAVE_POSIX_FALLOCATE 1
#define HAVE_STRNDUP 1
#define HAVE_ASPRINTF 1
#define HAVE_VASPRINTF 1
#define HAVE_OPEN_MEMSTREAM 1
#define HAVE_REAL_PATH 1
