/*
 * libthread unit tests, run inside 9front (tests/guest). Thread and
 * proc switches go through setjmp/longjmp (callee-saved registers) and
 * _threadinitstack's System V launcher; channels through rendezvous.
 * Output as libctest: "ok"/"FAIL" lines and "threadtest: N ok, M failed".
 */
#include <u.h>
#include <libc.h>
#include <thread.h>

static int nok, nfail;

static void
check(char *name, int ok)
{
	if(ok)
		nok++;
	else
		nfail++;
	print("%s %s\n", ok ? "ok" : "FAIL", name);
}

static void
adder(void *v)
{
	Channel *c = v;
	ulong a, sum;

	sum = 0;
	while((a = recvul(c)) != 0)
		sum += a;
	sendul(c, sum);
}

static void
pinger(void *v)
{
	Channel *c = v;
	int i;

	for(i = 0; i < 1000; i++)
		sendul(c, i);
	sendul(c, ~(ulong)0);
}

static long locals;

static void
deepthread(void *v)
{
	Channel *c = v;
	long a = 1, b = 2, d = 3, e = 4;

	yield();
	locals = a + b + d + e;
	sendul(c, 1);
}

void
threadmain(int, char**)
{
	Channel *c;
	ulong i, sum, n;

	c = chancreate(sizeof(ulong), 0);
	threadcreate(adder, c, 8192);
	for(i = 1; i <= 100; i++)
		sendul(c, i);
	sendul(c, 0);
	check("threadcreate + unbuffered channel", recvul(c) == 5050);

	c = chancreate(sizeof(ulong), 16);
	proccreate(pinger, c, 8192);
	sum = 0;
	for(n = 0; (i = recvul(c)) != ~(ulong)0; n++)
		sum += i;
	check("proccreate + buffered channel across procs", n == 1000 && sum == 499500);

	c = chancreate(sizeof(ulong), 0);
	threadcreate(deepthread, c, 8192);
	yield();
	recvul(c);
	check("yield keeps locals (callee-saved registers)", locals == 10);

	print("threadtest: %d ok, %d failed\n", nok, nfail);
	threadexitsall(nfail ? "failures" : nil);
}
