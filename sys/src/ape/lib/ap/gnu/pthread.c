/*
 * POSIX threads for APE (<pthread.h>), all in this one object:
 * linking pthread_create links the rest, and libgcc's gthr takes
 * threads as running when pthread_cancel is there.
 *
 * A thread is a proc, rfork(RFPROC|RFMEM|RFNOWAIT): it shares data, bss
 * and heap, and has its own copy of the stack segment, as libthread's
 * procs. On that copy are APE's errno (callmain.c: _errnoloc points at
 * a local of _callmain) and _privates, so both are per thread; slot 0 of
 * _privates is the thread's struct __pthread. The thread runs on a stack
 * from malloc (shared memory: its locals can be passed to others), and
 * goes back to its own stack segment to finish, so that whoever frees
 * the stack (pthread_join, or itself when detached) frees one nobody
 * runs on. main does the same once this object is linked: libgcc's
 * __wrap_main calls __plan9_mainstack, which runs main on a stack from
 * malloc, so that main's locals are shared memory too; APE's _apemain
 * calls it as well (the tree's programs link without --wrap=main).
 *
 * So the creating thread is on shared memory when it forks, and the new
 * proc returns from rfork onto the same stack as its parent: it must not
 * touch it. __apecompat_rforkthread makes the system call itself and
 * moves the child, before anything else, to privtop on its own stack
 * segment: below __plan9_mainstack's frame, where main's proc stopped
 * using the segment (main runs on the heap), so free in every copy.
 *
 * In a Rust program the threads are std's (its __plan9_thread_spawn,
 * weak here: a proc on a stack slot of its own, shared memory), as are
 * main's stack (std's __plan9_mainstack: this one is weak) and the procs
 * of APE's read buffering (__apecompat_rforkproc, buf.c). A proc that
 * pthread_create did not make (std's threads, buf.c's copy procs) has
 * its parent's slot 0 in its copy of _privates: pthread_self checks the
 * pid and gives such a proc a struct __pthread of its own.
 *
 * Waiting is on 9front's semaphores (semacquire, tsemacquire; a note
 * interrupts them, the wait goes on). Mutexes are Drepper's three-state
 * futex mutex with a semaphore for the futex; a condition queues its
 * waiters, each on its own semaphore, in order. exit in any thread ends
 * the others (proc(3)'s ctl "kill", from an atexit function).
 */
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* libap: system calls (plan9/sys9.h), per-proc slots (plan9/callmain.c) */
extern int _SEMACQUIRE(int *, int);
extern int _TSEMACQUIRE(int *, unsigned long);
extern int _SEMRELEASE(int *, int);
extern void _EXITS(char *);
extern void **_privates;
extern int _nprivates;

enum {
	RFPROC = 1<<4,
	RFMEM = 1<<5,
	RFNOWAIT = 1<<6,

	MAINSTACK = 16*1024*1024,	/* the kernel's USTKSIZE */
	STACK = 1024*1024,

	/* struct __pthread state */
	Running = 0,
	Detached,	/* running, nobody joins */
	Exited,	/* waits for pthread_join */
	Proc,	/* __apecompat_rforkproc's: no thread */
};

typedef struct Key Key;
struct Key {
	unsigned int	seq;	/* odd: in use */
	void	(*dtor)(void *);
};

struct __pthread {
	void	*(*fn)(void *);
	void	(*procfn)(void *);	/* Proc */
	void	*arg;
	void	*ret;
	char	*stack;	/* malloc's; main's stays */
	size_t	stacksize;
	int	pid;
	int	state;
	int	done;	/* semaphore: the thread is through */
	jmp_buf	exitjmp;	/* pthread_exit: back to the proc's stack */
	struct {
		unsigned int	seq;
		void	*val;
	} tsd[PTHREAD_KEYS_MAX];
	/* waiting on a condition */
	int	wsem;
	int	signaled;
	struct __pthread	*cnext;
	/* all threads, for exit */
	struct __pthread	*next;
};

/* Rust's std (library/std/src/sys/pal/plan9/proc.rs), if it is there */
extern void *__plan9_thread_spawn(size_t, void (*)(void *), void *) __attribute__((weak));
extern void __plan9_thread_detach(void *) __attribute__((weak));

/* runs fn(arg) on the stack below top, back on the caller's after */
extern void __apecompat_runon(void *top, void (*fn)(void *), void *arg);
/* rfork(flags); the child goes on at __apecompat_threadstart(t) on the
 * stack below privtop, without touching its parent's */
extern int __apecompat_rforkthread(int flags, void *privtop, struct __pthread *t)
	__attribute__((returns_twice));
_Noreturn void __apecompat_threadstart(struct __pthread *t);

