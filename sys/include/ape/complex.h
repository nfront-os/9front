/*
 * APE (GCC port): C99's <complex.h>. The functions are
 * musl's (libap: ape/lib/musl); creal, cimag, conj and CMPLX
 * gcc's builtins.
 */
#ifndef __APE_COMPLEX
#define __APE_COMPLEX

#ifdef __cplusplus
extern "C" {
#endif

#define complex		_Complex
#define _Complex_I	(__extension__ 1.0iF)
#define I		_Complex_I

double complex cacos(double complex);
float complex cacosf(float complex);
long double complex cacosl(long double complex);
double complex casin(double complex);
float complex casinf(float complex);
long double complex casinl(long double complex);
double complex catan(double complex);
float complex catanf(float complex);
long double complex catanl(long double complex);
double complex ccos(double complex);
float complex ccosf(float complex);
long double complex ccosl(long double complex);
double complex csin(double complex);
float complex csinf(float complex);
long double complex csinl(long double complex);
double complex ctan(double complex);
float complex ctanf(float complex);
long double complex ctanl(long double complex);
double complex cacosh(double complex);
float complex cacoshf(float complex);
long double complex cacoshl(long double complex);
double complex casinh(double complex);
float complex casinhf(float complex);
long double complex casinhl(long double complex);
double complex catanh(double complex);
float complex catanhf(float complex);
long double complex catanhl(long double complex);
double complex ccosh(double complex);
float complex ccoshf(float complex);
long double complex ccoshl(long double complex);
double complex csinh(double complex);
float complex csinhf(float complex);
long double complex csinhl(long double complex);
double complex ctanh(double complex);
float complex ctanhf(float complex);
long double complex ctanhl(long double complex);
double complex cexp(double complex);
float complex cexpf(float complex);
long double complex cexpl(long double complex);
double complex clog(double complex);
float complex clogf(float complex);
long double complex clogl(long double complex);
double cabs(double complex);
float cabsf(float complex);
long double cabsl(long double complex);
double complex cpow(double complex, double complex);
float complex cpowf(float complex, float complex);
long double complex cpowl(long double complex, long double complex);
double complex csqrt(double complex);
float complex csqrtf(float complex);
long double complex csqrtl(long double complex);
double carg(double complex);
float cargf(float complex);
long double cargl(long double complex);
double complex conj(double complex);
float complex conjf(float complex);
long double complex conjl(long double complex);
double complex cproj(double complex);
float complex cprojf(float complex);
long double complex cprojl(long double complex);

double		creal(double complex);
float		crealf(float complex);
long double	creall(long double complex);
double		cimag(double complex);
float		cimagf(float complex);
long double	cimagl(long double complex);

#define creal(z)	__builtin_creal(z)
#define crealf(z)	__builtin_crealf(z)
#define creall(z)	__builtin_creall(z)
#define cimag(z)	__builtin_cimag(z)
#define cimagf(z)	__builtin_cimagf(z)
#define cimagl(z)	__builtin_cimagl(z)
#define conj(z)		__builtin_conj(z)
#define conjf(z)	__builtin_conjf(z)
#define conjl(z)	__builtin_conjl(z)

#define CMPLX(x, y)	__builtin_complex((double)(x), (double)(y))
#define CMPLXF(x, y)	__builtin_complex((float)(x), (float)(y))
#define CMPLXL(x, y)	__builtin_complex((long double)(x), (long double)(y))

#ifdef __cplusplus
}
#endif
#endif
