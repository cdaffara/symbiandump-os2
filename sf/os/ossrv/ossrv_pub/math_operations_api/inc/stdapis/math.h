/*
* Portions Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * from: @(#)fdlibm.h 5.1 93/09/24
 * $FreeBSD: src/lib/msun/src/math.h,v 1.61 2005/04/16 21:12:47 das Exp $
 */

#ifndef _MATH_H_
#define	_MATH_H_

#if (defined(__SYMBIAN32__) && !defined(SYMBIAN))
#define SYMBIAN
#endif

#ifdef __SYMBIAN32__
#ifdef __VC32__
/* warning C4056: overflow in floating-point constant arithmetic 
 * Caused by negative floating point constants, it seems!
 * For example, static double foo = -1.0;
 */
#pragma warning( disable: 4056 )
#endif
#ifdef __ARMCC__
/* Warning:  #222-D: floating-point operation result is out of range
 * The compiler detects constant math that overflows, we want overflow though!
 * Warning:  #39-D: division by zero
*/
#pragma diag_suppress 222
#pragma diag_suppress 39
#pragma diag_suppress 2874
#endif
#endif //__SYMBIAN32__

#ifdef __SYMBIAN32__
#include <_ansi.h>
#endif //__SYMBIAN32__

#include <sys/cdefs.h>
#include <sys/_types.h>
#include <machine/_limits.h>

#ifdef  __SYMBIAN32__
#include <libm_aliases.h>
#endif  //__SYMBIAN32__
#ifdef __SYMBIAN32__
#include <signgam.h>

// IEC 559 math functionality
#define __STDC_IEC_559__		1
#endif //__SYMBIAN32__

/*
 * ANSI/POSIX
 */
#ifndef __SYMBIAN32__
extern const union __infinity_un {
	unsigned char	__uc[8];
	double		__ud;
} __infinity;

extern const union __nan_un {
	unsigned char	__uc[sizeof(float)];
	float		__uf;
} __nan;
#endif //__SYMBIAN32__

#if __GNUC_PREREQ__(3, 3) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 800)
#define	__MATH_BUILTIN_CONSTANTS
#endif

#if __GNUC_PREREQ__(3, 0) && !defined(__INTEL_COMPILER)
#define	__MATH_BUILTIN_RELOPS
#endif

#ifdef __SYMBIAN32__
#define	HUGE_VAL	__infinite(1.0)
#else
#ifdef __MATH_BUILTIN_CONSTANTS
#define	HUGE_VAL	__builtin_huge_val()
#else
#define	HUGE_VAL	(__infinity.__ud)
#endif
#endif //__SYMBIAN32__

#if __ISO_C_VISIBLE >= 1999
#define	FP_ILOGB0	(-__INT_MAX)
#define	FP_ILOGBNAN	__INT_MAX

#ifdef __MATH_BUILTIN_CONSTANTS
#define	HUGE_VALF	__builtin_huge_valf()
#define	HUGE_VALL	__builtin_huge_vall()
#define	INFINITY	__builtin_inf()
#define	NAN		__builtin_nan("")
#else
#define	HUGE_VALF	(float)HUGE_VAL
#define	HUGE_VALL	(long double)HUGE_VAL
#define	INFINITY	HUGE_VALF
#ifdef __SYMBIAN32__
#define NAN		nanvalf()
#else
#define	NAN		(__nan.__uf)
#endif //__SYMBIAN32__
#endif /* __MATH_BUILTIN_CONSTANTS */

#define	MATH_ERRNO	1
#define	MATH_ERREXCEPT	2
#define	math_errhandling	MATH_ERREXCEPT

/* XXX We need a <machine/math.h>. */
#if defined(__ia64__) || defined(__sparc64__)
#define	FP_FAST_FMA
#endif
#ifdef __ia64__
#define	FP_FAST_FMAL
#endif
#define	FP_FAST_FMAF