/*
 * amd64: the system call number in BP, the argument at 8(SP) (libap's
 * stubs, gen-9syscall.sh); the kernel gives the child R14 and R15 of
 * its parent (pc64/l.s forkret), the rest zero: privtop and t there.
 * 386: number in AX, argument at 4(SP), INT $64; the child gets every
 * register of its parent but AX: privtop and t in SI and DI.
 * riscv64: number and result in s0 (x8), the argument at 8(sp), ECALL
 * (the stubs of gen-9syscall.sh); the child gets every register of its
 * parent but s0 (riscv64/trap.c forkchild): privtop and t in s1 and s2.
 * arm64: number and result in x0, the argument at 8(SP), SVC; the kernel
 * saves x26-x30, sp, pc and psr of a system call, the child gets those
 * back (arm64/l.s forkret), the rest is the kernel's: privtop and t in
 * x26 and x27.
 */
#if defined(__riscv)
__asm__(
	".text\n"
	".globl __apecompat_runon\n"
	"__apecompat_runon:\n"
	"	addi	sp, sp, -16\n"
	"	sd	ra, 8(sp)\n"
	"	sd	s0, 0(sp)\n"
	"	mv	s0, sp\n"
	"	mv	sp, a0\n"
	"	mv	a0, a2\n"
	"	jalr	a1\n"
	"	mv	sp, s0\n"
	"	ld	ra, 8(sp)\n"
	"	ld	s0, 0(sp)\n"
	"	addi	sp, sp, 16\n"
	"	ret\n"
	".globl __apecompat_rforkthread\n"
	"__apecompat_rforkthread:\n"
	"	addi	sp, sp, -32\n"
	"	sd	s0, 24(sp)\n"
	"	sd	s1, 16(sp)\n"
	"	sd	s2, 0(sp)\n"
	"	mv	s1, a1\n"
	"	mv	s2, a2\n"
	"	sw	a0, 8(sp)\n"
	"	li	s0, 19\n"	/* RFORK */
	"	ecall\n"
	"	beqz	s0, 1f\n"
	"	mv	a0, s0\n"
	"	ld	s0, 24(sp)\n"
	"	ld	s1, 16(sp)\n"
	"	ld	s2, 0(sp)\n"
	"	addi	sp, sp, 32\n"
	"	ret\n"
	"1:	mv	sp, s1\n"
	"	mv	a0, s2\n"
	"	call	__apecompat_threadstart\n"
	"2:	j	2b\n"
);
#elif defined(__x86_64__)
__asm__(
	".text\n"
	".globl __apecompat_runon\n"
	"__apecompat_runon:\n"
	"	pushq	%rbp\n"
	"	movq	%rsp, %rbp\n"
	"	movq	%rdi, %rsp\n"
	"	movq	%rdx, %rdi\n"
	"	call	*%rsi\n"
	"	movq	%rbp, %rsp\n"
	"	popq	%rbp\n"
	"	ret\n"
	".globl __apecompat_rforkthread\n"
	"__apecompat_rforkthread:\n"
	"	pushq	%rbx\n"
	"	pushq	%rbp\n"
	"	pushq	%r12\n"
	"	pushq	%r13\n"
	"	pushq	%r14\n"
	"	pushq	%r15\n"
	"	movq	%rsi, %r14\n"
	"	movq	%rdx, %r15\n"
	"	subq	$16, %rsp\n"
	"	movq	%rdi, 8(%rsp)\n"
	"	movq	$19, %rbp\n"	/* RFORK */
	"	syscall\n"
	"	testq	%rax, %rax\n"
	"	jz	1f\n"
	"	addq	$16, %rsp\n"
	"	popq	%r15\n"
	"	popq	%r14\n"
	"	popq	%r13\n"
	"	popq	%r12\n"
	"	popq	%rbp\n"
	"	popq	%rbx\n"
	"	ret\n"
	"1:	movq	%r14, %rsp\n"
	"	movq	%r15, %rdi\n"
	"	call	__apecompat_threadstart\n"
	"2:	jmp	2b\n"
);
#elif defined(__aarch64__)
__asm__(
	".text\n"
	".globl __apecompat_runon\n"
	"__apecompat_runon:\n"
	"	stp	x29, x30, [sp, #-16]!\n"
	"	mov	x29, sp\n"
	"	mov	sp, x0\n"
	"	mov	x0, x2\n"
	"	blr	x1\n"
	"	mov	sp, x29\n"
	"	ldp	x29, x30, [sp], #16\n"
	"	ret\n"
	".globl __apecompat_rforkthread\n"
	"__apecompat_rforkthread:\n"
	"	sub	sp, sp, #48\n"
	"	str	x0, [sp, #8]\n"	/* the argument, at 8(SP) */
	"	stp	x29, x30, [sp, #16]\n"
	"	stp	x26, x27, [sp, #32]\n"
	"	mov	x26, x1\n"
	"	mov	x27, x2\n"
	"	mov	x0, #19\n"	/* RFORK */
	"	svc	#0\n"
	"	cbz	x0, 1f\n"
	"	ldp	x29, x30, [sp, #16]\n"
	"	ldp	x26, x27, [sp, #32]\n"
	"	add	sp, sp, #48\n"
	"	ret\n"
	"1:	mov	sp, x26\n"
	"	mov	x0, x27\n"
	"	bl	__apecompat_threadstart\n"
	"2:	b	2b\n"
);
#elif defined(__i386__)
__asm__(
	".text\n"
	".globl __apecompat_runon\n"
	"__apecompat_runon:\n"
	"	pushl	%ebp\n"
	"	movl	%esp, %ebp\n"
	"	movl	8(%ebp), %eax\n"
	"	movl	12(%ebp), %ecx\n"
	"	movl	16(%ebp), %edx\n"
	"	movl	%eax, %esp\n"
	"	subl	$12, %esp\n"
	"	pushl	%edx\n"
	"	call	*%ecx\n"
	"	movl	%ebp, %esp\n"
	"	popl	%ebp\n"
	"	ret\n"
	".globl __apecompat_rforkthread\n"
	"__apecompat_rforkthread:\n"
	"	pushl	%esi\n"
	"	pushl	%edi\n"
	"	movl	12(%esp), %eax\n"
	"	movl	16(%esp), %esi\n"
	"	movl	20(%esp), %edi\n"
	"	pushl	%eax\n"	/* the argument, at 4(SP) */
	"	pushl	%eax\n"
	"	movl	$19, %eax\n"	/* RFORK */
	"	int	$64\n"
	"	testl	%eax, %eax\n"
	"	jz	1f\n"
	"	addl	$8, %esp\n"
	"	popl	%edi\n"
	"	popl	%esi\n"
	"	ret\n"
	"1:	movl	%esi, %esp\n"
	"	subl	$12, %esp\n"
	"	pushl	%edi\n"
	"	call	__apecompat_threadstart\n"
	"2:	jmp	2b\n"
);
#else
#error "no __apecompat_runon, __apecompat_rforkthread for this architecture"
#endif

