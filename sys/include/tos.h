typedef struct Tos Tos;
typedef struct Plink Plink;
typedef struct Callq Callq;	/* NIX: nixcall(2) */
typedef struct Nixcall Nixcall;
typedef struct Nixret Nixret;

#pragma incomplete Plink

/*
 * NIX: system calls through a queue (nixcall(2)), after NIX's Callq;
 * a kernel process makes the calls: no trap per call.
 */
enum {
	CALLQSZ = 64,		/* NIX had 512 */
	NIXCALLARG = 8,		/* argument slots, as the system call stubs pass them */
};

struct Nixcall
{
	void	*tag;
	int	scall;
	uvlong	arg[NIXCALLARG];	/* NIX: a va_list, not portable to gcc */
};

struct Nixret
{
	void	*tag;
	vlong	sret;
	char	err[128];		/* ERRMAX; NIX: a kernel pointer */
};

struct Callq
{
	int	ksleep;			/* the kernel process sleeps: wake it */
	unsigned int	qr;	/* not uint: APE includes this too */
	unsigned int	qw;
	unsigned int	rr;
	unsigned int	rw;
	Nixcall	q[CALLQSZ];
	Nixret	r[CALLQSZ];
};

struct Tos {
	struct			/* Per process profiling */
	{
		Plink	*pp;	/* known to be 0(ptr) */
		Plink	*next;	/* known to be 4(ptr) */
		Plink	*last;
		Plink	*first;
		ulong	pid;
		ulong	what;
	} prof;
	uvlong	cyclefreq;	/* cycle clock frequency if there is one, 0 otherwise */
	vlong	kcycles;	/* cycles spent in kernel */
	vlong	pcycles;	/* cycles spent in process (kernel + user) */
	ulong	pid;		/* might as well put the pid here */
	ulong	clock;
	int	nixtype;	/* NIX: role of the core we run on (getcore(2)) */
	int	core;		/* NIX: the core we run on */
	Callq	callq;		/* NIX: queue-based system calls */
	/* top of stack is here */
};

extern Tos *_tos;
