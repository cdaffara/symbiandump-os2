
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *  Portions Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 */

/*
 * from: @(#)fdlibm.h 5.1 93/09/24
 * $FreeBSD: src/lib/msun/src/math.h,v 1.61 2005/04/16 21:12:47 das Exp $
 */

#ifndef _MATH_H_
#define _MATH_H_


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
#ifdef __WINSCW__
#ifndef __SOFTFP
#define __SOFTFP
#endif /* __SOFTFP */
#endif//__WINSCW__
#endif //__SYMBIAN32__

#include <sys/cdefs.h>
#include <sys/_types.h>
#include <stdapis/machine/_limits.h>

#ifdef  __SYMBIAN32__
#include <libm_aliases.h>
#endif  //__SYMBIAN32__
#ifdef __SYMBIAN32__
#include "signgam.h"

// IEC 559 math functionality
#define __STDC_IEC_559__        1
#endif //__SYMBIAN32__

/*
 * ANSI/POSIX
 */
#ifndef __SYMBIAN32__
extern const union __infinity_un {
    unsigned char   __uc[8];
    double      __ud;
} __infinity;

extern const union __nan_un {
    unsigned char   __uc[sizeof(float)];
    float       __uf;
} __nan;
#endif //__SYMBIAN32__

#if __GNUC_PREREQ__(3, 3) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 800)
#define __MATH_BUILTIN_CONSTANTS
#endif

#if __GNUC_PREREQ__(3, 0) && !defined(__INTEL_COMPILER)
#define __MATH_BUILTIN_RELOPS
#endif

#ifdef __SYMBIAN32__
#define HUGE_VAL    __infinite(1.0)
#else
#ifdef __MATH_BUILTIN_CONSTANTS
#define HUGE_VAL    __builtin_huge_val()
#else
#define HUGE_VAL    (__infinity.__ud)
#endif
#endif //__SYMBIAN32__

#if __ISO_C_VISIBLE >= 1999
#define FP_ILOGB0   (-__INT_MAX)
#define FP_ILOGBNAN __INT_MAX

#ifdef __MATH_BUILTIN_CONSTANTS
#define HUGE_VALF   __builtin_huge_valf()
#define HUGE_VALL   __builtin_huge_vall()
#define INFINITY    __builtin_inf()
#define NAN     __builtin_nan("")
#else
#define HUGE_VALF   (float)HUGE_VAL
#define HUGE_VALL   (long double)HUGE_VAL
#define INFINITY    HUGE_VALF
#ifdef __SYMBIAN32__
#define NAN     nanvalf()
#else
#define NAN     (__nan.__uf)
#endif //__SYMBIAN32__
#endif /* __MATH_BUILTIN_CONSTANTS */

#define MATH_ERRNO  1
#define MATH_ERREXCEPT  2
#define math_errhandling    MATH_ERREXCEPT

/* XXX We need a <machine/math.h>. */
#if defined(__ia64__) || defined(__sparc64__)
#define FP_FAST_FMA
#endif
#ifdef __ia64__
#define FP_FAST_FMAL
#endif
#define FP_FAST_FMAF

/* Symbolic constants to classify floating point numbers. */
#define FP_INFINITE 0x01
#define FP_NAN      0x02
#define FP_NORMAL   0x04
#define FP_SUBNORMAL    0x08
#define FP_ZERO     0x10

#define fpclassify(x) \
    ((sizeof (x) == sizeof (float)) ? __fpclassifyf(x) \
    : (sizeof (x) == sizeof (double)) ? __fpclassifyd(x) \
    : __fpclassifyl(x))

#define isfinite(x)                 \
    ((sizeof (x) == sizeof (float)) ? __isfinitef(x)    \
    : (sizeof (x) == sizeof (double)) ? __isfinite(x)   \
    : __isfinitel(x))
#define isinf(x)                    \
    ((sizeof (x) == sizeof (float)) ? __isinff(x)   \
    : (sizeof (x) == sizeof (double)) ? __isinf(x)  \
    : __isinfl(x))
#define isnan(x)                    \
    ((sizeof (x) == sizeof (float)) ? __isnanf(x)       \
    : (sizeof (x) == sizeof (double)) ? __isnan(x)  \
    : __isnanl(x))
#define isnormal(x)                 \
    ((sizeof (x) == sizeof (float)) ? __isnormalf(x)    \
    : (sizeof (x) == sizeof (double)) ? __isnormal(x)   \
    : __isnormall(x))


