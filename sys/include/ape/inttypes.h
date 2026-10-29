#ifndef _INTTYPES_H_
#define _INTTYPES_H_ 1

#include <stdint.h>

#define PRId8 "d"
#define PRId16 "d"
#define PRId32 "d"
#define PRId64 "lld"

#define PRIo8 "o"
#define PRIo16 "o"
#define PRIo32 "o"
#define PRIo64 "llo"

#define PRIx8 "x"
#define PRIx16 "x"
#define PRIx32 "x"
#define PRIx64 "llx"

#define PRIu8 "u"
#define PRIu16 "u"
#define PRIu32 "u"
#define PRIu64 "llu"

extern intmax_t strtoimax(const char *, char **, int);

#if defined(__GNUC__) || defined(_APE_GNU)
/*
 * GCC port, for the gcc driver's programs: the <inttypes.h> macros of C99 APE's lacks (PRIi, PRIX,
 * the SCN family, the MAX/PTR, LEAST/FAST variants), for APE's types:
 * int8..int32 int (promoted), 64 bit long long, intptr_t long on
 * amd64 and arm64 (the compiler's) and int on 386; intmax_t the compiler's (<stdint.h>: j).
 */


#ifndef PRId8
#define PRId8 "d"
#endif
#ifndef PRIdLEAST8
#define PRIdLEAST8 "d"
#endif
#ifndef PRIdFAST8
#define PRIdFAST8 "d"
#endif
#ifndef PRId16
#define PRId16 "d"
#endif
#ifndef PRIdLEAST16
#define PRIdLEAST16 "d"
#endif
#ifndef PRIdFAST16
#define PRIdFAST16 "d"
#endif
#ifndef PRId32
#define PRId32 "d"
#endif
#ifndef PRIdLEAST32
#define PRIdLEAST32 "d"
#endif
#ifndef PRIdFAST32
#define PRIdFAST32 "d"
#endif
#ifndef PRId64
#define PRId64 "lld"
#endif
#ifndef PRIdLEAST64
#define PRIdLEAST64 "lld"
#endif
#ifndef PRIdFAST64
#define PRIdFAST64 "lld"
#endif
#ifndef PRIdMAX
#define PRIdMAX "jd"
#endif
#ifndef PRIdPTR
#if __SIZEOF_POINTER__ == 8
#define PRIdPTR "ld"
#else
#define PRIdPTR "d"
#endif
#endif
#ifndef PRIi8
#define PRIi8 "i"
#endif
#ifndef PRIiLEAST8
#define PRIiLEAST8 "i"
#endif
#ifndef PRIiFAST8
#define PRIiFAST8 "i"
#endif
#ifndef PRIi16
#define PRIi16 "i"
#endif
#ifndef PRIiLEAST16
#define PRIiLEAST16 "i"
#endif
#ifndef PRIiFAST16
#define PRIiFAST16 "i"
#endif
#ifndef PRIi32
#define PRIi32 "i"
#endif
#ifndef PRIiLEAST32
#define PRIiLEAST32 "i"
#endif
#ifndef PRIiFAST32
#define PRIiFAST32 "i"
#endif
#ifndef PRIi64
#define PRIi64 "lli"
#endif
#ifndef PRIiLEAST64
#define PRIiLEAST64 "lli"
#endif
#ifndef PRIiFAST64
#define PRIiFAST64 "lli"
#endif
#ifndef PRIiMAX
#define PRIiMAX "ji"
#endif
#ifndef PRIiPTR
#if __SIZEOF_POINTER__ == 8
#define PRIiPTR "li"
#else
#define PRIiPTR "i"
#endif
#endif
#ifndef PRIo8
#define PRIo8 "o"
#endif
#ifndef PRIoLEAST8
#define PRIoLEAST8 "o"
#endif
#ifndef PRIoFAST8
#define PRIoFAST8 "o"
#endif
#ifndef PRIo16
#define PRIo16 "o"
#endif
#ifndef PRIoLEAST16
#define PRIoLEAST16 "o"
#endif
#ifndef PRIoFAST16
#define PRIoFAST16 "o"
#endif
#ifndef PRIo32
#define PRIo32 "o"
#endif
#ifndef PRIoLEAST32
#define PRIoLEAST32 "o"
#endif
#ifndef PRIoFAST32
#define PRIoFAST32 "o"
#endif
#ifndef PRIo64
#define PRIo64 "llo"
#endif
#ifndef PRIoLEAST64
#define PRIoLEAST64 "llo"
#endif
#ifndef PRIoFAST64
#define PRIoFAST64 "llo"
#endif
#ifndef PRIoMAX
#define PRIoMAX "jo"
#endif
#ifndef PRIoPTR
#if __SIZEOF_POINTER__ == 8
#define PRIoPTR "lo"
#else
#define PRIoPTR "o"
#endif
#endif
#ifndef PRIu8
#define PRIu8 "u"
#endif
#ifndef PRIuLEAST8
#define PRIuLEAST8 "u"
#endif
#ifndef PRIuFAST8
#define PRIuFAST8 "u"
#endif
#ifndef PRIu16
#define PRIu16 "u"
#endif
#ifndef PRIuLEAST16
#define PRIuLEAST16 "u"
#endif
#ifndef PRIuFAST16
#define PRIuFAST16 "u"
#endif
#ifndef PRIu32
#define PRIu32 "u"
#endif
#ifndef PRIuLEAST32
#define PRIuLEAST32 "u"
#endif
#ifndef PRIuFAST32
#define PRIuFAST32 "u"
#endif
#ifndef PRIu64
#define PRIu64 "llu"
#endif
#ifndef PRIuLEAST64
#define PRIuLEAST64 "llu"
#endif
#ifndef PRIuFAST64
#define PRIuFAST64 "llu"
#endif
#ifndef PRIuMAX
#define PRIuMAX "ju"
#endif
#ifndef PRIuPTR
#if __SIZEOF_POINTER__ == 8
#define PRIuPTR "lu"
#else
#define PRIuPTR "u"
#endif
#endif
#ifndef PRIx8
#define PRIx8 "x"
#endif
#ifndef PRIxLEAST8
#define PRIxLEAST8 "x"
#endif
#ifndef PRIxFAST8
#define PRIxFAST8 "x"
#endif
#ifndef PRIx16
#define PRIx16 "x"
#endif
#ifndef PRIxLEAST16
#define PRIxLEAST16 "x"
#endif
#ifndef PRIxFAST16
#define PRIxFAST16 "x"
#endif
#ifndef PRIx32
#define PRIx32 "x"
#endif
#ifndef PRIxLEAST32
#define PRIxLEAST32 "x"
#endif
#ifndef PRIxFAST32
#define PRIxFAST32 "x"
#endif
#ifndef PRIx64
#define PRIx64 "llx"
#endif
#ifndef PRIxLEAST64
#define PRIxLEAST64 "llx"
#endif
#ifndef PRIxFAST64
#define PRIxFAST64 "llx"
#endif
#ifndef PRIxMAX
#define PRIxMAX "jx"
#endif
#ifndef PRIxPTR
#if __SIZEOF_POINTER__ == 8
#define PRIxPTR "lx"
#else
#define PRIxPTR "x"
#endif
#endif
#ifndef PRIX8
#define PRIX8 "X"
#endif
#ifndef PRIXLEAST8
#define PRIXLEAST8 "X"
#endif
#ifndef PRIXFAST8
#define PRIXFAST8 "X"
#endif
#ifndef PRIX16
#define PRIX16 "X"
#endif
#ifndef PRIXLEAST16
#define PRIXLEAST16 "X"
#endif
#ifndef PRIXFAST16
#define PRIXFAST16 "X"
#endif
#ifndef PRIX32
#define PRIX32 "X"
#endif
#ifndef PRIXLEAST32
#define PRIXLEAST32 "X"
#endif
#ifndef PRIXFAST32
#define PRIXFAST32 "X"
#endif
#ifndef PRIX64
#define PRIX64 "llX"
#endif
#ifndef PRIXLEAST64
#define PRIXLEAST64 "llX"
#endif
#ifndef PRIXFAST64
#define PRIXFAST64 "llX"
#endif
#ifndef PRIXMAX
#define PRIXMAX "jX"
#endif
#ifndef PRIXPTR
#if __SIZEOF_POINTER__ == 8
#define PRIXPTR "lX"
#else
#define PRIXPTR "X"
#endif
#endif
#ifndef SCNd8
#define SCNd8 "hhd"
#endif
#ifndef SCNdLEAST8
#define SCNdLEAST8 "hhd"
#endif
#ifndef SCNdFAST8
#define SCNdFAST8 "hhd"
#endif
#ifndef SCNd16
#define SCNd16 "hd"
#endif
#ifndef SCNdLEAST16
#define SCNdLEAST16 "hd"
#endif
#ifndef SCNdFAST16
#define SCNdFAST16 "hd"
#endif
#ifndef SCNd32
#define SCNd32 "d"
#endif
#ifndef SCNdLEAST32
#define SCNdLEAST32 "d"
#endif
#ifndef SCNdFAST32
#define SCNdFAST32 "d"
#endif
#ifndef SCNd64
#define SCNd64 "lld"
#endif
#ifndef SCNdLEAST64
#define SCNdLEAST64 "lld"
#endif
#ifndef SCNdFAST64
#define SCNdFAST64 "lld"
#endif
#ifndef SCNdMAX
#define SCNdMAX "jd"
#endif
#ifndef SCNdPTR
#if __SIZEOF_POINTER__ == 8
#define SCNdPTR "ld"
#else
#define SCNdPTR "d"
#endif
#endif
#ifndef SCNi8
#define SCNi8 "hhi"
#endif
#ifndef SCNiLEAST8
#define SCNiLEAST8 "hhi"
#endif
#ifndef SCNiFAST8
#define SCNiFAST8 "hhi"
#endif
#ifndef SCNi16
#define SCNi16 "hi"
#endif
#ifndef SCNiLEAST16
#define SCNiLEAST16 "hi"
#endif
#ifndef SCNiFAST16
#define SCNiFAST16 "hi"
#endif
#ifndef SCNi32
#define SCNi32 "i"
#endif
#ifndef SCNiLEAST32
#define SCNiLEAST32 "i"
#endif
#ifndef SCNiFAST32
#define SCNiFAST32 "i"
#endif
#ifndef SCNi64
#define SCNi64 "lli"
#endif
#ifndef SCNiLEAST64
#define SCNiLEAST64 "lli"
#endif
#ifndef SCNiFAST64
#define SCNiFAST64 "lli"
#endif
#ifndef SCNiMAX
#define SCNiMAX "ji"
#endif
#ifndef SCNiPTR
#if __SIZEOF_POINTER__ == 8
#define SCNiPTR "li"
#else
#define SCNiPTR "i"
#endif
#endif
#ifndef SCNo8
#define SCNo8 "hho"
#endif
#ifndef SCNoLEAST8
#define SCNoLEAST8 "hho"
#endif
#ifndef SCNoFAST8
#define SCNoFAST8 "hho"
#endif
#ifndef SCNo16
#define SCNo16 "ho"
#endif
#ifndef SCNoLEAST16
#define SCNoLEAST16 "ho"
#endif
#ifndef SCNoFAST16
#define SCNoFAST16 "ho"
#endif
#ifndef SCNo32
#define SCNo32 "o"
#endif
#ifndef SCNoLEAST32
#define SCNoLEAST32 "o"
#endif
#ifndef SCNoFAST32
#define SCNoFAST32 "o"
#endif
#ifndef SCNo64
#define SCNo64 "llo"
#endif
#ifndef SCNoLEAST64
#define SCNoLEAST64 "llo"
#endif
#ifndef SCNoFAST64
#define SCNoFAST64 "llo"
#endif
#ifndef SCNoMAX
#define SCNoMAX "jo"
#endif
#ifndef SCNoPTR
#if __SIZEOF_POINTER__ == 8
#define SCNoPTR "lo"
#else
#define SCNoPTR "o"
#endif
#endif
#ifndef SCNu8
#define SCNu8 "hhu"
#endif
#ifndef SCNuLEAST8
#define SCNuLEAST8 "hhu"
#endif
#ifndef SCNuFAST8
#define SCNuFAST8 "hhu"
#endif
#ifndef SCNu16
#define SCNu16 "hu"
#endif
#ifndef SCNuLEAST16
#define SCNuLEAST16 "hu"
#endif
#ifndef SCNuFAST16
#define SCNuFAST16 "hu"
#endif
#ifndef SCNu32
#define SCNu32 "u"
#endif
#ifndef SCNuLEAST32
#define SCNuLEAST32 "u"
#endif
#ifndef SCNuFAST32
#define SCNuFAST32 "u"
#endif
#ifndef SCNu64
#define SCNu64 "llu"
#endif
#ifndef SCNuLEAST64
#define SCNuLEAST64 "llu"
#endif
#ifndef SCNuFAST64
#define SCNuFAST64 "llu"
#endif
#ifndef SCNuMAX
#define SCNuMAX "ju"
#endif
#ifndef SCNuPTR
#if __SIZEOF_POINTER__ == 8
#define SCNuPTR "lu"
#else
#define SCNuPTR "u"
#endif
#endif
#ifndef SCNx8
#define SCNx8 "hhx"
#endif
#ifndef SCNxLEAST8
#define SCNxLEAST8 "hhx"
#endif
#ifndef SCNxFAST8
#define SCNxFAST8 "hhx"
#endif
#ifndef SCNx16
#define SCNx16 "hx"
#endif
#ifndef SCNxLEAST16
#define SCNxLEAST16 "hx"
#endif
#ifndef SCNxFAST16
#define SCNxFAST16 "hx"
#endif
#ifndef SCNx32
#define SCNx32 "x"
#endif
#ifndef SCNxLEAST32
#define SCNxLEAST32 "x"
#endif
#ifndef SCNxFAST32
#define SCNxFAST32 "x"
#endif
#ifndef SCNx64
#define SCNx64 "llx"
#endif
#ifndef SCNxLEAST64
#define SCNxLEAST64 "llx"
#endif
#ifndef SCNxFAST64
#define SCNxFAST64 "llx"
#endif
#ifndef SCNxMAX
#define SCNxMAX "jx"
#endif
#ifndef SCNxPTR
#if __SIZEOF_POINTER__ == 8
#define SCNxPTR "lx"
#else
#define SCNxPTR "x"
#endif
#endif

/* C99's functions APE lacks (ap/gnu; it has strtoimax); C99
 * has <inttypes.h> define wchar_t (C++: the keyword) */
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { intmax_t quot, rem; } imaxdiv_t;
extern uintmax_t	strtoumax(const char *, char **, int);
extern intmax_t	imaxabs(intmax_t);
extern imaxdiv_t	imaxdiv(intmax_t, intmax_t);
/* musl's (ape/lib/musl) */
extern intmax_t	wcstoimax(const wchar_t *, wchar_t **, int);
extern uintmax_t	wcstoumax(const wchar_t *, wchar_t **, int);
#ifdef __cplusplus
}
#endif
#endif
#endif
