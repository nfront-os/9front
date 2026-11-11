/*
 * What gcc's APE adds for the gcc driver's programs (sys/src/ape/lib/ap/gnu,
 * sys/src/ape/lib/musl, the headers' parts under _APE_GNU), run inside 9front
 * (tests/guest/run.sh): C99's printf over musl's engine, the wide functions,
 * aligned blocks (plan9/malloc.c), mmap, POSIX additions, pthreads, select and
 * poll on a pipe written by a thread and by a forked child (gnu/buf.c: more
 * than a buffer, the end, close, fork; a file's dup polled, the original read), and exit with a thread and
 * a copy proc still running. What foot needs: C11 threads, getopt_long,
 * pipe2, mkostemp, strchrnul, mbrtoc32, locale objects, syslog, a thread's
 * name in /proc, timercmp, byte swaps, regex (musl's TRE); a pty of ptyfs
 * with an APE program and rc in it (line editing, echo, raw modes, size,
 * interrupt, hangup, end of file); SIGCHLD; MSG_NOSIGNAL (EPIPE, no SIGPIPE).
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
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <threads.h>
#include <getopt.h>
#include <uchar.h>
#include <locale.h>
#include <syslog.h>
#include <byteswap.h>
#include <regex.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

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

	/*
	 * setvbuf with no buffer and size 0: stdio picks one (it was a buffer of
	 * no bytes the writes ran past, into the block after it: free aborted)
	 */
	{
		FILE *f;
		char *p;
		int i;

		if((f = fopen("/tmp/apegnutest.vbuf", "w")) == NULL){
			check("setvbuf(NULL, 0)", 0, "fopen: %s", strerror(errno));
			return;
		}
		setvbuf(f, NULL, _IOLBF, 0);
		p = malloc(2);
		for(i = 0; i < 20; i++)
			fprintf(f, "a line longer than no buffer at all, number %d\n", i);
		free(p);
		fclose(f);
		f = fopen("/tmp/apegnutest.vbuf", "r");
		n = f != NULL ? (int)fread(buf, 1, sizeof buf, f) : -1;
		if(f != NULL)
			fclose(f);
		check("setvbuf(NULL, 0): a buffer of stdio's, the heap whole", n == sizeof buf && strncmp(buf, "a line longer", 13) == 0, "%d", n);
		remove("/tmp/apegnutest.vbuf");
	}
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
/*
 * 9front-ng: a memfd's seals (F_ADD_SEALS, F_GET_SEALS: weston's anonymous
 * files): without MFD_ALLOW_SEALING none can be added; shrink and grow
 * sealed, ftruncate to another size fails; write sealed, a writable shared
 * mapping fails, a readable one not; F_SEAL_SEAL ends adding
 */