static struct __pthread mainthread;
static int mainpid;	/* main's proc */
static char *privtop;	/* a child's stack, in its own stack segment */
static struct __pthread *threads;	/* all but main */
static int nthreads;	/* running, main included */
static int listlock;
static Key keys[PTHREAD_KEYS_MAX];
static int keylock;

static void
spinlock(int *l)
{
	while(__atomic_exchange_n(l, 1, __ATOMIC_ACQUIRE))
		sched_yield();
}

static void
spinunlock(int *l)
{
	__atomic_store_n(l, 0, __ATOMIC_RELEASE);
}

/* a note interrupts semacquire: wait on */
static void
semwait(int *s)
{
	while(_SEMACQUIRE(s, 1) < 0)
		;
}

static void
sempost(int *s)
{
	_SEMRELEASE(s, 1);
}

/* milliseconds until abstime on clock, rounded up; -1: passed */
static long
msuntil(clockid_t clock, const struct timespec *abstime)
{
	struct timespec now;
	int64_t ns;

	if(abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000)
		return -2;
	clock_gettime(clock, &now);
	ns = (int64_t)(abstime->tv_sec - now.tv_sec) * 1000000000
		+ (abstime->tv_nsec - now.tv_nsec);
	if(ns <= 0)
		return -1;
	return (ns + 999999) / 1000000;
}

/* 1 acquired, 0 timed out, -2 bad time */
static int
semtimedwait(int *s, clockid_t clock, const struct timespec *abstime)
{
	long ms;
	int r;

	for(;;){
		ms = msuntil(clock, abstime);
		if(ms == -2)
			return -2;
		if(ms < 0)
			return _SEMACQUIRE(s, 0) > 0;
		r = _TSEMACQUIRE(s, ms);
		if(r > 0)
			return 1;
		if(r == 0 && msuntil(clock, abstime) < 0)
			return 0;
		/* interrupted, or woke before the time: on */
	}
}

/* before main: main's proc */
__attribute__((constructor)) static void
mainpidinit(void)
{
	mainpid = getpid();
}

pthread_t
pthread_self(void)
{
	pthread_t t;
	int pid;

	t = _privates[0];
	pid = getpid();
	if(t != NULL && t->pid == pid)
		return t;
	if(pid == mainpid || mainpid == 0){
		t = &mainthread;
		if(t->pid == 0){
			t->pid = pid;
			t->state = Detached;
		}
	}else{
		/* a proc of its own (std's threads): the slot was its parent's */
		t = calloc(1, sizeof *t);
		if(t == NULL)
			abort();
		t->pid = pid;
		t->state = Detached;
	}
	_privates[0] = t;
	return t;
}

int
pthread_equal(pthread_t a, pthread_t b)
{
	return a == b;
}

/* proc(3)'s ctl "kill": a note could be caught (signal) */
static void
killproc(int pid)
{
	char path[64];
	int fd;

	snprintf(path, sizeof path, "/proc/%d/ctl", pid);
	fd = open(path, O_WRONLY);
	if(fd >= 0){
		write(fd, "kill", 4);
		close(fd);
	}
}

/*
 * GCC port branch: set before the pids are read; a thread that starts
 * meanwhile (its pid still 0 in the list) stores its pid, then reads this
 * (threadstarted): one of the two sees the other
 */
static int exiting;