#ifdef __MATH_BUILTIN_RELOPS
#define isgreater(x, y)     __builtin_isgreater((x), (y))
#define isgreaterequal(x, y)    __builtin_isgreaterequal((x), (y))
#define isless(x, y)        __builtin_isless((x), (y))
#define islessequal(x, y)   __builtin_islessequal((x), (y))
#define islessgreater(x, y) __builtin_islessgreater((x), (y))
#define isunordered(x, y)   __builtin_isunordered((x), (y))
#else
#define isgreater(x, y)     (!isunordered((x), (y)) && (x) > (y))
#define isgreaterequal(x, y)    (!isunordered((x), (y)) && (x) >= (y))
#define isless(x, y)        (!isunordered((x), (y)) && (x) < (y))
#define islessequal(x, y)   (!isunordered((x), (y)) && (x) <= (y))
#define islessgreater(x, y) (!isunordered((x), (y)) && \
                    ((x) > (y) || (y) > (x)))
#define isunordered(x, y)   (isnan(x) || isnan(y))
#endif /* __MATH_BUILTIN_RELOPS */

#define signbit(x)                  \
    ((sizeof (x) == sizeof (float)) ? __signbitf(x) \
    : (sizeof (x) == sizeof (double)) ? __signbit(x)    \
    : __signbitl(x))

typedef __double_t  double_t;
typedef __float_t   float_t;
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * XOPEN/SVID
 */
#if __BSD_VISIBLE || __XSI_VISIBLE
#define M_E     2.7182818284590452354   /* e */
#define M_LOG2E     1.4426950408889634074   /* log 2e */
#define M_LOG10E    0.43429448190325182765  /* log 10e */
#define M_LN2       0.69314718055994530942  /* log e2 */
#define M_LN10      2.30258509299404568402  /* log e10 */
#define M_PI        3.14159265358979323846  /* pi */
#define M_PI_2      1.57079632679489661923  /* pi/2 */
#define M_PI_4      0.78539816339744830962  /* pi/4 */
#define M_1_PI      0.31830988618379067154  /* 1/pi */
#define M_2_PI      0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI  1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2     1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2   0.70710678118654752440  /* 1/sqrt(2) */

#define MAXFLOAT    ((float)3.40282346638528860e+38)
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
#define HUGE        HUGE_VAL
#else
#define HUGE        MAXFLOAT
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
IMPORT_C double __infinite(double x) __SOFTFP;
IMPORT_C float  __infinitef(float x) __SOFTFP;

IMPORT_C double nanval(void )__SOFTFP;
IMPORT_C float  nanvalf(void ) __SOFTFP;
#endif //__SYMBIAN32__


IMPORT_C int    __fpclassifyd(double) __SOFTFP __pure2;
IMPORT_C int    __fpclassifyf(float) __SOFTFP __pure2;
IMPORT_C int    __fpclassifyl(long double) __SOFTFP __pure2;
IMPORT_C int    __isfinitef(float) __SOFTFP __pure2;
IMPORT_C int    __isfinite(double) __SOFTFP __pure2;
IMPORT_C int    __isfinitel(long double) __SOFTFP __pure2;
IMPORT_C int    __isinf(double) __SOFTFP __pure2;
IMPORT_C int    __isinff(float) __SOFTFP __pure2;
IMPORT_C int    __isinfl(long double) __SOFTFP __pure2;
#ifdef __SYMBIAN32__
IMPORT_C int    __isnan(double) __SOFTFP __pure2;
IMPORT_C int    __isnanf(float) __SOFTFP __pure2;
#endif //__SYMBIAN32__
IMPORT_C int    __isnanl(long double) __SOFTFP __pure2;
IMPORT_C int    __isnormalf(float) __SOFTFP __pure2;
IMPORT_C int    __isnormal(double) __SOFTFP __pure2;
IMPORT_C int    __isnormall(long double) __SOFTFP __pure2;
IMPORT_C int    __signbit(double) __SOFTFP __pure2;
IMPORT_C int    __signbitf(float) __SOFTFP __pure2;
IMPORT_C int    __signbitl(long double) __SOFTFP __pure2;