static void
testseals(void)
{
	char *p;
	int fd, pfd[2];

	fd = memfd_create("seals0", 0);
	if(fd >= 0){
		check("a memfd without MFD_ALLOW_SEALING: F_GET_SEALS is F_SEAL_SEAL", fcntl(fd, F_GET_SEALS) == F_SEAL_SEAL, NULL);
		check("its F_ADD_SEALS: EPERM", fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK) < 0 && errno == EPERM, "%s", strerror(errno));
		close(fd);
	}
	fd = memfd_create("seals", MFD_CLOEXEC|MFD_ALLOW_SEALING);
	check("memfd_create MFD_ALLOW_SEALING", fd >= 0, "%s", strerror(errno));
	if(fd < 0)
		return;
	check("no seals yet", fcntl(fd, F_GET_SEALS) == 0, NULL);
	ftruncate(fd, 8192);
	check("F_ADD_SEALS shrink and grow", fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK|F_SEAL_GROW) == 0, "%s", strerror(errno));
	check("F_GET_SEALS: them", fcntl(fd, F_GET_SEALS) == (F_SEAL_SHRINK|F_SEAL_GROW), NULL);
	check("ftruncate smaller: EPERM", ftruncate(fd, 4096) < 0 && errno == EPERM, "%s", strerror(errno));
	check("ftruncate larger: EPERM", ftruncate(fd, 16384) < 0 && errno == EPERM, "%s", strerror(errno));
	check("ftruncate to its size", ftruncate(fd, 8192) == 0, "%s", strerror(errno));
	check("F_ADD_SEALS write", fcntl(fd, F_ADD_SEALS, F_SEAL_WRITE) == 0, NULL);
	check("a writable shared mapping: EPERM", mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) == MAP_FAILED && errno == EPERM, NULL);
	p = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
	check("a readable one", p != MAP_FAILED, "%s", strerror(errno));
	if(p != MAP_FAILED)
		munmap(p, 4096);
	check("F_ADD_SEALS F_SEAL_SEAL", fcntl(fd, F_ADD_SEALS, F_SEAL_SEAL) == 0, NULL);
	check("then no more: EPERM", fcntl(fd, F_ADD_SEALS, F_SEAL_GROW) < 0 && errno == EPERM, NULL);
	close(fd);
	if(pipe(pfd) == 0){
		check("F_GET_SEALS of a pipe: EINVAL", fcntl(pfd[0], F_GET_SEALS) < 0 && errno == EINVAL, NULL);
		close(pfd[0]);
		close(pfd[1]);
	}
}

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
	/* 9front-ng: a private mapping is a copy (xkbcommon maps Wayland's keymap so) */
	q = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 8192);
	check("mmap MAP_PRIVATE of a memfd: a copy of its bytes", q != MAP_FAILED && q != p + 8192 && strcmp(q, "parent") == 0, "%p", q);
	if(q != MAP_FAILED){
		strcpy(q, "copy");
		check("written, the memfd's bytes stay", strcmp(p + 8192, "parent") == 0, NULL);
		munmap(q, 4096);
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

/*
 * 9front-ng: a dup of a file (a mounted one: not a stream) polled, the
 * original read, as libwayland does with its fd sources: both read through
 * one buffer, in order; another open of the file has its own offset
 */
static void
testdupfile(void)
{
	char *path = "/tmp/apegnutest.dup", buf[8];
	struct pollfd pf;
	int fd, d2, fd3, n;

	if((fd = open(path, O_RDWR|O_CREAT|O_TRUNC, 0600)) < 0){
		check("create a file for dup", 0, "%s", strerror(errno));
		return;
	}
	write(fd, "abcdef", 6);
	close(fd);
	fd = open(path, O_RDONLY);
	d2 = dup(fd);
	pf.fd = d2;
	pf.events = POLLIN;
	pf.revents = 0;
	check("poll of a file's dup: readable", poll(&pf, 1, 2000) == 1 && (pf.revents & POLLIN), "%#x", pf.revents);
	n = read(fd, buf, 3);
	check("the original read: the start", n == 3 && memcmp(buf, "abc", 3) == 0, "%d", n);
	n = read(d2, buf, 3);
	check("the dup read: the rest", n == 3 && memcmp(buf, "def", 3) == 0, "%d", n);
	check("then the end", read(fd, buf, 3) == 0, NULL);
	fd3 = open(path, O_RDONLY);
	n = read(fd3, buf, 3);
	check("another open: its own offset", n == 3 && memcmp(buf, "abc", 3) == 0, "%d", n);
	close(fd3);
	close(d2);
	close(fd);
	unlink(path);
}

/*
 * 9front-ng: descriptors over a PF_UNIX socket (sendmsg/recvmsg,
 * SCM_RIGHTS: Wayland's way to hand over shared memory): a child connects
 * and sends a memfd with some data; the parent takes the data in two
 * reads (the rest waits in the pipe: poll says so), maps the memfd and
 * sees the child's bytes; a plain write reads as plain data
 */
static void
testscm(void)
{
	struct sockaddr_un a;
	struct msghdr m;
	struct iovec iov;
	struct cmsghdr *c;
	struct pollfd pf;
	char buf[64], cbuf[CMSG_SPACE(sizeof(int))];
	char *p;
	int s, cs, fd, st, mfd;
	pid_t pid;

	memset(&a, 0, sizeof a);
	a.sun_family = AF_UNIX;
	strcpy(a.sun_path, "/tmp/apegnutest.sock");
	s = socket(AF_UNIX, SOCK_STREAM, 0);
	if(s < 0 || bind(s, (struct sockaddr *)&a, sizeof a) < 0 || listen(s, 1) < 0){
		check("PF_UNIX socket, bind, listen", 0, "%s", strerror(errno));
		return;
	}
	switch(pid = fork()){
	case -1:
		check("fork", 0, "%s", strerror(errno));
		return;
	case 0:
		cs = socket(AF_UNIX, SOCK_STREAM, 0);
		if(cs < 0 || connect(cs, (struct sockaddr *)&a, sizeof a) < 0)
			_exit(1);
		mfd = memfd_create("scm", 0);
		if(mfd < 0 || ftruncate(mfd, 4096) < 0)
			_exit(2);
		p = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mfd, 0);
		if(p == MAP_FAILED)
			_exit(3);
		strcpy(p, "through the socket");
		memset(&m, 0, sizeof m);
		iov.iov_base = "0123456789abcdef";
		iov.iov_len = 16;
		m.msg_iov = &iov;
		m.msg_iovlen = 1;
		m.msg_control = cbuf;
		m.msg_controllen = sizeof cbuf;
		c = CMSG_FIRSTHDR(&m);
		c->cmsg_level = SOL_SOCKET;
		c->cmsg_type = SCM_RIGHTS;
		c->cmsg_len = CMSG_LEN(sizeof(int));
		memcpy(CMSG_DATA(c), &mfd, sizeof(int));
		if(sendmsg(cs, &m, MSG_NOSIGNAL) != 16)
			_exit(4);
		if(write(cs, "plain", 5) != 5)
			_exit(5);
		/* the parent has it: this one's descriptor and mapping may go */
		read(cs, buf, 1);
		_exit(0);
	}
	fd = accept(s, NULL, NULL);
	check("accept", fd >= 0, "%s", strerror(errno));
	if(fd < 0)
		return;
	memset(&m, 0, sizeof m);
	iov.iov_base = buf;
	iov.iov_len = 10;
	m.msg_iov = &iov;
	m.msg_iovlen = 1;
	m.msg_control = cbuf;
	m.msg_controllen = sizeof cbuf;
	check("recvmsg: 10 of the 16 bytes", recvmsg(fd, &m, MSG_CMSG_CLOEXEC) == 10 && memcmp(buf, "0123456789", 10) == 0, NULL);
	c = CMSG_FIRSTHDR(&m);
	mfd = -1;
	if(c != NULL && c->cmsg_level == SOL_SOCKET && c->cmsg_type == SCM_RIGHTS && c->cmsg_len == CMSG_LEN(sizeof(int)))
		memcpy(&mfd, CMSG_DATA(c), sizeof(int));
	check("SCM_RIGHTS: a descriptor came", mfd >= 0, NULL);
	pf.fd = fd;
	pf.events = POLLIN;
	pf.revents = 0;
	check("poll: the rest is there", poll(&pf, 1, 1000) == 1 && (pf.revents & POLLIN), NULL);
	m.msg_control = NULL;
	m.msg_controllen = 0;
	iov.iov_len = sizeof buf;
	check("recvmsg: the other 6", recvmsg(fd, &m, 0) == 6 && memcmp(buf, "abcdef", 6) == 0, NULL);
	check("recvmsg of a plain write: its data", recvmsg(fd, &m, 0) == 5 && memcmp(buf, "plain", 5) == 0, NULL);
	if(mfd >= 0){
		p = mmap(NULL, 4096, PROT_READ, MAP_SHARED, mfd, 0);
		check("the passed memfd maps the sender's memory", p != MAP_FAILED && strcmp(p, "through the socket") == 0, NULL);
		if(p != MAP_FAILED)
			munmap(p, 4096);
		close(mfd);
	}
	write(fd, "x", 1);
	waitpid(pid, &st, 0);
	check("the sender", WIFEXITED(st) && WEXITSTATUS(st) == 0, "%#x", st);
	close(fd);
	close(s);
	unlink("/tmp/apegnutest.sock");
	unlink("/srv/UD.apegnutest.sock");	/* listen's entry (APE's PF_UNIX sockets are in /srv) */
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

/* 9front-ng: getentropy (POSIX.1-2024): random bytes from /dev/random, at most 256 */
static void
testentropy(void)
{
	unsigned char a[32], b[32], big[257];
	int i, zero;

	check("getentropy 32 bytes", getentropy(a, sizeof a) == 0, "%s", strerror(errno));
	check("getentropy again", getentropy(b, sizeof b) == 0, "%s", strerror(errno));
	zero = 1;
	for(i = 0; i < 32; i++)
		if(a[i] != 0)
			zero = 0;
	check("getentropy: not all zero", !zero, NULL);
	check("getentropy: two calls differ", memcmp(a, b, sizeof a) != 0, NULL);
	errno = 0;
	check("getentropy of 257 bytes: EINVAL", getentropy(big, sizeof big) < 0 && errno == EINVAL, "errno %d", errno);
}

static mtx_t c11m;
static cnd_t c11c;
static int c11v;
static tss_t c11k;
static once_flag c11once = ONCE_FLAG_INIT;
static int c11ones;

static void c11one(void) { c11ones++; }

static int
c11thread(void *a)
{
	tss_set(c11k, a);
	mtx_lock(&c11m);
	c11v = (int)(intptr_t)tss_get(c11k);
	cnd_signal(&c11c);
	mtx_unlock(&c11m);
	call_once(&c11once, c11one);
	return 42;
}

static void
testc11threads(void)
{
	thrd_t t;
	int r;
	struct timespec ts;

	check("mtx_init, cnd_init, tss_create", mtx_init(&c11m, mtx_plain) == thrd_success &&
		cnd_init(&c11c) == thrd_success && tss_create(&c11k, NULL) == thrd_success, NULL);
	call_once(&c11once, c11one);
	mtx_lock(&c11m);
	check("thrd_create", thrd_create(&t, c11thread, (void*)7) == thrd_success, NULL);
	while(c11v == 0)
		cnd_wait(&c11c, &c11m);
	mtx_unlock(&c11m);
	check("the thread's tss and the condition", c11v == 7, "%d", c11v);
	check("thrd_join: its result", thrd_join(t, &r) == thrd_success && r == 42, "%d", r);
	check("call_once: once", c11ones == 1, "%d", c11ones);
	check("tss_get in main: not the thread's", tss_get(c11k) == NULL, NULL);
	mtx_init(&c11m, mtx_recursive);
	check("mtx_recursive: twice", mtx_lock(&c11m) == thrd_success && mtx_trylock(&c11m) == thrd_success, NULL);
	mtx_unlock(&c11m);
	mtx_unlock(&c11m);
	timespec_get(&ts, TIME_UTC);
	ts.tv_nsec += 20000000;
	if(ts.tv_nsec >= 1000000000){
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	mtx_lock(&c11m);
	check("cnd_timedwait: thrd_timedout", cnd_timedwait(&c11c, &c11m, &ts) == thrd_timedout, NULL);
	mtx_unlock(&c11m);
	ts.tv_sec = 0;
	ts.tv_nsec = 1000000;
	check("thrd_sleep", thrd_sleep(&ts, NULL) == 0, NULL);
}

static void
testgetoptlong(void)
{
	static const struct option lo[] = {
		{"config", required_argument, NULL, 'c'},
		{"log-level", optional_argument, NULL, 'l'},
		{"hold", no_argument, NULL, 'H'},
		{NULL, 0, NULL, 0},
	};
	char *argv[] = {"foot", "-Hc", "a.ini", "--log-level=info", "--hold", "-lwarn", "--conf", "b.ini", "-x", "rc", "-l", NULL};
	char got[128];
	int c, n;

	opterr = 0;
	optind = 0;
	got[0] = 0;
	n = 0;
	while((c = getopt_long(11, argv, "+c:Hl::", lo, NULL)) != -1)
		n += snprintf(got + n, sizeof got - n, "%c%s ", c, optarg != NULL ? optarg : "");
	check("getopt_long: short, grouped, long, =, abbreviated, optional, unknown, stops at the command",
		strcmp(got, "H ca.ini linfo H lwarn cb.ini ? ") == 0 && optind == 9, "%s, optind %d", got, optind);
	opterr = 1;
	optind = 1;
}

static void
testgnuext(void)
{
	char tmpl[] = "/tmp/apegnuXXXXXX", buf[64];
	int fd[2], f, n;
	char32_t c;
	mbstate_t st;
	locale_t l;
	struct timeval a = {1, 500}, b = {1, 700}, r;

	check("pipe2 O_CLOEXEC|O_NONBLOCK", pipe2(fd, O_CLOEXEC|O_NONBLOCK) == 0, "%s", strerror(errno));
	check("pipe2: close-on-exec, non-blocking", (fcntl(fd[0], F_GETFD) & FD_CLOEXEC) != 0 &&
		(fcntl(fd[1], F_GETFL) & O_NONBLOCK) != 0, NULL);
	close(fd[0]);
	close(fd[1]);
	errno = 0;
	check("pipe2 with other flags: EINVAL", pipe2(fd, O_APPEND) < 0 && errno == EINVAL, NULL);
	f = mkostemp(tmpl, O_CLOEXEC);
	check("mkostemp O_CLOEXEC", f >= 0 && (fcntl(f, F_GETFD) & FD_CLOEXEC) != 0 && strncmp(tmpl, "/tmp/apegnu", 11) == 0 &&
		strcmp(tmpl + 11, "XXXXXX") != 0, "%s", tmpl);
	if(f >= 0){
		close(f);
		unlink(tmpl);
	}
	f = open("/dev/null", O_RDONLY|O_CLOEXEC);
	check("open O_CLOEXEC: FD_CLOEXEC", f >= 0 && (fcntl(f, F_GETFD) & FD_CLOEXEC) != 0, NULL);
	close(f);
	f = open("/dev/null", O_RDONLY);
	check("open without: not", f >= 0 && (fcntl(f, F_GETFD) & FD_CLOEXEC) == 0, NULL);
	close(f);
	check("strchrnul", *strchrnul("abc", 'x') == 0 && *strchrnul("abc", 'b') == 'b', NULL);
	memset(&st, 0, sizeof st);
	check("mbrtoc32 of ö", mbrtoc32(&c, "\xc3\xb6", 2, &st) == 2 && c == 0xf6, NULL);
	check("c32rtomb of €", c32rtomb(buf, 0x20ac, &st) == 3 && memcmp(buf, "\xe2\x82\xac", 3) == 0, NULL);
	check("setlocale(LC_CTYPE, \"\")", setlocale(LC_CTYPE, "") != NULL, NULL);
	l = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
	check("newlocale C, uselocale", l != (locale_t)0 && uselocale(l) != (locale_t)0, NULL);
	freelocale(l);
	errno = 0;
	check("newlocale de_DE: ENOENT", newlocale(LC_ALL_MASK, "de_DE", (locale_t)0) == (locale_t)0 && errno == ENOENT, NULL);
	timersub(&b, &a, &r);
	check("timercmp, timersub", timercmp(&a, &b, <) && !timercmp(&a, &b, >=) && r.tv_sec == 0 && r.tv_usec == 200, NULL);
	check("bswap_16/32/64", bswap_16(0x1234) == 0x3412 && bswap_32(0x12345678) == 0x78563412 &&
		bswap_64(0x0102030405060708ULL) == 0x0807060504030201ULL, NULL);

	check("pthread_setname_np", pthread_setname_np(pthread_self(), "apegnu-name") == 0, NULL);
	snprintf(tmpl, sizeof tmpl, "/proc/%d/args", getpid());
	buf[0] = 0;
	if((f = open(tmpl, O_RDONLY)) >= 0){
		n = read(f, buf, sizeof buf - 1);
		buf[n > 0 ? n : 0] = 0;
		close(f);
	}
	check("its /proc/n/args: name [args]", strcmp(buf, "apegnutest [apegnu-name]") == 0, "%s", buf);

	/* syslog to standard error (no /sys/log/apegnutest) */
	if(pipe(fd) == 0){
		int e = dup(2);

		dup2(fd[1], 2);
		openlog("apegnutest", LOG_PERROR|LOG_PID, LOG_USER);
		errno = ENOENT;
		syslog(LOG_ERR, "hello %d: %m", 5);
		closelog();
		dup2(e, 2);
		close(e);
		close(fd[1]);
		n = read(fd[0], buf, sizeof buf - 1);
		buf[n > 0 ? n : 0] = 0;
		close(fd[0]);
		snprintf(tmpl, sizeof tmpl, "apegnutest[%d]: hello 5: ", getpid());
		check("syslog LOG_PERROR, LOG_PID, %m", strncmp(buf, tmpl, strlen(tmpl)) == 0 && buf[strlen(buf)-1] == '\n', "%s", buf);
	}
}

static void
testregex(void)
{
	regex_t re;
	regmatch_t m[3];
	const char *s = "siehe https://9front.org/über-uns oder nicht";
	char buf[64];
	int e;

	e = regcomp(&re, "(https?|ftp)://[^[:space:]]+", REG_EXTENDED);
	check("regcomp ERE", e == 0, "%d", e);
	if(e == 0){
		e = regexec(&re, s, 3, m, 0);
		check("regexec: the URL, through the ü", e == 0 && m[0].rm_so == 6 && m[0].rm_eo == 34 &&
			m[1].rm_so == 6 && m[1].rm_eo == 11 && re.re_nsub == 1,
			"%d: %ld-%ld %ld-%ld", e, (long)m[0].rm_so, (long)m[0].rm_eo, (long)m[1].rm_so, (long)m[1].rm_eo);
		check("regexec: no match", regexec(&re, "kein link", 0, NULL, 0) == REG_NOMATCH, NULL);
		regfree(&re);
	}
	check("REG_ICASE on ÄÖ", regcomp(&re, "^äö$", REG_EXTENDED|REG_ICASE|REG_NOSUB) == 0 &&
		regexec(&re, "ÄÖ", 0, NULL, 0) == 0, NULL);
	regfree(&re);
	e = regcomp(&re, "a(b", REG_EXTENDED);
	regerror(e, &re, buf, sizeof buf);
	check("regerror: Missing ')'", e == REG_EPAREN && strcmp(buf, "Missing ')'") == 0, "%d %s", e, buf);
}

static volatile int ptyalarm, ptyint;

static void ptyonalarm(int s) { (void)s; ptyalarm = 1; }
static void ptyonint(int s) { (void)s; ptyint = 1; }

/*
 * the master's output until want shows (10 s at most): what came up to
 * its end in got; what came after it stays for the next call
 */
static char ptybuf[1024];
static int nptybuf;

static int
ptyexpect(int m, char *want, char *got, int ngot)
{
	char *e;
	int k, n;

	ptyalarm = 0;
	alarm(10);
	ptybuf[nptybuf] = 0;
	while((e = strstr(ptybuf, want)) == NULL && nptybuf < (int)sizeof ptybuf - 1 && !ptyalarm){
		k = read(m, ptybuf + nptybuf, sizeof ptybuf - 1 - nptybuf);
		if(k <= 0)
			break;
		nptybuf += k;
		ptybuf[nptybuf] = 0;
	}
	alarm(0);
	n = e != NULL ? (int)(e - ptybuf) + (int)strlen(want) : nptybuf;
	snprintf(got, ngot, "%.*s", n, ptybuf);
	memmove(ptybuf, ptybuf + n, nptybuf - n);
	nptybuf -= n;
	ptybuf[nptybuf] = 0;
	return e != NULL;
}

static void
ptysay(char *fmt, ...)
{
	char buf[128];
	va_list a;

	va_start(a, fmt);
	vsnprintf(buf, sizeof buf, fmt, a);
	va_end(a);
	write(1, buf, strlen(buf));
}

/* the programs in the pty: an APE one, then rc */
static void
ptychild(char *name)
{
	struct termios t, cooked;
	struct winsize w;
	char buf[64];
	int s, n, c;

	setsid();
	if((s = open(name, O_RDWR)) < 0 || ioctl(s, TIOCSCTTY, 0) < 0)
		_exit(10);
	dup2(s, 0);
	dup2(s, 1);
	dup2(s, 2);
	if(s > 2)
		close(s);
	ptysay("hi isatty=%d tc=%d\n", isatty(0), tcgetattr(0, &cooked) == 0 && (cooked.c_lflag & ICANON) != 0);
	n = read(0, buf, sizeof buf - 1);
	buf[n > 0 ? n : 0] = 0;
	ptysay("got[%s]\n", buf);
	t = cooked;
	t.c_lflag &= ~(ICANON|ECHO);
	t.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &t);
	ptysay("raw\n");
	n = read(0, buf, sizeof buf - 1);
	buf[n > 0 ? n : 0] = 0;
	ptysay("rawgot[%s]\n", buf);
	tcsetattr(0, TCSANOW, &cooked);
	memset(&w, 0, sizeof w);
	ioctl(0, TIOCGWINSZ, &w);
	ptysay("ws %d %d\n", w.ws_row, w.ws_col);
	if((c = open("/dev/consctl", O_WRONLY)) >= 0)
		write(c, "rawon", 5);
	ptysay("rawon %d\n", c >= 0);
	n = read(0, buf, 1);
	buf[n > 0 ? n : 0] = 0;
	ptysay("cons[%s]\n", buf);
	if(c >= 0)
		close(c);
	signal(SIGINT, ptyonint);
	ptysay("sig\n");
	n = read(0, buf, sizeof buf);
	ptysay("int %d %d\n", ptyint, n);
	execl("/bin/rc", "rc", "-c", "echo native; x=`{read}; echo rc got $x", NULL);
	_exit(11);
}

static void
testpty(void)
{
	char got[512], *name;
	struct winsize w = {30, 100, 0, 0};
	int m, st, pid;
	void (*old)(int);

	old = signal(SIGALRM, ptyonalarm);
	m = posix_openpt(O_RDWR|O_NOCTTY);
	check("posix_openpt (ptyfs started)", m >= 0, "%s", strerror(errno));
	if(m < 0)
		return;
	name = ptsname(m);
	check("ptsname: /dev/pttyN", name != NULL && strncmp(name, "/dev/ptty", 9) == 0, "%s", name ? name : "nil");
	check("grantpt, unlockpt", grantpt(m) == 0 && unlockpt(m) == 0, NULL);
	check("TIOCSWINSZ on the master", ioctl(m, TIOCSWINSZ, &w) == 0, "%s", strerror(errno));
	if((pid = fork()) == 0){
		close(m);
		ptychild(name);
	}
	check("the slave a tty with termios; NL is CR NL", ptyexpect(m, "hi isatty=1 tc=1\r\n", got, sizeof got), "%s", got);
	write(m, "abc\177d\r", 6);
	check("a line edited, echoed", ptyexpect(m, "got[abd\r\n]", got, sizeof got) && strstr(got, "abc\b \bd\r\n") != NULL, "%s", got);
	check("raw mode", ptyexpect(m, "raw\r\n", got, sizeof got), "%s", got);
	write(m, "xy", 2);
	check("raw: the bytes at once, no echo", ptyexpect(m, "rawgot[xy]", got, sizeof got) && strcmp(got, "rawgot[xy]") == 0, "%s", got);
	check("TIOCGWINSZ on the slave: the master's size", ptyexpect(m, "ws 30 100", got, sizeof got), "%s", got);
	check("consctl rawon (TIOCSCTTY bound it)", ptyexpect(m, "rawon 1", got, sizeof got), "%s", got);
	write(m, "q", 1);
	check("rawon: a byte, no echo", ptyexpect(m, "cons[q]", got, sizeof got) && strcmp(got, "\r\ncons[q]") == 0, "%s", got);
	check("SIGINT handler set", ptyexpect(m, "sig\r\n", got, sizeof got), "%s", got);
	write(m, "\003", 1);
	check("^C: interrupt to the note group (SIGINT)", ptyexpect(m, "int 1", got, sizeof got), "%s", got);
	check("rc in the pty", ptyexpect(m, "native\r\n", got, sizeof got), "%s", got);
	write(m, "zz\r", 3);
	check("rc reads a line of it", ptyexpect(m, "rc got zz\r\n", got, sizeof got), "%s", got);
	nptybuf = 0;
	ptyalarm = 0;
	alarm(10);
	while(read(m, got, sizeof got) > 0)
		;
	alarm(0);
	check("the slave gone: the master reads end of file", !ptyalarm, NULL);
	ptyalarm = 0;
	alarm(10);
	st = -1;
	if(waitpid(pid, &st, 0) < 0)
		kill(pid, SIGKILL);	/* not to hang the tests */
	alarm(0);
	check("the programs exited", st != -1 && WIFEXITED(st) && WEXITSTATUS(st) == 0, "%x", st);
	close(m);

	/* hangup: the master closes under a reader */
	nptybuf = 0;
	m = posix_openpt(O_RDWR);
	name = ptsname(m);
	if((pid = fork()) == 0){
		char b[8];
		int s;

		close(m);
		setsid();
		signal(SIGHUP, SIG_IGN);
		s = open(name, O_RDWR);
		ioctl(s, TIOCSCTTY, 0);
		write(s, "r\n", 2);
		_exit(read(s, b, sizeof b) == 0 ? 7 : 8);
	}
	ptyexpect(m, "r\r\n", got, sizeof got);
	close(m);
	ptyalarm = 0;
	alarm(10);
	st = 0;
	if(waitpid(pid, &st, 0) < 0)
		kill(pid, SIGKILL);
	alarm(0);
	check("the master closed: the slave reads end of file", !ptyalarm && WIFEXITED(st) && WEXITSTATUS(st) == 7, "%x", st);
	signal(SIGALRM, old);
}

static volatile int nchld;

static void onchld(int s) { (void)s; nchld++; }

/* SIGCHLD (gnu/sigchld.c): the handler on a child's exit, its status by waitpid */
static void
testsigchld(void)
{
	struct sigaction sa;
	int pid, pid2, st, r, i;

	memset(&sa, 0, sizeof sa);
	sa.sa_handler = onchld;
	sigemptyset(&sa.sa_mask);
	check("sigaction SIGCHLD", sigaction(SIGCHLD, &sa, NULL) == 0, NULL);
	if((pid = fork()) == 0)
		_exit(3);
	for(i = 0; i < 500 && nchld == 0; i++)
		usleep(10000);
	check("SIGCHLD when the child exited", nchld == 1, "%d", nchld);
	st = 0;
	r = waitpid(-1, &st, WNOHANG);
	check("waitpid WNOHANG: its pid and status 3", r == pid && WIFEXITED(st) && WEXITSTATUS(st) == 3, "%d %x", r, st);
	check("waitpid WNOHANG again: 0", waitpid(-1, &st, WNOHANG) == 0, NULL);
	if((pid = fork()) == 0){
		usleep(200000);
		_exit(4);
	}
	if((pid2 = fork()) == 0)
		_exit(5);
	st = 0;
	r = waitpid(pid, &st, 0);
	check("waitpid of the later child, the other exited first", r == pid && WEXITSTATUS(st) == 4, "%d %x", r, st);
	r = waitpid(pid2, &st, 0);
	check("then the other's", r == pid2 && WEXITSTATUS(st) == 5, "%d %x", r, st);
	for(i = 0; i < 100 && nchld < 3; i++)
		usleep(10000);
	check("SIGCHLD for each", nchld == 3, "%d", nchld);
	{
		sigset_t set, old;
		int fd[2], ep, n0;
		char c;
		struct epoll_event ev;

		sigemptyset(&set);
		check("sigaddset SIGTERM, SIGCHLD (APE refused them)", sigaddset(&set, SIGTERM) == 0 && sigaddset(&set, SIGCHLD) == 0 &&
			sigismember(&set, SIGCHLD) == 1, NULL);
		sigprocmask(SIG_BLOCK, &set, &old);
		n0 = nchld;
		pipe(fd);
		if((pid = fork()) == 0){
			usleep(100000);
			write(fd[1], "x", 1);	/* after the parent's child below exited */
			_exit(0);
		}
		if((pid2 = fork()) == 0)
			_exit(7);
		r = read(fd[0], &c, 1);
		check("a blocked SIGCHLD interrupts no read", r == 1 && nchld == n0, "%d %d", r, nchld - n0);
		usleep(100000);
		ep = epoll_create1(0);
		memset(&ev, 0, sizeof ev);
		ev.events = EPOLLIN;
		epoll_ctl(ep, EPOLL_CTL_ADD, fd[0], &ev);
		r = epoll_pwait(ep, &ev, 1, 2000, &old);
		check("epoll_pwait opening the mask: the pending SIGCHLD, EINTR", r < 0 && errno == EINTR && nchld > n0, "%d %d", r, nchld - n0);
		sigprocmask(SIG_SETMASK, &old, NULL);
		waitpid(pid, &st, 0);
		waitpid(pid2, &st, 0);
		close(ep);
		close(fd[0]);
		close(fd[1]);
	}
	{
		/* an eventfd written by plain write, as foot's shutdown does: readable on it */
		struct epoll_event ev;
		uint64_t v;
		int e, ep;

		e = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
		ep = epoll_create1(0);
		memset(&ev, 0, sizeof ev);
		ev.events = EPOLLIN;
		epoll_ctl(ep, EPOLL_CTL_ADD, e, &ev);
		v = 1;
		check("eventfd: write(2) on it", write(e, &v, sizeof v) == sizeof v, "%s", strerror(errno));
		check("eventfd: epoll sees it readable", epoll_wait(ep, &ev, 1, 2000) == 1, NULL);
		v = 0;
		check("eventfd: its value read back", read(e, &v, sizeof v) == sizeof v && v == 1, "%llu", (unsigned long long)v);
		close(ep);
		close(e);
	}
	signal(SIGCHLD, SIG_DFL);
	if((pid = fork()) == 0)
		_exit(6);
	r = waitpid(pid, &st, 0);
	check("SIG_DFL: the note ignored, waitpid as before", r == pid && WEXITSTATUS(st) == 6, "%d %x", r, st);
}

static int npipe;
static void onpipe(int s) { (void)s; npipe++; }

/* a write to a pipe whose reader is gone: SIGPIPE, but not with MSG_NOSIGNAL (libwayland's sends) */
static void
testnosignal(void)
{
	struct msghdr m;
	struct iovec iov;
	int p[2], n, e;

	if(pipe(p) < 0){
		check("pipe", 0, "%s", strerror(errno));
		return;
	}
	close(p[0]);
	signal(SIGPIPE, SIG_DFL);
	n = send(p[1], "x", 1, MSG_NOSIGNAL);
	e = errno;
	check("send with MSG_NOSIGNAL to a gone reader: EPIPE, alive", n == -1 && e == EPIPE, "%d %s", n, strerror(e));
	memset(&m, 0, sizeof m);
	iov.iov_base = "y";
	iov.iov_len = 1;
	m.msg_iov = &iov;
	m.msg_iovlen = 1;
	n = sendmsg(p[1], &m, MSG_NOSIGNAL);
	e = errno;
	check("sendmsg with MSG_NOSIGNAL: EPIPE, alive", n == -1 && e == EPIPE, "%d %s", n, strerror(e));
	signal(SIGPIPE, onpipe);
	n = send(p[1], "z", 1, 0);
	check("send without it: SIGPIPE", n == -1 && npipe == 1, "%d, %d signals", n, npipe);
	signal(SIGPIPE, SIG_DFL);
	close(p[1]);
}

int
main(void)
{
	pthread_t s;

	teststdio();
	testalloc();
	testmmap();
	testshm();
	testseals();
	testentropy();
	testc11threads();
	testgetoptlong();
	testgnuext();
	testregex();
	testpty();
	testsigchld();
	testnosignal();
	testscm();
	testposix();
	testthreads();
	testlocals();
	testselect();
	testbuf();
	testdupfile();
	/* exit goes with a thread and a copy proc still there (run.rc: ps) */
	pthread_create(&s, NULL, sleeper, NULL);
	printf("apegnutest: %d ok, %d failed\n", nok, nfail);
	exit(nfail != 0);
}
