#include <u.h>
#include <libc.h>

/*
 * GCC port: front does exec(f, &f+1), relying on 8c/6c passing the
 * variadic arguments contiguously on the stack after f. Under the
 * System V ABI they arrive in registers, so &f+1 points at unrelated
 * stack contents. Collect them with va_arg into an argv array instead.
 */
int
execl(char *f, ...)
{
	va_list arg;
	char **argv;
	int n;

	va_start(arg, f);
	for(n = 0; va_arg(arg, char*) != nil; n++)
		;
	va_end(arg);
	argv = malloc((n+1)*sizeof(char*));
	if(argv == nil)
		return -1;
	va_start(arg, f);
	for(n = 0; (argv[n] = va_arg(arg, char*)) != nil; n++)
		;
	va_end(arg);
	n = exec(f, argv);
	free(argv);
	return n;
}
