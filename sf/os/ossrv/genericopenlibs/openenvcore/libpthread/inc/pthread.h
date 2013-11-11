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
* Description:  POSIX thread exported header file 
*
*/

#ifndef  PTHREAD_H
#define  PTHREAD_H

#include <pthreadtypes.h>
#include <sched.h>
#include <e32def.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>
#include <pthreadalias.h>

#define POSIX_THREAD_DESTRUCTOR_ITERATIONS  4
#define POSIX_THREAD_KEYS_MAX               128
#define PTHREAD_STACK_MIN                   0x2000
#define POSIX_THREAD_THREADS_MAX            64
#define PTHREAD_THREADS_MAX                 1024
#define POSIX_SEM_NSEMS_MAX                 256
#define SEM_NSEMS_MAX                       1024
#define _POSIX_SEM_VALUE_MAX                32767
#define SEM_VALUE_MAX                       INT_MAX

/* internal use*/
typedef enum 
{
    _ENeedsNormalInit =-3,
    _ENeedsRecursiveInit,
    _ENeedsErrorCheckInit,
    _EInitialized =1,
    _EDestroyed,
    _EInvalid,
}_handle_state;

enum _LockStatus
{
    _ELockNotCreated,
    _ELockCreating,
    _ELockCreated,
};

enum _OnceStatus
{
    _ENotDone,
    _EDoing,
    _EDone,
};

typedef int pthread_once_t;

struct _pthread_mutex_t;
typedef struct 
{
    _handle_state  iState;
    struct _pthread_mutex_t* iPtr;
    int   iReentry;
}pthread_mutex_t;

typedef struct 
{
    _handle_state iState;
    int iSharedType;
    int iMutexType;
}pthread_mutexattr_t;


struct _CondNode;
struct _CondQueue
{
  struct _CondNode*       iHead;
  struct _CondNode*       iTail;
  pthread_mutex_t iMutex;
};

typedef struct 
{
  _handle_state    iState;
  struct _CondQueue       iQueue;
}pthread_cond_t;

typedef struct _pthread_condattr_t*   pthread_condattr_t;

#define PTHREAD_ONCE_INIT _ENotDone

#define PTHREAD_MUTEX_INITIALIZER { _ENeedsNormalInit, NULL,0 }
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP { _ENeedsRecursiveInit, NULL, 0 }
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP { _ENeedsErrorCheckInit, NULL,0 }

#define PTHREAD_COND_INITIALIZER { _ENeedsNormalInit, NULL,NULL }

enum 
{ 
    PTHREAD_CREATE_JOINABLE = 0,  /* Default */ 
    PTHREAD_CREATE_DETACHED = 1
};

enum 
{ 
    PTHREAD_SCOPE_SYSTEM  = 1  /* Default */
};

enum 
{  
    PTHREAD_CANCEL_ASYNCHRONOUS = 0,  
    PTHREAD_CANCEL_DEFERRED = 1  /* Default */
};

enum 
{  
    PTHREAD_PROCESS_PRIVATE = 0,   
};

enum
{
  PTHREAD_MUTEX_NORMAL,
  PTHREAD_MUTEX_RECURSIVE,
  PTHREAD_MUTEX_ERRORCHECK,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
};



