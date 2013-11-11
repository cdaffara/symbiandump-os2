/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef _LIBC_WSD_DEFS_H_
#define _LIBC_WSD_DEFS_H_

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <sys/syslimits.h>
#include "gdtoatypes.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef MY_TZNAME_MAX
#define MY_TZNAME_MAX	255
#endif //MY_TZNAME_MAX

#ifndef MAX_REG_ERRORS
#define MAX_REG_ERRORS 18
#endif //MAX_REG_ERRORS

#ifndef TZ_STRLEN_MAX
#define TZ_STRLEN_MAX 255
#endif //TZ_STRLEN_MAX

#ifndef ATEXIT_SIZE
#define	ATEXIT_SIZE	32
#endif //ATEXIT_SIZE

#ifndef TZ_MAX_TIMES
#define TZ_MAX_TIMES	370
#endif //TZ_MAX_TIMES

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS	50	/* Maximum number of leap second corrections */
#endif //TZ_MAX_LEAPS

#ifndef TZ_MAX_TYPES
#ifndef NOSOLAR
#define TZ_MAX_TYPES	256 /* Limited by what (unsigned char)'s can hold */
#endif /* !defined NOSOLAR */
#ifdef NOSOLAR
/*
** Must be at least 14 for Europe/Riga as of Jan 12 1995,
** as noted by Earl Chew <earl@hpato.aus.hp.com>.
*/
#define TZ_MAX_TYPES	20	/* Maximum number of local time types */
#endif /* !defined NOSOLAR */
#endif /* !defined TZ_MAX_TYPES */

#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS	50	/* Maximum number of abbreviation characters */
				/* (limited by what unsigned chars can hold) */
#endif /* !defined TZ_MAX_CHARS */


#ifndef BIGGEST
#define BIGGEST(a, b)	(((a) > (b)) ? (a) : (b))
#endif //BIGGEST

/*#define	std(flags, file) \
  	{0,0,0,flags,file,{0},0,__sF+file,__sclose,__sread,__sseek,__swrite, \
	 {0}, __sFX + file}
*/
#ifndef CNAMES_MAX
#define CNAMES_MAX 98
#endif //CNAME_MAX

#ifndef ASCTIME_RESULT_MAX
#define ASCTIME_RESULT_MAX 68
#endif //ASCTIME_RESULT_MAX


#ifndef MRT_MAX_SYSCALLS
#ifdef GCC_DEF
#define MRT_MAX_SYSCALLS      6
#else 
#define MRT_MAX_SYSCALLS      455   //Max syscalls used in FBSD
#endif 
#endif //MRT_MAX_SYSCALLS

#ifndef thread_key_t
#define thread_key_t		pthread_key_t
#endif //thread_key_t

#ifndef once_t
#define once_t			pthread_once_t
#endif //once_t

#ifndef ONCE_INITIALIZER
#define ONCE_INITIALIZER	PTHREAD_ONCE_INIT
#endif //ONCE_INITIALIZER

#ifndef EMSG
#define EMSG ""
#endif //EMSG

/*struct rerr {
	int code;
	char *name;
	char *explain;
};
*/
struct ttinfo {				/* time type information */
	long		tt_gmtoff;	/* UTC offset in seconds */
	int		tt_isdst;	/* used to set tm_isdst */
	int		tt_abbrind;	/* abbreviation list index */
	int		tt_ttisstd;	/* TRUE if transition is std time */
	int		tt_ttisgmt;	/* TRUE if transition is UTC */
};

struct lsinfo {				/* leap second information */
	time_t		ls_trans;	/* transition time */
	long		ls_corr;	/* correction to apply */
};

static const char	gmt[] = "UTC";

