#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/*
 * getenv as Plan 9's libc has it, for Plan 9's sources built
 * under APE (libc.h here defines getenv to it): the /env file read now (not
 * the environment of the program's start), a copy of its own that the
 * caller frees (newwindow.c and scroll.c of libdraw free it: APE's getenv
 * gave its environment's string, freed and then used again); the NULs
 * between a list's elements as blanks.
 */
char*
_getenv9(char *name)
{
	char path[128], *s, *p;
	int f, n, r;

	if(name[0] == '\0' || strcmp(name, ".") == 0 || strcmp(name, "..") == 0
	|| strchr(name, '/') != NULL || strlen(name) >= sizeof path - 6)
		return NULL;
	snprintf(path, sizeof path, "/env/%s", name);
	if((f = open(path, O_RDONLY)) < 0)
		return NULL;
	n = 0;
	s = NULL;
	for(;;){
		if((p = realloc(s, n+128+1)) == NULL){
			free(s);
			close(f);
			return NULL;
		}
		s = p;
		if((r = read(f, s+n, 128)) <= 0)
			break;
		n += r;
	}
	close(f);
	if(r < 0){
		free(s);
		return NULL;
	}
	while(n > 0 && s[n-1] == '\0')
		n--;
	s[n] = '\0';
	while(--n >= 0)
		if(s[n] == '\0')
			s[n] = ' ';
	return s;
}
