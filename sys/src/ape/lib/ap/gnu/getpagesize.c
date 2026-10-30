/*
 * BSD's getpagesize (libbacktrace's mmap path), an object of its own:
 * programs with their own (libiberty's) keep theirs.
 */
int
getpagesize(void)
{
	return 4096;
}
