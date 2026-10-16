/*
 * Host unit tests for the GAS ports of 9front's lib387
 * (sys/src/libc/386/387/*.S, symbols prefixed p9_, see run.sh): the
 * x87 routines against the host's libm (i386: x87 too) on random
 * arguments, 100000 calls each -- an unbalanced x87 stack would
 * overflow and turn the results into NaN --, NaN for arguments out of
 * the domain (the routines call NaN()), and the callee-saved registers
 * and the direction flag (callcheck386.s).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double p9_asin(double), p9_acos(double), p9_atan(double), p9_atan2(double, double);
double p9_exp(double), p9_log(double), p9_log10(double);
double p9_sin(double), p9_cos(double), p9_tan(double), p9_sqrt(double);
long callcheck(void *fn, long a, long b, long c, long d, ...);

/* libc's NaN(), which the routines call out of the domain */
double p9_NaN(void) { return NAN; }

static int nfail, ntest;

#define T(name, cond) do{ ntest++; if(!(cond)){ nfail++; if(nfail < 20) printf("FAIL %s (%s:%d)\n", name, __FILE__, __LINE__); } }while(0)

static int
near(double a, double b)
{
	if(isnan(a) || isnan(b))
		return isnan(a) && isnan(b);
	return fabs(a - b) <= 1e-13 * (fabs(b) > 1 ? fabs(b) : 1);
}

static double
r(double lo, double hi)
{
	return lo + (hi - lo) * (rand() / (double)RAND_MAX);
}

int
main(void)
{
	int i;
	double x, y;

	srand(1);
	for(i = 0; i < 100000; i++){
		x = r(-1, 1);
		T("asin", near(p9_asin(x), asin(x)));
		T("acos", near(p9_acos(x), acos(x)));
		x = r(-1e3, 1e3);
		y = r(-1e3, 1e3);
		T("atan", near(p9_atan(x), atan(x)));
		T("atan2", near(p9_atan2(x, y), atan2(x, y)));
		x = r(-700, 700);
		T("exp", near(p9_exp(x), exp(x)));
		x = r(1e-300, 1e300);
		T("log", near(p9_log(x), log(x)));
		T("log10", near(p9_log10(x), log10(x)));
		T("sqrt", near(p9_sqrt(x), sqrt(x)));
		x = r(-1e3, 1e3);
		T("sin", near(p9_sin(x), sin(x)));
		T("cos", near(p9_cos(x), cos(x)));
		T("tan", near(p9_tan(x), tan(x)));
	}
	T("asin domain", isnan(p9_asin(2)));
	T("acos domain", isnan(p9_acos(-2)));
	T("log domain", isnan(p9_log(-1)));
	T("log10 domain", isnan(p9_log10(-1)));
	T("sqrt domain", isnan(p9_sqrt(-1)));
	T("asin after domain errors", near(p9_asin(0.5), asin(0.5)));
	T("callee-saved/DF asin", callcheck(p9_asin, 0, 0x3fe00000, 0, 0) == 0);
	T("callee-saved/DF log", callcheck(p9_log, 0, 0xbff00000, 0, 0) == 0);
	T("callee-saved/DF exp", callcheck(p9_exp, 0, 0x3ff00000, 0, 0) == 0);
	printf("%d tests, %d failed\n", ntest, nfail);
	return nfail != 0;
}
