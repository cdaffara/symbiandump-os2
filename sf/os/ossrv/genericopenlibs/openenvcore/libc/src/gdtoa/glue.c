 /*
  * Machine-independent glue to integrate David Gay's gdtoa
  * package into libc.
  *
  * $FreeBSD: src/lib/libc/gdtoa/glue.c,v 1.2 2003/06/21 08:20:14 das Exp $
  * © Portions copyright (c) 2005-2006  Nokia Corporation.  All rights reserved.
  */
 
#include <pthread.h>
#include "libc_wsd_defs.h"

#ifdef __SYMBIAN32__
#ifndef EMULATOR
pthread_mutex_t __gdtoa_locks[] = {
         PTHREAD_MUTEX_INITIALIZER,
         PTHREAD_MUTEX_INITIALIZER
 };
#else
GET_GLOBAL_ARRAY_FROM_TLS(__gdtoa_locks, pthread_mutex_t)
#define __gdtoa_locks (GET_WSD_VAR_NAME(__gdtoa_locks, g)())
#endif
#endif


