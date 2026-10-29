/*
 * What gcc's APE adds for the gcc driver's programs (sys/src/ape/lib/ap/gnu,
 * sys/src/ape/lib/musl, the headers' parts under _APE_GNU), run inside 9front
 * (tests/guest/run.sh): C99's printf over musl's engine, the wide functions,
 * aligned blocks (plan9/malloc.c), mmap, POSIX additions, pthreads, select and
 * poll on a pipe written by a thread (gnu/buf.c), and exit with a thread and
 * a copy proc still running.
 *
 * Output: one "ok name" or "FAIL name: detail" line per check, then
 * "apegnutest: N ok, M failed".
 */
#define _BSD_EXTENSION
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <wchar.h>
#include <math.h>

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
teststdio(void)
{
	char buf[128];
	wchar_t wb[32];
	int n;

	n = snprintf(buf, sizeof buf, "%Lf %a %zu %jd %hhd", 1.5L, 1.0, sizeof(int), (intmax_t)-3, (char)300);
	check("C99 printf (long double, %a, z, j, hh)", strcmp(buf, "1.500000 0x1p+0 4 -3 44") == 0 && n == (int)strlen(buf),
		"%s", buf);
	check("snprintf(NULL, 0) counts", snprintf(NULL, 0, "%d", 12345) == 5, NULL);
	check("strtold", strtold("2.5", NULL) == 2.5L, NULL);
	check("C99 math (float, long double)", sqrtf(16.0f) == 4.0f && fabsl(-2.0L) == 2.0L, NULL);
	swprintf(wb, 32, L"%ls-%d", L"ab", 7);
	check("swprintf", wcscmp(wb, L"ab-7") == 0, NULL);
}

static void
testalloc(void)
{
	char *p, *q;
	void *a;

	p = aligned_alloc(64, 100);
	check("aligned_alloc(64)", p != NULL && ((uintptr_t)p & 63) == 0, "%p", p);
	if(p == NULL)
		return;
	memset(p, 1, 100);
	q = realloc(p, 5000);
	check("realloc of an aligned block", q != NULL && q[99] == 1, NULL);
	free(q);
	a = NULL;
	check("posix_memalign(4096)", posix_memalign(&a, 4096, 10000) == 0 && ((uintptr_t)a & 4095) == 0, "%p", a);
	free(a);
	check("posix_memalign(3): EINVAL", posix_memalign(&a, 3, 10) != 0, NULL);
}

static void
testmmap(void)
{
	char *p;
	int fd;

	p = mmap(NULL, 10000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	check("mmap anonymous: zeros", p != MAP_FAILED && p[0] == 0 && p[9999] == 0, NULL);
	if(p != MAP_FAILED){
		p[5] = 'x';
		check("munmap", munmap(p, 10000) == 0, NULL);
	}
	fd = open("/tmp/apegnutest", O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(fd < 0){
		check("mmap of a file", 0, "open: %s", strerror(errno));
		return;
	}
	write(fd, "mapped file", 11);
	p = mmap(NULL, 11, PROT_READ, MAP_PRIVATE, fd, 0);
	check("mmap of a file", p != MAP_FAILED && memcmp(p, "mapped file", 11) == 0, NULL);
	if(p != MAP_FAILED)
		munmap(p, 11);
	close(fd);
	remove("/tmp/apegnutest");
}

static void
testposix(void)
{
	struct rlimit rl;
	size_t big;

	big = SIZE_MAX;	/* not a constant: gcc would warn of the bound */
	check("strncasecmp(SIZE_MAX)", strncasecmp("ABC", "abd", big) < 0, NULL);
	check("strncasecmp(3)", strncasecmp("ABCx", "abcy", 3) == 0, NULL);
	check("fsync", fsync(1) == 0, NULL);
	check("getrlimit(RLIMIT_STACK)", getrlimit(RLIMIT_STACK, &rl) == 0 && rl.rlim_cur > 0, NULL);
}

static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
static long counter;
static int ready, others;
static pthread_t mainself;

static void *
worker(void *v)
{
	int i;

	for(i = 0; i < 20000; i++){
		pthread_mutex_lock(&mu);
		counter++;
		pthread_mutex_unlock(&mu);
	}
	pthread_mutex_lock(&mu);
	ready++;
	if(!pthread_equal(pthread_self(), mainself))
		others++;
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&mu);
	return v;
}

static void
testthreads(void)
{
	pthread_t t[4];
	void *r;
	int i, ok;

	mainself = pthread_self();
	check("pthread_self", pthread_equal(mainself, pthread_self()), NULL);
	ok = 1;
	for(i = 0; i < 4; i++)
		ok &= pthread_create(&t[i], NULL, worker, (void*)(uintptr_t)i) == 0;
	check("pthread_create", ok, NULL);
	pthread_mutex_lock(&mu);
	while(ready < 4)
		pthread_cond_wait(&cv, &mu);
	pthread_mutex_unlock(&mu);
	ok = 1;
	for(i = 0; i < 4; i++)
		ok &= pthread_join(t[i], &r) == 0 && r == (void*)(uintptr_t)i;
	check("pthread_join", ok, NULL);
	check("mutex: 4 x 20000", counter == 80000, "%ld", counter);
	check("pthread_self in threads", others == 4, "%d", others);
}

static int pfd[2];

static void *
writer(void *v)
{
	usleep(200*1000);
	write(pfd[1], "hello", 5);
	return v;
}

static void
testselect(void)
{
	char buf[16];
	pthread_t w;
	fd_set rf;
	struct timeval tv;
	struct pollfd pf;
	int n;

	if(pipe(pfd) < 0){
		check("pipe", 0, "%s", strerror(errno));
		return;
	}
	pf.fd = pfd[0];
	pf.events = POLLIN;
	check("poll: nothing yet", poll(&pf, 1, 100) == 0, NULL);
	pthread_create(&w, NULL, writer, NULL);
	FD_ZERO(&rf);
	FD_SET(pfd[0], &rf);
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	n = select(pfd[0]+1, &rf, NULL, NULL, &tv);
	check("select: a thread's write", n == 1 && FD_ISSET(pfd[0], &rf), "%d", n);
	check("poll: ready", poll(&pf, 1, 0) == 1 && (pf.revents & POLLIN), NULL);
	n = read(pfd[0], buf, sizeof buf);
	check("read after select", n == 5 && memcmp(buf, "hello", 5) == 0, "%d", n);
	pthread_join(w, NULL);
}

static void *
sleeper(void *v)
{
	for(;;)
		sleep(1);
	return v;
}

int
main(void)
{
	pthread_t s;

	teststdio();
	testalloc();
	testmmap();
	testposix();
	testthreads();
	testselect();
	/* exit goes with a thread and a copy proc still there (run.rc: ps) */
	pthread_create(&s, NULL, sleeper, NULL);
	printf("apegnutest: %d ok, %d failed\n", nok, nfail);
	exit(nfail != 0);
}
