#include <u.h>
#include <libc.h>

/*
 * Plan 9's sources under APE (sys/src/ape/lib/9's libc.h, as libdraw's):
 * getenv as Plan 9's, the /env file read now and a copy the caller frees
 * (newwindow.c and scroll.c free it); a list's NULs as blanks; none for
 * what is not there. Prints ok/FAIL lines and "ape9test: all passed".
 */
static int nfail;

static void
check(int ok, char *what, char *got)
{
	print("%s %s: %s\n", ok ? "ok" : "FAIL", what, got != nil ? got : "nil");
	if(!ok)
		nfail++;
}

static void
setenv9(char *name, char *val, int n)
{
	char path[64];
	int fd;

	snprint(path, sizeof path, "/env/%s", name);
	if((fd = create(path, OWRITE, 0666)) < 0)
		sysfatal("create %s: %r", path);
	write(fd, val, n);
	close(fd);
}

void
main(void)
{
	char *s;
	int i;

	/* set after the start: APE's own getenv does not see it */
	setenv9("ape9one", "one", 3);
	s = getenv("ape9one");
	check(s != nil && strcmp(s, "one") == 0, "getenv reads /env now", s);
	free(s);
	/* freed and read again, many times (the old one gave the environment's string) */
	for(i = 0; i < 1000; i++){
		s = getenv("ape9one");
		if(s == nil || strcmp(s, "one") != 0)
			break;
		free(s);
	}
	check(i == 1000, "a copy each time, freed", i == 1000 ? "1000" : "broke");
	setenv9("ape9list", "a\0b\0c\0", 6);
	s = getenv("ape9list");
	check(s != nil && strcmp(s, "a b c") == 0, "a list's NULs as blanks", s);
	free(s);
	s = getenv("ape9none");
	check(s == nil, "none for what is not there", s);
	if(nfail == 0)
		print("ape9test: all passed\n");
	exits(nfail ? "fail" : nil);
}