/* Symbolic constants to classify floating point numbers. */
#define	FP_INFINITE	0x01
#define	FP_NAN		0x02
#define	FP_NORMAL	0x04
#define	FP_SUBNORMAL	0x08
#define	FP_ZERO		0x10

#define	fpclassify(x) \
    ((sizeof (x) == sizeof (float)) ? __fpclassifyf(x) \
    : (sizeof (x) == sizeof (double)) ? __fpclassifyd(x) \
    : __fpclassifyl(x))

#define	isfinite(x)					\
    ((sizeof (x) == sizeof (float)) ? __isfinitef(x)	\
    : (sizeof (x) == sizeof (double)) ? __isfinite(x)	\
    : __isfinitel(x))
#define	isinf(x)					\
    ((sizeof (x) == sizeof (float)) ? __isinff(x)	\
    : (sizeof (x) == sizeof (double)) ? __isinf(x)	\
    : __isinfl(x))
#define	isnan(x)					\
    ((sizeof (x) == sizeof (float)) ? __isnanf(x)		\
    : (sizeof (x) == sizeof (double)) ? __isnan(x)	\
    : __isnanl(x))
#define	isnormal(x)					\
    ((sizeof (x) == sizeof (float)) ? __isnormalf(x)	\
    : (sizeof (x) == sizeof (double)) ? __isnormal(x)	\
    : __isnormall(x))


#ifdef __MATH_BUILTIN_RELOPS
#define	isgreater(x, y)		__builtin_isgreater((x), (y))
#define	isgreaterequal(x, y)	__builtin_isgreaterequal((x), (y))
#define	isless(x, y)		__builtin_isless((x), (y))
#define	islessequal(x, y)	__builtin_islessequal((x), (y))
#define	islessgreater(x, y)	__builtin_islessgreater((x), (y))
#define	isunordered(x, y)	__builtin_isunordered((x), (y))
#else
#define	isgreater(x, y)		(!isunordered((x), (y)) && (x) > (y))
#define	isgreaterequal(x, y)	(!isunordered((x), (y)) && (x) >= (y))
#define	isless(x, y)		(!isunordered((x), (y)) && (x) < (y))
#define	islessequal(x, y)	(!isunordered((x), (y)) && (x) <= (y))
#define	islessgreater(x, y)	(!isunordered((x), (y)) && \
					((x) > (y) || (y) > (x)))
#define	isunordered(x, y)	(isnan(x) || isnan(y))
#endif /* __MATH_BUILTIN_RELOPS */

#define	signbit(x)					\
    ((sizeof (x) == sizeof (float)) ? __signbitf(x)	\
    : (sizeof (x) == sizeof (double)) ? __signbit(x)	\
    : __signbitl(x))

typedef	__double_t	double_t;
typedef	__float_t	float_t;
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * XOPEN/SVID
 */
#if __BSD_VISIBLE || __XSI_VISIBLE
#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#define	MAXFLOAT	((float)3.40282346638528860e+38)
#ifdef __SYMBIAN32__
#ifndef __WINSCW__
extern int _signgam;
#endif //__WINSCW__
#else
extern int signgam;
#endif //__SYMBIAN32__
#endif /* __BSD_VISIBLE || __XSI_VISIBLE */

#if __BSD_VISIBLE
#if 0
/* Old value from 4.4BSD-Lite math.h; this is probably better. */
#define	HUGE		HUGE_VAL
#else
#define	HUGE		MAXFLOAT
#endif
#endif /* __BSD_VISIBLE */

/*
 * Most of these functions depend on the rounding mode and have the side
 * effect of raising floating-point exceptions, so they are not declared
 * as __pure2.  In C99, FENV_ACCESS affects the purity of these functions.
 */
__BEGIN_DECLS
/*
 * ANSI/POSIX
 */
#ifdef __SYMBIAN32__
IMPORT_C double __infinite(double x);
IMPORT_C float  __infinitef(float x);

IMPORT_C double nanval(void );
IMPORT_C float  nanvalf(void );
#endif //__SYMBIAN32__