/* exit: the other threads go too */
static void
killothers(void)
{
	pthread_t self, t;

	self = pthread_self();
	__atomic_store_n(&exiting, 1, __ATOMIC_SEQ_CST);
	spinlock(&listlock);
	for(t = threads; t != NULL; t = t->next)
		if(t != self && __atomic_load_n(&t->pid, __ATOMIC_SEQ_CST) != 0)
			killproc(t->pid);
	if(self != &mainthread && mainthread.pid != 0)
		killproc(mainthread.pid);
	spinunlock(&listlock);
}

/*
 * GCC port branch: the same for a program that does not end with APE's exit:
 * Go's runtime in a cgo program of ../9front-go (its exit ends with exits;
 * runtime/cgo's gcc_plan9.c calls this first, for threads of C's own)
 */
void
__pthread_killothers(void)
{
	killothers();
}

static void
threadinit(void)
{
	static int once;

	if(__atomic_exchange_n(&once, 1, __ATOMIC_SEQ_CST))
		return;
	pthread_self();
	__atomic_add_fetch(&nthreads, 1, __ATOMIC_SEQ_CST);	/* main */
	atexit(killothers);
}

/* libgcc's __wrap_main (config/i386/plan9-main.c): main on a stack from
 * malloc, as the other threads (their locals are shared memory) */
struct Mainargs {
	int	(*main)(int, char **, char **);
	int	argc;
	char	**argv;
	char	**envp;
	int	ret;
};

static void
runmain(void *v)
{
	struct Mainargs *a = v;

	a->ret = a->main(a->argc, a->argv, a->envp);
}

__attribute__((weak)) int
__plan9_mainstack(int (*main)(int, char **, char **), int argc, char **argv, char **envp)
{
	struct Mainargs a;
	char *stack;

	/* once: APE's _apemain, then libgcc's __wrap_main (--wrap=main) */
	if(mainthread.stack != NULL)
		return main(argc, argv, envp);
	stack = malloc(MAINSTACK);
	if(stack == NULL)
		return main(argc, argv, envp);
	/* below this frame (and runon's) the segment is not used again */
	privtop = (char *)((uintptr_t)((char *)&a - 65536) & ~(uintptr_t)15);
	threadinit();
	mainthread.stack = stack;
	mainthread.stacksize = MAINSTACK;
	a.main = main;
	a.argc = argc;
	a.argv = argv;
	a.envp = envp;
	a.ret = 0;
	__apecompat_runon((void *)((uintptr_t)(stack + MAINSTACK) & ~(uintptr_t)15), runmain, &a);
	return a.ret;
}

/* the thread's keys' destructors, as POSIX: rounds while values remain */
static void
tsdclean(pthread_t t)
{
	void (*dtor)(void *);
	void *v;
	int i, round, again;

	for(round = 0; round < PTHREAD_DESTRUCTOR_ITERATIONS; round++){
		again = 0;
		for(i = 0; i < PTHREAD_KEYS_MAX; i++){
			v = t->tsd[i].val;
			if(v == NULL || t->tsd[i].seq != keys[i].seq || !(keys[i].seq & 1))
				continue;
			t->tsd[i].val = NULL;
			dtor = keys[i].dtor;
			if(dtor != NULL){
				dtor(v);
				again = 1;
			}
		}
		if(!again)
			break;
	}
}

static void
threadrun(void *v)
{
	pthread_t t = v;

	t->ret = t->fn(t->arg);
	tsdclean(t);
}

static void
unlist(pthread_t t)
{
	pthread_t *l;

	spinlock(&listlock);
	for(l = &threads; *l != NULL; l = &(*l)->next)
		if(*l == t){
			*l = t->next;
			break;
		}
	spinunlock(&listlock);
}

static void
freethread(pthread_t t)
{
	free(t->stack);
	free(t);
}

