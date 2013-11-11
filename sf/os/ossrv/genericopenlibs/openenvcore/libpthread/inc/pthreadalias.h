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
* Name        : pthreadalias.h
* Part of     : pthread
* Interface   : POSIX, pthread
* Version     :
*
*/



#ifndef  PTHREADALIAS_H
#define  PTHREADALIAS_H

#define _pthread_create pthread_create
#define _pthread_self pthread_self
#define _pthread_equal pthread_equal
#define _pthread_join pthread_join
#define _pthread_detach pthread_detach
#define _pthread_exit pthread_exit
#define _pthread_attr_init pthread_attr_init
#define _pthread_attr_destroy pthread_attr_destroy
#define _pthread_attr_getdetachstate pthread_attr_getdetachstate
#define _pthread_attr_setdetachstate pthread_attr_setdetachstate
#define _pthread_attr_getstacksize pthread_attr_getstacksize
#define _pthread_attr_setstacksize pthread_attr_setstacksize
#define _pthread_once pthread_once 
#define _pthread_mutexattr_init pthread_mutexattr_init
#define _pthread_mutexattr_destroy pthread_mutexattr_destroy
#define _pthread_mutexattr_getpshared pthread_mutexattr_getpshared
#define _pthread_mutexattr_setpshared pthread_mutexattr_setpshared
#define _pthread_mutexattr_gettype pthread_mutexattr_gettype
#define _pthread_mutexattr_settype pthread_mutexattr_settype
#define _pthread_mutex_init pthread_mutex_init
#define _pthread_mutex_destroy pthread_mutex_destroy
#define _pthread_mutex_lock pthread_mutex_lock
#define _pthread_mutex_timedlock pthread_mutex_timedlock
#define _pthread_mutex_trylock pthread_mutex_trylock
#define _pthread_mutex_unlock pthread_mutex_unlock
#define _pthread_condattr_init pthread_condattr_init
#define _pthread_condattr_destroy pthread_condattr_destroy
#define _pthread_cond_init pthread_cond_init
#define _pthread_cond_destroy pthread_cond_destroy
#define _pthread_cond_timedwait pthread_cond_timedwait
#define _pthread_cond_wait pthread_cond_wait
#define _pthread_cond_signal pthread_cond_signal
#define _pthread_cond_broadcast pthread_cond_broadcast
#define _pthread_key_create pthread_key_create
#define _pthread_key_delete pthread_key_delete
#define _pthread_setspecific pthread_setspecific
#define _pthread_getspecific pthread_getspecific
#define _pthread_attr_getscope pthread_attr_getscope
#define _pthread_attr_setscope pthread_attr_setscope
#define _pthread_attr_setschedpolicy pthread_attr_setschedpolicy
#define _pthread_attr_getschedpolicy pthread_attr_getschedpolicy
#define _pthread_attr_getschedparam pthread_attr_getschedparam
#define _pthread_attr_setschedparam pthread_attr_setschedparam 
#define _pthread_getschedparam pthread_getschedparam 
#define _pthread_setschedparam pthread_setschedparam 

#endif /* PTHREADALIAS_H */

