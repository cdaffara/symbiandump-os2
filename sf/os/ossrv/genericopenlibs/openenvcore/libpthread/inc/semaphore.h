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
* Name        : semaphore.h
* Part of     : semaphore
* Interface   : POSIX, semaphores
* POSIX implementation of semaphores on Symbian
* Version     :
*
*/



#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif              /* __cplusplus */

/*Semaphores supported */

#define SEM_FAILED -1
typedef struct _sem_t* sem_t;

/*
This function initializes the semaphore object.
The semaphore count is set to the initial count value. 
*/
IMPORT_C extern int sem_init (sem_t * sem, int pshared, unsigned int value);

/*
This function destroys the semaphore  object. 
*/
IMPORT_C extern int  sem_destroy (sem_t* sem);

/*
This function atomically decrements sem's count if it is greater than 0 and returns immediately,
or it returns immediately with a return code of -1 after having set errno to EAGAIN. 
sem_trywait never blocks.
*/
IMPORT_C extern int sem_trywait (sem_t* sem);

/*
This function atomically decrements sem's count if it is greater than 0 and returns immediately
or it suspends the calling thread until it can resume following a call to sem_post or sem_post_multiple
*/
IMPORT_C extern int sem_wait (sem_t* sem);

/*
This function atomically decrements sem's count if it is greater than 0 and returns immediately, or it 
suspends the calling thread. If timeout time lapses before the thread can be resumed because of a sem_post,
case, then sem_timedwait returns with a return code of -1 after having set errno to ETIMEDOUT. If the 
thread  can return without suspending then abstime parameter is not checked.
*/
IMPORT_C extern int sem_timedwait (sem_t* sem, const struct timespec* abstime);

/*
The function increments (unlocks) the semaphore pointed to by sem .
If there are threads blocked on the semaphore when sem_post is called, the highest priority thread that has been blocked the longest on
the semaphore will be allowed to return from sem_wait .
If successful, the function returns zero; otherwise, it returns -1 and set errno to indicate the error.
*/
IMPORT_C extern int sem_post (sem_t* sem);

/*
This function establish a connection between a named semaphore and a process
*/
/* IMPORT_C extern sem_t* sem_open (const char* name,int oflag,mode_t mode, unsigned int value); */

/*
This function indicate that the calling process is finished using the named semaphore indicated by sem*/
/* IMPORT_C extern int sem_close (sem_t* sem); */

/*
This function cleans up resources associated with a system-wide semaphore.
*/
/* IMPORT_C extern int sem_unlink (const char* name); */

/*
This function updates the location referenced by the sval argument with the value of the semaphore 
referenced by sem without affecting the state of the semaphore. 
The updated value represents an actual semaphore value that occurred at some unspecified time 
during the call, but it need not be the actual value of the semaphore when it is returned to 
the calling process. If sem is locked, then the object to which sval points shall either be set 
to zero or to a negative number whose absolute value represents the number of processes waiting
for the semaphore at some unspecified time during the call.
*/
IMPORT_C extern int sem_getvalue (sem_t* sem, int* sval);


#ifdef __cplusplus
}               /* End of extern "C" */
#endif              /* __cplusplus */

#endif              /* __SEMAPHORE_H__ */