struct state {
	int		leapcnt;
	int		timecnt;
	int		typecnt;
	int		charcnt;
	time_t		ats[TZ_MAX_TIMES];
	unsigned char	types[TZ_MAX_TIMES];
	struct ttinfo	ttis[TZ_MAX_TYPES];
	char		chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + 1, sizeof gmt),
				(2 * (MY_TZNAME_MAX + 1)))];
	struct lsinfo	lsis[TZ_MAX_LEAPS];
};
struct arc4_stream {
	u_int8_t i;
	u_int8_t j;
	u_int8_t s[256];
};
struct atexit {
	struct atexit *next;			/* next in list */
	int ind;				/* next index in this table */
	struct atexit_fn {
		int fn_type;			/* ATEXIT_? from above */
		union {
			void (*std_func)(void);
			void (*cxa_func)(void *);
		} fn_ptr;			/* function pointer */
		void *fn_arg;			/* argument for CXA callback */
		void *fn_dso;			/* shared module handle */
	} fns[ATEXIT_SIZE];			/* the table itself */
};

struct cname {
	char *name;
	char code;
};
 
/* hold a buncha junk that would grow the ABI */
struct __sFILEX {
	unsigned char	*_up;	/* saved _p when _p is doing ungetc data */
	pthread_mutex_t	fl_mutex;	/* used for MT-safety */
	pthread_t	fl_owner;	/* current owner */
	int		fl_count;	/* recursive lock count */
	int		orientation;	/* orientation for fwide() */
	mbstate_t	mbstate;	/* multibyte conversion state */
	char fmode; /* mode of file, whether text or binary */
	void *nl_tbuf; /* buffer used for newline translation, only if fmode == 't' text-mode */
};

#ifndef _GLUE_H_
#define _GLUE_H_
struct glue {
	struct	glue *next;
	int	niobs;
	FILE	*iobs;
};
#endif //_GLUE_H_

struct LocaleInfo;