IMPORT_C double acos(double) __SOFTFP;
IMPORT_C double asin(double) __SOFTFP;
IMPORT_C double atan(double) __SOFTFP;
IMPORT_C double atan2(double, double) __SOFTFP;
IMPORT_C double cos(double) __SOFTFP;
IMPORT_C double sin(double) __SOFTFP;
IMPORT_C double tan(double) __SOFTFP;

IMPORT_C double cosh(double) __SOFTFP;
IMPORT_C double sinh(double) __SOFTFP;
IMPORT_C double tanh(double) __SOFTFP;

IMPORT_C double exp(double) __SOFTFP;
IMPORT_C double frexp(double, int *) __SOFTFP;  /* fundamentally !__pure2 */
IMPORT_C double ldexp(double, int) __SOFTFP;
IMPORT_C double log(double) __SOFTFP;
IMPORT_C double log10(double) __SOFTFP;
IMPORT_C double modf(double, double *) __SOFTFP;    /* fundamentally !__pure2 */

IMPORT_C double pow(double, double) __SOFTFP;
IMPORT_C double sqrt(double) __SOFTFP;

IMPORT_C double ceil(double) __SOFTFP;
IMPORT_C double fabs(double) __SOFTFP __pure2;
IMPORT_C double floor(double) __SOFTFP;
IMPORT_C double fmod(double, double) __SOFTFP;

/*
 * These functions are not in C90.
 */
#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE
IMPORT_C double acosh(double) __SOFTFP;
IMPORT_C double asinh(double) __SOFTFP;
IMPORT_C double atanh(double) __SOFTFP;
IMPORT_C double cbrt(double) __SOFTFP;
IMPORT_C double erf(double) __SOFTFP;
IMPORT_C double erfc(double) __SOFTFP;
IMPORT_C double exp2(double) __SOFTFP;
IMPORT_C double expm1(double) __SOFTFP;
IMPORT_C double fma(double, double, double) __SOFTFP;
IMPORT_C double hypot(double, double) __SOFTFP;


IMPORT_C int    ilogb(double) __SOFTFP __pure2;
/*
int (isinf)(double) __pure2;
int (isnan)(double) __pure2;
*/
IMPORT_C double lgamma(double) __SOFTFP;
IMPORT_C long long llrint(double) __SOFTFP;
IMPORT_C long long llround(double) __SOFTFP;
IMPORT_C double log1p(double) __SOFTFP;
IMPORT_C double logb(double) __SOFTFP;
IMPORT_C long   lrint(double) __SOFTFP;
IMPORT_C long   lround(double) __SOFTFP;
IMPORT_C double nextafter(double, double) __SOFTFP;
IMPORT_C double remainder(double, double) __SOFTFP;
IMPORT_C double remquo(double, double, int *) __SOFTFP;
IMPORT_C double rint(double) __SOFTFP;
#endif /* __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999 || __XSI_VISIBLE */

#if __BSD_VISIBLE || __XSI_VISIBLE
IMPORT_C double j0(double) __SOFTFP;
IMPORT_C double j1(double) __SOFTFP;
IMPORT_C double jn(int, double) __SOFTFP;
IMPORT_C double scalb(double, double) __SOFTFP;
IMPORT_C double y0(double) __SOFTFP;
IMPORT_C double y1(double) __SOFTFP;
IMPORT_C double yn(int, double) __SOFTFP;

#if __XSI_VISIBLE <= 500 || __BSD_VISIBLE
IMPORT_C double gamma(double) __SOFTFP;
#endif
#endif /* __BSD_VISIBLE || __XSI_VISIBLE */

#if __BSD_VISIBLE || __ISO_C_VISIBLE >= 1999
IMPORT_C double copysign(double, double) __SOFTFP __pure2;
IMPORT_C double fdim(double, double) __SOFTFP;
IMPORT_C double fmax(double, double) __SOFTFP __pure2;
IMPORT_C double fmin(double, double) __SOFTFP __pure2;
IMPORT_C double nearbyint(double) __SOFTFP;
IMPORT_C double round(double) __SOFTFP;
IMPORT_C double scalbln(double, long) __SOFTFP;
IMPORT_C double scalbn(double, int) __SOFTFP;
IMPORT_C double trunc(double) __SOFTFP;
#endif

/*
 * BSD math library entry points
 */
#if __BSD_VISIBLE
IMPORT_C double drem(double, double) __SOFTFP;
IMPORT_C int    finite(double) __SOFTFP __pure2;
IMPORT_C int    isnanf(float) __SOFTFP __pure2;

