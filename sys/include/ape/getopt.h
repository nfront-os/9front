/*
 * APE (GCC port): GNU's getopt_long (libap, ap/gnu/getopt_long.c) on
 * the same optind, optarg, optopt and opterr as getopt (unistd.h).
 */
#ifndef __APE_GETOPT_H
#define __APE_GETOPT_H
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

struct option {
	const char	*name;
	int	has_arg;
	int	*flag;
	int	val;
};

#define no_argument	0
#define required_argument	1
#define optional_argument	2

extern char	*optarg;
extern int	optind, opterr, optopt;
extern int	getopt_long(int, char *const *, const char *, const struct option *, int *);
extern int	getopt_long_only(int, char *const *, const char *, const struct option *, int *);

#ifdef __cplusplus
}
#endif
#endif