/* the thread is through: off the stack from malloc, which can go */
static void
threadend(pthread_t t)
{
	int s;

	unlist(t);
	__atomic_sub_fetch(&nthreads, 1, __ATOMIC_SEQ_CST);
	sempost(&mainthread.done);	/* main's pthread_exit waits for the last */
	s = Running;
	if(__atomic_compare_exchange_n(&t->state, &s, Exited, 0,
	    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
		sempost(&t->done);	/* pthread_join frees it */
	else
		freethread(t);	/* detached */
}

/*
 * a new thread's slots: slot 0 its struct __pthread, the others empty, as
 * thread-local storage starts; its copy of the stack segment has its
 * parent's (Go's g in slot 1 of a cgo program of ../9front-go: a C thread
 * calling Go took its parent's g for its own)
 */
static void
privinit(pthread_t t)
{
	memset(_privates, 0, _nprivates * sizeof _privates[0]);
	_privates[0] = t;
}

/* GCC port branch: the pid, then exiting (killothers): an exit that began
 * while this thread was made has passed it in the list */
static void
threadstarted(pthread_t t)
{
	__atomic_store_n(&t->pid, getpid(), __ATOMIC_SEQ_CST);
	if(__atomic_load_n(&exiting, __ATOMIC_SEQ_CST))
		_EXITS(NULL);
}

/* the new proc: on its own copy of the stack segment */
void
__apecompat_threadstart(pthread_t t)
{
	void (*fn)(void *);
	void *arg;

	if(t->state == Proc){
		fn = t->procfn;
		arg = t->arg;
		free(t);
		fn(arg);
		_EXITS(NULL);
	}
	privinit(t);
	threadstarted(t);
	if(setjmp(t->exitjmp) == 0)
		__apecompat_runon((void *)((uintptr_t)(t->stack + t->stacksize) & ~(uintptr_t)15),
			threadrun, t);
	threadend(t);
	_EXITS(NULL);
	for(;;)
		;
}

/* a thread of std's: on a stack slot of its own, std ends the proc */
static void
stdthreadstart(void *v)
{
	pthread_t t = v;

	privinit(t);
	threadstarted(t);
	if(setjmp(t->exitjmp) == 0)
		threadrun(t);
	threadend(t);
}

static char *
childtop(void)
{
	char here;

	/* main on its own stack (no __wrap_main): the child's copy of the
	 * segment is free below the creator's frame */
	if(privtop == NULL)
		privtop = (char *)((uintptr_t)(&here - 65536) & ~(uintptr_t)15);
	return privtop;
}

/*
 * fn(arg) in a new proc, rfork(flags) with RFPROC and RFMEM, on its own
 * copy of the stack segment (APE's read buffering, buf.c: main's stack
 * here is shared memory)
 */
int
__apecompat_rforkproc(int flags, void (*fn)(void *), void *arg)
{
	pthread_t t;
	int pid;

	t = calloc(1, sizeof *t);
	if(t == NULL){
		errno = ENOMEM;
		return -1;
	}
	t->procfn = fn;
	t->arg = arg;
	t->state = Proc;
	pid = __apecompat_rforkthread(flags, childtop(), t);
	if(pid < 0)
		free(t);
	return pid;
}

int
pthread_create(pthread_t *tp, const pthread_attr_t *attr, void *(*fn)(void *), void *arg)
{
	pthread_t t;
	size_t size;
	void *h;
	int pid;

	threadinit();
	size = attr != NULL && attr->__stacksize != 0 ? attr->__stacksize : STACK;
	t = calloc(1, sizeof *t);
	if(t == NULL)
		return EAGAIN;
	if(__plan9_thread_spawn == NULL){
		t->stack = malloc(size);
		if(t->stack == NULL){
			free(t);
			return EAGAIN;
		}
		t->stacksize = size;
	}
	t->fn = fn;
	t->arg = arg;
	t->state = attr != NULL && attr->__detach == PTHREAD_CREATE_DETACHED ? Detached : Running;
	spinlock(&listlock);
	t->next = threads;
	threads = t;
	spinunlock(&listlock);
	__atomic_add_fetch(&nthreads, 1, __ATOMIC_SEQ_CST);
	*tp = t;
	if(__plan9_thread_spawn != NULL){
		/* joined through t->done; std frees its stack slot */
		h = __plan9_thread_spawn(size, stdthreadstart, t);
		if(h != NULL)
			__plan9_thread_detach(h);
		pid = h != NULL ? 0 : -1;
	}else
		pid = __apecompat_rforkthread(RFPROC|RFMEM|RFNOWAIT, childtop(), t);
	if(pid < 0){
		unlist(t);
		__atomic_sub_fetch(&nthreads, 1, __ATOMIC_SEQ_CST);
		freethread(t);
		return EAGAIN;
	}
	return 0;
}

int
pthread_join(pthread_t t, void **ret)
{
	if(t == pthread_self())
		return EDEADLK;
	if(t == &mainthread || t->state == Detached)
		return EINVAL;
	semwait(&t->done);
	if(ret != NULL)
		*ret = t->ret;
	freethread(t);
	return 0;
}

int
pthread_detach(pthread_t t)
{
	int s;

	if(t == &mainthread)
		return 0;
	s = Running;
	if(__atomic_compare_exchange_n(&t->state, &s, Detached, 0,
	    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
		return 0;
	if(s == Detached)
		return EINVAL;
	/* it is through and waited for a join */
	semwait(&t->done);
	freethread(t);
	return 0;
}

void
pthread_exit(void *ret)
{
	pthread_t t;

	t = pthread_self();
	t->ret = ret;
	tsdclean(t);
	if(t != &mainthread)
		longjmp(t->exitjmp, 1);
	/* main: the process goes on until the last thread is through */
	while(__atomic_load_n(&nthreads, __ATOMIC_SEQ_CST) > 1)
		semwait(&mainthread.done);
	exit(0);
}

/* no cancellation */
int
pthread_cancel(pthread_t t)
{
	return ENOTSUP;
}

int
pthread_attr_init(pthread_attr_t *a)
{
	a->__detach = PTHREAD_CREATE_JOINABLE;
	a->__stacksize = STACK;
	return 0;
}

int
pthread_attr_destroy(pthread_attr_t *a)
{
	return 0;
}

int
pthread_attr_setdetachstate(pthread_attr_t *a, int d)
{
	if(d != PTHREAD_CREATE_JOINABLE && d != PTHREAD_CREATE_DETACHED)
		return EINVAL;
	a->__detach = d;
	return 0;
}

int
pthread_attr_getdetachstate(const pthread_attr_t *a, int *d)
{
	*d = a->__detach;
	return 0;
}

int
pthread_attr_setstacksize(pthread_attr_t *a, size_t size)
{
	if(size < PTHREAD_STACK_MIN)
		return EINVAL;
	a->__stacksize = size;
	return 0;
}

int
pthread_attr_getstacksize(const pthread_attr_t *a, size_t *size)
{
	*size = a->__stacksize;
	return 0;
}

/* mutexes */

int
pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a)
{
	memset(m, 0, sizeof *m);
	m->__type = a != NULL ? a->__type : PTHREAD_MUTEX_DEFAULT;
	return 0;
}

int
pthread_mutex_destroy(pthread_mutex_t *m)
{
	return __atomic_load_n(&m->__val, __ATOMIC_SEQ_CST) != 0 ? EBUSY : 0;
}

/* 0 the word is ours; else ETIMEDOUT or EINVAL (abstime) */
static int
lockword(pthread_mutex_t *m, const struct timespec *abstime)
{
	int c, r;

	c = 0;
	if(__atomic_compare_exchange_n(&m->__val, &c, 1, 0,
	    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
		return 0;
	if(c != 2)
		c = __atomic_exchange_n(&m->__val, 2, __ATOMIC_ACQUIRE);
	while(c != 0){
		if(abstime == NULL)
			semwait(&m->__sem);
		else{
			r = semtimedwait(&m->__sem, CLOCK_REALTIME, abstime);
			if(r == -2)
				return EINVAL;
			if(r == 0)
				return ETIMEDOUT;
		}
		c = __atomic_exchange_n(&m->__val, 2, __ATOMIC_ACQUIRE);
	}
	return 0;
}

static int
mutexlock(pthread_mutex_t *m, const struct timespec *abstime)
{
	pthread_t self;
	int r;

	if(m->__type == PTHREAD_MUTEX_NORMAL)
		return lockword(m, abstime);
	self = pthread_self();
	if(m->__owner == self){
		if(m->__type == PTHREAD_MUTEX_ERRORCHECK)
			return EDEADLK;
		m->__count++;
		return 0;
	}
	r = lockword(m, abstime);
	if(r != 0)
		return r;
	m->__owner = self;
	m->__count = 1;
	return 0;
}

int
pthread_mutex_lock(pthread_mutex_t *m)
{
	return mutexlock(m, NULL);
}

int
pthread_mutex_timedlock(pthread_mutex_t *m, const struct timespec *abstime)
{
	return mutexlock(m, abstime);
}

int
pthread_mutex_trylock(pthread_mutex_t *m)
{
	pthread_t self;
	int c;

	self = NULL;
	if(m->__type != PTHREAD_MUTEX_NORMAL){
		self = pthread_self();
		if(m->__owner == self){
			if(m->__type == PTHREAD_MUTEX_ERRORCHECK)
				return EBUSY;
			m->__count++;
			return 0;
		}
	}
	c = 0;
	if(!__atomic_compare_exchange_n(&m->__val, &c, 1, 0,
	    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
		return EBUSY;
	if(self != NULL){
		m->__owner = self;
		m->__count = 1;
	}
	return 0;
}

int
pthread_mutex_unlock(pthread_mutex_t *m)
{
	if(m->__type != PTHREAD_MUTEX_NORMAL){
		if(m->__owner != pthread_self())
			return EPERM;
		if(--m->__count > 0)
			return 0;
		m->__owner = NULL;
	}
	if(__atomic_exchange_n(&m->__val, 0, __ATOMIC_RELEASE) == 2)
		sempost(&m->__sem);
	return 0;
}

int
pthread_mutexattr_init(pthread_mutexattr_t *a)
{
	a->__type = PTHREAD_MUTEX_DEFAULT;
	return 0;
}

int
pthread_mutexattr_destroy(pthread_mutexattr_t *a)
{
	return 0;
}

int
pthread_mutexattr_settype(pthread_mutexattr_t *a, int type)
{
	if(type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE
	&& type != PTHREAD_MUTEX_ERRORCHECK)
		return EINVAL;
	a->__type = type;
	return 0;
}

int
pthread_mutexattr_gettype(const pthread_mutexattr_t *a, int *type)
{
	*type = a->__type;
	return 0;
}

/* conditions: the waiters in a queue, each on its own semaphore */

int
pthread_cond_init(pthread_cond_t *c, const pthread_condattr_t *a)
{
	memset(c, 0, sizeof *c);
	c->__clock = a != NULL ? a->__clock : CLOCK_REALTIME;
	return 0;
}

int
pthread_cond_destroy(pthread_cond_t *c)
{
	return c->__head != NULL ? EBUSY : 0;
}

static int
condwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *abstime)
{
	pthread_t self, prev, *l;
	int r, w;

	self = pthread_self();
	spinlock(&c->__lock);
	self->signaled = 0;
	self->cnext = NULL;
	if(c->__tail != NULL)
		c->__tail->cnext = self;
	else
		c->__head = self;
	c->__tail = self;
	spinunlock(&c->__lock);

	pthread_mutex_unlock(m);
	if(abstime == NULL){
		semwait(&self->wsem);
		r = 0;
	}else{
		w = semtimedwait(&self->wsem, c->__clock, abstime);
		r = 0;
		if(w != 1){
			spinlock(&c->__lock);
			if(!self->signaled){
				/* still queued: out, and the time is up */
				prev = NULL;
				for(l = &c->__head; *l != NULL; prev = *l, l = &(*l)->cnext)
					if(*l == self){
						*l = self->cnext;
						if(c->__tail == self)
							c->__tail = prev;
						break;
					}
				r = w == -2 ? EINVAL : ETIMEDOUT;
			}
			spinunlock(&c->__lock);
			if(r == 0)	/* signaled meanwhile: its post is due */
				semwait(&self->wsem);
		}
	}
	mutexlock(m, NULL);
	return r;
}

int
pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
	return condwait(c, m, NULL);
}

int
pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *abstime)
{
	return condwait(c, m, abstime);
}

int
pthread_cond_signal(pthread_cond_t *c)
{
	pthread_t t;

	spinlock(&c->__lock);
	t = c->__head;
	if(t != NULL){
		c->__head = t->cnext;
		if(c->__head == NULL)
			c->__tail = NULL;
		t->signaled = 1;
	}
	spinunlock(&c->__lock);
	if(t != NULL)
		sempost(&t->wsem);
	return 0;
}

int
pthread_cond_broadcast(pthread_cond_t *c)
{
	pthread_t t, next;

	spinlock(&c->__lock);
	t = c->__head;
	c->__head = c->__tail = NULL;
	for(next = t; next != NULL; next = next->cnext)
		next->signaled = 1;
	spinunlock(&c->__lock);
	for(; t != NULL; t = next){
		next = t->cnext;	/* before the post: then it may go */
		sempost(&t->wsem);
	}
	return 0;
}

int
pthread_condattr_init(pthread_condattr_t *a)
{
	a->__clock = CLOCK_REALTIME;
	return 0;
}

int
pthread_condattr_destroy(pthread_condattr_t *a)
{
	return 0;
}

int
pthread_condattr_setclock(pthread_condattr_t *a, clockid_t clock)
{
	if(clock != CLOCK_REALTIME && clock != CLOCK_MONOTONIC)
		return EINVAL;
	a->__clock = clock;
	return 0;
}

int
pthread_condattr_getclock(const pthread_condattr_t *a, clockid_t *clock)
{
	*clock = a->__clock;
	return 0;
}

/* read-write locks, writers first */

int
pthread_rwlock_init(pthread_rwlock_t *rw, const pthread_rwlockattr_t *a)
{
	memset(rw, 0, sizeof *rw);
	rw->__readers.__clock = CLOCK_REALTIME;
	rw->__writers.__clock = CLOCK_REALTIME;
	return 0;
}

int
pthread_rwlock_destroy(pthread_rwlock_t *rw)
{
	return rw->__nreaders != 0 || rw->__writer ? EBUSY : 0;
}

static int
rdlock(pthread_rwlock_t *rw, int try, const struct timespec *abstime)
{
	int r;

	r = 0;
	pthread_mutex_lock(&rw->__m);
	while(rw->__writer || rw->__wwait > 0){
		if(try){
			r = EBUSY;
			break;
		}
		if(abstime == NULL)
			r = pthread_cond_wait(&rw->__readers, &rw->__m);
		else
			r = pthread_cond_timedwait(&rw->__readers, &rw->__m, abstime);
		if(r != 0)
			break;
	}
	if(r == 0)
		rw->__nreaders++;
	pthread_mutex_unlock(&rw->__m);
	return r;
}

static int
wrlock(pthread_rwlock_t *rw, int try, const struct timespec *abstime)
{
	int r;

	r = 0;
	pthread_mutex_lock(&rw->__m);
	rw->__wwait++;
	while(rw->__writer || rw->__nreaders > 0){
		if(try){
			r = EBUSY;
			break;
		}
		if(abstime == NULL)
			r = pthread_cond_wait(&rw->__writers, &rw->__m);
		else
			r = pthread_cond_timedwait(&rw->__writers, &rw->__m, abstime);
		if(r != 0)
			break;
	}
	rw->__wwait--;
	if(r == 0)
		rw->__writer = 1;
	else if(rw->__wwait == 0 && !rw->__writer)
		pthread_cond_broadcast(&rw->__readers);	/* they waited for us */
	pthread_mutex_unlock(&rw->__m);
	return r;
}

int
pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
	return rdlock(rw, 0, NULL);
}

int
pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	return rdlock(rw, 1, NULL);
}

int
pthread_rwlock_timedrdlock(pthread_rwlock_t *rw, const struct timespec *abstime)
{
	return rdlock(rw, 0, abstime);
}

int
pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
	return wrlock(rw, 0, NULL);
}

int
pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	return wrlock(rw, 1, NULL);
}