typedef struct
{
	/* regcomp.c */
	char _s_nuls[10]; //static

	#ifndef NDEBUG
	int _g_never; //global
	#endif //NDEBUG

	/* regerror.c */
	//struct rerr _s_rerrs[MAX_REG_ERRORS]; //to be initialized, static

	/* regexec.c */
	int _s_nope; //static

	/* localtime.c */
	char _s_wildabbr[9]; //static
#ifdef ALL_STATE	
	struct state *	_s_lclptr; //static
	struct state *	_s_gmtptr; //static
#endif //ALL_STATE	
	struct state	_s_lclmem; //static
	struct state	_s_gmtmem; //static
 	char		_s_lcl_TZname[TZ_STRLEN_MAX + 1]; //static
 	int		_s_lcl_is_set; //static
 	int		_s_gmt_is_set; //static
 	pthread_mutex_t	_s_lcl_mutex; //= PTHREAD_MUTEX_INITIALIZER, static
 	pthread_mutex_t	_s_gmt_mutex; //= PTHREAD_MUTEX_INITIALIZER, static
 //	char *	_g_tzname[2]; //to be initialized, global
	char **	_g__tzname; //to be initialized, global
	struct tm	_g_stm; //global

#ifdef USG_COMPAT	
	time_t			_g_timezone; //global
	int			_g_daylight; //global
#endif /* defined USG_COMPAT */
#ifdef ALTZONE
	time_t			_g_altzone; //global
#endif /* defined ALTZONE */

	/* pwd_funcs.cpp */
       pthread_mutex_t _s_getpwent_mutex;// PTHREAD_MUTEX_INITIALIZER,static
	int _g_isfirstpwent;// to be initialized to 1, global

	/* grp_funcs.cpp */
       pthread_mutex_t _s_getgrent_mutex;//PTHREAD_MUTEX_INITIALIZER, static
	int _g_isfirstgrent; //to be initialized to 1, global

	/* arc4random.c */
	pthread_mutex_t	_s_arc4random_mtx;//PTHREAD_MUTEX_INITIALIZER, static
	struct arc4_stream _s_rs; //static
	int _s_rs_initialized;//static
	int _s_rs_stired;//static

	/* atexit.c */
	pthread_mutex_t _s_atexit_mutex;//PTHREAD_MUTEX_INITIALIZER, static
	struct atexit *_s___atexit;//static

	/* exit.c */
	int _g___isthreaded; //global

	/* getopt.c */
	int	_g_opterrp;// to be initiaized to 1, global
	int _g_optindp;// to be initiaized to 1,global
	int _g_optoptp;//global
	int _g_optresetp;//global
	char	*_g_optargp;	 //global

	/* getopt_long.c */
	char _s_arr_place[2]; //static to be initialized
	char * _s_place;
	int _s_nonopt_start;//to be initialized to -1, static
	int _s_nonopt_end;//to be initialized to -1, static
	int _s_dash_prefix;//to be initialized to NO_PREFIX, static

	/* estlib.cpp */
	char** _g_environp;
	const char* _g___progname;  

	/* stdio.h */
	char* _g_tmpdirptr;  //global
	/* findfp.c */
	int _g___sdidinit; //global
	FILE _s_usual[OPEN_MAX - 3]; //static
	struct __sFILEX _s_usual_extra[OPEN_MAX - 3]; //static
	struct glue _s_uglue; //= { NULL, FOPEN_MAX - 3, usual }, static
	struct __sFILEX _s___sFX[3]; //static
	FILE _g___sF[3]; //global, to be intialized
	FILE *_g___stdinp;// &__sF[0], global
	FILE *_g___stdoutp; // &__sF[1], global
	FILE *_g___stderrp; //&__sF[2], global
	struct glue _g___sglue; //{ &uglue, 3, __sF }, global
	struct glue *_s_lastglue; // &uglue, static
	
	struct LocaleInfo *localeptr;
	
	//localeInfo.cpp
	char _g_LC_CTYPE_LocaleName[30];// = {'\0'};
	char _g_LC_COLLATE_LocaleName[30] ;//= {'\0'};
	char _g_LC_NUMERIC_LocaleName[30];// = {'\0'};
	char _g_LC_MONETARY_LocaleName[30];// = {'\0'};
	char _g_LC_TIME_LocaleName[30];// ={'\0'};
	char _g_LC_ALL_LocaleName[30];// = {'\0'};
	wchar_t _g_LC_ALL_WlocaleName[30];// = {'\0'};
	/* Mrtsyscalltable.cpp */
	int _g_Syscall[MRT_MAX_SYSCALLS]; //global , to be initialized

	//cname.h
	struct cname _s_cnames[CNAMES_MAX];

	//strerror.c
	char _s_ebuf[NL_TEXTMAX];

	//tmpnam.c
	u_long _s_tmpcount;
	char _s_buf[L_tmpnam];

	//wtmpnam.c
	u_long _s_wtmpcount;
	wchar_t _s_wbuf[L_tmpnam];
	
	//atexit.c
	struct atexit _s___atexit0;	/* one guaranteed table */

	//localeconv.c
 	struct lconv _s_lret;

	//asctime.c
	char		_s_result[ASCTIME_RESULT_MAX];

	//inet_ntoa.c
	char _s_ret[18];

	//findfp.c
	FILE _s_empty;
	struct __sFILEX _s_emptyx;

	//fdopen
	int _s_nofile;

	//gets.c
	int _s_warned;
	char _s_w[53];

	//collcmp.c
	char _s_s1[2];
	char _s_s2[2];

	//strtok.c
	char *_s_last;

	//err.c
	FILE *_s_err_file; /* file to use for error output */
	void (*_s_err_exit)(int);

	//setenv.c
	char **_s_alloced;

	//getopt.c
	char _s_arr_getopt_place[2];
	char *_s_getopt_place;

	//rand.c
	u_long _s_next;

	//longtime.c
	pthread_mutex_t _s_localtime_mutex;//PTHREAD_MUTEX_INITIALIZER;
	pthread_key_t _s_localtime_key;//-1;
	pthread_mutex_t _s_gmtime_mutex;// = PTHREAD_MUTEX_INITIALIZER;
	pthread_key_t _s_gmtime_key;// = -1;
	
	//getservent.c
	thread_key_t _s_servdata_key;
	once_t _s_servdata_init_once;// = ONCE_INITIALIZER;
	int _s_servdata_thr_keycreated;// = 0;
	long int _g_num_line;

	//mbtowc.c
	mbstate_t _s_mbtowc_mbs;
	
	//mbsrtowcs.c
	mbstate_t _s_mbsrtowcs_mbs;
	
	//mbsnrtowcs.c
	mbstate_t _s_mbsnrtowcs_mbs;
	
	//wctomb.c
	mbstate_t _s_wctomb_mbs;
	
	//wcsrtombs.c
	mbstate_t _s_wcsrtombs_mbs;
	
	//wcsnrtombs.c
	mbstate_t _s_wcsnrtombs_mbs;

	//mbrlen.c
	mbstate_t _s_mbrlen_mbs;

	//mblen.c
	mbstate_t _s_mblen_mbs;
	
	//mbrtowc.c
	mbstate_t _s_mbrtowc_mbs;

	//wcrtomb.c
	mbstate_t _s_wcrtomb_mbs;
	
	//vfscanf.c
	int _g___scanfdebug;// = 0;
	
	//localeconv.c
	int _g___mlocale_changed;// = 1;
	int _g___nlocale_changed;// = 1;
	#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	unsigned char _g___clocale_set;// = 0;
    unsigned char _g___applocale_changed;// = 0;
    #endif
	//lmonetary.cpp
	int	_g__monetary_using_locale;
	
	//lnumeric.cpp
	int	_g__numeric_using_locale;
	
	//timelocal.cpp
	int _g__time_using_locale;

	//exit.c
	void (*_s___cleanup)();
	
	//getpriority.cpp
	int _g_ProcPriority;
	
	//used in timefuncs.cpp
	time_t _g_prevchange;
	time_t _g_nextchange;
	struct tm _g_cachetm;

	// set_fmode.c, flags.c
	char _s_stdio_fmode; 

	//misc.c
	double _s_private_mem[PRIVATE_mem];
	double *_s_pmem_next;

	 Bigint* _s_p5s;
	
	 Bigint* _g_freelist[Kmax+1];
	 
	 pthread_mutex_t _g___gdtoa_locks[2];
	
}_libc_wsd;

