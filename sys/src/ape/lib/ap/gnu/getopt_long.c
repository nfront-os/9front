/*
 * GNU's getopt_long and getopt_long_only, with getopt's globals
 * (bsd/getopt.c: optind, optarg, optopt, opterr). Options and the
 * arguments are not reordered: the first argument that is not an
 * option ends them (as POSIX getopt, and GNU's with a leading + or
 * POSIXLY_CORRECT); a leading - in opts (GNU: non-options as option 1)
 * is not supported and ignored. A leading : makes a missing argument
 * ':' and quiet. Short options are handled here too, with a position
 * of their own: a program should not mix getopt and getopt_long.
 */
#include <getopt.h>
#include <stdio.h>
#include <string.h>

static int sp = 1;	/* in a group of short options: the next one's place */

static int
longopt(int argc, char *const *argv, const char *arg, const struct option *lo, int *idx, int quiet, int colon)
{
	const char *eq;
	size_t n;
	int i, hit, nhit;

	eq = strchr(arg, '=');
	n = eq != NULL ? (size_t)(eq - arg) : strlen(arg);
	hit = -1;
	nhit = 0;
	for(i = 0; lo[i].name != NULL; i++){
		if(strncmp(lo[i].name, arg, n) != 0)
			continue;
		if(strlen(lo[i].name) == n){	/* exact */
			hit = i;
			nhit = 1;
			break;
		}
		if(hit < 0 || lo[hit].has_arg != lo[i].has_arg || lo[hit].flag != lo[i].flag || lo[hit].val != lo[i].val)
			nhit++;
		if(hit < 0)
			hit = i;
	}
	optind++;
	if(hit < 0){
		if(!quiet)
			fprintf(stderr, "%s: unrecognized option '--%.*s'\n", argv[0], (int)n, arg);
		optopt = 0;
		return '?';
	}
	if(nhit > 1){
		if(!quiet)
			fprintf(stderr, "%s: option '--%.*s' is ambiguous\n", argv[0], (int)n, arg);
		optopt = 0;
		return '?';
	}
	optarg = NULL;
	if(eq != NULL){
		if(lo[hit].has_arg == no_argument){
			if(!quiet)
				fprintf(stderr, "%s: option '--%s' doesn't allow an argument\n", argv[0], lo[hit].name);
			optopt = lo[hit].flag == NULL ? lo[hit].val : 0;
			return '?';
		}
		optarg = (char*)eq + 1;
	}else if(lo[hit].has_arg == required_argument){
		if(optind >= argc){
			if(!quiet)
				fprintf(stderr, "%s: option '--%s' requires an argument\n", argv[0], lo[hit].name);
			optopt = lo[hit].flag == NULL ? lo[hit].val : 0;
			return colon ? ':' : '?';
		}
		optarg = argv[optind++];
	}
	if(idx != NULL)
		*idx = hit;
	if(lo[hit].flag != NULL){
		*lo[hit].flag = lo[hit].val;
		return 0;
	}
	return lo[hit].val;
}

static int
getopt1(int argc, char *const *argv, const char *opts, const struct option *lo, int *idx, int only)
{
	const char *cp, *a;
	int c, quiet, colon;

	if(optind == 0){	/* GNU's way to start again */
		optind = 1;
		sp = 1;
	}
	if(*opts == '+' || *opts == '-')
		opts++;
	colon = *opts == ':';
	quiet = colon || !opterr;
	optarg = NULL;
	if(sp == 1){
		if(optind >= argc || (a = argv[optind])[0] != '-' || a[1] == 0)
			return -1;
		if(strcmp(a, "--") == 0){
			optind++;
			return -1;
		}
		if(a[1] == '-')
			return longopt(argc, argv, a + 2, lo, idx, quiet, colon);
		/* getopt_long_only: -name is long unless it is a short option */
		if(only && (a[2] != 0 || strchr(opts, a[1]) == NULL)){
			const struct option *o;
			size_t n;

			n = strcspn(a + 1, "=");
			for(o = lo; o->name != NULL; o++)
				if(strncmp(o->name, a + 1, n) == 0)
					return longopt(argc, argv, a + 1, lo, idx, quiet, colon);
		}
	}
	a = argv[optind];
	optopt = c = a[sp];
	if(c == ':' || (cp = strchr(opts, c)) == NULL){
		if(!quiet)
			fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], c);
		if(a[++sp] == 0){
			optind++;
			sp = 1;
		}
		return '?';
	}
	if(cp[1] == ':'){
		if(a[sp+1] != 0){
			optarg = (char*)&a[sp+1];
			optind++;
		}else if(cp[2] == ':'){
			optind++;	/* an optional argument only right after the option */
		}else if(++optind >= argc){
			if(!quiet)
				fprintf(stderr, "%s: option requires an argument -- '%c'\n", argv[0], c);
			sp = 1;
			return colon ? ':' : '?';
		}else
			optarg = argv[optind++];
		sp = 1;
	}else if(a[++sp] == 0){
		sp = 1;
		optind++;
	}
	return c;
}

int
getopt_long(int argc, char *const *argv, const char *opts, const struct option *lo, int *idx)
{
	return getopt1(argc, argv, opts, lo, idx, 0);
}

int
getopt_long_only(int argc, char *const *argv, const char *opts, const struct option *lo, int *idx)
{
	return getopt1(argc, argv, opts, lo, idx, 1);
}
