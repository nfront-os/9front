#include <atom.h>

#define ARG	8

/* atomically set *keyp non-zero and return previous contents */
TEXT	tas(SB), 1, $-4			/* int tas(int *keyp) */
	MOV	$1, R10
	FENCE_RW
	AMOW(Amoswap, AQ|RL, 10, ARG, ARG)
	FENCE_RW
	RET

/* unsigned long long _barrier(unsigned long long): fence, return the argument */
TEXT	_barrier(SB), 1, $-4
	FENCE
	RET
