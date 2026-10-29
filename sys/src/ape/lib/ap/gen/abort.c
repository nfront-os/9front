#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#ifdef __GNUC_MINOR__
/* GCC port: as in plan9/_exit.c: a native program (Rust's std) ends whole */
extern _Noreturn void __plan9_exit(char *) __attribute__((weak));
#endif

_Noreturn void
abort(void)
{
#ifdef __GNUC_MINOR__
	if(__plan9_exit)
		__plan9_exit("abort");
#endif
	kill(getpid(), SIGABRT);
}
