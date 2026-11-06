/*
 * GCC port: musl 1.2.5's src/string/x86_64/memmove.s (MIT: sys/src/ape/lib/musl/COPYRIGHT),
 * unchanged, instead of a translation of memmove.s: APE on amd64 had only
 * gen/memmove.c, a byte at a time (a read of the card's memory through BAR 0
 * is one uncached access each: SDL's 300K readback took 8 s); musl's copies
 * whole quadwords (rep movsq) once the destination is aligned, whatever the
 * source's alignment.
 */
.global memmove
.type memmove,@function
memmove:
	mov %rdi,%rax
	sub %rsi,%rax
	cmp %rdx,%rax
.hidden __memcpy_fwd
	jae __memcpy_fwd
	mov %rdx,%rcx
	lea -1(%rdi,%rdx),%rdi
	lea -1(%rsi,%rdx),%rsi
	std
	rep movsb
	cld
	lea 1(%rdi),%rax
	ret
