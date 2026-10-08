#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

extern char **environ;

/*
 * GCC port: front passes &arg0 as argv, relying on 6c passing the
 * variadic arguments contiguously on the stack after arg0. Under the
 * System V ABI they arrive in registers; _execlargv collects them with
 * va_arg into an argv array instead (as libc/port/execl.c).
 */
static char**
_execlargv(const char *arg0, va_list arg, char ***envp)
{
	va_list a;
	char **argv;
	int n;

	va_copy(a, arg);
	for(n = 1; arg0 != 0 && va_arg(a, char*) != 0; n++)
		;
	va_end(a);
	argv = malloc((n+1)*sizeof(char*));
	if(argv == 0)
		return 0;
	argv[0] = (char*)arg0;
	for(n = 1; arg0 != 0 && (argv[n] = va_arg(arg, char*)) != 0; n++)
		;
	argv[n] = 0;
	if(envp != 0)
		*envp = va_arg(arg, char**);
	return argv;
}

int
execl(const char *name, const char *arg0, ...)
{
	va_list arg;
	char **argv;
	int n;

	va_start(arg, arg0);
	argv = _execlargv(arg0, arg, 0);
	va_end(arg);
	if(argv == 0)
		return -1;
	n = execve(name, argv, environ);
	free(argv);
	return n;
}
