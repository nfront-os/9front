/*
 * What gcc's APE adds for the gcc driver's programs (sys/src/ape/lib/ap/gnu,
 * sys/src/ape/lib/musl, the headers' parts under _APE_GNU), run inside 9front
 * (tests/guest/run.sh): C99's printf over musl's engine, the wide functions,
 * aligned blocks (plan9/malloc.c), mmap, POSIX additions, pthreads, select and
 * poll on a pipe written by a thread and by a forked child (gnu/buf.c: more
 * than a buffer, the end, close, fork), and exit with a thread and
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
#include <sys/wait.h>

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

/*
 * 9front-ng: shared memory objects (memfd_create, shm_open): segments at an
 * address the kernel chooses, the same memory in every process; a child
 * that detaches what fork gave it and maps the descriptor again sees the
 * parent's bytes and writes some back; memfd's segment goes with its last
 * descriptor
 */
static void
testshm(void)
{
	char *p, *q, path[64];
	int fd, fd2, st;
	pid_t pid;

	fd = memfd_create("apegnutest", MFD_CLOEXEC);
	check("memfd_create", fd >= 0, "%s", strerror(errno));
	if(fd < 0)
		return;
	check("mmap of a memfd before ftruncate fails", mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) == MAP_FAILED, NULL);
	check("ftruncate a memfd", ftruncate(fd, 1<<20) == 0, "%s", strerror(errno));
	p = mmap(NULL, 1<<20, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	check("mmap MAP_SHARED of a memfd", p != MAP_FAILED, "%s", strerror(errno));
	if(p == MAP_FAILED){
		close(fd);
		return;
	}
	q = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 8192);
	check("a second mmap: the same memory at an offset", q == p + 8192, "%p %p", p, q);
	strcpy(p + 8192, "parent");
	check("ftruncate larger, within what it reserved", ftruncate(fd, 2<<20) == 0, "%s", strerror(errno));
	switch(pid = fork()){
	case -1:
		check("fork", 0, "%s", strerror(errno));
		break;
	case 0:
		munmap(q, 4096);
		munmap(p, 1<<20);
		p = mmap(NULL, 2<<20, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		if(p == MAP_FAILED || strcmp(p + 8192, "parent") != 0)
			_exit(1);
		strcpy(p + (1<<20) + 5, "child");
		_exit(0);
	default:
		waitpid(pid, &st, 0);
		check("another mapping in the child sees the parent's bytes", WIFEXITED(st) && WEXITSTATUS(st) == 0, "%#x", st);
		check("the child's write seen here", strcmp(p + (1<<20) + 5, "child") == 0, NULL);
	}
	fd2 = dup(fd);
	if(fd2 >= 0){
		q = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd2, 0);
		check("mmap of a dup: the same segment", q == p, "%p %p", p, q);
		if(q != MAP_FAILED)
			munmap(q, 4096);
		close(fd2);
	}
	munmap(p + 8192, 4096);
	check("munmap of the last mapping", munmap(p, 1<<20) == 0, NULL);
	snprintf(path, sizeof path, "#g/ape.memfd.%d.0", getpid());
	check("the memfd's segment in #g while open", access(path, 0) == 0, "%s", path);
	close(fd);
	check("gone with the last descriptor", access(path, 0) < 0, "%s", path);

	fd = shm_open("/apegnutest", O_RDWR|O_CREAT|O_EXCL, 0600);
	check("shm_open O_CREAT", fd >= 0, "%s", strerror(errno));
	if(fd >= 0){
		check("shm_open O_EXCL of an existing one: EEXIST", shm_open("/apegnutest", O_RDWR|O_CREAT|O_EXCL, 0600) < 0 && errno == EEXIST, NULL);
		ftruncate(fd, 4096);
		p = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		if(p != MAP_FAILED)
			strcpy(p, "named");
		fd2 = shm_open("/apegnutest", O_RDWR, 0);
		q = fd2 >= 0 ? mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd2, 0) : MAP_FAILED;
		check("shm_open by name: the same memory", p != MAP_FAILED && q == p && strcmp(q, "named") == 0, NULL);
		if(q != MAP_FAILED)
			munmap(q, 4096);
		if(p != MAP_FAILED)
			munmap(p, 4096);
		close(fd2);
		close(fd);
		check("shm_unlink", shm_unlink("/apegnutest") == 0 && shm_open("/apegnutest", O_RDWR, 0) < 0, NULL);
	}
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

/* main's locals shared with threads: main runs on shared memory (pthread.c's
 * __plan9_mainstack, from APE's _apemain: the tree links without --wrap=main) */
struct Shared {
	pthread_mutex_t	mu;
	long	n;
};

static void *
addlocal(void *v)
{
	struct Shared *sh = v;
	int i;

	for(i = 0; i < 10000; i++){
		pthread_mutex_lock(&sh->mu);
		sh->n++;
		pthread_mutex_unlock(&sh->mu);
	}
	return NULL;
}

