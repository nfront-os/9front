#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern	char **environ;

/* POSIX setenv and unsetenv over putenv's environ */

static int
badname(const char *name)
{
	if(name == NULL || *name == '\0' || strchr(name, '=') != NULL){
		errno = EINVAL;
		return 1;
	}
	return 0;
}

int
setenv(const char *name, const char *val, int overwrite)
{
	char *s;
	size_t n;

	if(badname(name))
		return -1;
	if(!overwrite && getenv(name) != NULL)
		return 0;
	n = strlen(name);
	s = malloc(n + 1 + strlen(val) + 1);
	if(s == NULL){
		errno = ENOMEM;
		return -1;
	}
	memcpy(s, name, n);
	s[n] = '=';
	strcpy(s+n+1, val);
	return putenv(s);
}

int
unsetenv(const char *name)
{
	char **p, **q;
	size_t n;

	if(badname(name))
		return -1;
	n = strlen(name);
	for(p = q = environ; *p != NULL; p++)
		if(strncmp(*p, name, n) != 0 || (*p)[n] != '=')
			*q++ = *p;
	*q = NULL;
	return 0;
}
