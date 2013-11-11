/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tmutex.h"

#define WITHOUT_ERROR 0
#define WITH_ERROR 0


#define CHECK(a) if (!(a)) INFO_PRINTF2(_L("Unexpected failure at line %d\n"), __LINE__);
pthread_t myThread,myThread1,myThread2,myThread3;
pthread_mutex_t myMutex1,myMutex2;

TInt CTestMutex::PThreadAttrStaticInitUnlockL(TInt val, TInt errVal )
    {
    _LIT(KFunc, "In PThreadAttrStaticInitUnlockL");
    INFO_PRINTF1(KFunc);
    TInt err=0;
    switch (val)
        {
        default:
        case PTHREAD_MUTEX_NORMAL:
            {
            pthread_mutex_t lMutex = PTHREAD_MUTEX_INITIALIZER;
            err = pthread_mutex_unlock (&lMutex);
            if ( err )
                {
                if ( errVal && ( err == EINVAL || err == ENOSYS))
                    {
                    err = KErrNone;
                    }
                else
                    {
                    ERR_PRINTF1(_L("pthread_mutex_unlocklock Error"));
                    }
                }
            pthread_mutex_destroy (&lMutex);
            }
            break;
        case PTHREAD_MUTEX_RECURSIVE:
            {
            pthread_mutex_t lMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
            err = pthread_mutex_unlock (&lMutex);
            if ( err )
                {
                if ( errVal && ( err == EINVAL || err == ENOSYS))
                    {
                    err = KErrNone;
                    }
                else
                    {
                    ERR_PRINTF1(_L("pthread_mutex_unlocklock Error"));
                    }
                }
            pthread_mutex_destroy (&lMutex);
            }
            break;
        case PTHREAD_MUTEX_ERRORCHECK:
            {
            pthread_mutex_t lMutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
            err = pthread_mutex_unlock (&lMutex);
            if ( err )
                {
                if ( errVal && ( err == EINVAL || err == ENOSYS))
                    {
                    err = KErrNone;
                    }
                else
                    {
                    ERR_PRINTF1(_L("pthread_mutex_unlocklock Error"));
                    }
                }
            pthread_mutex_destroy (&lMutex);
            }
            break;
        }
    
    return KErrNone;
    }


TInt CTestMutex::PThreadAttrStaticInitTryLockL(  )
    {
    _LIT(KFunc, "In PThreadAttrStaticInitTryLockL");
    INFO_PRINTF1( KFunc);
    pthread_mutex_t lMutex = PTHREAD_MUTEX_INITIALIZER;
    
	TInt err = pthread_mutex_trylock (&iMutex);
    if ( err)
        {
        INFO_PRINTF1( _L("pthread_mutex_trylock error!"));
        }
    else
        {  
    
        err = pthread_mutex_unlock (&lMutex);
        if ( err )
            {
            INFO_PRINTF1( _L("pthread_mutex_unlock error!"));
            }
        else
            {
            err = pthread_mutex_destroy (&lMutex);
            if ( err )
                {
                INFO_PRINTF1( _L("pthread_mutex_destroy error!"));
                }
            }
        }
    
    return KErrNone;
    }

TInt CTestMutex::PThreadAttrStaticInitTimedLockL(TInt val, TInt typeTimeLockCheck)
    {
    _LIT(KFunc, "In PThreadAttrStaticInitTimedLockL");
    INFO_PRINTF1(KFunc);
    pthread_mutex_t lMutex = PTHREAD_MUTEX_INITIALIZER;
    
    struct timespec abstime;
	struct timeval  now;
	gettimeofday( &now, NULL);
        
    abstime.tv_sec = now.tv_sec + val;
    abstime.tv_nsec = 0;
    

    TInt err = pthread_mutex_timedlock (&iMutex, &abstime);
    if ( typeTimeLockCheck == 1)
        {
        // testing timeout
        if (err == ETIMEDOUT)
            {
            err = KErrNone;
            }
        }
    else
        {  
		err = pthread_mutex_unlock (&lMutex);
        if ( err )
            {
            ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
            }
        else
            {
            err = pthread_mutex_destroy (&lMutex);
            if ( err )
                {
                ERR_PRINTF2( _L("pthread_mutex_destroy returned %d"), err);
                }
            }
        }
    return KErrNone;
    }
   
TInt CTestMutex::PThreadAttrStaticInitL( )
    {
    _LIT(KFunc, "ThreadEntryFunction1027L");
    INFO_PRINTF1(KFunc);
    pthread_mutex_t lMutex = PTHREAD_MUTEX_INITIALIZER;
    TInt err = pthread_mutex_lock (&lMutex);
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_mutex_lock erturned %d"), err);
        }
    else
        {
        err = pthread_mutex_unlock (&lMutex);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutex_unlock returned %d"), err);
            }
        else
            {
            err = pthread_mutex_destroy (&lMutex);
            if ( err )
                {
                ERR_PRINTF2 ( _L("pthread_mutex_destroy returned %d"), err);
                }
            }
        }
    return KErrNone;
    }

void CTestMutex::SendSignal()
    {
    }
    
TInt CTestMutex::Delay(TInt val)
    {
    _LIT(KFunc, "In Delay");
    INFO_PRINTF1 ( KFunc);
    User::After (val);
    return KErrNone;
    }
    
TInt CTestMutex::WaitForThreadCompleteL(TInt val )
    {
    _LIT(KFunc, "In WaitForThreadCompleteL");
    INFO_PRINTF1 ( KFunc);
    
    while ( iNoofThreadToComplete < val)
        {
        //spin over here.
        User::After (100000);
        }
    return KErrNone;
    }

TInt CTestMutex::SetExitValueL(TInt val )
    {
    _LIT(KFunc, "In SetExitValueL");
    INFO_PRINTF1 ( KFunc);
     
	iExitValueL = val;
    if (iErroCode)
        {
        return KErrGeneral;
        }
    else
        {
        User::After (200000);
        }
    return KErrNone;
    }

TInt CTestMutex::CreateThreadL(TInt threadType)
    {
    _LIT(KFunc, "In CreateThreadL");
    INFO_PRINTF1 ( KFunc);
     
    TInt err=0;
    switch(threadType)
        {
        case 1: // PTHREAD_MUTEX_RECURSIVE
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionRecursiveL,(void*)this);
            break;
        case 2: // PTHREAD_MUTEX_ERRORCHECK
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionErrorCheckL,(void*)this);
            break;
        case 650: // 1_6_2_650 (race between pthread_mutex_destroy and pthread_mutex_destroy)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction650L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 338: //1_6_2_338 (race between pthread_mutex_lock and ptheard_mutex_destroy)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction338L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 339: //1_6_2_339 (race between pthread_mutex_unlock and pthread_mutex_destroy)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction339L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 334: //1_6_2_334 (race between call to pthread_mutex_init and pthread_mutex_destroy)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction334L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 333: //1_6_2_333 (race between call to pthread_mutex_init and pthread_mutex_lock)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction333L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 335: //1_6_2_335 (race between call to pthread_mutex_init and pthread_mutex_unlock)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction335L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 332: //1_6_2_332 (race between two calls to pthread_mutex_init)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction332L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;

        case 362: //1_6_2_362 (pthread_mutex_lock called on a locked mutex of type PTHREAD_MUTEX_ERRORCHECK from thread that did not lock the mutex)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction362L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
            
        case 1027: //1_6_2_1027 (pthread_mutex_lock called on a locked mutex of type PTHREAD_MUTEX_NORMAL from thread that did locked the mutex)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1027L,(void*)this);
            break;
        case 1037: //1_6_2_1037 (mutex is signalled before pthread_mutex_timedwait times out)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1037L,(void*)this);
            break;
        case 1040: //1_6_2_1037 (mutex is signalled before pthread_mutex_timedwait times out)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1040L,(void*)this);
            break;
            
        case 1062: //1_6_2_1062 (pthread_mutex_unlock called on a locked mutex of type PTHREAD_MUTEX_ERRORCHECK from thread that did not lock the mutex)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1062L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 1063: //1_6_2_1063 (pthread_mutex_unlock called on a locked mutex of type PTHREAD_MUTEX_NORMAL from thread that did not lock the mutex)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1063L,(void*)this);
            User::After(100000);
            StartDelayedServiceL();
            break;
        case 1070: //1_6_2_1070 (pthread_mutex_unlock called when n threads suspended on a pthread_mutex_lock)
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1070L,(void*)this);
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1070L,(void*)this);
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunction1070L,(void*)this);
            break;
        default:
            err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionL,(void*)this);
            break;
        }
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_create returned %d"), err);
        }
    return err;
    }
  
