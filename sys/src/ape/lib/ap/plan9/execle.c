#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

/*
 * GCC port: front walks the arguments from &name on the stack (argv at
 * &arg0, envp after its terminating nil), relying on 6c passing them
 * contiguously. Under the System V ABI they arrive in registers; collect
 * them with va_arg instead (as libc/port/execl.c).
 */
int
execle(const char *name, const char *arg0, const char *arg1, ...)
{
	va_list arg, a;
	char **argv, **envp;
	int n;

	va_start(arg, arg1);
	va_copy(a, arg);
	n = 0;
	if(arg0 != 0){
		n++;
		if(arg1 != 0)
			for(n++; va_arg(a, char*) != 0; n++)
				;
	}
	va_end(a);
	argv = malloc((n+1)*sizeof(char*));
	if(argv == 0){
		va_end(arg);
		return -1;
	}
	argv[0] = (char*)arg0;
	if(arg0 == 0)
		envp = (char**)arg1;
	else{
		argv[1] = (char*)arg1;
		if(arg1 != 0)
			for(n = 2; (argv[n] = va_arg(arg, char*)) != 0; n++)
				;
		envp = va_arg(arg, char**);
	}
	va_end(arg);
	n = execve(name, argv, envp);
	free(argv);
	return n;
}