int
pthread_rwlock_timedwrlock(pthread_rwlock_t *rw, const struct timespec *abstime)
{
	return wrlock(rw, 0, abstime);
}

int
pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	pthread_mutex_lock(&rw->__m);
	if(rw->__writer)
		rw->__writer = 0;
	else if(rw->__nreaders > 0)
		rw->__nreaders--;
	if(rw->__nreaders == 0){
		if(rw->__wwait > 0)
			pthread_cond_signal(&rw->__writers);
		else
			pthread_cond_broadcast(&rw->__readers);
	}
	pthread_mutex_unlock(&rw->__m);
	return 0;
}

/* once: 0 not yet, 1 running, 2 done */

static pthread_mutex_t oncelock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t oncecond = PTHREAD_COND_INITIALIZER;

int
pthread_once(pthread_once_t *o, void (*fn)(void))
{
	int s;

	if(__atomic_load_n(o, __ATOMIC_ACQUIRE) == 2)
		return 0;
	s = 0;
	if(__atomic_compare_exchange_n(o, &s, 1, 0, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)){
		fn();
		pthread_mutex_lock(&oncelock);
		__atomic_store_n(o, 2, __ATOMIC_RELEASE);
		pthread_cond_broadcast(&oncecond);
		pthread_mutex_unlock(&oncelock);
		return 0;
	}
	pthread_mutex_lock(&oncelock);
	while(__atomic_load_n(o, __ATOMIC_ACQUIRE) != 2)
		pthread_cond_wait(&oncecond, &oncelock);
	pthread_mutex_unlock(&oncelock);
	return 0;
}

