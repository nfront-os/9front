#include <u.h>
#include <libc.h>

Waitmsg*
system(char *name, char **argv)
{
	char err[ERRMAX];
	Waitmsg *w;
	int pid;

	switch(pid = fork()){	/* assign = */
	case -1:
		return nil;
	case 0:
		exec(name, argv);
		errstr(err, sizeof err);
		_exits(err);
	}
	for(;;){
		w = wait();
		if(w == nil)
			break;
		if(w->pid == pid)
			return w;
		free(w);
	}
	return nil;
}

/*
 * GCC port: front passes &name+1 as the argument vector, relying on 6c
 * passing the variadic arguments contiguously on the stack; collect
 * them with va_arg instead (as libc/port/execl.c).
 */
Waitmsg*
systeml(char *name, ...)
{
	va_list arg;
	char *argv[64];
	int n;

	va_start(arg, name);
	for(n = 0; n < nelem(argv)-1 && (argv[n] = va_arg(arg, char*)) != nil; n++)
		;
	argv[n] = nil;
	va_end(arg);
	return system(name, argv);
}