/*
 * Reentrant version of gamma & lgamma; passes signgam back by reference
 * as the second argument; user must allocate space for signgam.
 */
IMPORT_C double lgamma_r(double, int *) __SOFTFP;

/*
 * IEEE Test Vector
 */
IMPORT_C double significand(double) __SOFTFP;
#endif /* __BSD_VISIBLE */

/* float versions of ANSI/POSIX functions */
#if __ISO_C_VISIBLE >= 1999
IMPORT_C float  acosf(float) __SOFTFP;

IMPORT_C float  acosf(float) __SOFTFP;
IMPORT_C float  asinf(float) __SOFTFP;
IMPORT_C float  atanf(float) __SOFTFP;
IMPORT_C float  atan2f(float, float) __SOFTFP;
IMPORT_C float  cosf(float) __SOFTFP;
IMPORT_C float  sinf(float) __SOFTFP;
IMPORT_C float  tanf(float) __SOFTFP;

IMPORT_C float  coshf(float) __SOFTFP;
IMPORT_C float  sinhf(float) __SOFTFP;
IMPORT_C float  tanhf(float) __SOFTFP;
IMPORT_C float  exp2f(float) __SOFTFP;
IMPORT_C float  expf(float) __SOFTFP;
IMPORT_C float  expm1f(float) __SOFTFP;
IMPORT_C float  frexpf(float, int *) __SOFTFP;  /* fundamentally !__pure2 */
IMPORT_C int    ilogbf(float) __SOFTFP __pure2;
#ifndef __SYMBIAN32__
float   ldexpf(float, int);
#endif //__SYMBIAN32__
IMPORT_C float  log10f(float) __SOFTFP;
IMPORT_C float  log1pf(float) __SOFTFP;
IMPORT_C float  logf(float) __SOFTFP;
IMPORT_C float  modff(float, float *) __SOFTFP; /* fundamentally !__pure2 */

IMPORT_C float  powf(float, float) __SOFTFP;
IMPORT_C float  sqrtf(float) __SOFTFP;

IMPORT_C float  ceilf(float) __SOFTFP;
IMPORT_C float  fabsf(float) __SOFTFP __pure2;
IMPORT_C float  floorf(float) __SOFTFP;
IMPORT_C float  fmodf(float, float) __SOFTFP;
IMPORT_C float  roundf(float) __SOFTFP;

IMPORT_C float  erff(float) __SOFTFP;
IMPORT_C float  erfcf(float) __SOFTFP;
IMPORT_C float  hypotf(float, float) __SOFTFP;
IMPORT_C float  lgammaf(float) __SOFTFP;

IMPORT_C float  acoshf(float) __SOFTFP;
IMPORT_C float  asinhf(float) __SOFTFP;
IMPORT_C float  atanhf(float) __SOFTFP;
IMPORT_C float  cbrtf(float) __SOFTFP;
IMPORT_C float  logbf(float) __SOFTFP;
IMPORT_C float  copysignf(float, float) __SOFTFP __pure2;
IMPORT_C long long llrintf(float) __SOFTFP;
IMPORT_C long long llroundf(float) __SOFTFP;
IMPORT_C long   lrintf(float) __SOFTFP;
IMPORT_C long   lroundf(float) __SOFTFP;
IMPORT_C float  nearbyintf(float) __SOFTFP;
IMPORT_C float  nextafterf(float, float) __SOFTFP;
IMPORT_C float  remainderf(float, float) __SOFTFP;
IMPORT_C float  remquof(float, float, int *) __SOFTFP;
IMPORT_C float  rintf(float) __SOFTFP;
IMPORT_C float  scalblnf(float, long) __SOFTFP;
IMPORT_C float  scalbnf(float, int) __SOFTFP;
IMPORT_C float  truncf(float) __SOFTFP;

IMPORT_C float  fdimf(float, float) __SOFTFP;
IMPORT_C float  fmaf(float, float, float) __SOFTFP;
IMPORT_C float  fmaxf(float, float) __SOFTFP __pure2;
IMPORT_C float  fminf(float, float) __SOFTFP __pure2;
#endif

/*
 * float versions of BSD math library entry points
 */
#if __BSD_VISIBLE

#ifdef __SYMBIAN_COMPILE_UNUSED__
float   dremf(float, float);
#endif

