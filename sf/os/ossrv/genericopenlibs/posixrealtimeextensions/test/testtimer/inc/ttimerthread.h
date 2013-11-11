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
* Description:
* Name        : ttimerthread.h
* 
*
*/

#ifndef __TTIMERTHREAD__
#define __TTIMERTHREAD__

#include <e32base.h>
#include <time.h>
#include <signal.h>
#include <test/testexecutestepbase.h>

class TTimerTestThreadParams
    {
public:
    TTimerTestThreadParams(CTestStep& aTestStep)
    : iTestStep(aTestStep),
      iTestResult(ETrue)
    {
        
        
    }
    ~TTimerTestThreadParams()
     {
     
     }
    public:
        CTestStep&          iTestStep;
        TBool               iTestResult;
    };
class CTimerTestThread:public CBase
    {
    public:
        
      static CTimerTestThread* NewL(TTimerTestThreadParams& aParam);
      ~CTimerTestThread();
      CTimerTestThread(TTimerTestThreadParams& aParam);
      static TInt OEEntry(TAny* aData);
      static void OEMainL(TTimerTestThreadParams& aParams);
    private:
      void doOperation();
      TBool compareTimeSpec(struct itimerspec timeSpec1, struct itimerspec timeSpec2);
    private:
     CTestStep&                   iParentStep;
     TTimerTestThreadParams&          iParams;
        
    };

#endif //__TTIMERTHREAD__