IMPORT_C int	__fpclassifyd(double) __pure2;
IMPORT_C int	__fpclassifyf(float) __pure2;
IMPORT_C int	__fpclassifyl(long double) __pure2;
IMPORT_C int	__isfinitef(float) __pure2;
IMPORT_C int	__isfinite(double) __pure2;
IMPORT_C int	__isfinitel(long double) __pure2;
IMPORT_C int	__isinf(double) __pure2;
IMPORT_C int	__isinff(float) __pure2;
IMPORT_C int	__isinfl(long double) __pure2;
#ifdef __SYMBIAN32__
IMPORT_C int	__isnan(double) __pure2;
IMPORT_C int	__isnanf(float) __pure2;
#endif //__SYMBIAN32__
IMPORT_C int	__isnanl(long double) __pure2;
IMPORT_C int	__isnormalf(float) __pure2;
IMPORT_C int	__isnormal(double) __pure2;
IMPORT_C int	__isnormall(long double) __pure2;
IMPORT_C int	__signbit(double) __pure2;
IMPORT_C int	__signbitf(float) __pure2;
IMPORT_C int	__signbitl(long double) __pure2;

IMPORT_C double	acos(double);
IMPORT_C double	asin(double);
IMPORT_C double	atan(double);
IMPORT_C double	atan2(double, double);
IMPORT_C double	cos(double);
IMPORT_C double	sin(double);
IMPORT_C double	tan(double);

IMPORT_C double	cosh(double);
IMPORT_C double	sinh(double);
IMPORT_C double	tanh(double);

IMPORT_C double	exp(double);
IMPORT_C double	frexp(double, int *);	/* fundamentally !__pure2 */
IMPORT_C double	ldexp(double, int);
IMPORT_C double	log(double);
IMPORT_C double	log10(double);
IMPORT_C double	modf(double, double *);	/* fundamentally !__pure2 */

IMPORT_C double	pow(double, double);
IMPORT_C double	sqrt(double);

IMPORT_C double	ceil(double);
IMPORT_C double	fabs(double) __pure2;
IMPORT_C double	floor(double);
IMPORT_C double	fmod(double, double);

/*
 * These functions are not in C90.
 */
#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE
IMPORT_C double	acosh(double);
IMPORT_C double	asinh(double);
IMPORT_C double	atanh(double);
IMPORT_C double	cbrt(double);
IMPORT_C double	erf(double);
IMPORT_C double	erfc(double);
IMPORT_C double	exp2(double);
IMPORT_C double	expm1(double);
IMPORT_C double	fma(double, double, double);
IMPORT_C double	hypot(double, double);


IMPORT_C int	ilogb(double) __pure2;
/*
int	(isinf)(double) __pure2;
int	(isnan)(double) __pure2;
*/
IMPORT_C double	lgamma(double);
IMPORT_C long long llrint(double);
IMPORT_C long long llround(double);
IMPORT_C double	log1p(double);
IMPORT_C double	logb(double);
IMPORT_C long	lrint(double);
IMPORT_C long	lround(double);
IMPORT_C double	nextafter(double, double);
IMPORT_C double	remainder(double, double);
IMPORT_C double	remquo(double, double, int *);
IMPORT_C double	rint(double);
#endif /* __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE */

#if __BSD_VISIBLE || __XSI_VISIBLE
IMPORT_C double	j0(double);
IMPORT_C double	j1(double);
IMPORT_C double	jn(int, double);
IMPORT_C double	scalb(double, double);
IMPORT_C double	y0(double);
IMPORT_C double	y1(double);
IMPORT_C double	yn(int, double);

#if __XSI_VISIBLE <= 500 || __BSD_VISIBLE
IMPORT_C double	gamma(double);
#endif
#endif /* __BSD_VISIBLE || __XSI_VISIBLE */

