/*
 * APE (ANSI/POSIX environment) unit tests, built with the gcc port
 * toolchain like pcc builds (manifest key Ape) and run inside 9front
 * (tests/guest/run.sh). They cover the amd64 parts of libap the port
 * rewrote: main9.s/_callmain (argv, environ), setjmp.s, the system call
 * stubs (gen-9syscall.sh --ape), the _NOTIFY/notecont trampolines
 * behind signal(), sigsetjmp/siglongjmp out of a handler.
 *
 * Output: one "ok name" or "FAIL name: detail" line per check, then
 * "apetest: N ok, M failed".
 */
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* $cputype of the system the test runs on (tests/guest/run.sh: ARCH=$objtype) */
#if defined(__x86_64__)
#define CPUTYPE	"amd64"
#elif defined(__i386__)
#define CPUTYPE	"386"
#else
#define CPUTYPE	"arm64"
#endif

static int nok, nfail;

static void
check(char *name, int ok, char *fmt, ...)
{
	char buf[256];
	va_list arg;

	if(ok){
		nok++;
		printf("ok %s\n", name);
		return;
	}
	nfail++;
	buf[0] = 0;
	if(fmt != NULL){
		va_start(arg, fmt);
		vsnprintf(buf, sizeof buf, fmt, arg);
		va_end(arg);
	}
	printf("FAIL %s: %s\n", name, buf);
}

static void
testmain(int argc, char **argv, char **envp)
{
	check("argv[0]", argc >= 1 && argv[0] != NULL && strstr(argv[0], "apetest") != NULL,
		"argc %d argv[0] %s", argc, argc > 0 ? argv[0] : "nil");
	check("argv[argc] == NULL", argv[argc] == NULL, NULL);
	check("environ/getenv", envp != NULL && getenv("cputype") != NULL && strcmp(getenv("cputype"), CPUTYPE) == 0,
		"%s", getenv("cputype") ? getenv("cputype") : "nil");
}

static void
teststdio(void)
{
	char buf[64];
	FILE *f;
	double d;

	snprintf(buf, sizeof buf, "%d %ld %lld %x %5.2f %s", -5, -6L, -7LL, 255, 3.14159, "ab");
	check("snprintf", strcmp(buf, "-5 -6 -7 ff  3.14 ab") == 0, "%s", buf);
	d = strtod("2.5e3", NULL);
	check("strtod", d == 2500.0, "%g", d);
	f = fopen("/tmp/apetest", "w+");
	check("fopen", f != NULL, NULL);
	if(f == NULL)
		return;
	fprintf(f, "line %d\n", 42);
	rewind(f);
	buf[0] = 0;
	fgets(buf, sizeof buf, f);
	check("fprintf/rewind/fgets", strcmp(buf, "line 42\n") == 0, "%s", buf);
	fclose(f);
	{
		struct stat st;
		check("stat", stat("/tmp/apetest", &st) == 0 && st.st_size == 8, "size %lld", (long long)st.st_size);
	}
	unlink("/tmp/apetest");
}

static jmp_buf jb;

static void
deep(int n)
{
	if(n == 0)
		longjmp(jb, 0);	/* ansi: longjmp(0) returns 1 */
	deep(n-1);
}

static void
testjmp(void)
{
	volatile int v;
	int r;

	v = 1;
	r = setjmp(jb);
	if(r == 0){
		v = 2;
		deep(5);
	}
	check("setjmp/longjmp(0) => 1", r == 1 && v == 2, "r=%d v=%d", r, v);
}

static volatile int gotsig;
static sigjmp_buf sjb;

static void
onusr1(int sig)
{
	gotsig = sig;
}

static void
onalrm(int sig)
{
	siglongjmp(sjb, sig);
}

static void
testsignal(void)
{
	int r;

	signal(SIGUSR1, onusr1);
	gotsig = 0;
	kill(getpid(), SIGUSR1);
	sleep(1);
	check("signal handler runs and returns", gotsig == SIGUSR1, "gotsig %d", gotsig);

	signal(SIGALRM, onalrm);
	r = sigsetjmp(sjb, 1);
	if(r == 0){
		alarm(1);
		for(;;)
			sleep(10);
	}
	check("siglongjmp out of a handler", r == SIGALRM, "r=%d", r);
}

static void
testfork(void)
{
	int p[2], n, st;
	char buf[32];
	pid_t pid;

	pipe(p);
	pid = fork();
	if(pid == 0){
		close(p[0]);
		dup2(p[1], 1);
		execl("/bin/echo", "echo", "ape", "exec", NULL);
		_exit(1);
	}
	close(p[1]);
	n = read(p[0], buf, sizeof buf - 1);
	buf[n > 0 ? n : 0] = 0;
	close(p[0]);
	check("fork/execl/pipe/waitpid", waitpid(pid, &st, 0) == pid && strcmp(buf, "ape exec\n") == 0,
		"%s", buf);
}

int
main(int argc, char **argv, char **envp)
{
	testmain(argc, argv, envp);
	teststdio();
	testjmp();
	testsignal();
	testfork();
	printf("apetest: %d ok, %d failed\n", nok, nfail);
	return nfail != 0;
}