_libc_wsd* GetGlobals();

#ifdef __cplusplus
}
#endif

int InitWSDVar(_libc_wsd* aPtr);

#ifndef EMULATOR
#define EMULATOR (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#endif //EMULATOR

#define GET_WSD_VAR_NAME(var,varprefix) 	_##varprefix##_##var

#define RETURN_WSD_VAR(var,varprefix) (GetGlobals()->GET_WSD_VAR_NAME(var,varprefix))

#define GET_WSD_VAR_FROM_TLS(var,type,varprefix)\
type *_##varprefix##_##var()\
{\
	return (&RETURN_WSD_VAR(var,varprefix));\
}

#define GET_WSD_ARRAY_FROM_TLS(var,type,varprefix)\
type *_##varprefix##_##var()\
{\
	return (RETURN_WSD_VAR(var,varprefix));\
}

#define GET_STATIC_VAR_FROM_TLS(var,type) 		GET_WSD_VAR_FROM_TLS(var,type,s)

#define GET_GLOBAL_VAR_FROM_TLS(var, type) 	GET_WSD_VAR_FROM_TLS(var,type,g)

#define GET_STATIC_ARRAY_FROM_TLS(var,type) 	GET_WSD_ARRAY_FROM_TLS(var,type,s)

#define GET_GLOBAL_ARRAY_FROM_TLS(var, type) 	GET_WSD_ARRAY_FROM_TLS(var,type,g)

#endif //__WINSCW__

#endif //_LIBC_WSD_DEFS_H_