#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999
IMPORT_C double	copysign(double, double) __pure2;
IMPORT_C double	fdim(double, double);
IMPORT_C double	fmax(double, double) __pure2;
IMPORT_C double	fmin(double, double) __pure2;
IMPORT_C double	nearbyint(double);
IMPORT_C double	round(double);
IMPORT_C double	scalbln(double, long);
IMPORT_C double	scalbn(double, int);
IMPORT_C double	trunc(double);
#endif

/*
 * BSD math library entry points
 */
#if __BSD_VISIBLE
IMPORT_C double	drem(double, double);
IMPORT_C int	finite(double) __pure2;

/*
 * Reentrant version of gamma & lgamma; passes signgam back by reference
 * as the second argument; user must allocate space for signgam.
 */
IMPORT_C double	lgamma_r(double, int *);

/*
 * IEEE Test Vector
 */
IMPORT_C double	significand(double);
#endif /* __BSD_VISIBLE */

/* float versions of ANSI/POSIX functions */
#if __ISO_C_VISIBLE >= 1999
IMPORT_C float	acosf(float);

IMPORT_C float	acosf(float);
IMPORT_C float	asinf(float);
IMPORT_C float	atanf(float);
IMPORT_C float	atan2f(float, float);
IMPORT_C float	cosf(float);
IMPORT_C float	sinf(float);
IMPORT_C float	tanf(float);

IMPORT_C float	coshf(float);
IMPORT_C float	sinhf(float);
IMPORT_C float	tanhf(float);
IMPORT_C float	exp2f(float);
IMPORT_C float	expf(float);
IMPORT_C float	expm1f(float);
IMPORT_C float	frexpf(float, int *);	/* fundamentally !__pure2 */
IMPORT_C int	ilogbf(float) __pure2;
#ifndef __SYMBIAN32__
float	ldexpf(float, int);
#endif //__SYMBIAN32__
IMPORT_C float	log10f(float);
IMPORT_C float	log1pf(float);
IMPORT_C float	logf(float);
IMPORT_C float	modff(float, float *);	/* fundamentally !__pure2 */

IMPORT_C float	powf(float, float);
IMPORT_C float	sqrtf(float);

IMPORT_C float	ceilf(float);
IMPORT_C float	fabsf(float) __pure2;
IMPORT_C float	floorf(float);
IMPORT_C float	fmodf(float, float);
IMPORT_C float	roundf(float);

IMPORT_C float	erff(float);
IMPORT_C float	erfcf(float);
IMPORT_C float	hypotf(float, float);
IMPORT_C float	lgammaf(float);

IMPORT_C float	acoshf(float);
IMPORT_C float	asinhf(float);
IMPORT_C float	atanhf(float);
IMPORT_C float	cbrtf(float);
IMPORT_C float	logbf(float);
IMPORT_C float	copysignf(float, float) __pure2;
IMPORT_C long long llrintf(float);
IMPORT_C long long llroundf(float);
IMPORT_C long	lrintf(float);
IMPORT_C long	lroundf(float);
IMPORT_C float	nearbyintf(float);
IMPORT_C float	nextafterf(float, float);
IMPORT_C float	remainderf(float, float);
IMPORT_C float	remquof(float, float, int *);
IMPORT_C float	rintf(float);
IMPORT_C float	scalblnf(float, long);
IMPORT_C float	scalbnf(float, int);
IMPORT_C float	truncf(float);

IMPORT_C float	fdimf(float, float);
IMPORT_C float	fmaf(float, float, float);
IMPORT_C float	fmaxf(float, float) __pure2;
IMPORT_C float	fminf(float, float) __pure2;
#endif

/*
 * float versions of BSD math library entry points
 */
#if __BSD_VISIBLE

#ifdef __SYMBIAN_COMPILE_UNUSED__
float	dremf(float, float);
#endif

IMPORT_C int	finitef(float) __pure2;
IMPORT_C float	gammaf(float);
IMPORT_C float	j0f(float);
IMPORT_C float	j1f(float);
IMPORT_C float	jnf(int, float);
IMPORT_C float	scalbf(float, float);
IMPORT_C float	y0f(float);
IMPORT_C float	y1f(float);
IMPORT_C float	ynf(int, float);