void* CTestMutex::ThreadEntryFunction1070L(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        }
    else
        {
        User::After (1000000);
        err = pthread_mutex_unlock (&(self->iMutex));
        if ( err )
            {
            self->iErroCode = 1;
            self->ERR_PRINTF2( _L("pthread_mutex_unlock"), err);
            }
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    self->iNoofThreadToComplete++;
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction1063L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_unlock (&(self->iMutex));
    if ( err != EPERM )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction1062L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_unlock (&(self->iMutex));
    if ( err != EPERM)
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }
    
void* CTestMutex::ThreadEntryFunction1040L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        }
    else
        {
        User::After (1000000);
        err = pthread_mutex_unlock (&(self->iMutex));
        if ( err )
            {
            self->iErroCode = 1;
            self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
            }
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction1037L(void* arg)
    {
   
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        }
    else
        {
        User::After (300000);
        err = pthread_mutex_unlock (&(self->iMutex));
        if ( err )
            {
            self->iErroCode = 1;
            self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
            }
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction1027L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    User::After (100000);
    TInt err = pthread_mutex_unlock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction362L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        err = pthread_mutex_unlock (&(self->iMutex));
        if ( err )
            {
            self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction332L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_init (&(self->iMutex), NULL);
    if ( err )
        {
        // negative test.
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_init returned %d"), err);
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction335L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_unlock (&(self->iMutex));
    if ( err == KErrNone)
        {
        // negative test.
        self->iErroCode = 1;
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction333L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        err = pthread_mutex_unlock (&(self->iMutex));
        if ( err )
            {
            self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction334L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_destroy (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_destory returned %d"), err);
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }

void* CTestMutex::ThreadEntryFunction338L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        }
    else
        {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        err = pthread_mutex_unlock (&(self->iMutex));
        if ( err )
            {
            self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
            }
        }
    return NULL;
    }
    
void* CTestMutex::ThreadEntryFunction339L(void* arg)
    {
    
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_unlock (&(self->iMutex));
    
    if ( err != EPERM  )
    {
       self->iErroCode = 1;
       self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
    }  
    else
    {
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
    }
    return NULL;
    }
    

void CTestMutex::StartDelayedServiceL()
    {
    TCallBack cb(HandleIdleCallbackL,this); 
    //iIdleTimer->Cancel();
    //iIdleTimer->Start(cb);
    }

TInt CTestMutex::HandleIdleCallbackL(TAny *aThis)
    {
    static_cast < CTestMutex*> ( aThis )->HandleIdleCommandL();
    return 0;
    }       

void CTestMutex::HandleIdleCommandL()
    {
    SendSignal();
    }

void* CTestMutex::ThreadEntryFunction650L(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_destroy (&(self->iMutex));
    if ( err )
        {
        self->iErroCode = 1;
        self->ERR_PRINTF2( _L("pthread_mutex_destory returned %d"), err);
        }
    else
        {
        User::After (50000);
        while (!(self->iExitValueL))
            {
            User::After (100000);
            }
        }
    return NULL;
    }
    
void* CTestMutex::ThreadEntryFunctionL(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->ERR_PRINTF2( _L("pthread_mutex_lock retuned %d"), err);
        }
    User::After(10000000);
    err = pthread_mutex_unlock(&(self->iMutex));
    if ( err )
        {
        self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
        return NULL;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexDestroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexDestroy(TInt errVal)
    {
    _LIT(KFunc, "In PThreadMutexDestroy");
    INFO_PRINTF1 ( KFunc);
   
    TInt err = pthread_mutex_destroy (&iMutex);
    
    if ( err )
        {
        if ( errVal && (err == EINVAL || err == EBUSY))
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutex_destroy returned %d"), err);
            }
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexLock
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexLock(TInt errVal )
    {
    _LIT(KFunc, "In PThreadMutexLock");
    INFO_PRINTF1 ( KFunc);
    
    TInt err = pthread_mutex_lock (&iMutex);
    if ( err )
        {
        if ( errVal && (err == EINVAL || err == EDEADLK))
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutex_lock returned %d"), err);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexTimedlock
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexTimedlock(TInt val,TInt typeTimeLockCheck)
    {
    struct timespec abstime;
	struct timeval  now;
	gettimeofday( &now, NULL);
    
    abstime.tv_sec = now.tv_sec + val;
    abstime.tv_nsec = 0;
    
    _LIT(KFunc, "In PThreadMutexTimedlock");
    INFO_PRINTF1 ( KFunc);

    TInt err = pthread_mutex_timedlock (&iMutex, &abstime);
    if ( err)
        {
        if ( typeTimeLockCheck == 1 && (err == ETIMEDOUT || err == EINVAL) )
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutex_timedlock returned %d"), err);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexTrylock
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexTrylock(TInt val )
    {
    _LIT(KFunc, "In PThreadMutexTrylock");
    INFO_PRINTF1 ( KFunc);
    
    TInt err = pthread_mutex_trylock (&iMutex);
    if ( err )
        {
        if (val && (err == EBUSY || err == EINVAL))
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 (_L("pthread_mutex_trylock returned %d"), err);
            }            
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexUnlock
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexUnlock(TInt errVal  )
    {
    _LIT(KFunc, "In PThreadMutexTrylock");
    INFO_PRINTF1 ( KFunc);
    
    TInt    err = pthread_mutex_unlock(&iMutex);
    if ( err )
        {
        if ( errVal && ( err == EPERM || err == ENOSYS))
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutex_unlockreturned %d"), err);
            }
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CTestMutex::PThreadOnceCallBack
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTestMutex::PThreadOnceCallBack (void)
    {
    TInt i = 0;
    for (;i < 10; i++)
        {
        }
    }
// -----------------------------------------------------------------------------
// CTestMutex::PThreadOnce
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadOnce(  )
    {
    _LIT(KFunc, "In PThreadOnce");
    INFO_PRINTF1( KFunc);
    pthread_once_t once;
    TInt err = KErrNone;
    for (TInt i = 0 ; i < 100; i ++)
        {
        err = pthread_once ( &once, &PThreadOnceCallBack);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_once returned %d"), err);
            }
        err = pthread_once ( &once, &PThreadOnceCallBack);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_once returned %d"),err);
            }
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CTestMutex::
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexAttrInit(TInt errVal )
    {
    _LIT(KFunc, "In PThreadMutexAttrInit");
    INFO_PRINTF1 ( KFunc);
    
    TInt err = pthread_mutexattr_init ( &iMutexAttr );
    if ( err )
        {
        if ( errVal && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_init returned %d"), err);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::\
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexAttrInitInvalidPointerL(TInt errVal  )
    {
    _LIT(KFunc, "In PThreadMutexAttrInitInvalidPointerL");
    INFO_PRINTF1 ( KFunc);
    
    TInt err = pthread_mutexattr_init ( NULL );
    if ( err )
        {
        if ( errVal && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_init returned %d"), err);
            }
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexattrDestroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexAttrDestroy(TInt val)
    {
    _LIT(KFunc, "In PThreadMutexattrDestroy");
    INFO_PRINTF1 ( KFunc);
    
    TInt err = pthread_mutexattr_destroy ( &iMutexAttr );
    if ( err )
        {
        if ( val && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            }
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexAttrGetpshared
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexAttrGetpshared(TInt errVal)
    {
    _LIT(KFunc, "In PThreadMutexAttrGetpshared");
    INFO_PRINTF1 ( KFunc);
    
    TInt val;
    TInt err = pthread_mutexattr_getpshared (&iMutexAttr, &val);
    if ( err )
        {
        if ( errVal && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_getpshared returned %d"),err);
            }
        
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexAttrSetpshared
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexAttrSetpshared(TInt val,TInt errVal )
    {
    _LIT(KFunc, "In PThreadMutexAttrSetpshared");
    INFO_PRINTF1 ( KFunc);

    TInt err = pthread_mutexattr_setpshared (&iMutexAttr, val );
    if ( err )
        {
        if ( errVal && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_setpshared returned %d"),err);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexattrGettype
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexattrGettype(TInt errVal)
    {
    _LIT(KFunc, "In PThreadMutexattrGettype");
    INFO_PRINTF1 ( KFunc);
    
    TInt val;
    TInt err = pthread_mutexattr_gettype (&iMutexAttr, &val);
    if ( err )
        {
        if ( errVal && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_gettype returned %d"), err);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexattrSettype
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexattrSettype(TInt val, TInt errVal)
    {
    _LIT(KFunc, "In PThreadMutexattrSettype");
    INFO_PRINTF1 ( KFunc);

    TInt err = pthread_mutexattr_settype (&iMutexAttr, val );
    if ( err )
        {
        if ( errVal && err == EINVAL)
            {
            err = KErrNone;
            }
        else
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_settype returned %d"), err);
            }
        }
        
	return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexInit
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexInit(TInt initVal,TInt errVal)
    {
    _LIT(KFunc, "In PThreadMutexInit");
    INFO_PRINTF1 ( KFunc);
    
    TInt err;
    
    if ( !initVal )
        {
        // no parameter is passed.
        err = pthread_mutex_init (&iMutex, NULL);
        }
    else
        {
        // here assumption is that for STIF testcase, we have already initialize the mutex attr.
        err = pthread_mutex_init (&iMutex, &iMutexAttr);
        }
    if (err)
        {
        if ( errVal && (err == EINVAL || err == ENOSYS || err == ENOMEM))
            {
            err = KErrNone;
            }
         else
            {
            ERR_PRINTF2 ( _L("pthread_mutex_init returned %d"), err);                
            }
        }
    return err;
    }


// -----------------------------------------------------------------------------
// CTestMutex::SemInit
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemInit(  )
    {
    _LIT(KFunc, "In SemInit");
    INFO_PRINTF1 ( KFunc);
    sem_t semid;
    TInt err = sem_init ( &semid, 0, 1);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"),err);
        return err;
        }
    err = sem_destroy ( &semid);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
        }
    return err;
    
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemDestroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemDestroy(  )
    {
    _LIT(KFunc, "In SemDestroy");
    INFO_PRINTF1 ( KFunc);
    sem_t semid;
    TInt err = KErrNone;
    for (TInt i = 0; i < 100 ; i++ )
        {
        err = sem_init ( &semid, 0, 1);
        if ( err )
            {
            ERR_PRINTF2 ( _L("sem_init returned %d"), err);
            }
        err = sem_destroy ( &semid);
        if ( err )
            {
            ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
            }
        }
    return err;
   }

// -----------------------------------------------------------------------------
// CTestMutex::SemTryWait
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemTryWait(  )
    {
    _LIT(KFunc, "In SemTryWait");
    INFO_PRINTF1 ( KFunc);
    sem_t semid;
    TInt err = sem_init ( &semid, 0, 5);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"), err);
        return err;
        }
    for (TInt i = 0; i < 5; i ++)
        {
        err = sem_trywait ( &semid );
        if ( err )
            {
            ERR_PRINTF2 ( _L("sem_trywait returned %d"),err);
            }
        }
    err = sem_destroy ( &semid);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"),err);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemWait
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemWait(  )
    {
    _LIT(KFunc, "In SemWait");
    INFO_PRINTF1 ( KFunc);
    TInt err = sem_init ( &iSemId, 0, 1);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"), err);
        return err;
        }
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemaphoreL,(void*)this);
    err = sem_wait ( &iSemId );
    User::After ( 500000);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_wait returned %d"), err);
        }
    err = sem_post (&iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_post returned %d"), err);
        }
    err = sem_destroy ( &iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"),err);
        }
    return err;
    }

void* CTestMutex::ThreadEntryFunctionSemaphoreL(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = sem_wait (&(self->iSemId));
    if ( err )
        {
        self->ERR_PRINTF2( _L("sem_wait returned %d"), err);
        }
        
    User::After(10000000);
    err = sem_post (&(self->iSemId));
    if ( err )
        {
        self->ERR_PRINTF2( _L("sem_post returned %d"), err);
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemTimedWait
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemTimedWait(  )
    {
    _LIT(KFunc, "In SemTimedWait");
    struct timespec abstime;
	struct timeval  now;
	gettimeofday( &now, NULL);
    abstime.tv_sec = now.tv_sec + 10;
    abstime.tv_nsec = 0;
    
    INFO_PRINTF1 ( KFunc);
    TInt err = sem_init ( &iSemId, 0, 1);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"),err);
        return err;
        }
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemaphoreL,(void*)this);
    err = sem_timedwait ( &iSemId, &abstime );
    switch ( err )
        {
        case ETIMEDOUT:
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_timedwait (timeout) returned %d"), err);
                }
            err = sem_destroy ( &iSemId);
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
                }
            return err;
        default:
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_timedwait returned %d"), err);
                err = sem_destroy ( &iSemId);
                if ( err )
                    {
                    ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
                    }
                return err;
                }
            break;
        }
    err = sem_post (&iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_post returned %d"),  err);
        }
    err = sem_destroy ( &iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
        }
    return err;
    }

void* CTestMutex::ThreadEntryFunctionSemPostL(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = KErrNone;
    for (TInt i = 0; i < 10 ; i++)
        {
        sem_wait (&(self->iSemId));
        if ( err )
            {
            self->ERR_PRINTF2( _L("sem_wait returned %d"), err);
            }
            
		err = sem_post (&(self->iSemId));
        if ( err )
            {
            self->ERR_PRINTF2( _L("sem_post returned %d"), err);
            }
        }    
    return NULL;
    }
// -----------------------------------------------------------------------------
// CTestMutex::SemPost
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemPost(  )
    {
    _LIT(KFunc, "In SemPost");
    INFO_PRINTF1 ( KFunc);
    TInt err = sem_init ( &iSemId, 0, 1);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"), err);
        return err;
        }
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemPostL,(void*)this);
    User::After ( 50000);
    for (TInt i = 0; i < 10 ; i++)
        {
        err = sem_wait ( &iSemId );
        if ( err )
            {
            ERR_PRINTF2 ( _L("sem_wait returned %d"), err);
            }
        err = sem_post (&iSemId);
        if ( err )
            {
            ERR_PRINTF2 ( _L("sem_post returned %d"), err);
            }
        }
    err = sem_destroy ( &iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
        }
    return err;
   }

// -----------------------------------------------------------------------------
// CTestMutex::SemOpen
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemOpen(  )
    {
    _LIT(KFunc, "In SemOpen");
    INFO_PRINTF1 ( KFunc);
    TInt err = KErrNone;
	#if 0    
		sem_t* semid = sem_open ("sem 1" /*name */, 0 /*oflag*/, 0 /*mode*/, 1 /*value*/);
		if ( semid == NULL)
			{
			ERR_PRINTF2 ( _L("sem_open returned %d"), errno);
			}
		else
			{
			err = sem_close (semid);
			if ( err )
				{
				ERR_PRINTF2 ( _L("sem_close returned %d"), err);
				}
			}

	#endif    
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemClose
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemClose(  )
    {
    _LIT(KFunc, "In SemClose");
    INFO_PRINTF1 ( KFunc);
    TInt err = KErrNone;
	#if 0    
		for (TInt i = 0; i < 100; i++)
			{
			sem_t* semid = sem_open ("sem 1" /*name */, 0 /*oflag*/, 0 /*mode*/, 1 /*value*/);
			if ( semid == NULL)
				{
				ERR_PRINTF2 ( _L("sem_open returned %d"), errno);
				}
			else
				{
				err = sem_close (semid);
				if ( err )
					{
					ERR_PRINTF2 ( _L("sem_close returned %d"), err);
					}
				}
			}
	#endif    
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemUnlink
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemUnlink(  )
    {
    _LIT(KFunc, "In SemUnlink");
    INFO_PRINTF1 ( KFunc);
    TInt err = KErrNone;
	#if 0    
		const char *semName = (const char*) (KSemName().Ptr());
		sem_t* semid = sem_open ( semName/*name */, 0 /*oflag*/, 0 /*mode*/, 1 /*value*/);
		if ( semid == NULL)
			{
			ERR_PRINTF2 ( _L("sem_open returned "), errno);
			}
		else
			{
			err = sem_unlink (semName);
			if ( err )
				{
				ERR_PRINTF2 ( _L("sem_unlink returned %d"), err);
				}
			}
	#endif    
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemGetValue
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemGetValue(  )
    {
    
    _LIT(KFunc, "In SemGetValue");
    INFO_PRINTF1 ( KFunc);
    TInt err = sem_init ( &iSemId, 0, 100);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"), err);
        return err;
        }
    TInt value;
    err = sem_getvalue ( &iSemId, &value);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_getvalue returned %d"), err);
        }
    err = sem_destroy ( &iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemMicroSleepwaitNP
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemMicroSleepwaitNP(  )
    {
    _LIT(KFunc, "In SemMicroSleepwaitNP");
	struct timeval  now;
	gettimeofday( &now, NULL);
    
    INFO_PRINTF1 ( KFunc);
    TInt err = sem_init ( &iSemId, 0, 1);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init returned %d"), err);
        return err;
        }
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemaphoreL,(void*)this);
    err = 0;
    switch ( err )
        {
        case ETIMEDOUT:
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_timedwait (timeout) returned %d"), errno);
                }
            err = sem_destroy ( &iSemId);
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
                }
            return err;
        default:
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_timedwait returned %d"), err);
                err = sem_destroy ( &iSemId);
                if ( err )
                    {
                    ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
                    }
                return err;
                }
            break;
        }
    err = sem_post (&iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_post returned %d"), err);
        }
    err = sem_destroy ( &iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
        }
    return err;
    }


void* CTestMutex::ThreadEntryFunctionSemaphoreMultiplePostEntry1L(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = sem_wait (&(self->iSemId));
    if ( err )
        {
        self->ERR_PRINTF2( _L("sem_wait returned %d"), err);
       	}
    User::After ( 100000);
    return NULL;
    }

void* CTestMutex::ThreadEntryFunctionSemaphoreMultiplePostEntry2L(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = sem_wait (&(self->iSemId));
    if ( err )
        {
        self->ERR_PRINTF2( _L("sem_wait returned %d"), err);
        }
    User::After ( 100000);
    return NULL;
    }
    
void* CTestMutex::ThreadEntryFunctionSemaphoreMultiplePostEntry3L(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = sem_wait (&(self->iSemId));
    if ( err )
        {
        self->ERR_PRINTF2( _L("sem_wait returned %d"), err);
        }
    User::After ( 100000);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CTestMutex::SemPostMultiple
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::SemPostMultiple(  )
    {
    _LIT(KFunc, "In SemPostMultiple");
	struct timeval  now;
	gettimeofday( &now, NULL);
    
    INFO_PRINTF1 ( KFunc);
    TInt err = sem_init ( &iSemId, 0, 1);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_init  returned %d"), err);
        return err;
        }
    err = sem_wait ( &iSemId);
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemaphoreMultiplePostEntry1L,(void*)this);
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemaphoreMultiplePostEntry2L,(void*)this);
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionSemaphoreMultiplePostEntry3L,(void*)this);
    User::After ( 500000);
    err += sem_post(&iSemId);
    err += sem_post(&iSemId);
    err += sem_post(&iSemId);
    
    switch ( err )
        {
        case ETIMEDOUT:
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_timedwait (timeout) returned %d"), errno);
                }
            err = sem_destroy ( &iSemId);
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
                }
            return err;
        default:
            if ( err )
                {
                ERR_PRINTF2 ( _L("sem_timedwait returned %d"), err);
                err = sem_destroy ( &iSemId);
                if ( err )
                    {
                    ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
                    }
                return err;
                }
            break;
        }
    err = sem_post (&iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_post returned %d"), err);
        }
    err = sem_destroy ( &iSemId);
    if ( err )
        {
        ERR_PRINTF2 ( _L("sem_destroy returned %d"), err);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondAttrInit
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondAttrInit(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondAttrDestroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondAttrDestroy(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondInit
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondInit(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondDestroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondDestroy(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondTimedwait(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondWait
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondWait(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondSignal
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondSignal(  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestMutex::PThreadCondBroadcast
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadCondBroadcast(  )
    {
    return KErrNone;
    }


TInt CTestMutex::PThreadMutexLockErrorCheck(  )
    {
    _LIT(KFunc, "In PThreadMutexLockErrorCheck");
    INFO_PRINTF1 ( KFunc);
    pthread_mutexattr_t mutexAttr;
    TInt err = KErrNone;
    err = pthread_mutexattr_init(&mutexAttr);
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_mutexattr_init returned %d"), err);
        return err;
        }
    err = pthread_mutexattr_settype ( &mutexAttr, PTHREAD_MUTEX_ERRORCHECK);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroyc  returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_settype returned %d"), err);
        return err;
        }
        
    err = pthread_mutex_init(&iMutex, &mutexAttr);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_init returned %d"), err);
        return err;
        }

    err = pthread_mutex_lock (&iMutex);
    if ( err)
        {
        PThreadMutexDestroy (WITHOUT_ERROR);
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_lock returned %d"), err);
        return err;
        }
    
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionErrorCheckL,(void*)this);
    if ( err )
        {
        PThreadMutexDestroy (WITHOUT_ERROR);
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_create returned %d"), err);
        return err;
        } 
    User::After(5000000);
    err = pthread_mutex_lock (&iMutex);
    if ( err == EDEADLK)
        {
        ERR_PRINTF2 ( _L("pthread_mutex_lock returned %d"), err);
        }
    err = pthread_mutex_unlock(&iMutex);
    if ( err )
        {
        err = pthread_mutex_destroy (&iMutex);
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_unlock returned %d"), err);
        return err;
        }
    err = pthread_mutex_destroy (&iMutex);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_destroy returned %d"),err);
        return err;
        }
    err = pthread_mutexattr_destroy (&mutexAttr);
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
        return err;
        }
    
    return KErrNone;
    }

void* CTestMutex::ThreadEntryFunctionErrorCheckL(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_unlock (&(self->iMutex));
    if ( err == EINVAL)
        {
        self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
        }
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// CTestMutex::PThreadMutexLockRecursive
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestMutex::PThreadMutexLockRecursive(  )
    {
    _LIT(KFunc, "In PThreadMutexLockRecursive");
    INFO_PRINTF1 ( KFunc);
    pthread_mutexattr_t mutexAttr;
    TInt err = KErrNone;
    err = pthread_mutexattr_init(&mutexAttr);
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_mutexattr_init returned %d"), err);
        return err;
        }
    err = pthread_mutexattr_settype ( &mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_settype returned %d"),  err);
        return err;
        }
        
    err = pthread_mutex_init(&iMutex, &mutexAttr);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_init returned %d"), err);
        return err;
        }

    err = pthread_mutex_lock (&iMutex);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_lock returned %d"),  err);
        return err;
        }
    
    err = pthread_create(&iThreadId,NULL,ThreadEntryFunctionRecursiveL,(void*)this);
    if ( err )
        {
        PThreadMutexDestroy (WITHOUT_ERROR);
        ERR_PRINTF2 ( _L("pthread_create returned %d"),  err);
        return err;
        }
    User::After(5000000);
    err = pthread_mutex_lock (&iMutex);
    if ( err )
        {
        err = pthread_mutex_destroy (&iMutex);
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_lock returned %d"),  err);
        return err;
        }
    err = pthread_mutex_unlock(&iMutex);
    if ( err )
        {
        err = pthread_mutex_destroy (&iMutex);
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_unlock returned %d"),  err);
        return err;
        }
    err = pthread_mutex_unlock(&iMutex);
    if ( err )
        {
        err = pthread_mutex_destroy (&iMutex);
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"), err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_unlock returned %d"),  err);
        return err;
        }
    User::After(5000000);
    err = pthread_mutex_destroy (&iMutex);
    if ( err )
        {
        err = pthread_mutexattr_destroy (&mutexAttr);
        if ( err )
            {
            ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
            return err;
            }
        ERR_PRINTF2 ( _L("pthread_mutex_destroy returned %d"), err);
        return err;
        }
    err = pthread_mutexattr_destroy (&mutexAttr);
    if ( err )
        {
        ERR_PRINTF2 ( _L("pthread_mutexattr_destroy returned %d"),  err);
        return err;
        }
    
    return KErrNone;
    }
 
void* CTestMutex::ThreadEntryFunctionRecursiveL(void* arg)
    {
    CTestMutex *self = static_cast<CTestMutex*> (arg);
    TInt err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->WARN_PRINTF2(_L("pthread_mutex_lock returned %d"), err);
        
        }
    err = pthread_mutex_lock (&(self->iMutex));
    if ( err )
        {
        self->WARN_PRINTF2( _L("pthread_mutex_lock returned %d"), err);
        
        }
    err = pthread_mutex_unlock(&(self->iMutex));
    if ( err )
        {
        self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
        return NULL;
        }
    err = pthread_mutex_unlock(&(self->iMutex));
    if ( err )
        {
        self->ERR_PRINTF2( _L("pthread_mutex_unlock returned %d"), err);
        return NULL;
        }
    return NULL;
    }

TInt CTestMutex::IntgTest1()
	{
	TInt errVal;
    _LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal);
	if(!err)
		{
			
		TInt val;
	    _LIT( Kval, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), Kval, val);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexAttrDestroy(val);
		}
	
	return err;
		
	}

TInt CTestMutex::IntgTest2()
	{
	TInt val=0;
	_LIT( Kval, "Parameter1" );
		
	TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
	if(!res)
		{
		_LIT(Kerr , "No first parameter in ini file") ;
		ERR_PRINTF1(Kerr);
		return KErrGeneral;
		}
	TInt err = PThreadMutexAttrDestroy(val);
	return err;		
	}

TInt CTestMutex::IntgTest3()
	{
	TInt errVal;
    _LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal);
	if(!err)
		{
			
		TInt errVal1;
		_LIT( KerrVal1, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexAttrGetpshared(errVal1);
		if(!err)
			{
				
			TInt val;
		    _LIT( Kval, "Parameter3" );
			
			res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexAttrDestroy(val);
			}
		}
	return err;	
	}

TInt CTestMutex::IntgTest4()
	{
	TInt errVal;
	_LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No third parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrGetpshared(errVal);
	return err;
	}
	
	
TInt CTestMutex::IntgTest5()
	{
	TInt errVal;
    _LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal);
	if(!err)
		{
			
		TInt errVal1;
		_LIT( KerrVal1, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrGettype(errVal1);
		if(!err)
			{
				
			TInt val;
		    _LIT( Kval, "Parameter3" );
			
			res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexAttrDestroy(val);
			}
		}
	return err;	
	}

TInt CTestMutex::IntgTest6()
	{
	TInt errVal;
    _LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}	
		TInt errVal1;
		_LIT( KerrVal1, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		
		err = PThreadMutexattrSettype(mutexType, errVal1);
		if(!err)
			{
				
			TInt val;
		    _LIT( Kval, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexAttrDestroy(val);
			}
		}
	return err;	
	}

TInt CTestMutex::IntgTest7()
	{
	TInt errVal;
	_LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No third parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexattrGettype(errVal);
	return err;
	}
	
TInt CTestMutex::IntgTest8()
	{
	TInt errVal;
	_LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInitInvalidPointerL(errVal);
	return err;
	}

    

TInt CTestMutex::IntgTest9()
	{
	TInt errVal;
    _LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal);
	if(!err)
		{
		TInt processType;
		_LIT( KprocessType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KprocessType, processType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}	
		TInt errVal1;
		_LIT( KerrVal1, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		
		err = PThreadMutexAttrSetpshared(processType, errVal1);
		if(!err)
			{
				
			TInt val;
		    _LIT( Kval, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexAttrDestroy(val);
			}
		}
	return err;	
	}


TInt CTestMutex::IntgTest10()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal;
		_LIT( KerrVal, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
	TInt err = PThreadMutexInit(attrType, errVal);
	if(!err)
		{
		TInt val;
	    _LIT( Kval, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), Kval, val);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexDestroy(val);
		}
	return err;	
	}

TInt CTestMutex::IntgTest11()
	{
	TInt val=0;
	_LIT( Kval, "Parameter1" );
		
	TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexDestroy(val);
	return err;		
	}


TInt CTestMutex::IntgTest12()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal;
	_LIT( KerrVal, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal);
	if(!err)
		{
		
		TInt threadType;
	    _LIT( KthreadType, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = CreateThreadL(threadType);
		if(!err)
			{
			TInt val1;
		    _LIT( Kval1, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), Kval1, val1);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			SetExitValueL(val1);
			
			TInt val;
		    _LIT( Kval, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexDestroy(val);
			}
		}
	return err;	
	}
TInt CTestMutex::IntgTest13()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal;
	_LIT( KerrVal, "Parameter2" );
		
	res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
		_LIT(Kerr , "No second parameter in ini file") ;
		ERR_PRINTF1(Kerr);
		return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal);
	if(!err)
		{
		
		TInt threadType;
	    _LIT( KthreadType, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = CreateThreadL(threadType);
		if(!err)
			{
			
			TInt val;
		    _LIT( Kval, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexDestroy(val);
			if(!err)
				{
				TInt val1;
			    _LIT( Kval1, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), Kval1, val1);
				if(!res)
					{
					_LIT(Kerr , "No third parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = SetExitValueL(val1);
				if(!err)
					{
					TInt errVal1;
				    _LIT( KerrVal1, "Parameter6" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
					if(!res)
						{
					 	_LIT(Kerr , "No third parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexDestroy(errVal1);
					}
				}
			}
		}
	return err;	
	}


TInt CTestMutex::IntgTest14()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexLock(errVal2);
		if(!err)
			{
			TInt threadType;
		    _LIT( KthreadType, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = CreateThreadL(threadType);
			if(!err)
				{
				TInt exitType;
			    _LIT( KexitType, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
				if(!res)
					{
				 	_LIT(Kerr , "No third parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = SetExitValueL(exitType);
				if(!err)
					{
					TInt errVal2;
				    _LIT( KerrVal2, "Parameter6" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
					if(!res)
						{
					 	_LIT(Kerr , "No third parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexDestroy(errVal2);		
					}
				}
			}
		}
		return err;
	}

TInt CTestMutex::IntgTest15()
	{
	TInt errVal;
	_LIT( KerrVal, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt attrType;
	_LIT( KattrType, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}

	TInt err = PThreadMutexInit(attrType,errVal);
	return err;
	}


TInt CTestMutex::IntgTest16()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt attrType;
	    _LIT( KattrType, "Parameter2" );
		
		TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexInit(attrType, errVal2);
		if(!err)					
			{
			TInt errVal3;
		    _LIT( KerrVal3, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}	
			err = PThreadMutexDestroy(errVal3);
			if(!err)
				{
				TInt errVal4;
			    _LIT( KerrVal4, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No fifth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexAttrDestroy(errVal4);									
				}
			}
		}
	return err;
	}

TInt CTestMutex::IntgTest17()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		TInt threadType;
	    _LIT( KthreadType, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = CreateThreadL(threadType);
		if(!err)			
			{
			TInt exitType;
		    _LIT( KexitType, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = SetExitValueL(exitType);							
			}
		}
	return err;
	}


TInt CTestMutex::IntgTest18()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		} 
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		TInt threadType;
	    _LIT( KthreadType, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = CreateThreadL(threadType);
		if(!err)			
			{
			TInt exitType;
		    _LIT( KexitType, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = SetExitValueL(exitType);							
			if(!err)
				{
				TInt errVal2;
			    _LIT( KerrVal2, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
				if(!res)
					{
				 	_LIT(Kerr , "No fourth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}	
				err = PThreadMutexDestroy(errVal2);									
				}
			}
		}
	return err;
	}
		
	
TInt CTestMutex::IntgTest19()
	{	
	TInt threadType;
    _LIT( KthreadType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = CreateThreadL(threadType);	
	if(!err)
		{
		TInt exitType;
	    _LIT( KexitType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = SetExitValueL(exitType);				
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter1" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No first parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal1;
			_LIT( KerrVal1, "Parameter3" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
			if(!res)
				{
			 	_LIT(Kerr , "No third parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal1);
			if(!err)							
				{
				TInt errVal2;
			    _LIT( KerrVal2, "Parameter4" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
				if(!res)
					{
				 	_LIT(Kerr , "No fourth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}	
				err = PThreadMutexDestroy(errVal2);									
				}
			}
		}
	return err;
	}

TInt CTestMutex::IntgTest20()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt threadType;
				    _LIT( KthreadType, "Parameter7" );
					
					TBool res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = CreateThreadL(threadType);	
					if(!err)											
						{
						TInt errVal5;
						_LIT( KerrVal5, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
						if(!res)
							{
						 	_LIT(Kerr , "No eightth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal5);
						if(!err)						
							{
							TInt exitType;
						    _LIT( KexitType, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = SetExitValueL(exitType);				
							if(!err)					
								{
								TInt errVal6;
							    _LIT( KerrVal6, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}	
								err = PThreadMutexDestroy(errVal6);
								if(!err)					
									{
									TInt errVal7;
								    _LIT( KerrVal7, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}
									err = PThreadMutexAttrDestroy(errVal7);								
									}
								}									
							}
						}
					}
				}
			return err;
			}
		}
	return err;
	}

TInt CTestMutex::IntgTest21()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexLock(errVal5);
					if(!err)
						{
						TInt errVal6;
						_LIT( KerrVal6, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No eightth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal6);
						if(!err)
							{
							TInt errVal7;
						    _LIT( KerrVal7, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
							if(!res)
								{
							 	_LIT(Kerr , "No tenth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}	
							err = PThreadMutexDestroy(errVal7);
							if(!err)
								{
								TInt errVal8;
							    _LIT( KerrVal8, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
								if(!res)
									{
								 	_LIT(Kerr , "No eleventh parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}
								err = PThreadMutexAttrDestroy(errVal8);
								}
							}
						}
					}
				}
			}
		}
	return err;
	}
		

TInt CTestMutex::IntgTest22()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt threadType;
				    _LIT( KthreadType, "Parameter7" );
					
					TBool res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = CreateThreadL(threadType);	
					if(!err)
						{
						TInt errVal5;
						_LIT( KerrVal5, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
						if(!res)
							{
						 	_LIT(Kerr , "No eighth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexLock(errVal5);	
					
						if(!err)											
							{
							TInt errVal6;
							_LIT( KerrVal6, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = PThreadMutexUnlock(errVal6);
							if(!err)						
								{
								TInt exitType;
							    _LIT( KexitType, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}
								err = SetExitValueL(exitType);				
								if(!err)					
									{
									TInt errVal7;
								    _LIT( KerrVal7, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}	
									err = PThreadMutexDestroy(errVal7);
									if(!err)					
										{
										TInt errVal8;
									    _LIT( KerrVal8, "Parameter12" );
										
										res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
										if(!res)
											{
										 	_LIT(Kerr , "No twelfth parameter in ini file") ;
										 	ERR_PRINTF1(Kerr);
										 	return KErrGeneral;
											}
										err = PThreadMutexAttrDestroy(errVal8);								
										
										}
									}
								}
							}
						}
					}
				}
			}
		}
	return err;
	}


TInt CTestMutex::IntgTest23()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexLock(errVal5);
					if(!err)
						{
						TInt errVal6;
						_LIT( KerrVal6, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No eighth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal6);
						if(!err)
							{
							TInt errVal7;
							_LIT( KerrVal7, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = PThreadMutexUnlock(errVal7);
							if(!err)
							{
								TInt errVal8;
							    _LIT( KerrVal8, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}	
								err = PThreadMutexDestroy(errVal8);
								if(!err)
									{
									TInt errVal9;
								    _LIT( KerrVal9, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KerrVal9, errVal9);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}
									err = PThreadMutexAttrDestroy(errVal9);
									}
									
								}
							}
						}
					}
				}
			}
		}
	return err;
	}
		

TInt CTestMutex::IntgTest24()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexUnlock(errVal5);
					if(!err)
						{
						TInt errVal6;
						_LIT( KerrVal6, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No eighth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal6);
						if(!err)
							{
							TInt errVal7;
							_LIT( KerrVal7, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = PThreadMutexUnlock(errVal7);
							if(!err)
								{
								TInt errVal8;
							    _LIT( KerrVal8, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}	
								err = PThreadMutexDestroy(errVal8);
								if(!err)
									{
									TInt errVal9;
								    _LIT( KerrVal9, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KerrVal9, errVal9);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}
									err = PThreadMutexAttrDestroy(errVal9);
									}
									
								}
							}
						}
					}
				}
			}
		}
		return err;
	}
	
TInt CTestMutex::IntgTest25()
	{
	TInt val=0;
	_LIT( Kval, "Parameter1" );
		
	TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexLock(val);
	return err;		
	}

TInt CTestMutex::IntgTest26()
	{
	
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		
		TInt threadType;
	    _LIT( KthreadType, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = CreateThreadL(threadType);
		if(!err)
			{
			TInt val=0;
    	    _LIT( Kval, "Parameter4" );
			TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "Failed to read fourth parameter value from ini file") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
			err = Delay(val);
			if(!err)
			 	{
				TInt val=0;
				_LIT( Kval, "Parameter5" );
				TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
				if(!res)
					{
				 	_LIT(Kerr , "Failed to read fifth parameter value from ini file") ;
				 	INFO_PRINTF1(Kerr) ;
				 	return KErrGeneral ;
				 	}
				TInt typeTimeLockCheck=0;
				_LIT( KtypeTimeLockCheck, "Parameter6" );
				res = GetIntFromConfig(ConfigSection(), KtypeTimeLockCheck, typeTimeLockCheck);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter value from ini file") ;
				 	INFO_PRINTF1(Kerr) ;
				 	typeTimeLockCheck = 0 ;
				 	}
				 err = PThreadMutexTimedlock(val,typeTimeLockCheck);
				 if(!err)
					{
					TInt errVal2;
					_LIT( KerrVal2, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexUnlock(errVal2);
					if(!err)						
						{
						TInt exitType;
					    _LIT( KexitType, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
						if(!res)
							{
						 	_LIT(Kerr , "No eighth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = SetExitValueL(exitType);				
						if(!err)					
							{
							TInt errVal3;
						    _LIT( KerrVal3, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}	
							err = PThreadMutexDestroy(errVal3);	
							}
						}
					}
			 	}
			}
		}
	return err;
	}

TInt CTestMutex::IntgTest27()
	{
	TInt val=0;
	_LIT( Kval, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt typeTimeLockCheck=0;
	_LIT( KtypeTimeLockCheck, "Parameter2" );
		
	res = GetIntFromConfig(ConfigSection(), KtypeTimeLockCheck, typeTimeLockCheck);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	typeTimeLockCheck = 0;
		}
	TInt err = PThreadAttrStaticInitTimedLockL(val,typeTimeLockCheck);
	return err;		
	}

TInt  CTestMutex::IntgTest28()
	{
	TInt val=0;
	_LIT( Kval, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read fifth parameter value from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
	 	}
	TInt typeTimeLockCheck=0;
	_LIT( KtypeTimeLockCheck, "Parameter2" );
	res = GetIntFromConfig(ConfigSection(), KtypeTimeLockCheck, typeTimeLockCheck);
	if(!res)
		{
	 	_LIT(Kerr , "No sixth parameter value from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	typeTimeLockCheck = 0 ;
	 	}
	 TInt err = PThreadMutexTimedlock(val,typeTimeLockCheck);			
	 return err;
	}


TInt CTestMutex::IntgTest29()
	{
	
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		
		TInt threadType;
	    _LIT( KthreadType, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = CreateThreadL(threadType);
		if(!err)
			{
			TInt val=0;
    	    _LIT( Kval, "Parameter4" );
			TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
			if(!res)
				{
			 	_LIT(Kerr , "Failed to read fourth parameter value from ini file") ;
			 	INFO_PRINTF1(Kerr) ;
			 	return KErrGeneral ;
				}
			err = Delay(val);
			if(!err)
			 	{
				TInt val=0;
				_LIT( Kval, "Parameter5" );
				TBool res = GetIntFromConfig(ConfigSection(), Kval, val);
				if(!res)
					{
				 	_LIT(Kerr , "Failed to read fifth parameter value from ini file") ;
				 	INFO_PRINTF1(Kerr) ;
				 	return KErrGeneral ;
				 	}
				TInt typeTimeLockCheck=0;
				_LIT( KtypeTimeLockCheck, "Parameter6" );
				res = GetIntFromConfig(ConfigSection(), KtypeTimeLockCheck, typeTimeLockCheck);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter value from ini file") ;
				 	INFO_PRINTF1(Kerr) ;
				 	typeTimeLockCheck = 0 ;
				 	}
				 err = PThreadMutexTimedlock(val,typeTimeLockCheck);
				 if(!err)
					{
					TInt exitType;
				    _LIT( KexitType, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
					if(!res)
						{
					 	_LIT(Kerr , "No eighth parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = SetExitValueL(exitType);
					if(!err)						
						{
						TInt errVal2;
						_LIT( KerrVal2, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
						if(!res)
							{
						 	_LIT(Kerr , "No seventh parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal2);
						if(!err)					
							{
							TInt errVal3;
						    _LIT( KerrVal3, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}	
							err = PThreadMutexDestroy(errVal3);	
							}
						}
					}
			 	}
			}
		}
	return err;
	}


TInt CTestMutex::IntgTest30()
	{
	
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		TInt val1;
	    _LIT( Kval1, "Parameter3" );
		
		TBool res = GetIntFromConfig(ConfigSection(), Kval1, val1);
		if(!res)
			{
			val1=0;
			}
		err = PThreadMutexTrylock(val1);
		if(!err)
			{
			TInt val2;
		    _LIT( Kval2, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), Kval2, val2);
			if(!res)
				{
			 	val2=0;
				}
			err = PThreadMutexTrylock(val2);				
			if(!err)
				{
				TInt errVal2;
				_LIT( KerrVal2, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
				if(!res)
					{
				 	_LIT(Kerr , "No fifth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexUnlock(errVal2);
				if(!err)					
					{
					TInt errVal3;
				    _LIT( KerrVal3, "Parameter6" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
					if(!res)
						{
					 	_LIT(Kerr , "No sixth parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}	
					err = PThreadMutexDestroy(errVal3);						
					}
				}
			}
		}
	return err;		
	}



TInt CTestMutex::IntgTest31()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	errVal5 =0;
						}
					err = PThreadMutexTrylock(errVal5);
					if(!err)
						{
						TInt errVal6;
						_LIT( KerrVal6, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No eightth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal6);
						if(!err)
							{
							TInt errVal7;
						    _LIT( KerrVal7, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}	
							err = PThreadMutexDestroy(errVal7);
							if(!err)
								{
								TInt errVal8;
							    _LIT( KerrVal8, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}
								err = PThreadMutexAttrDestroy(errVal8);
								}
							}
						}
					}
				}
			}
		}
	return err;
	}

TInt CTestMutex::IntgTest32()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	errVal5 = 0;
						}
					err = PThreadMutexTrylock(errVal5);
					if(!err)
						{
						TInt errVal6;
						_LIT( KerrVal6, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No eightth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal6);
						if(!err)
							{
							TInt errVal7;
							_LIT( KerrVal7, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = PThreadMutexUnlock(errVal7);
							if(!err)
							{
															
								TInt errVal8;
							    _LIT( KerrVal8, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}	
								err = PThreadMutexDestroy(errVal8);
								if(!err)
									{
									TInt errVal9;
								    _LIT( KerrVal9, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KerrVal9, errVal9);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}
									err = PThreadMutexAttrDestroy(errVal9);
									}
								}
							}
						}
					}
				}
			}
		}
	return err;
	}
	

TInt  CTestMutex::IntgTest33()
	{
	TInt errVal=0;
	_LIT( KerrVal, "Parameter1" );
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal, errVal);
	if(!res)
		{
	 	_LIT(Kerr , "Failed to read fifth parameter value from ini file") ;
	 	INFO_PRINTF1(Kerr) ;
	 	return KErrGeneral ;
	 	}
	
	TInt err = PThreadMutexTrylock(errVal);			
	return err;
	}

TInt CTestMutex::IntgTest34()
	{
	TInt attrType;
	_LIT( KattrType, "Parameter1" );

	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );

	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexLock(errVal2);
		if(!err)				
			{
			TInt errVal3;
			_LIT( KerrVal3, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexUnlock(errVal3);
			
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No fifth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	errVal4 = 0;
					}
				err = PThreadMutexTrylock(errVal4);
				if(!err)
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter6" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No sixth parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexUnlock(errVal5);
					if(!err)
						{
														
						TInt errVal6;
					    _LIT( KerrVal6, "Parameter7" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No seventh parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}	
						err = PThreadMutexDestroy(errVal6);
						}
					}
				}
			}
		}
	return err;
	}

TInt CTestMutex::IntgTest35()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt errVal5;
					_LIT( KerrVal5, "Parameter7" );
					
					res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = PThreadMutexUnlock(errVal5);
					if(!err)
						{
						TInt errVal6;
						_LIT( KerrVal6, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
						if(!res)
							{
						 	_LIT(Kerr , "No eightth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = PThreadMutexUnlock(errVal6);
						if(!err)
							{
							TInt errVal7;
						    _LIT( KerrVal7, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
							if(!res)
								{
							 	_LIT(Kerr , "No tenth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}	
							err = PThreadMutexDestroy(errVal7);
							if(!err)
								{
								TInt errVal8;
							    _LIT( KerrVal8, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal8, errVal8);
								if(!res)
									{
								 	_LIT(Kerr , "No eleventh parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}
								err = PThreadMutexAttrDestroy(errVal8);
								}
							}
						}
					}
				}
			}
		}
	return err;
	}
		
TInt CTestMutex::IntgTest36()
	{
	TInt val1;
    _LIT( Kval1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), Kval1, val1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
    _LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadAttrStaticInitUnlockL(val1, errVal1);
	if(!err)
		{
		TInt val2;
	    _LIT( Kval2, "Parameter3" );
		
		TBool res = GetIntFromConfig(ConfigSection(), Kval2, val2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
	    _LIT( KerrVal2, "Parameter4" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No fourth parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadAttrStaticInitUnlockL(val2, errVal2);	
		if(!err)
			{
			TInt val3;
		    _LIT( Kval3, "Parameter5" );
			
			TBool res = GetIntFromConfig(ConfigSection(), Kval3, val3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
		    _LIT( KerrVal3, "Parameter6" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No sixth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadAttrStaticInitUnlockL(val3, errVal3);	
			
			}
		}
	return err;
	}
	


TInt CTestMutex::IntgTest37()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt threadType;
				    _LIT( KthreadType, "Parameter7" );
					
					TBool res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = CreateThreadL(threadType);	
					if(!err)											
						{
						TInt exitType;
					    _LIT( KexitType, "Parameter8" );
						
						res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
						if(!res)
							{
						 	_LIT(Kerr , "No eighth parameter in ini file") ;
						 	ERR_PRINTF1(Kerr);
						 	return KErrGeneral;
							}
						err = SetExitValueL(exitType);
						
						if(!err)						
							{
							TInt errVal5;
							_LIT( KerrVal5, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = PThreadMutexUnlock(errVal5);				
							if(!err)					
								{
								TInt errVal6;
							    _LIT( KerrVal6, "Parameter10" );
								
								res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
								if(!res)
									{
								 	_LIT(Kerr , "No tenth parameter in ini file") ;
								 	ERR_PRINTF1(Kerr);
								 	return KErrGeneral;
									}	
								err = PThreadMutexDestroy(errVal6);
								if(!err)					
									{
									TInt errVal7;
								    _LIT( KerrVal7, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}
									err = PThreadMutexAttrDestroy(errVal7);								
									}
								}									
							}
						}
					}
				}
			return err;
			}
		}
	return err;
	}
	
TInt CTestMutex::IntgTest38()
	{
	TInt attrType;
    _LIT( KattrType, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt errVal1;
	_LIT( KerrVal1, "Parameter2" );
	
	res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No second parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexInit(attrType, errVal1);
	if(!err)
		{
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexLock(errVal2);
		if(!err)				
			{
			TInt errVal3;
			_LIT( KerrVal3, "Parameter4" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexUnlock(errVal3);
			if(!err)
				{
				TInt errVal4;
			    _LIT( KerrVal4, "Parameter5" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No fifth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}	
				err = PThreadMutexDestroy(errVal4);
				}
			}
		}
	return err;
	}
		
							
TInt CTestMutex::IntgTest39()
	{
	TInt errVal1;
    _LIT( KerrVal1, "Parameter1" );
	
	TBool res = GetIntFromConfig(ConfigSection(), KerrVal1, errVal1);
	if(!res)
		{
	 	_LIT(Kerr , "No first parameter in ini file") ;
	 	ERR_PRINTF1(Kerr);
	 	return KErrGeneral;
		}
	TInt err = PThreadMutexAttrInit(errVal1);
	if(!err)
		{
		TInt mutexType;
		_LIT( KmutexType, "Parameter2" );
		
		res = GetIntFromConfig(ConfigSection(), KmutexType, mutexType);
		if(!res)
			{
		 	_LIT(Kerr , "No second parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		TInt errVal2;
		_LIT( KerrVal2, "Parameter3" );
		
		res = GetIntFromConfig(ConfigSection(), KerrVal2, errVal2);
		if(!res)
			{
		 	_LIT(Kerr , "No third parameter in ini file") ;
		 	ERR_PRINTF1(Kerr);
		 	return KErrGeneral;
			}
		err = PThreadMutexattrSettype(mutexType, errVal2);
		if(!err)
			{
			TInt attrType;
		    _LIT( KattrType, "Parameter4" );
			
			TBool res = GetIntFromConfig(ConfigSection(), KattrType, attrType);
			if(!res)
				{
			 	_LIT(Kerr , "No fourth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			TInt errVal3;
			_LIT( KerrVal3, "Parameter5" );
			
			res = GetIntFromConfig(ConfigSection(), KerrVal3, errVal3);
			if(!res)
				{
			 	_LIT(Kerr , "No fifth parameter in ini file") ;
			 	ERR_PRINTF1(Kerr);
			 	return KErrGeneral;
				}
			err = PThreadMutexInit(attrType, errVal3);
			if(!err)
				{
				TInt errVal4;
				_LIT( KerrVal4, "Parameter6" );
				
				res = GetIntFromConfig(ConfigSection(), KerrVal4, errVal4);
				if(!res)
					{
				 	_LIT(Kerr , "No sixth parameter in ini file") ;
				 	ERR_PRINTF1(Kerr);
				 	return KErrGeneral;
					}
				err = PThreadMutexLock(errVal4);
				if(!err)				
					{
					TInt threadType;
				    _LIT( KthreadType, "Parameter7" );
					
					TBool res = GetIntFromConfig(ConfigSection(), KthreadType, threadType);
					if(!res)
						{
					 	_LIT(Kerr , "No seventh parameter in ini file") ;
					 	ERR_PRINTF1(Kerr);
					 	return KErrGeneral;
						}
					err = CreateThreadL(threadType);	
					if(!err)											
						{
						TInt val1=0;
			    	    _LIT( Kval1, "Parameter8" );
						TBool res = GetIntFromConfig(ConfigSection(), Kval1, val1);
						if(!res)
						{
						 	_LIT(Kerr , "Failed to read eighth parameter value from ini file") ;
						 	INFO_PRINTF1(Kerr) ;
						 	return KErrGeneral ;
						 }
						 err = Delay(val1);
						if(!err)
							{
							TInt errVal5;
							_LIT( KerrVal5, "Parameter9" );
							
							res = GetIntFromConfig(ConfigSection(), KerrVal5, errVal5);
							if(!res)
								{
							 	_LIT(Kerr , "No ninth parameter in ini file") ;
							 	ERR_PRINTF1(Kerr);
							 	return KErrGeneral;
								}
							err = PThreadMutexUnlock(errVal5);
							if(!err)						
								{
								TInt val2=0;
					    	    _LIT( Kval2, "Parameter10" );
								TBool res = GetIntFromConfig(ConfigSection(), Kval2, val2);
								if(!res)
								{
								 	_LIT(Kerr , "Failed to read tenth parameter value from ini file") ;
								 	INFO_PRINTF1(Kerr) ;
								 	return KErrGeneral ;
								 }
								 err = Delay(val1);
								 if(!err)
									 {
									 TInt exitType;
								    _LIT( KexitType, "Parameter11" );
									
									res = GetIntFromConfig(ConfigSection(), KexitType, exitType);
									if(!res)
										{
									 	_LIT(Kerr , "No eleventh parameter in ini file") ;
									 	ERR_PRINTF1(Kerr);
									 	return KErrGeneral;
										}
									err = SetExitValueL(exitType);				
									if(!err)					
										{
										TInt val3=0;
							    	    _LIT( Kval3, "Parameter12" );
										TBool res = GetIntFromConfig(ConfigSection(), Kval3, val3);
										if(!res)
										{
										 	_LIT(Kerr , "Failed to read twelfth parameter value from ini file") ;
										 	INFO_PRINTF1(Kerr) ;
										 	return KErrGeneral ;
										 }
										 err = WaitForThreadCompleteL(val3);
										TInt errVal6;
									    _LIT( KerrVal6, "Parameter13" );
										
										res = GetIntFromConfig(ConfigSection(), KerrVal6, errVal6);
										if(!res)
											{
										 	_LIT(Kerr , "No thirteenth parameter in ini file") ;
										 	ERR_PRINTF1(Kerr);
										 	return KErrGeneral;
											}	
										err = PThreadMutexDestroy(errVal6);
										if(!err)					
											{
											TInt errVal7;
										    _LIT( KerrVal7, "Parameter14" );
											
											res = GetIntFromConfig(ConfigSection(), KerrVal7, errVal7);
											if(!res)
												{
											 	_LIT(Kerr , "No fourteenth parameter in ini file") ;
											 	ERR_PRINTF1(Kerr);
											 	return KErrGeneral;
												}
											err = PThreadMutexAttrDestroy(errVal7);								
											}
										}
									 }
								}
							}
						}
					}
				}
			}
		}
		return err;
	}

void* thread_entry_func(void * /*arg*/)
{
    int ret = pthread_mutex_lock(&myMutex1);
    User::After(TTimeIntervalMicroSeconds32(2000000));
    ret = pthread_mutex_unlock(&myMutex1);
    ret = pthread_mutex_trylock(&myMutex1); // waits here forever if deadlock occurs
    if (ret == EBUSY) 
    {
   	 // Process continue processing the part of the problem that we can without the lock. We do not want to waste time blocking. 
     printf("Lock is acquired by the other thread , so the message displayed is EBUSY\n");
    }
   printf("Out of trylock\n");
   return 0;
}

 LOCAL_C void MainL1()
{
    printf("Starting mutex test\n");
    int ret = pthread_mutex_init(&myMutex1, NULL);
    if ( !ret )
    	printf("Unexpected failure at line %d\n", __LINE__);
    ret = pthread_mutex_init(&myMutex1, NULL);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    ret = pthread_create(&myThread, NULL, &thread_entry_func, NULL);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    User::After(TTimeIntervalMicroSeconds32(1000000));
    ret = pthread_mutex_lock(&myMutex1);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    printf("Got Mutex\n");
    pthread_join(myThread, 0);
}

 // Checking pthread_mutex_trylock() for waiting indefinitely leading to deadlock
 TInt CTestMutex::PThreadMutexTryLockWait1(void)

{
    INFO_PRINTF1(_L("Hello Open C!\n")) ;
    MainL1();
    INFO_PRINTF1(_L("Out of Trylock!!\n")) ;
    return 0;
}
 
 
 void* thread_entry_func3(void* /*arg*/)
 {
    int ret = pthread_mutex_lock(&myMutex2); 
    // This lock will get semaphore but might wait for mutex in case of deadlock situation
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    printf("out of Lock within third thread \n") ;
    User::After(TTimeIntervalMicroSeconds32(2000000));
    return 0;
 }

 void* thread_entry_func2(void* /*arg*/)
 {
    int ret = pthread_mutex_lock(&myMutex2);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    printf("out of Lock within second thread \n");
    User::After(TTimeIntervalMicroSeconds32(3000000));
    
    ret = pthread_mutex_unlock(&myMutex2);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    
    ret = pthread_mutex_trylock(&myMutex2); // This trylock will get mutex but might wait for semaphore in case of deadlock situation 
    if (ret == EBUSY) 
        {
          // Process continue processing the part of the problem that we can without the lock. We do not want to waste time blocking. 
         printf("Lock is acquired by the other thread , so the message displayed is EBUSY\n");
        }
    printf("Out of second trylock\n");
    return 0;
 }

 void* thread_entry_func1(void* /*arg*/)
 {
    int ret = pthread_mutex_lock(&myMutex2);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    User::After(TTimeIntervalMicroSeconds32(2000000));
    
    ret = pthread_mutex_trylock(&myMutex2);
    if (ret == EBUSY) 
        {
       	 // Process continue processing the part of the problem that we can without the lock. We do not want to waste time blocking. 
          printf("Lock is acquired by the other thread , so the message displayed is EBUSY\n") ;
        }
    printf("Out of first trylock\n") ;
    User::After(TTimeIntervalMicroSeconds32(1000000));
    
    ret = pthread_mutex_unlock(&myMutex2);
    if ( !ret )
        	printf("Unexpected failure at line %d\n", __LINE__);
    User::After(TTimeIntervalMicroSeconds32(1000000));
    return 0;
 }

 LOCAL_C void MainL2()
 {
     printf("Starting mutex test\n") ;
     int ret =  pthread_mutex_init(&myMutex2, NULL);
     if ( !ret )
         	printf("Unexpected failure at line %d\n", __LINE__);
     ret = pthread_create(&myThread1, NULL, &thread_entry_func1, NULL);
     if ( !ret )
         	printf("Unexpected failure at line %d\n", __LINE__);
     User::After(TTimeIntervalMicroSeconds32(1000000));
     
     ret = pthread_create(&myThread2, NULL, &thread_entry_func2, NULL);
     if ( !ret )
         	printf("Unexpected failure at line %d\n", __LINE__);
     User::After(TTimeIntervalMicroSeconds32(1500000));
     
     ret = pthread_create(&myThread3, NULL, &thread_entry_func3, NULL);
     if ( !ret )
         	printf("Unexpected failure at line %d\n", __LINE__);
     User::After(TTimeIntervalMicroSeconds32(2000000));
     //INFO_PRINTF1(_L("Got Mutex\n")) ;
     pthread_join(myThread3, 0);
     pthread_join(myThread2, 0);
     pthread_join(myThread1, 0);
 }

 // Checking pthread_mutex_trylock() for waiting indefinitely leading to deadlock
 TInt CTestMutex::PThreadMutexTryLockWait2(void)
 {
     INFO_PRINTF1(_L("Hello Open C!\n")) ;
     MainL2();
     INFO_PRINTF1(_L("Out of both the Trylocks!!\n")) ;
     return 0;
 }





// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
