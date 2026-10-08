/*
 * GCC port: GAS translation. 6a's GLOBL reserves zero-filled common
 * storage; .comm does the same, so a program defining its own argv0
 * (rc: char *argv0 = "rc") still links.
 */
	.comm	argv0, 8, 8
	.comm	_tos, 8, 8
