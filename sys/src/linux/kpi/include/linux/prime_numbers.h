/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/prime_numbers.h (ours, not Linux's: GPL) */
#ifndef _KPI_LINUX_PRIME_NUMBERS_H
#define _KPI_LINUX_PRIME_NUMBERS_H
#include <linux/types.h>
/* the primes, by trial division (kpi/linux/lib.c): the selftests' loops */
bool	is_prime_number(unsigned long x);
unsigned long	next_prime_number(unsigned long x);	/* the first prime after x */
#define for_each_prime_number(prime, max)	for_each_prime_number_from(prime, 1, max)
/* from (1 counts, as with Linux's), then the primes up to max */
#define for_each_prime_number_from(prime, from, max) \
	for (prime = (from); prime <= (max); prime = next_prime_number(prime))
#endif
