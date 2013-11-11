/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:test cases for concurrency test for timer_settime ,timer_gettime 
* Name        : ttimerthread.cpp
* 
*
*/

#include "ttimerthread.h"
#include <stdapis/errno.h>

CTimerTestThread* CTimerTestThread::NewL(TTimerTestThreadParams& aParams)
    {
    CTimerTestThread* self = new (ELeave) CTimerTestThread(aParams); 
     return self;
    }
CTimerTestThread::CTimerTestThread(TTimerTestThreadParams& aParams)
    :iParentStep(aParams.iTestStep),iParams(aParams)
    {
    }

CTimerTestThread::~CTimerTestThread()
    {
    }

TInt CTimerTestThread::OEEntry(TAny* aData)
    {
    TTimerTestThreadParams* params = static_cast<TTimerTestThreadParams*>(aData);
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if (!cleanup)
        {
        return KErrNoMemory;
        }

    TRAPD(err, OEMainL(*params));
    delete cleanup;
    return err;
    }
void CTimerTestThread::OEMainL(TTimerTestThreadParams& aParams)
    {

    CTimerTestThread* timerThread = CTimerTestThread::NewL(aParams);
    timerThread->doOperation();
    delete timerThread;
    }
void CTimerTestThread::doOperation()
    {   
    timer_t timerid;
    struct itimerspec timeSpec1;
    struct itimerspec timeSpec2;

    iParams.iTestResult=EFalse;

    int ret = timer_create(CLOCK_REALTIME, NULL, &timerid);
    if(ret != -1)
    {
        iParams.iTestStep.INFO_PRINTF1(_L("Timer Created"));
    
        timeSpec1.it_value.tv_sec = 10;
        /* 500 million nsecs = .5 secs */
        timeSpec1.it_value.tv_nsec = 500000000;

        timeSpec1.it_interval.tv_sec = 10;
        /* 500 million nsecs = .5 secs */
        timeSpec1.it_interval.tv_nsec = 500000000;

        ret = timer_settime(timerid, 0, &timeSpec1, NULL);
        if (ret == -1)
        {

            iParams.iTestStep.ERR_PRINTF2(_L("Timer Set Time Failed - %d"), errno);
            return;
        }
        iParams.iTestStep.INFO_PRINTF1(_L("Set Timer Success"));

        //    get_timer()
        ret = timer_gettime(timerid, &timeSpec2);

        if(ret == -1)
        {
            iParams.iTestStep.ERR_PRINTF2(_L("Timer Get Time Failed - %d"), errno);
            return;

        }
        iParams.iTestStep.INFO_PRINTF1(_L("Get Timer Success"));
        
        compareTimeSpec(timeSpec1, timeSpec2);

        ret = timer_delete(timerid);

        if(ret == -1)
        {
            iParams.iTestStep.ERR_PRINTF2(_L("Timer Get Time Failed - %d"), errno);
            return;

        }
      iParams.iTestStep.INFO_PRINTF1(_L("Timer Deleted"));


    }
    else
    {
        iParams.iTestStep.ERR_PRINTF2(_L("Timer Create Failed - %d"), errno);
        return;

    }
   iParams.iTestResult=ETrue;
    return;

    }

TBool CTimerTestThread::compareTimeSpec(struct itimerspec timeSpec1, struct itimerspec timeSpec2)
    {

    if((timeSpec1.it_value.tv_sec == timeSpec2.it_value.tv_sec )
        && (timeSpec1.it_value.tv_nsec  == timeSpec2.it_value.tv_nsec )
        && (timeSpec1.it_interval.tv_sec == timeSpec2.it_interval.tv_sec )
        && (timeSpec1.it_interval.tv_nsec == timeSpec2.it_interval.tv_nsec ))
        {
            iParams.iTestStep.INFO_PRINTF1(_L("Timer Spec Matched"));
            return ETrue;

        }
    else
        {
            iParams.iTestStep.INFO_PRINTF1(_L("Timer spec Not Matched"));
            return EFalse;
        }


    }