/* keys: a value counts while its key's sequence number is the one it
 * was set under (a deleted key's slot comes back with a new one) */

int
pthread_key_create(pthread_key_t *k, void (*dtor)(void *))
{
	int i;

	spinlock(&keylock);
	for(i = 0; i < PTHREAD_KEYS_MAX; i++)
		if(!(keys[i].seq & 1)){
			keys[i].seq++;
			keys[i].dtor = dtor;
			spinunlock(&keylock);
			*k = i;
			return 0;
		}
	spinunlock(&keylock);
	return EAGAIN;
}

int
pthread_key_delete(pthread_key_t k)
{
	if(k >= PTHREAD_KEYS_MAX || !(keys[k].seq & 1))
		return EINVAL;
	spinlock(&keylock);
	keys[k].seq++;
	keys[k].dtor = NULL;
	spinunlock(&keylock);
	return 0;
}

void *
pthread_getspecific(pthread_key_t k)
{
	pthread_t t;

	if(k >= PTHREAD_KEYS_MAX)
		return NULL;
	t = pthread_self();
	if(t->tsd[k].seq != keys[k].seq)
		return NULL;
	return t->tsd[k].val;
}

int
pthread_setspecific(pthread_key_t k, const void *v)
{
	pthread_t t;

	if(k >= PTHREAD_KEYS_MAX || !(keys[k].seq & 1))
		return EINVAL;
	t = pthread_self();
	t->tsd[k].seq = keys[k].seq;
	t->tsd[k].val = (void *)v;
	return 0;
}