/*
 * Float versions of reentrant version of gamma & lgamma; passes
 * signgam back by reference as the second argument; user must
 * allocate space for signgam.
 */
IMPORT_C float	lgammaf_r(float, int *);

/*
 * float version of IEEE Test Vector
 */
IMPORT_C float	significandf(float);
#endif	/* __BSD_VISIBLE */

/*
 * long double versions of ISO/POSIX math functions
 */
#if __ISO_C_VISIBLE >= 1999
#if 0
long double	acoshl(long double);
long double	acosl(long double);
long double	asinhl(long double);
long double	asinl(long double);
long double	atan2l(long double, long double);
long double	atanhl(long double);
long double	atanl(long double);
long double	cbrtl(long double);
#endif
#ifndef __SYMBIAN32__
long double	ceill(long double);
#endif //__SYMBIAN32__
IMPORT_C long double	copysignl(long double, long double) __pure2;
#if 0
long double	coshl(long double);
long double	cosl(long double);
long double	erfcl(long double);
long double	erfl(long double);
long double	exp2l(long double);
long double	expl(long double);
long double	expm1l(long double);
#endif
IMPORT_C long double	fabsl(long double) __pure2;
IMPORT_C long double	fdiml(long double, long double);
#ifndef __SYMBIAN32__
long double	floorl(long double);
long double	fmal(long double, long double, long double);
#endif //__SYMBIAN32__
IMPORT_C long double	fmaxl(long double, long double) __pure2;
#ifndef __SYMBIAN32__
long double	fminl(long double, long double) __pure2;
#endif //__SYMBIAN32__
#if 0
long double	fmodl(long double, long double);
#endif
#ifndef __SYMBIAN32__
long double	frexpl(long double value, int *); /* fundamentally !__pure2 */
#endif //__SYMBIAN32__
#if 0
long double	hypotl(long double, long double);
#endif
IMPORT_C int		ilogbl(long double) __pure2;
#ifndef __SYMBIAN32__
long double	ldexpl(long double, int);
#endif //__SYMBIAN32__
#if 0
long double	lgammal(long double);
long long	llrintl(long double);
#endif
#ifndef __SYMBIAN32__
long long	llroundl(long double);
#endif //__SYMBIAN32__
#if 0
long double	log10l(long double);
long double	log1pl(long double);
long double	log2l(long double);
long double	logbl(long double);
long double	logl(long double);
long		lrintl(long double);
#endif
#ifndef __SYMBIAN32__
long		lroundl(long double);
#endif //__SYMBIAN32__
#if 0
long double	modfl(long double, long double *); /* fundamentally !__pure2 */
long double	nanl(const char *) __pure2;
long double	nearbyintl(long double);
#endif
#ifndef __SYMBIAN32__
long double	nextafterl(long double, long double);
double		nexttoward(double, long double);
#endif //__SYMBIAN32__
IMPORT_C float		nexttowardf(float, long double);
#ifndef __SYMBIAN32__
long double	nexttowardl(long double, long double);
#endif //__SYMBIAN32__
#if 0
long double	powl(long double, long double);
long double	remainderl(long double, long double);
long double	remquol(long double, long double, int *);
long double	rintl(long double);
#endif
#ifndef __SYMBIAN32__
long double	roundl(long double);
#endif //__SYMBIAN32__
IMPORT_C long double	scalblnl(long double, long);
#ifndef __SYMBIAN32__
long double	scalbnl(long double, int);
#endif //__SYMBIAN32__
#if 0
long double	sinhl(long double);
long double	sinl(long double);
long double	sqrtl(long double);
long double	tanhl(long double);
long double	tanl(long double);
#endif
IMPORT_C long double	truncl(long double);

#endif /* __ISO_C_VISIBLE >= 1999 */
__END_DECLS

#endif /* !_MATH_H_ */
