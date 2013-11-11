/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))

#include <regex.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "sysif.h"
#include "libc_wsd_defs.h"
#include "localeinfo.h"
#include "localetlsinfo.h"
#include "local.h"
#include <fcntl.h>


extern "C" {
extern int __sclose(void *cookie);
extern int __sread (void* cookie,char *buf,int n);
extern fpos_t __sseek (void* cookie,fpos_t offset,int whence);
extern int __swrite (void* cookie,char const *buf,int n);
} //extern "C"

void InitStdIO(FILE *tfile, FILE *stfile, int flags, int fileno, struct __sFILEX *tsfx)
{
	tfile->_p = 0;
	tfile->_r = 0;
	tfile->_w = 0;
	tfile->_flags = flags;
	tfile->_file = fileno;
	tfile->_bf._base = 0;
	tfile->_bf._size = 0;
	tfile->_lbfsize = 0;
	tfile->_cookie = stfile + fileno;
	tfile->_close = __sclose;
	tfile->_read = __sread;
	tfile->_seek = __sseek;
	tfile->_write = __swrite;
	tfile->_ub._base = 0;
	tfile->_ub._size = 0;
	tfile->_extra = tsfx + fileno;
	INITEXTRA(tfile);
	__smakebuf(tfile);
}


TInt InitWSDVar(_libc_wsd *ptr)
{
	pthread_mutex_t tmutex = PTHREAD_MUTEX_INITIALIZER;
	//Init_rerrs(ptr);
	strcpy(ptr->GET_WSD_VAR_NAME(wildabbr, s),"WILDABBR");
	ptr->GET_WSD_VAR_NAME(lcl_mutex, s) = tmutex;
	ptr->GET_WSD_VAR_NAME(gmt_mutex, s) = tmutex;
	ptr->GET_WSD_VAR_NAME(_tzname, g) = NULL;
	ptr->GET_WSD_VAR_NAME(getpwent_mutex, s) = tmutex;
	ptr->GET_WSD_VAR_NAME(isfirstpwent, g) = 1;

	ptr->GET_WSD_VAR_NAME(getgrent_mutex, s) = tmutex;
	ptr->GET_WSD_VAR_NAME(isfirstgrent, g) = 1;

	ptr->GET_WSD_VAR_NAME(arc4random_mtx, s) = tmutex;

	ptr->GET_WSD_VAR_NAME(atexit_mutex, s) = tmutex;
	
	ptr->GET_WSD_VAR_NAME(opterrp, g) = 1;
	ptr->GET_WSD_VAR_NAME(optindp, g) = 1;
	
	strcpy(ptr->GET_WSD_VAR_NAME(arr_place, s), EMSG);
	ptr->GET_WSD_VAR_NAME(place, s) = ptr->GET_WSD_VAR_NAME(arr_place, s);
	
	ptr->GET_WSD_VAR_NAME(nonopt_start, s) = -1;
	ptr->GET_WSD_VAR_NAME(nonopt_end,  s) = -1;
#ifdef GNU_COMPATIBLE
	ptr->GET_WSD_VAR_NAME(dash_prefix, s) = NO_PREFIX;
#endif //GNU_COMPATIBLE

	ptr->GET_WSD_VAR_NAME(tmpdirptr, g) = NULL;
	ptr->GET_WSD_VAR_NAME(environp, g) = NULL;
	ptr->GET_WSD_VAR_NAME(__progname, g) = NULL;  
	ptr->GET_WSD_VAR_NAME(uglue, s).next = NULL;
	ptr->GET_WSD_VAR_NAME(uglue, s).niobs = OPEN_MAX;
	ptr->GET_WSD_VAR_NAME(uglue, s).iobs = ptr->GET_WSD_VAR_NAME(usual, s);
	InitStdIO(&ptr->GET_WSD_VAR_NAME(__sF, g)[0], &ptr->GET_WSD_VAR_NAME(__sF, g)[0], __SRD, STDIN_FILENO, &ptr->GET_WSD_VAR_NAME(__sFX, s)[0]);
	InitStdIO(&ptr->GET_WSD_VAR_NAME(__sF, g)[1], &ptr->GET_WSD_VAR_NAME(__sF, g)[0], __SWR, STDOUT_FILENO, &ptr->GET_WSD_VAR_NAME(__sFX, s)[0]);
	InitStdIO(&ptr->GET_WSD_VAR_NAME(__sF, g)[2], &ptr->GET_WSD_VAR_NAME(__sF, g)[0], __SWR|__SNBF, STDERR_FILENO, &ptr->GET_WSD_VAR_NAME(__sFX, s)[0]);
	ptr->_s___cleanup = _cleanup;
	ptr->GET_WSD_VAR_NAME(__stdinp, g) = &ptr->GET_WSD_VAR_NAME(__sF, g)[0];
	ptr->GET_WSD_VAR_NAME(__stdoutp, g) = &ptr->GET_WSD_VAR_NAME(__sF, g)[1] ;
	ptr->GET_WSD_VAR_NAME(__stderrp, g) = &ptr->GET_WSD_VAR_NAME(__sF, g)[2] ;
	ptr->GET_WSD_VAR_NAME(__sglue, g).next = &ptr->GET_WSD_VAR_NAME(uglue, s);
	ptr->GET_WSD_VAR_NAME(__sglue, g).niobs = 3;
	ptr->GET_WSD_VAR_NAME(__sglue, g).iobs = ptr->GET_WSD_VAR_NAME(__sF, g);
	ptr->GET_WSD_VAR_NAME(lastglue, s) = &ptr->GET_WSD_VAR_NAME(uglue, s);

	ptr->GET_WSD_VAR_NAME(LC_CTYPE_LocaleName,g)[0] = '\0';
	ptr->GET_WSD_VAR_NAME(LC_COLLATE_LocaleName,g)[0] = '\0';
	ptr->GET_WSD_VAR_NAME(LC_NUMERIC_LocaleName,g)[0] = '\0';
	ptr->GET_WSD_VAR_NAME(LC_MONETARY_LocaleName,g)[0] = '\0';
	ptr->GET_WSD_VAR_NAME(LC_TIME_LocaleName,g)[0] = '\0';
	ptr->GET_WSD_VAR_NAME(LC_ALL_LocaleName,g)[0] = '\0';
	
	ptr->localeptr = new _localeinfo;
	ptr->localeptr->_s_iNewLocale = NULL;
	ptr->localeptr->_s_iMonetary_locale = NULL;
	ptr->localeptr->_s_iNumeric_locale = NULL;
	ptr->localeptr->_s_iTime_locale = NULL;
	ptr->localeptr->_s_monetaryLocale = NULL;
	ptr->localeptr->_s_numericLocale = NULL;
	ptr->localeptr->_s_timeLocale = NULL;
	
	strcpy(ptr->GET_WSD_VAR_NAME(w,s),"warning: this program uses gets(), which is unsafe.\n");

	strcpy(ptr->GET_WSD_VAR_NAME(arr_getopt_place, s), EMSG);
	ptr->GET_WSD_VAR_NAME(getopt_place, s) = ptr->GET_WSD_VAR_NAME(arr_getopt_place, s);

	ptr->GET_WSD_VAR_NAME(next,s) = 1;

	ptr->GET_WSD_VAR_NAME(localtime_mutex, s) = tmutex;
	ptr->GET_WSD_VAR_NAME(localtime_key,s) = -1;

	ptr->GET_WSD_VAR_NAME(gmtime_mutex, s) = tmutex;
	ptr->GET_WSD_VAR_NAME(gmtime_key,s) = -1;

	ptr->GET_WSD_VAR_NAME(servdata_init_once, s) = ONCE_INITIALIZER;
	ptr->GET_WSD_VAR_NAME(servdata_thr_keycreated, s) = 0;
	
	ptr->GET_WSD_VAR_NAME(__scanfdebug, g) = 0;
	
	ptr->GET_WSD_VAR_NAME(__mlocale_changed, g) = 1;
	ptr->GET_WSD_VAR_NAME(__nlocale_changed, g) = 1;
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	ptr->GET_WSD_VAR_NAME(__clocale_set, g) = 0;
	ptr->GET_WSD_VAR_NAME(__applocale_changed, g) = 0;
    #endif
	
	ptr->GET_WSD_VAR_NAME(_monetary_using_locale, g) = 0;
	ptr->GET_WSD_VAR_NAME(_numeric_using_locale, g) = 0;
	ptr->GET_WSD_VAR_NAME(_time_using_locale, g) = 0;
	ptr->GET_WSD_VAR_NAME(prevchange, g) = -1;
	ptr->GET_WSD_VAR_NAME(nextchange, g) = -1;
	
	ptr->GET_WSD_VAR_NAME(stm,g).tm_sec = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_min = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_hour = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_mday = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_mon = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_year = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_wday = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_isdst = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_gmtoff = -1;
	ptr->GET_WSD_VAR_NAME(stm,g).tm_zone = NULL;

	ptr->GET_WSD_VAR_NAME(stdio_fmode, s) = 'b';

	ptr->GET_WSD_VAR_NAME(pmem_next, s) = ptr->GET_WSD_VAR_NAME(private_mem, s);
	ptr->GET_WSD_VAR_NAME(p5s, s) = NULL;
	
	for(int i=0; i<16; i++)
	    {
	    ptr->GET_WSD_VAR_NAME(freelist, g)[i] = 0;
	    }
	ptr->GET_WSD_VAR_NAME(__gdtoa_locks, g)[0] = tmutex;
	ptr->GET_WSD_VAR_NAME(__gdtoa_locks, g)[1] = tmutex;


	return KErrNone;
}

#endif //EMULATOR