IMPORT_C int    finitef(float) __SOFTFP __pure2;
IMPORT_C float  gammaf(float) __SOFTFP;
IMPORT_C float  j0f(float) __SOFTFP;
IMPORT_C float  j1f(float) __SOFTFP;
IMPORT_C float  jnf(int, float) __SOFTFP;
IMPORT_C float  scalbf(float, float) __SOFTFP;
IMPORT_C float  y0f(float) __SOFTFP;
IMPORT_C float  y1f(float) __SOFTFP;
IMPORT_C float  ynf(int, float) __SOFTFP;

/*
 * Float versions of reentrant version of gamma & lgamma; passes
 * signgam back by reference as the second argument; user must
 * allocate space for signgam.
 */
IMPORT_C float  lgammaf_r(float, int *) __SOFTFP;

/*
 * float version of IEEE Test Vector
 */
IMPORT_C float  significandf(float) __SOFTFP;
#endif  /* __BSD_VISIBLE */

/*
 * long double versions of ISO/POSIX math functions
 */
#if __ISO_C_VISIBLE >= 1999
#if 0
long double acoshl(long double);
long double acosl(long double);
long double asinhl(long double);
long double asinl(long double);
long double atan2l(long double, long double);
long double atanhl(long double);
long double atanl(long double);
long double cbrtl(long double);
#endif
#ifndef __SYMBIAN32__
long double ceill(long double);
#endif //__SYMBIAN32__
IMPORT_C long double    copysignl(long double, long double) __SOFTFP __pure2;
#if 0
long double coshl(long double);
long double cosl(long double);
long double erfcl(long double);
long double erfl(long double);
long double exp2l(long double);
long double expl(long double);
long double expm1l(long double);
#endif
IMPORT_C long double    fabsl(long double) __SOFTFP __pure2;
IMPORT_C long double    fdiml(long double, long double) __SOFTFP;
#ifndef __SYMBIAN32__
long double floorl(long double);
long double fmal(long double, long double, long double);
#endif //__SYMBIAN32__
IMPORT_C long double    fmaxl(long double, long double) __SOFTFP __pure2;
#ifndef __SYMBIAN32__
long double fminl(long double, long double) __pure2;
#endif //__SYMBIAN32__
#if 0
long double fmodl(long double, long double);
#endif
#ifndef __SYMBIAN32__
long double frexpl(long double value, int *); /* fundamentally !__pure2 */
#endif //__SYMBIAN32__
#if 0
long double hypotl(long double, long double);
#endif
IMPORT_C int        ilogbl(long double) __SOFTFP __pure2;
#ifndef __SYMBIAN32__
long double ldexpl(long double, int);
#endif //__SYMBIAN32__
#if 0
long double lgammal(long double);
long long   llrintl(long double);
#endif
#ifndef __SYMBIAN32__
long long   llroundl(long double);
#endif //__SYMBIAN32__
#if 0
long double log10l(long double);
long double log1pl(long double);
long double log2l(long double);
long double logbl(long double);
long double logl(long double);
long        lrintl(long double);
#endif
#ifndef __SYMBIAN32__
long        lroundl(long double);
#endif //__SYMBIAN32__
#if 0
long double modfl(long double, long double *); /* fundamentally !__pure2 */
long double nanl(const char *) __pure2;
long double nearbyintl(long double);
#endif
#ifndef __SYMBIAN32__
long double nextafterl(long double, long double);
double      nexttoward(double, long double);
#endif //__SYMBIAN32__
IMPORT_C float      nexttowardf(float, long double) __SOFTFP;
#ifndef __SYMBIAN32__
long double nexttowardl(long double, long double);
#endif //__SYMBIAN32__
#if 0
long double powl(long double, long double);
long double remainderl(long double, long double);
long double remquol(long double, long double, int *);
long double rintl(long double);
#endif
#ifndef __SYMBIAN32__
long double roundl(long double);
#endif //__SYMBIAN32__
IMPORT_C long double    scalblnl(long double, long) __SOFTFP;
#ifndef __SYMBIAN32__
long double scalbnl(long double, int);
#endif //__SYMBIAN32__
#if 0
long double sinhl(long double);
long double sinl(long double);
long double sqrtl(long double);
long double tanhl(long double);
long double tanl(long double);
#endif
IMPORT_C long double    truncl(long double) __SOFTFP;

#endif /* __ISO_C_VISIBLE >= 1999 */
__END_DECLS

#endif /* !_MATH_H_ */
