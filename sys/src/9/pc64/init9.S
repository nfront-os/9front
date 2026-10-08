/*
 * GCC port: GAS/System V version of ../pc/init9.c, which 9front's pc64
 * mkfile links into initcode.out:
 *
 *	_main(char *argv0) { startboot(argv0, &argv0); }
 *
 * init0() (main.c) leaves 0(%rsp) = nil (pc), 8(%rsp) = argv0 ("boot"),
 * 16(%rsp) = nil; 6c took argv0 from the stack, so &argv0 is the argv.
 * Under System V the arguments go in %rdi, %rsi.
 */
.globl _main
_main:
	movq	8(%rsp), %rdi		/* argv0 */
	leaq	8(%rsp), %rsi		/* argv = &argv0 */
	andq	$~15, %rsp
	call	startboot
1:
	jmp	1b
