/* dlopen & co.: 9front has no dynamic loading */
#include <stddef.h>
#include <dlfcn.h>

static char *err;

void*
dlopen(const char *file, int mode)
{
	(void)file;
	(void)mode;
	err = "dlopen: no dynamic loading on 9front";
	return NULL;
}

void*
dlsym(void *h, const char *name)
{
	(void)h;
	(void)name;
	err = "dlsym: no dynamic loading on 9front";
	return NULL;
}

int
dlclose(void *h)
{
	(void)h;
	err = "dlclose: no dynamic loading on 9front";
	return -1;
}

char*
dlerror(void)
{
	char *e;

	e = err;
	err = NULL;
	return e;
}