int
pthread_barrier_init(pthread_barrier_t *b, const pthread_barrierattr_t *a, unsigned count)
{
	(void)a;
	if(count == 0)
		return EINVAL;
	pthread_mutex_init(&b->__m, NULL);
	pthread_cond_init(&b->__c, NULL);
	b->__count = count;
	b->__waiting = 0;
	b->__generation = 0;
	return 0;
}

int
pthread_barrier_destroy(pthread_barrier_t *b)
{
	return b->__waiting != 0 ? EBUSY : 0;
}

/* the last to arrive wakes the others and is the serial thread */
int
pthread_barrier_wait(pthread_barrier_t *b)
{
	unsigned gen;

	pthread_mutex_lock(&b->__m);
	gen = b->__generation;
	if(++b->__waiting == b->__count){
		b->__waiting = 0;
		b->__generation++;
		pthread_cond_broadcast(&b->__c);
		pthread_mutex_unlock(&b->__m);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	}
	while(gen == b->__generation)
		pthread_cond_wait(&b->__c, &b->__m);
	pthread_mutex_unlock(&b->__m);
	return 0;
}

int
pthread_barrierattr_init(pthread_barrierattr_t *a)
{
	a->__dummy = 0;
	return 0;
}

int
pthread_barrierattr_destroy(pthread_barrierattr_t *a)
{
	(void)a;
	return 0;
}

int
pthread_sigmask(int how, const sigset_t *set, sigset_t *old)
{
	return sigprocmask(how, (sigset_t *)set, old) < 0 ? errno : 0;
}

int
pthread_getcpuclockid(pthread_t t, clockid_t *id)
{
	(void)t;
	(void)id;
	return ENOENT;
}
