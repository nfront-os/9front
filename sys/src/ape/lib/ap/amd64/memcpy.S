/*
 * GCC port: musl 1.2.5's src/string/x86_64/memcpy.s (MIT: sys/src/ape/lib/musl/COPYRIGHT),
 * unchanged, instead of a translation of memcpy.s: APE on amd64 had only
 * gen/memmove.c, a byte at a time (a read of the card's memory through BAR 0
 * is one uncached access each: SDL's 300K readback took 8 s); musl's copies
 * whole quadwords (rep movsq) once the destination is aligned, whatever the
 * source's alignment.
 */
.global memcpy
.global __memcpy_fwd
.hidden __memcpy_fwd
.type memcpy,@function
memcpy:
__memcpy_fwd:
	mov %rdi,%rax
	cmp $8,%rdx
	jc 1f
	test $7,%edi
	jz 1f
2:	movsb
	dec %rdx
	test $7,%edi
	jnz 2b
1:	mov %rdx,%rcx
	shr $3,%rcx
	rep
	movsq
	and $7,%edx
	jz 1f
2:	movsb
	dec %edx
	jnz 2b
1:	ret