static void
testlocals(void)
{
	struct Shared sh;
	pthread_t t[2];

	pthread_mutex_init(&sh.mu, NULL);
	sh.n = 0;
	pthread_create(&t[0], NULL, addlocal, &sh);
	pthread_create(&t[1], NULL, addlocal, &sh);
	pthread_join(t[0], NULL);
	pthread_join(t[1], NULL);
	check("threads on main's locals", sh.n == 20000, "%ld", sh.n);
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

/*
 * APE's read buffering (ap/gnu/buf.c) without threads: a forked child
 * writes; more than a buffer holds (PERFDMAX 16K: the copy proc waits
 * for room); the end (EOF, POLLHUP); two descriptors; close of a buffered
 * descriptor (its copy proc goes); fork after buffering (the child reads
 * its descriptor itself).
 */
static void
testbuf(void)
{
	char buf[8192], c;
	int a[2], b[2], n, total, st, ok;
	pid_t pid;
	fd_set rf;
	struct timeval tv;
	struct pollfd pf[2];
	long want;
	int i;

	if(pipe(a) < 0 || pipe(b) < 0){
		check("buf: pipes", 0, "%s", strerror(errno));
		return;
	}
	pid = fork();
	if(pid == 0){
		close(a[0]);
		close(b[0]);
		usleep(100*1000);
		write(b[1], "B", 1);	/* the second descriptor first */
		for(i = 0; i < 100000; i++){	/* 100000 bytes: six buffers */
			c = i % 251;
			write(a[1], &c, 1);
		}
		close(a[1]);
		close(b[1]);
		_exit(0);
	}
	close(a[1]);
	close(b[1]);
	FD_ZERO(&rf);
	FD_SET(a[0], &rf);
	FD_SET(b[0], &rf);
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	n = select((a[0] > b[0] ? a[0] : b[0]) + 1, &rf, NULL, NULL, &tv);
	check("buf: select on two, without threads", n >= 1 && (FD_ISSET(a[0], &rf) || FD_ISSET(b[0], &rf)), "%d", n);
	pf[0].fd = b[0];
	pf[0].events = POLLIN;
	n = poll(pf, 1, 5000);
	check("buf: poll, the second", n == 1 && (pf[0].revents & POLLIN) && read(b[0], &c, 1) == 1 && c == 'B', "%d", n);
	/* all of it, through select, in order */
	total = 0;
	ok = 1;
	for(;;){
		FD_ZERO(&rf);
		FD_SET(a[0], &rf);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		if(select(a[0]+1, &rf, NULL, NULL, &tv) != 1){
			ok = 0;
			break;
		}
		n = read(a[0], buf, sizeof buf);
		if(n <= 0)
			break;
		for(i = 0; i < n; i++)
			if((unsigned char)buf[i] != (total+i) % 251)
				ok = 0;
		total += n;
	}
	want = 100000;
	check("buf: 100000 bytes through select, in order, then EOF", ok && total == want && n == 0, "%d bytes, ok %d", total, ok);
	pf[0].fd = a[0];
	pf[0].events = POLLIN;
	check("buf: poll after the end: POLLHUP", poll(pf, 1, 0) == 1 && (pf[0].revents & POLLHUP), "%x", pf[0].revents);
	pf[1].fd = b[0];
	pf[1].events = POLLIN;
	n = poll(pf+1, 1, 5000);
	check("buf: the second's end", n == 1 && read(b[0], &c, 1) == 0, "%d", n);
	check("buf: close of buffered descriptors", close(a[0]) == 0 && close(b[0]) == 0, NULL);
	check("buf: the writer", waitpid(pid, &st, 0) == pid && WIFEXITED(st) && WEXITSTATUS(st) == 0, "%x", st);

	/* close while the copy proc waits in a read: it goes */
	if(pipe(a) < 0)
		return;
	pf[0].fd = a[0];
	pf[0].events = POLLIN;
	check("buf: poll times out", poll(pf, 1, 50) == 0, NULL);
	check("buf: close with a copy proc reading", close(a[0]) == 0, NULL);
	close(a[1]);

	/* fork after buffering: the child reads the descriptor itself */
	if(pipe(a) < 0)
		return;
	pf[0].fd = a[0];
	pf[0].events = POLLIN;
	poll(pf, 1, 10);
	pid = fork();
	if(pid == 0){
		n = read(a[0], buf, sizeof buf);
		_exit(n == 2 && memcmp(buf, "hi", 2) == 0 ? 0 : 1);
	}
	usleep(100*1000);
	write(a[1], "hi", 2);
	/* the parent's copy proc may take it first: then give it back */
	usleep(100*1000);
	if(poll(pf, 1, 0) == 1){
		n = read(a[0], buf, sizeof buf);
		write(a[1], buf, n);
	}
	check("buf: fork after buffering", waitpid(pid, &st, 0) == pid && WIFEXITED(st) && WEXITSTATUS(st) == 0, "%x", st);
	close(a[0]);
	close(a[1]);
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
	testshm();
	testposix();
	testthreads();
	testlocals();
	testselect();
	testbuf();
	/* exit goes with a thread and a copy proc still there (run.rc: ps) */
	pthread_create(&s, NULL, sleeper, NULL);
	printf("apegnutest: %d ok, %d failed\n", nok, nfail);
	exit(nfail != 0);
}
