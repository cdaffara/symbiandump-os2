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



#ifndef __TESTTIME_BLR_H__
#define __TESTTIME_BLR_H__

#include <test/TestExecuteStepBase.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/time.h>
#include <e32math.h>

_LIT(Kasctime_test_all, "asctime_test_all");
_LIT(Kasctime_test_null, "asctime_test_null");
_LIT(Klocaltime_test_all, "localtime_test_all");
_LIT(Kgmtime_test_all, "gmtime_test_all");
_LIT(Kmktime_test_null, "mktime_test_null");
_LIT(Kmktime_test_epoch, "mktime_test_epoch");
_LIT(Kmktime_test_extreme, "mktime_test_extreme");
_LIT(Kctime_test_all, "ctime_test_all");
_LIT(Kdifftime_test_all, "difftime_test_all");
_LIT(Kdifftime_test_extreme, "difftime_test_extreme");
_LIT(Ktzset_test, "tzset_test");

_LIT(Kasctime_r_all, "asctime_r_all");
_LIT(Kasctime_r_null, "asctime_r_null");
_LIT(Klocaltime_r_all, "localtime_r_all");
_LIT(Kgmtime_r_all, "gmtime_r_all");
_LIT(Kctime_r_all, "ctime_r_all");

_LIT(Kmultithread_asctime_r, "multithread_asctime_r");
_LIT(Kmultithread_localtime_r, "multithread_localtime_r");
_LIT(Kmultithread_ctime_r, "multithread_ctime_r");
_LIT(Kmultithread_gmtime_r, "multithread_gmtime_r");

_LIT(Kclock_getres_pos_test, "clock_getres_pos_test");
_LIT(Kclock_gettime_pos_test, "clock_gettime_pos_test");
_LIT(Kclock_settime_pos_test, "clock_settime_pos_test");
_LIT(Kclock_getcpuid_pos_test, "clock_getcpuid_pos_test");	    
_LIT(Kclock_getres_neg_test, "clock_getres_neg_test");
_LIT(Kclock_gettime_neg_test, "clock_gettime_neg_test");
_LIT(Kclock_settime_neg_test, "clock_settime_neg_test");
_LIT(Kclock_getcpuid_neg_test, "clock_getcpuid_neg_test");	    
_LIT(Kadjtime_pos_test, "adjtime_pos_test");
_LIT(Kadjtime_neg_test, "adjtime_neg_test");
_LIT(Kgetoffset_test,"getoffset_test");
_LIT(Kgetrule_test,"getrule_test");
_LIT(Ktzset1, "tzset1");
_LIT(Ktimegm_Test,"timegm_Test");

_LIT(Kclock_getres_null_test, "clock_getres_null_test");
_LIT(Kadjtime_null_test, "adjtime_null_test");
_LIT(Kadjtime_range_test, "adjtime_range_test");
_LIT(Kclock_gettime_null_test, "clock_gettime_null_test");
_LIT(Kclock_settime_null_test, "clock_settime_null_test");
_LIT(Kclock_settime_range1_test, "clock_settime_range1_test");
_LIT(Kclock_settime_range2_test, "clock_settime_range2_test");
_LIT(Kmktime_test_extreme1, "mktime_test_extreme1");
_LIT(KtimeTestWithTzset, "timeTestWithTzset");
_LIT(Klocal_and_mktime, "local_and_mktime");

class CTestTimeBlr : public CTestStep
	{
public:
	~CTestTimeBlr(); 
	CTestTimeBlr(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	//local functions
	TBool SameString(char *aString1, char *aString2);
	void StringToDes(char *aString, TDes &aDescriptor);
	void RandomizeString(char *aString, TInt aLength);
	void ReadStringParam(char* expected);
	void ReadIntParam(TInt &aInt);
	TInt getConfigString(const char *aLabel, char *aData);
	TInt getConfigIntParam(const char *aLabel, TInt &aInt);
	TInt timeTestWithTzset();
	//Test methods
	TInt asctime_test_all();
	TInt asctime_test_null();
	TInt localtime_test_all();
    TInt gmtime_test_all();
    TInt mktime_test_null();
    TInt mktime_test_epoch();
    TInt mktime_test_extreme();
    TInt ctime_test_all();
    TInt difftime_test_all();
    TInt difftime_test_extreme();
    TInt tzset_test();
    TInt asctime_r_all();
    TInt asctime_r_null();
    TInt localtime_r_all();
    TInt gmtime_r_all();
    TInt ctime_r_all();
    TInt multithread_asctime_r();
    TInt multithread_localtime_r();
	TInt multithread_ctime_r();        
    TInt multithread_gmtime_r();
    TInt clock_getres_pos_test();
    TInt clock_gettime_pos_test();
    TInt clock_settime_pos_test();
    TInt clock_getcpuid_pos_test();
    TInt clock_getres_neg_test();
    TInt clock_gettime_neg_test();
    TInt clock_settime_neg_test();
    TInt clock_getcpuid_neg_test();
	TInt adjtime_pos_test();
	TInt adjtime_neg_test();
	TInt getoffset_test();
	TInt getrule_test();
	TInt tzset1();
	TInt timegm_Test();
	TInt clock_getres_null_test();
	TInt adjtime_null_test();
	TInt adjtime_range_test();
	TInt clock_gettime_null_test();
	TInt clock_settime_null_test();
	TInt clock_settime_range1_test();
	TInt clock_settime_range2_test();
	TInt mktime_test_extreme1();
	TInt local_and_mktime();

private: //data
	//Input param cnt
	TInt iParamCnt;
    // Randomizer seed
    TInt64 iRandomizerSeed;
	};
#endif 