#ifdef __cplusplus
extern "C"
{
#endif    /* __cplusplus */

typedef void * (*thread_begin_routine)(void *);
/*
This function creates a thread, with attributes specified by attrib,
within a process. If attrib is NULL, then default attributes will be used.
On successful completion, pthread_create() will store the ID of the 
created thread in the location referenced by threadhdl.
*/ 
IMPORT_C extern int pthread_create(pthread_t *threadhdl, pthread_attr_t *attrib, 
                   thread_begin_routine begin_routine , void * param);  
/*
This function return the handle to current thread.
*/                    
IMPORT_C extern pthread_t pthread_self ();

/*
Compare two thread handles
*/
IMPORT_C extern int pthread_equal(pthread_t t1,pthread_t t2);
/*
Waiting for thread termination
*/
IMPORT_C extern int pthread_join(pthread_t thHandle, void **retValPtr);
/*
detach from thread 
*/
IMPORT_C extern int pthread_detach(pthread_t thrHandle);
/*
exit current thread
*/
IMPORT_C extern void pthread_exit(void *retValPtr);
/*
Initialise the thread attributes
*/
IMPORT_C int pthread_attr_init(pthread_attr_t *attrib);
/*
Destroy the thread attributes
*/
IMPORT_C int pthread_attr_destroy(pthread_attr_t *attrib);
/*
Get the detach_state of the current thread
*/
IMPORT_C int pthread_attr_getdetachstate(const pthread_attr_t *attrib,
                                         int *detState);
/*
Set the detach_state of the current thread
*/                                         
IMPORT_C int pthread_attr_setdetachstate(pthread_attr_t *attrib, 
                                         int detState);                                         
/*
Get the stack size of the current thread
*/
IMPORT_C int pthread_attr_getstacksize(const pthread_attr_t *attrib,
                                       size_t *stkSize);
/*
Set the stack size of the current thread
*/                                       
IMPORT_C int pthread_attr_setstacksize(pthread_attr_t *attrib, 
                                       size_t stkSize);                                       

/*
This function ensures that a piece of initialization code is executed at most once. 
The once_control argument must point to a static  variable statically initialized to PTHREAD_ONCE_INIT

*/
typedef void (*thread_init_routine) (void);
IMPORT_C extern int pthread_once (pthread_once_t * once_control, 
                                  thread_init_routine init_routine);

/*
extern int pthread_key_create (pthread_key_t * key, void (*destructor) (void *));
extern int pthread_key_delete (pthread_key_t key);
extern int pthread_setspecific (pthread_key_t key, const void *value);
extern void* pthread_getspecific (pthread_key_t key);
*/

/*
This function initializes the mutex attribute object attr and fills it with default values for the attributes.
*/
IMPORT_C extern int pthread_mutexattr_init (pthread_mutexattr_t * attr);

/*
This function destroys the mutex attribute object attr and marks it as a destroyed object.
*/
IMPORT_C extern int pthread_mutexattr_destroy (pthread_mutexattr_t * attr);

/*
This function retrieves the current value of the process shared attribute in 
attr and stores it in the location pointed to by pshared.
*/
IMPORT_C extern int pthread_mutexattr_getpshared (const pthread_mutexattr_t* attr, int* pshared);

/*
This function sets the current value of the process shared attribute in attr to the value specifed in the  
pshared variable.
*/
IMPORT_C extern int pthread_mutexattr_setpshared (pthread_mutexattr_t * attr, int pshared);

/*
This function retrieves the current value of the mutex kind attribute in attr and 
stores it in the location pointed to by type.
*/
IMPORT_C extern int pthread_mutexattr_gettype (pthread_mutexattr_t * attr, int *type);


/*
This function sets the current value of the mutex kind attribute in attr to the value specifed in the  type variable.
*/
IMPORT_C extern int pthread_mutexattr_settype (pthread_mutexattr_t * attr, int type);

/*
This function initializes the mutex object pointed to by mutex according to the mutex attributes specified in attr. 
If attr is NULL, default attributes are used instead.

*/
IMPORT_C extern int pthread_mutex_init (pthread_mutex_t * mutex, const pthread_mutexattr_t * attr);

/*
This function destroys the mutex object.  The mutex must be unlocked on entrance.
This implementation requires even statically initialized mutexes to be explicitly destroyed.
*/
IMPORT_C extern int pthread_mutex_destroy (pthread_mutex_t * mutex);

/*
This function locks the given mutex. If the mutex is currently
unlocked, it becomes locked and owned by the calling thread, and
this function  returns immediately. If the mutex is already
locked by another thread, this function suspends the calling
thread until the mutex is unlocked. 

If the mutex is already locked by the calling thread, the behavior of
this function depends on the kind of the mutex. If the mutex is
of the  PTHREAD_MUTEX_NORMAL or PTHREAD_MUTEX_DEFAULT kind, the calling thread is suspended until the mutex
is unlocked, thus effectively causing the calling thread to
deadlock. If the mutex is of the PTHREAD_MUTEX_ERRORCHECK kind,
this function returns immediately with the error code EDEADLK.
If the mutex is of the PTHREAD_MUTEX_RECURSIVE kind, this function
succeeds and returns immediately, recording the number of times the
calling thread has locked the mutex. An equal number of
pthread_mutex_unlock operations must be performed before the mutex
returns to the unlocked state.
*/
IMPORT_C extern int pthread_mutex_lock (pthread_mutex_t * mutex);


/*
This function behaves identically to pthread_mutex_lock, except that if it cannot acquire the lock before 
the abs_timeout time, the call returns with the error code ETIMEDOUT. 
*/
IMPORT_C extern int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);

/*
This function behaves identically to pthread_mutex_lock, except that it does not 
block the calling thread if the mutex is already locked by another thread (or by 
the calling thread in the case of a PTHREAD_MUTEX_RECURSIVE mutex). Instead, pthread_mutex_trylock 
returns immediately with the error code EAGAIN.
*/
IMPORT_C extern int pthread_mutex_trylock (pthread_mutex_t * mutex);

/*
This function unlocks the given mutex. The mutex is assumed
to be locked and owned by the calling thread on entrance to
this function. If the mutex is of the PTHREAD_MUTEX_NORMAL or PTHREAD_MUTEX_DEFAULT kind,
this function always returns it to the unlocked state. If it
is of the PTHREAD_MUTEX_RECURSIVE kind, it decrements the locking count of the
mutex (number of pthread_mutex_lock operations performed on it by
the calling thread), and only when this count reaches zero is the
mutex actually unlocked.

On PTHREAD_MUTEX_ERRORCHECK mutexes, this function actually checks
at run-time that the mutex is locked on entrance, and that it was
locked by the same thread that is now calling pthread_mutex_unlock.
If these conditions are not met, an error code is returned and the
mutex remains unchanged.  
*/
IMPORT_C extern int pthread_mutex_unlock (pthread_mutex_t * mutex);

/*
This function initializes the condition attribute object attr and 
sets it with default values for the attributes. 
*/
IMPORT_C extern int pthread_condattr_init (pthread_condattr_t * /*attr*/);

/*
This function destroys the condtion variable attribute object.
*/
IMPORT_C extern int pthread_condattr_destroy (pthread_condattr_t * attr);
/*
extern int pthread_condattr_getpshared (const pthread_condattr_t * attr, int *pshared);
extern int pthread_condattr_setpshared (pthread_condattr_t * attr, int pshared);
*/

/*
This function initializes the condition variable object pointed to by cond 
using the attributes of the attr variable. If attr is NULL, default attributes 
are used instead.
*/
IMPORT_C extern int pthread_cond_init (pthread_cond_t * cond, const pthread_condattr_t * /*attr*/);

/*
This function destroys the condition variable  object. 
*/
IMPORT_C extern int pthread_cond_destroy (pthread_cond_t * cond);

/*
This function atomically unlocks mutex and waits on cond, 
as pthread_cond_wait does, but it also bounds the duration 
of the wait. If cond has not been signalled within the amount 
of time specified by abstime, the mutex mutex is re-acquired and 
pthread_cond_timedwait returns the error ETIMEDOUT. The abstime 
parameter specifies an absolute time, with the same origin as time(2) and gettimeofday(2). 
*/
IMPORT_C extern int pthread_cond_timedwait (pthread_cond_t * cond,pthread_mutex_t * mutex,const struct timespec *abstime);

/*
This function atomically unlocks the mutex (as per pthread_unlock_mutex) 
and waits for the condition variable cond to be signalled. The thread 
execution is suspended and does not consume any CPU time until the condition 
variable is signalled. The mutex must be locked by the calling thread on entrance 
to pthread_cond_wait. Before returning to the calling thread, pthread_cond_wait 
re-acquires mutex (as per pthread_lock_mutex).
*/
IMPORT_C extern int pthread_cond_wait (pthread_cond_t * cond, pthread_mutex_t * mutex);

/*
This function restarts one of the threads that are waiting on the condition 
variable cond. If no threads are waiting on cond, nothing happens. If several 
threads are waiting on cond, exactly one is restarted. 

*/
IMPORT_C extern int pthread_cond_signal (pthread_cond_t * cond);

/*
This function restarts all the threads that are waiting on the 
condition variable cond. Nothing happens if no threads are waiting on cond. 
*/
IMPORT_C extern int pthread_cond_broadcast (pthread_cond_t * cond);


typedef void (*destructor_routine)(void *);
/*
Thread-specific data key creation
*/
IMPORT_C int pthread_key_create(pthread_key_t *key, destructor_routine dest);

/*
Thread-specific data key deletion
*/
IMPORT_C int pthread_key_delete(pthread_key_t key);

/*
Setting Thread-specific data 
*/
IMPORT_C int pthread_setspecific(pthread_key_t key, const void *val);

/*
Getting Thread-specific data 
*/
IMPORT_C void* pthread_getspecific(pthread_key_t key);

/*
Getting contention scope 
*/
IMPORT_C int pthread_attr_getscope(const pthread_attr_t *attrib,
                                   int* scope);
/*
Setting contention scope 
*/
IMPORT_C int pthread_attr_setscope(pthread_attr_t *attrib,int conscope); 

/*
Setting scheduling policy
*/
IMPORT_C int pthread_attr_setschedpolicy(pthread_attr_t *attrib, 
                                         int policy);                                  

/*
Getting scheduling policy of the thread
*/
IMPORT_C int pthread_attr_getschedpolicy(const pthread_attr_t *attrib,
                                         int *policy);

/*
Getting scheduling param of the thread
*/
IMPORT_C int pthread_attr_getschedparam(const pthread_attr_t *attrib,
                                        struct sched_param *param);

/*
Setting scheduling param
*/                                        
IMPORT_C int pthread_attr_setschedparam(pthread_attr_t *attrib, 
                                        const struct sched_param *param);

/*
Getting dynamic scheduling param of the thread
*/                                        
IMPORT_C int pthread_getschedparam(pthread_t thr, int *policy,
                                   struct sched_param *param);

/*
Dynamically Setting scheduling params
*/                                   
IMPORT_C int pthread_setschedparam(pthread_t th, int policy, 
                                   const struct sched_param *param);
                                         
#ifdef __cplusplus
}                               /* End of  "C" */
#endif                          /* __cplusplus */


#endif /* PTHREAD_H */

