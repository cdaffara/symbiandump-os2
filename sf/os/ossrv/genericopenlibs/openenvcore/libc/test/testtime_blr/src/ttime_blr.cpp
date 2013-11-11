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



#include "ttime_blr.h"
#include <stdlib.h>


const TInt KMaxParamLength=100;

//----------------------Local declaration of thread funcs-------------------

void* asctime_thread_1( void* aParam );
void* asctime_thread_2( void* aParam );
void* localtime_thread_1( void* aParam );
void* localtime_thread_2( void* aParam );
void* gmtime_thread_1( void* aParam );
void* gmtime_thread_2( void* aParam );
void* ctime_thread_1( void* aParam );
void* ctime_thread_2( void* aParam );

CTestTimeBlr::~CTestTimeBlr() 
	{ 
	}  

CTestTimeBlr::CTestTimeBlr(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestTimeBlr::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	iRandomizerSeed = 1234567890;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestTimeBlr::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestTimeBlr::doTestStepL()
	{
	int err;

	if(TestStepName() == Kasctime_test_all)
		{
   		INFO_PRINTF1(_L("asctime_test_all():"));
   		err = asctime_test_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kasctime_test_null)
		{
   		INFO_PRINTF1(_L("asctime_test_null():"));
   		err = asctime_test_null();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Klocaltime_test_all)
		{
   		INFO_PRINTF1(_L("localtime_test_all():"));
   		err = localtime_test_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kgmtime_test_all)
		{
   		INFO_PRINTF1(_L("gmtime_test_all():"));
   		err = gmtime_test_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmktime_test_null)
		{
   		INFO_PRINTF1(_L("mktime_test_null():"));
   		err = mktime_test_null();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmktime_test_epoch)
		{
   		INFO_PRINTF1(_L("mktime_test_epoch():"));
   		err = mktime_test_epoch();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmktime_test_extreme)
		{
   		INFO_PRINTF1(_L("mktime_test_extreme():"));
   		err = mktime_test_extreme();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kctime_test_all)
		{
   		INFO_PRINTF1(_L("ctime_test_all():"));
   		err = ctime_test_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kdifftime_test_all)
		{
   		INFO_PRINTF1(_L("difftime_test_all():"));
   		err = difftime_test_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kdifftime_test_extreme)
		{
   		INFO_PRINTF1(_L("difftime_test_extreme():"));
   		err = difftime_test_extreme();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktzset_test)
		{
   		INFO_PRINTF1(_L("tzset_test():"));
   		err = tzset_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kasctime_r_all)
		{
   		INFO_PRINTF1(_L("asctime_r_all():"));
   		err = asctime_r_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kasctime_r_null)
		{
   		INFO_PRINTF1(_L("asctime_r_null():"));
   		err = asctime_r_null();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Klocaltime_r_all)
		{
   		INFO_PRINTF1(_L("localtime_r_all():"));
   		err = localtime_r_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kgmtime_r_all)
		{
   		INFO_PRINTF1(_L("gmtime_r_all():"));
   		err = gmtime_r_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kctime_r_all)
		{
   		INFO_PRINTF1(_L("ctime_r_all():"));
   		err = ctime_r_all();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmultithread_asctime_r)
		{
   		INFO_PRINTF1(_L("multithread_asctime_r():"));
   		err = multithread_asctime_r();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmultithread_localtime_r)
		{
   		INFO_PRINTF1(_L("multithread_localtime_r():"));
   		err = multithread_localtime_r();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmultithread_ctime_r)
		{
   		INFO_PRINTF1(_L("multithread_ctime_r():"));
   		err = multithread_ctime_r();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmultithread_gmtime_r)
		{
   		INFO_PRINTF1(_L("multithread_gmtime_r():"));
   		err = multithread_gmtime_r();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_getres_pos_test)
		{
   		INFO_PRINTF1(_L("clock_getres_pos_test():"));
   		err = clock_getres_pos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_gettime_pos_test)
		{
   		INFO_PRINTF1(_L("clock_gettime_pos_test():"));
   		err = clock_gettime_pos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_settime_pos_test)
		{
   		INFO_PRINTF1(_L("clock_settime_pos_test():"));
   		err = clock_settime_pos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_getcpuid_pos_test)
		{
   		INFO_PRINTF1(_L("clock_getcpuid_pos_test():"));
   		err = clock_getcpuid_pos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_getres_neg_test)
		{
   		INFO_PRINTF1(_L("clock_getres_neg_test():"));
   		err = clock_getres_neg_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_gettime_neg_test)
		{
   		INFO_PRINTF1(_L("clock_gettime_neg_test():"));
   		err = clock_gettime_neg_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_settime_neg_test)
		{
   		INFO_PRINTF1(_L("clock_settime_neg_test():"));
   		err = clock_settime_neg_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_getcpuid_neg_test)
		{
   		INFO_PRINTF1(_L("clock_getcpuid_neg_test():"));
   		err = clock_getcpuid_neg_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kadjtime_pos_test)
		{
   		INFO_PRINTF1(_L("adjtime_pos_test():"));
   		err = adjtime_pos_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kadjtime_neg_test)
		{
   		INFO_PRINTF1(_L("adjtime_neg_test():"));
   		err = adjtime_neg_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == Kgetoffset_test)
		{
   		INFO_PRINTF1(_L("getoffset_test:"));
   		err = getoffset_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   else if(TestStepName() == Kgetrule_test)
		{
   		INFO_PRINTF1(_L("getrule_test:"));
   		err = getrule_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktzset1)
		{
   		INFO_PRINTF1(_L("tzset1():"));
   		err = tzset1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Ktimegm_Test)
   			{
   	   		INFO_PRINTF1(_L("timegm_Test():"));
   	   		err = timegm_Test();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	else if(TestStepName() == Kclock_getres_null_test)
		{
   		INFO_PRINTF1(_L("clock_getres_null_test():"));
   		err = clock_getres_null_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kadjtime_null_test)
		{
   		INFO_PRINTF1(_L("adjtime_null_test():"));
   		err = adjtime_null_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kadjtime_range_test)
		{
   		INFO_PRINTF1(_L("adjtime_range_test():"));
   		err = adjtime_range_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_gettime_null_test)
		{
   		INFO_PRINTF1(_L("clock_gettime_null_test():"));
   		err = clock_gettime_null_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_settime_null_test)
		{
   		INFO_PRINTF1(_L("clock_settime_null_test():"));
   		err = clock_settime_null_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_settime_range1_test)
		{
   		INFO_PRINTF1(_L("clock_settime_range1_test():"));
   		err = clock_settime_range1_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kclock_settime_range2_test)
		{
   		INFO_PRINTF1(_L("clock_settime_range2_test():"));
   		err = clock_settime_range2_test();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == Kmktime_test_extreme1)
		{
   		INFO_PRINTF1(_L("mktime_test_extreme1():"));
   		err = mktime_test_extreme1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Klocal_and_mktime)
		{
   		INFO_PRINTF1(_L("local_and_mktime():"));
   		err = local_and_mktime();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	return TestStepResult(); 

	}
 
// -----------------------------------------------------------------------------
// Utility functions for testing.
// -----------------------------------------------------------------------------

TBool CTestTimeBlr::SameString(char *aString1, char *aString2)
	{
	TInt i;
	
	for ( i=0; ( (aString1[i]!=0) && (aString2[i]!=0) && (aString2[i] == aString1[i]) ); i++)
		{
		} // Go through the strings
	
	if ( aString2[i] != aString1[i] )	// If the scanning ended because of a difference between the strings
		{
		return EFalse;
		}
		
	else	// else
		{
		return ETrue;
		}
	
	}
	
void CTestTimeBlr::StringToDes(char *aString, TDes &aDescriptor)
	{
	aDescriptor.Zero();
	
	for ( TInt i=0; aString[i]!=0; i++ )
		{
		aDescriptor.Append(aString[i]);
		}
		
	}
	
void CTestTimeBlr::RandomizeString(char *aString, TInt aLength)
	{
	TInt16 integer;
	TInt i;
  
	for (i=0; i<aLength; i++)
		{
		Math::Int(integer, Math::FRand(iRandomizerSeed)*255);
		aString[i] = integer;
		}
	aString[i] = 0;
 	}

void CTestTimeBlr::ReadStringParam(char* expected)
	{
	_LIT( KaKey, "Param%d" );
    TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(KaKey,++iParamCnt);
	TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, string);
	if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve string parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	TBuf8<256> bufstring;
	bufstring.Copy(string);
	TInt paramLength=string.Length();
	char* text=(char *)(bufstring.Ptr());
	*(text+paramLength)='\0';
	strcpy(expected,text);
	return;
	}
	
	
void CTestTimeBlr::ReadIntParam(TInt &aInt)
	{
	_LIT( Kparam, "Param%d" );
    TBuf<8> pNameBuf;
    TPtrC string;
    pNameBuf.Format(Kparam,++iParamCnt);
	TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve integer parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	return;
}
// -----------------------------------------------------------------------------
// Test methods
// -----------------------------------------------------------------------------
//                  

TInt CTestTimeBlr::asctime_test_all()
    {
    __UHEAP_MARK;
    TInt ret;
  	// Create temporary variables in stack
  	struct tm *timeptr=(struct tm*)malloc(sizeof(struct tm));
	char expected[KMaxParamLength];
		
  	// Read parameters
         
  	ReadStringParam(expected);
  	strcat(expected,"\n");
    timeptr->tm_sec=0;		
	timeptr->tm_min=0;		
	timeptr->tm_hour=0;	
	timeptr->tm_mday=0;	
	timeptr->tm_mon=0;	
	timeptr->tm_year=0;	
	timeptr->tm_wday=0;	
	timeptr->tm_yday=0;	
	timeptr->tm_isdst=0;
	timeptr->tm_gmtoff=0;
    
    // Do some testing
	char *s=asctime(timeptr);    
	
    // Set test result
    if(SameString(expected,s))
    	{
		INFO_PRINTF1(_L("Test passed."));
       	ret = KErrNone;
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    free(timeptr);
    __UHEAP_MARKEND;    
    return ret;
    }  

TInt CTestTimeBlr::asctime_test_null()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	// Read parameters
    
    // Do some testing
	char *s=asctime(NULL);    
	
    // Set test result
    if(s==NULL)
    	{
		INFO_PRINTF1(_L("Test passed."));
       	ret = KErrNone;
    	}
    else
    	{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
    	}
    
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestTimeBlr::getConfigString(const char *aLabel, char *aData)
    {
    TPtrC8 pLabel((TUint8*)aLabel);
    TBuf<20> pNameBuf;
    pNameBuf.Copy(pLabel);
    TPtrC string;
    TBool res = GetStringFromConfig(ConfigSection(), pNameBuf, string);
    if(!res)
        {
        _LIT(Kerr , "Unable to retrieve string parameter") ;
        INFO_PRINTF1(Kerr);
        return KErrGeneral;
        }
    TBuf8<50> bufstring;
    bufstring.Copy(string);
    char* text=(char *)(bufstring.PtrZ());
    strcpy(aData,text);
    return KErrNone;
   
    
    }

TInt CTestTimeBlr::getConfigIntParam(const char *aLabel, TInt &aInt)
    {
    TPtrC8 pLabel((TUint8*)aLabel);
    TBuf<20> pNameBuf;
    pNameBuf.Copy(pLabel);
    TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
    if(!res)
        {
        _LIT(Kerr , "Unable to retrieve integer parameter") ;
        INFO_PRINTF1(Kerr);
        return KErrGeneral;
        }
    return KErrNone;
}

TInt CTestTimeBlr::timeTestWithTzset()
    {
    __UHEAP_MARK;
    char timeZoneRule[50];
    if(getConfigString("TZ",timeZoneRule) != KErrNone)
        {
        ERR_PRINTF1(_L("cannot read TZ rule from config file\n"));
        return KErrGeneral;
        }

    if(setenv("TZ",timeZoneRule,1))
        {
        ERR_PRINTF1(_L("error setting the TZ env variable to EST\n"));
        return KErrGeneral;
        } 
    tzset();
    time_t time1;
    if(getConfigIntParam("TTIME",time1) != KErrNone)
        {
        ERR_PRINTF1(_L("cannot read time from config file\n"));
        return KErrGeneral;
        }
    
    tm * stm;
    if((stm = localtime(&time1)) == NULL)
        {
        ERR_PRINTF1(_L("error on localtime\n"));
        return KErrGeneral;
        }
    int sec, min, hour, mday, mon, year, wday, yday, isDst, gmtOffset;
    TInt err;
    
    err = getConfigIntParam("SEC",sec);
    err = getConfigIntParam("MIN",min);
    err = getConfigIntParam("HOUR",hour);
    err = getConfigIntParam("DAY",mday);
    err = getConfigIntParam("MONTH",mon);
    err = getConfigIntParam("YEAR",year);
    err = getConfigIntParam("WDAY",wday);
    err = getConfigIntParam("YDAY",yday);
    err = getConfigIntParam("DAYLIGHTSAVING",isDst);
    err = getConfigIntParam("GMTOFFSET",gmtOffset);
    
    if(err != KErrNone)
        {
        ERR_PRINTF1(_L("error getting time parameters from config file\n"));
        return KErrGeneral;
        }
    if(stm->tm_sec != sec || stm->tm_min != min || stm->tm_hour != hour || stm->tm_mday != mday || stm->tm_mon != mon || stm->tm_year != year || stm->tm_wday != wday || stm->tm_yday != yday || stm->tm_isdst != isDst || stm->tm_gmtoff != gmtOffset)
        {
        ERR_PRINTF1(_L("error on localtime\n"));
        return KErrGeneral;
        }
    
    time_t time2;
    
    if((time2 = mktime(stm)) == (time_t)-1)
        {
        ERR_PRINTF1(_L("error on mktime\n"));
        return KErrGeneral;
        }
    
    if(time2 != time1)
        {
        ERR_PRINTF1(_L("error on mktime\n"));
        return KErrGeneral;
        }
    __UHEAP_MARKEND;
    return KErrNone;
  }

TInt CTestTimeBlr::localtime_test_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	struct tm* tptr;
  	TInt ret=KErrGeneral;
  	time_t t;
  	struct tm* testptr=(struct tm*)malloc(sizeof(struct tm));
  	int flag=0;
  	
  	// Read parameters
  	ReadIntParam(t);
  	ReadIntParam(testptr->tm_sec);
  	ReadIntParam(testptr->tm_min);
  	ReadIntParam(testptr->tm_hour);
  	ReadIntParam(testptr->tm_mday);
  	ReadIntParam(testptr->tm_mon);
  	ReadIntParam(testptr->tm_year);
  	ReadIntParam(testptr->tm_wday);
  	ReadIntParam(testptr->tm_yday);
  	ReadIntParam(testptr->tm_isdst);
  	  	
    // Do some testing
	tptr=localtime(&t);
	
    // Set test result
    if(testptr->tm_sec==tptr->tm_sec)
		{
		flag++;
		}
   	if(testptr->tm_min==tptr->tm_min)
		{
		flag++;
		}
    if(testptr->tm_hour==tptr->tm_hour)
		{
		flag++;
		}
    if(testptr->tm_mday==tptr->tm_mday)
		{
		flag++;
		}
    if(testptr->tm_mon==tptr->tm_mon)
		{
		flag++;
		}
    if(testptr->tm_year==tptr->tm_year)
		{
		flag++;
		}
    if(testptr->tm_wday==tptr->tm_wday)
		{
		flag++;
		}
    if(testptr->tm_yday==tptr->tm_yday)
		{
		flag++;
		}
    if(testptr->tm_isdst==tptr->tm_isdst)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    free(testptr);
    __UHEAP_MARKEND;
    return ret;
    }


TInt CTestTimeBlr::gmtime_test_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct tm* tptr;
  	time_t t;
  	struct tm* testptr=(struct tm*)malloc(sizeof(struct tm));
  	int flag=0;
  	
  	// Read parameters
  	ReadIntParam(t);
  	ReadIntParam(testptr->tm_sec);
  	ReadIntParam(testptr->tm_min);
  	ReadIntParam(testptr->tm_hour);
  	ReadIntParam(testptr->tm_mday);
  	ReadIntParam(testptr->tm_mon);
  	ReadIntParam(testptr->tm_year);
  	ReadIntParam(testptr->tm_wday);
  	ReadIntParam(testptr->tm_yday);
  	ReadIntParam(testptr->tm_isdst);
  	  	
    //Do some testing
	tptr=gmtime(&t);
	
    // Set test result
    if(testptr->tm_sec==tptr->tm_sec)
		{
		flag++;
		}
   	if(testptr->tm_min==tptr->tm_min)
		{
		flag++;
		}
    if(testptr->tm_hour==tptr->tm_hour)
		{
		flag++;
		}
    if(testptr->tm_mday==tptr->tm_mday)
		{
		flag++;
		}
    if(testptr->tm_mon==tptr->tm_mon)
		{
		flag++;
		}
    if(testptr->tm_year==tptr->tm_year)
		{
		flag++;
		}
    if(testptr->tm_wday==tptr->tm_wday)
		{
		flag++;
		}
    if(testptr->tm_yday==tptr->tm_yday)
		{
		flag++;
		}
    if(testptr->tm_isdst==tptr->tm_isdst)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    free(testptr);
    __UHEAP_MARKEND;\
    return ret;   
    }
    

TInt CTestTimeBlr::mktime_test_null()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;	
  	// Read parameters
    
    // Do some testing
		time_t t=mktime(NULL);    
	
    // Set test result
    if(t==-1)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    __UHEAP_MARKEND; 
    return ret; 
    }

TInt CTestTimeBlr::mktime_test_epoch()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct tm* tptr;
  	time_t t=0;
  		
  	// Read parameters
    tptr=localtime(&t);
    
    // Do some testing
	t=mktime(tptr);    
	
    // Set test result
    if(t==0)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    __UHEAP_MARKEND;
    return ret;
    }


TInt CTestTimeBlr::mktime_test_extreme()
    {
   	__UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t=0;
  	struct tm* tptr=(struct tm*)malloc(sizeof(struct tm));
    	  	
  	// Read parameters
   	tptr->tm_hour		=	0xffffffff;
	tptr->tm_isdst		=	0xffffffff;
	tptr->tm_mday		=	0xffffffff;
	tptr->tm_min		=	0xffffffff;
	tptr->tm_mon		=	0xffffffff;
	tptr->tm_sec		=	0xffffffff;
	tptr->tm_wday		=	0xffffffff;
	tptr->tm_yday		=	0xffffffff;
	tptr->tm_year		=	0xffffffff;
    tptr->tm_zone       =   NULL;  
    // Do some testing
	t=mktime(tptr);    
	
    // Set test result
    if(t==-1)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    free(tptr);
    __UHEAP_MARKEND;
    return ret;
    }

    
TInt CTestTimeBlr::ctime_test_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t;
  	char expected[KMaxParamLength];
  	
  	// Read parameters
    ReadIntParam(t);
    ReadStringParam(expected);
    strcat(expected,"\n");
    
    // Do some testing
	char* s=ctime(&t);    
	
    // Set test result
    if(SameString(expected,s))
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
  
    __UHEAP_MARKEND; 
    return ret;
    }

TInt CTestTimeBlr::difftime_test_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t1,t2;
  	int expected;
  	int actual;
  	
  	// Read parameters
    ReadIntParam(t1);
    ReadIntParam(t2);
    ReadIntParam(expected);
       
    // Do some testing
		actual=difftime(t1,t2);
	
    // Set test result
    if(actual==expected)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    __UHEAP_MARKEND; 
    return ret; 
    }
    

TInt CTestTimeBlr::difftime_test_extreme()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t=0xffffff;
  	
  	// Read parameters
       
    // Do some testing
	double actual=difftime(t,0);
	
    // Set test result
    if(actual==0xffffff)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    
    __UHEAP_MARKEND;
    return ret;
    }

TInt CTestTimeBlr::tzset_test()
    {
    __UHEAP_MARK;
	tzset();
	INFO_PRINTF1(_L("Test passed."));
	__UHEAP_MARKEND;
    return KErrNone;
    }

TInt CTestTimeBlr::asctime_r_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	struct tm *timeptr=(struct tm*)malloc(sizeof(struct tm));
	char expected[KMaxParamLength];
	char buf[KMaxParamLength];
		
  	// Read parameters
  	ReadStringParam(expected);
  	strcat(expected,"\n");
    timeptr->tm_sec=0;		
	timeptr->tm_min=0;		
	timeptr->tm_hour=0;	
	timeptr->tm_mday=0;	
	timeptr->tm_mon=0;	
	timeptr->tm_year=0;	
	timeptr->tm_wday=0;	
	timeptr->tm_yday=0;	
	timeptr->tm_isdst=0;
	timeptr->tm_gmtoff=0;
	   
    // Do some testing
	char *s=asctime_r(timeptr,buf);    
	
    // Set test result
    if(SameString(expected,s))
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    free(timeptr);
    __UHEAP_MARKEND; 
    return ret;   
    }
    

TInt CTestTimeBlr::asctime_r_null()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  		TInt ret=KErrGeneral;
  	// Read parameters
    
    // Do some testing
		char *s=asctime_r(NULL,NULL);    
	
    // Set test result
    if(s==NULL)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
     
    __UHEAP_MARKEND;
    return ret;
    }    
    
    
TInt CTestTimeBlr::localtime_r_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t;
  	struct tm* testptr=(struct tm*)malloc(sizeof(struct tm));
  	struct tm* result=(struct tm*)malloc(sizeof(struct tm));
  	int flag=0;
  	
  	// Read parameters
  	ReadIntParam(t);
  	ReadIntParam(testptr->tm_sec);
  	ReadIntParam(testptr->tm_min);
  	ReadIntParam(testptr->tm_hour);
  	ReadIntParam(testptr->tm_mday);
  	ReadIntParam(testptr->tm_mon);
  	ReadIntParam(testptr->tm_year);
  	ReadIntParam(testptr->tm_wday);
  	ReadIntParam(testptr->tm_yday);
  	ReadIntParam(testptr->tm_isdst);
  	  	
    // Do some testing
		result=localtime_r(&t,result);
	
    // Set test result
    if(testptr->tm_sec==result->tm_sec)
		{
		flag++;
		}
   	if(testptr->tm_min==result->tm_min)
		{
		flag++;
		}
    if(testptr->tm_hour==result->tm_hour)
		{
		flag++;
		}
    if(testptr->tm_mday==result->tm_mday)
		{
		flag++;
		}
    if(testptr->tm_mon==result->tm_mon)
		{
		flag++;
		}
    if(testptr->tm_year==result->tm_year)
		{
		flag++;
		}
    if(testptr->tm_wday==result->tm_wday)
		{
		flag++;
		}
    if(testptr->tm_yday==result->tm_yday)
		{
		flag++;
		}
    if(testptr->tm_isdst==result->tm_isdst)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    
    free(testptr);
    free(result);
    __UHEAP_MARKEND;
    return ret;
    }


TInt CTestTimeBlr::gmtime_r_all()
    {
     
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t;
  	struct tm* testptr=(struct tm*)malloc(sizeof(struct tm));
  	struct tm* result=(struct tm*)malloc(sizeof(struct tm));
  	int flag=0;
  	
  	// Read parameters
  	ReadIntParam(t);
  	ReadIntParam(testptr->tm_sec);
  	ReadIntParam(testptr->tm_min);
  	ReadIntParam(testptr->tm_hour);
  	ReadIntParam(testptr->tm_mday);
  	ReadIntParam(testptr->tm_mon);
  	ReadIntParam(testptr->tm_year);
  	ReadIntParam(testptr->tm_wday);
  	ReadIntParam(testptr->tm_yday);
  	ReadIntParam(testptr->tm_isdst);
  	  	
    // Do some testing
		result=gmtime_r(&t,result);
	
    // Set test result
    if(testptr->tm_sec==result->tm_sec)
		{
		flag++;
		}
   	if(testptr->tm_min==result->tm_min)
		{
		flag++;
		}
    if(testptr->tm_hour==result->tm_hour)
		{
		flag++;
		}
    if(testptr->tm_mday==result->tm_mday)
		{
		flag++;
		}
    if(testptr->tm_mon==result->tm_mon)
		{
		flag++;
		}
    if(testptr->tm_year==result->tm_year)
		{
		flag++;
		}
    if(testptr->tm_wday==result->tm_wday)
		{
		flag++;
		}
    if(testptr->tm_yday==result->tm_yday)
		{
		flag++;
		}
    if(testptr->tm_isdst==result->tm_isdst)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
	else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    
    free(testptr);
    free(result);
    __UHEAP_MARKEND; 
    return ret;  
    }



TInt CTestTimeBlr::ctime_r_all()
    {
    __UHEAP_MARK;
  	// Create temporary variables in stack
  	TInt ret=KErrGeneral;
  	time_t t;
  	char expected[KMaxParamLength];
  	char result[KMaxParamLength];
  	
  	// Read parameters
    ReadIntParam(t);
    ReadStringParam(expected);
    strcat(expected,"\n");
    
    // Do some testing
	char* s=ctime_r(&t,result);    
	
    // Set test result
    if(SameString(expected,s))
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
    __UHEAP_MARKEND; 
    return ret;
    }


/******************************************************************************/
//--------------------------------Clock test cases-----------------------
/******************************************************************************/

TInt CTestTimeBlr::clock_getres_pos_test()
	{
	__UHEAP_MARK;
	
	struct timespec tp;
	int ret=clock_getres(CLOCK_REALTIME,&tp);
	if(tp.tv_sec==0 && tp.tv_nsec==1000000)	
		{
	    	INFO_PRINTF1(_L("Test passed."));
	    	ret = KErrNone;
	    }
	    else
	    {
	    	ERR_PRINTF1(_L("Test Failed."));
	    	ret = KErrGeneral;
	    }
	__UHEAP_MARKEND;
	return ret;
	}	


TInt CTestTimeBlr::clock_getres_neg_test()
	{
	__UHEAP_MARK;
	struct timespec tp;
	int ret=clock_getres(CLOCK_MONOTONIC,&tp);
	if(ret==-1)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
	__UHEAP_MARKEND;
	return ret;
	}


TInt CTestTimeBlr::clock_gettime_pos_test()
	{
	__UHEAP_MARK;
	struct timespec tp;
	int ret=clock_gettime(CLOCK_REALTIME,&tp);
	if(ret!=-1)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
	__UHEAP_MARKEND;
	return ret;
	}


TInt CTestTimeBlr::clock_gettime_neg_test()
	{
	__UHEAP_MARK;
	struct timespec tp;
	int ret=clock_gettime(CLOCK_MONOTONIC,&tp);
	if(ret==-1)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
	    }
	else
	    {
	    	ERR_PRINTF1(_L("Test Failed."));
	    	ret = KErrGeneral;
	    }
	__UHEAP_MARKEND;
	return ret;
	}


TInt CTestTimeBlr::clock_settime_pos_test()
	{
	__UHEAP_MARK;
	struct timespec* tp=(struct timespec*)malloc(sizeof(struct timespec));
	tp->tv_sec=0;
	tp->tv_nsec=0;
	int ret=clock_settime(CLOCK_REALTIME,tp);
	free(tp);
	if(ret!=-1)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
	    }
	__UHEAP_MARKEND;
	return ret;
	}

TInt CTestTimeBlr::clock_settime_neg_test()
	{
	__UHEAP_MARK;
	struct timespec* tp=(struct timespec*)malloc(sizeof(struct timespec));
	tp->tv_sec=0;
	tp->tv_nsec=0;
	int ret=clock_settime(CLOCK_MONOTONIC,tp);
	free(tp);
	if(ret==-1)
		{
	    	INFO_PRINTF1(_L("Test passed."));
	    	ret = KErrNone;
	    }
	else
	    {
	    	ERR_PRINTF1(_L("Test Failed."));
	    	ret = KErrGeneral;
	    }
	__UHEAP_MARKEND;
	return ret;
	}


TInt CTestTimeBlr::clock_getcpuid_pos_test()
	{
	__UHEAP_MARK;
	clockid_t* clk=(clockid_t*)malloc(sizeof(clockid_t));
	int ret=clock_getcpuclockid(0,clk);
	if(*clk==CLOCK_REALTIME && ret==0)
		{
	    	INFO_PRINTF1(_L("Test passed."));
	    	ret = KErrNone;
	    }
	else
	    {
	    	ERR_PRINTF1(_L("Test Failed."));
	    	ret = KErrGeneral;
	    }
	free(clk);
	__UHEAP_MARKEND;
	return ret;
	}
     

TInt CTestTimeBlr::clock_getcpuid_neg_test()
	{
	__UHEAP_MARK;
	clockid_t* clk=(clockid_t*)malloc(sizeof(clockid_t));
	int ret=clock_getcpuclockid(1,clk);
	free(clk);
	if(ret==-1)
		{
	    	INFO_PRINTF1(_L("Test passed."));
	    	ret = KErrNone;
	    }
	else
	    {
	    	ERR_PRINTF1(_L("Test Failed."));
	    	ret = KErrGeneral;
	    }
	__UHEAP_MARKEND;
	return ret;
	}
   
   
TInt CTestTimeBlr::adjtime_pos_test()
	{
	__UHEAP_MARK;
	struct timeval* tp=(struct timeval*)malloc(sizeof(struct timeval));
	struct timeval* tv=(struct timeval*)malloc(sizeof(struct timeval));
	tp->tv_sec=0;
	tp->tv_usec=0;
	int ret=adjtime(tp,tv);
	free(tp);
	free(tv);
	if(ret!=-1)
		{
	   	INFO_PRINTF1(_L("Test passed."));
	   	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
		}
	__UHEAP_MARKEND;
	return ret;
	}

TInt CTestTimeBlr::adjtime_neg_test()
	{
	__UHEAP_MARK;
	struct timeval* tp=(struct timeval*)malloc(sizeof(struct timeval));
	struct timeval* tv=(struct timeval*)malloc(sizeof(struct timeval));
	tp->tv_sec=-10;
	tp->tv_usec=-10;
	int ret=adjtime(tp,tv);
	free(tp);
	free(tv);
	if(ret!=-1)
		{
	   	INFO_PRINTF1(_L("Test passed."));
	   	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
	    }
	__UHEAP_MARKEND;
	return ret;
	}
     

/******************************************************************************/
//--------------------------------Multithread test cases-----------------------
/******************************************************************************/


TInt CTestTimeBlr::multithread_asctime_r()
	{
	int retVal = KErrGeneral;
	int dummy=0;
	
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_DETACHED );
	
	//Create one thread now
	retVal = pthread_create( &threadID1, &threadAttr, asctime_thread_1, (void*)&dummy );
	INFO_PRINTF2(_L("Thread1 Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create another thread
		retVal = pthread_create( &threadID2, &threadAttr, asctime_thread_2, (void*)&dummy );
		INFO_PRINTF2(_L("Thread2 Creation returned with %d"), retVal);
		}

	INFO_PRINTF1(_L("Test passed."));
	return retVal;	
	}


TInt CTestTimeBlr::multithread_localtime_r()
	{
	int retVal = 0;
	int dummy=0;
	
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_DETACHED );
	
	//Create one thread now
	retVal = pthread_create( &threadID1, &threadAttr, localtime_thread_1, (void*)&dummy );
	INFO_PRINTF2(_L("Thread1 Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create another thread
		retVal = pthread_create( &threadID2, &threadAttr, localtime_thread_2, (void*)&dummy );
		INFO_PRINTF2(_L("Thread2 Creation returned with %d"), retVal);
		}

	INFO_PRINTF1(_L("Test passed."));
	return retVal;	
	}


TInt CTestTimeBlr::multithread_gmtime_r()
	{
	int retVal = 0;
	int dummy=0;
	
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_DETACHED );
	
	//Create one thread now
	retVal = pthread_create( &threadID1, &threadAttr, gmtime_thread_1, (void*)&dummy );
	INFO_PRINTF2(_L("Thread1 Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create another thread
		retVal = pthread_create( &threadID2, &threadAttr, gmtime_thread_2, (void*)&dummy );
		INFO_PRINTF2(_L("Thread2 Creation returned with %d"), retVal);
		}

	INFO_PRINTF1(_L("Test passed."));
	return retVal;		
	}

TInt CTestTimeBlr::multithread_ctime_r()
	{
	int retVal = 0;
	int dummy=0;
	
	pthread_t threadID1 = 0;
	pthread_t threadID2 = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_DETACHED );
	
	//Create one thread now
	retVal = pthread_create( &threadID1, &threadAttr, ctime_thread_1, (void*)&dummy );
	INFO_PRINTF2(_L("Thread1 Creation returned with %d"), retVal);
	if( retVal == 0)
		{
		//Create another thread
		retVal = pthread_create( &threadID2, &threadAttr, ctime_thread_2, (void*)&dummy );
		INFO_PRINTF2(_L("Thread2 Creation returned with %d"), retVal);
		}

	 INFO_PRINTF1(_L("Test passed."));
	 return retVal;		
	}




/******************************************************************************/
//--------------------------------Locals for multithread fns-------------------
/******************************************************************************/

void* asctime_thread_1( void* /*aParam*/ )
	{
	TInt retVal = KErrNone;
	__UHEAP_MARK;
	struct tm* timeptr=(struct tm*)malloc(sizeof(struct tm));
	timeptr->tm_sec=0;		
	timeptr->tm_min=0;		
	timeptr->tm_hour=0;	
	timeptr->tm_mday=0;	
	timeptr->tm_mon=0;	
	timeptr->tm_year=0;	
	timeptr->tm_wday=0;	
	timeptr->tm_yday=0;	
	timeptr->tm_isdst=0;
	timeptr->tm_gmtoff=0;
	
	char expected[KMaxParamLength];
	char input[KMaxParamLength];
		
  	strcpy(expected,"Sun Jan  0 00:00:00 1900\n");         
	char* s=asctime_r(timeptr,input);
		
	if(!strcmp(s,expected))
		{
		retVal=KErrNone;
		}
	else
		{
		retVal=KErrGeneral;
		}
	free(timeptr);

	__UHEAP_MARKEND;
	return (TInt*)retVal;	
	}
	

void* asctime_thread_2( void* /*aParam*/ )
	{
	TInt retVal = KErrNone;
	__UHEAP_MARK;
	struct tm* timeptr=(struct tm*)malloc(sizeof(struct tm));
	timeptr->tm_sec=0;		
	timeptr->tm_min=0;		
	timeptr->tm_hour=0;	
	timeptr->tm_mday=0;	
	timeptr->tm_mon=0;	
	timeptr->tm_year=0;	
	timeptr->tm_wday=0;	
	timeptr->tm_yday=0;	
	timeptr->tm_isdst=0;
	timeptr->tm_gmtoff=0;
	
	char expected[KMaxParamLength];
	char input[KMaxParamLength];
		
  	strcpy(expected,"Sun Jan  0 00:00:00 1900\n");         
	char* s=asctime_r(timeptr,input);
		
	if(!strcmp(s,expected))
		{
		retVal=KErrNone;
		}
	else
		{
		retVal=KErrGeneral;
		}
	free(timeptr);

	__UHEAP_MARKEND;
	return (TInt*)retVal;	
	}


void* localtime_thread_1( void* /*aParam*/ )
    {
    __UHEAP_MARK;
    // Create temporary variables in stack
  	struct tm* result=(struct tm*)malloc(sizeof(struct tm));
  	time_t t=0;
  	int flag=0;
  	TInt retVal;	
    // Do some testing
	struct tm* result1=localtime_r(&t,result);
	
    // Set test result
    if(result1->tm_sec==0)
		{
		flag++;
		}
   	if(result1->tm_min==0)
		{
		flag++;
		}
    if(result1->tm_hour==0)
		{
		flag++;
		}
    if(result1->tm_mday==1)
		{
		flag++;
		}
    if(result1->tm_mon==0)
		{
		flag++;
		}
    if(result1->tm_year==70)
		{
		flag++;
		}
    if(result1->tm_wday==4)
		{
		flag++;
		}
    if(result1->tm_yday==0)
		{
		flag++;
		}
    if(result1->tm_isdst==0)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	retVal=KErrNone;
		}
    else
		{
		retVal=KErrGeneral;
		}
    
    free(result);
    __UHEAP_MARKEND;
    return(TInt*)retVal;
    }


void* localtime_thread_2( void* /*aParam*/ )
    {
    __UHEAP_MARK;
    // Create temporary variables in stack
  	struct tm* result=(struct tm*)malloc(sizeof(struct tm));
  	time_t t=0;
  	int flag=0;
  	TInt retVal;	
    // Do some testing
	struct tm* result1=localtime_r(&t,result);
	
    // Set test result
    if(result1->tm_sec==0)
		{
		flag++;
		}
   	if(result1->tm_min==0)
		{
		flag++;
		}
    if(result1->tm_hour==0)
		{
		flag++;
		}
    if(result1->tm_mday==1)
		{
		flag++;
		}
    if(result1->tm_mon==0)
		{
		flag++;
		}
    if(result1->tm_year==70)
		{
		flag++;
		}
    if(result1->tm_wday==4)
		{
		flag++;
		}
    if(result1->tm_yday==0)
		{
		flag++;
		}
    if(result1->tm_isdst==0)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	retVal=KErrNone;
		}
    else
		{
    	retVal=KErrGeneral;
		}
    
    free(result);
    __UHEAP_MARKEND;
    return(TInt*)retVal;
    }



void* gmtime_thread_1( void* /*aParam*/ )
    {
    __UHEAP_MARK;
    // Create temporary variables in stack
  	struct tm* result=(struct tm*)malloc(sizeof(struct tm));
  	time_t t=0;
  	int flag=0;
  	TInt retVal;	
    // Do some testing
	struct tm* result1=gmtime_r(&t,result);
	
    // Set test result
    if(result1->tm_sec==0)
		{
		flag++;
		}
   	if(result1->tm_min==0)
		{
		flag++;
		}
    if(result1->tm_hour==0)
		{
		flag++;
		}
    if(result1->tm_mday==1)
		{
		flag++;
		}
    if(result1->tm_mon==0)
		{
		flag++;
		}
    if(result1->tm_year==70)
		{
		flag++;
		}
    if(result1->tm_wday==4)
		{
		flag++;
		}
    if(result1->tm_yday==0)
		{
		flag++;
		}
    if(result1->tm_isdst==0)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	retVal=KErrNone;
		}
    else
		{
    	retVal=KErrGeneral;
		}
    
    free(result);
    __UHEAP_MARKEND;
    return(TInt*)retVal;
    }


void* gmtime_thread_2( void* /*aParam*/ )
    {
    __UHEAP_MARK;
    // Create temporary variables in stack
  	struct tm* result=(struct tm*)malloc(sizeof(struct tm));
  	time_t t=0;
  	int flag=0;
  	TInt retVal;	
    // Do some testing
	struct tm* result1=gmtime_r(&t,result);
	
    // Set test result
    if(result1->tm_sec==0)
		{
		flag++;
		}
   	if(result1->tm_min==0)
		{
		flag++;
		}
    if(result1->tm_hour==0)
		{
		flag++;
		}
    if(result1->tm_mday==1)
		{
		flag++;
		}
    if(result1->tm_mon==0)
		{
		flag++;
		}
    if(result1->tm_year==70)
		{
		flag++;
		}
    if(result1->tm_wday==4)
		{
		flag++;
		}
    if(result1->tm_yday==0)
		{
		flag++;
		}
    if(result1->tm_isdst==0)
		{
		flag++;
		}
        
    if(flag==9)
		{
    	retVal=KErrNone;
		}
    else
		{
	 	retVal=KErrGeneral;
		}
    
    free(result);
    __UHEAP_MARKEND;
    return(TInt*)retVal;
    }



void* ctime_thread_1( void* /*aParam*/ )
	{
	TInt retVal = KErrNone;
    __UHEAP_MARK;
   
    // Create temporary variables in stack
  	time_t t=0;
  	char expected[KMaxParamLength];
  	char result1[KMaxParamLength];
  	
  	// Read parameters
    strcpy(expected,"Thu Jan  1 00:00:00 1970\n");
        
    // Do some testing
	char* s1=ctime_r(&t,result1);    
	
    // Set test result
    if(!strcmp(expected,s1))
		{
    	retVal=KErrNone;
		}
    else
		{
    	retVal=KErrGeneral;
		}
    
    __UHEAP_MARKEND;
	return (TInt*)retVal;	
	}
	

void* ctime_thread_2( void* /*aParam*/ )
	{
	TInt retVal = KErrNone;
    __UHEAP_MARK;
   
    // Create temporary variables in stack
  	time_t t=0;
  	char expected[KMaxParamLength];
  	char result2[KMaxParamLength];
  	
  	// Read parameters
    strcpy(expected,"Thu Jan  1 00:00:00 1970\n");
       
    // Do some testing
	char* s2=ctime_r(&t,result2);    
	
    // Set test result
    if(!strcmp(expected,s2))
		{
    	retVal=KErrNone;
		}
    else
		{
    	retVal=KErrGeneral;
		}
    
    __UHEAP_MARKEND;
	return (TInt*)retVal;	
	}
	
   /*
	*TestCase Name :getoffset_test
	*API Name		:getoffset()
	*--------------------------------------
	*/

	TInt CTestTimeBlr::getoffset_test()
	    {
	    struct tm* tptr;
	  	time_t t;
	  	char name[3];
	  	char value[12];
	   	ReadIntParam(t);
	  	ReadStringParam(name);
	   	ReadStringParam(value);
	    int ret=setenv(name,value,1);
	  	tptr=localtime(&t);
		unsetenv(name);
		if(tptr==NULL)
			{
			return KErrGeneral;
			}
		return KErrNone;
	    }
    
  /*
   *---------------------------------------------------------------------------- 
   *Testcase Name:getrule_test()
   *API Name	 :getrule()
   *----------------------------------------------------------------------------
   */ 
    
 TInt CTestTimeBlr::getrule_test()
    {
    struct tm* tptr;
  	time_t t=13;
  	char name[3];
    char value[20];
  	ReadStringParam(name);
	ReadStringParam(value);
    int ret=setenv(name,value,1);
  	tptr=localtime(&t);
	unsetenv(name);
	if(tptr==NULL)
		{
		return KErrGeneral;
		}
	return KErrNone;
    }
    
// CTestTimeBlr::tzset1
// To Cover for transtime()
// Example test method function.
// The string used in putenv() is just to traverse through the entire function
// -----------------------------------------------------------------------------
//
TInt CTestTimeBlr::tzset1()
    {
    __UHEAP_MARK;
	putenv("TZ=UTC+05:30:15UTC+01:21:10,J08,J30");
	tzset();
	INFO_PRINTF1(_L("Test passed."));
	__UHEAP_MARKEND;
    return KErrNone;
    }

/* --------------------------------------------------------------------------
 * Test Case Name		:timegm_Test()
 * API tested	 		: timegm()
 * Test Case Description:To Cover timegm()
 * API Description		:timegm - inverses for gmtime
 * ---------------------------------------------------------------------------
 */

TInt CTestTimeBlr::timegm_Test()
	{
	TInt ret = KErrGeneral;
	struct tm* tptr;
    time_t t;
    TInt param1;
    _LIT( KPassed, "Passed" );
    _LIT( KFailed, "Failed" );
    ReadIntParam(t);
    param1 = t;
    INFO_PRINTF2(_L("Value read from .ini file is  :%d"),t);
    tptr = gmtime(&t);
    INFO_PRINTF2(_L("Value returned by gmtime() is :%d"),t);
    ReadIntParam(t);
    t = timegm(tptr);
    if(t == param1)
    	{
    	INFO_PRINTF1( KPassed);
    	ret = KErrNone;
    	}
    else 
    	{
    	INFO_PRINTF1( KFailed);
    	ret = KErrGeneral;
    	}
    return ret;
    }
    
TInt CTestTimeBlr::clock_getres_null_test()
	{
	int ret=clock_getres(CLOCK_REALTIME,NULL);
	if(ret==-1 && errno == EFAULT)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestTimeBlr::adjtime_null_test()
	{
	struct timeval* tv=(struct timeval*)malloc(sizeof(struct timeval));
	int ret=adjtime(NULL,tv);
	free(tv);
	if(ret==-1 && errno == EFAULT)
		{
	   	INFO_PRINTF1(_L("Test passed."));
	   	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
	    }
	return ret;
	}
TInt CTestTimeBlr::adjtime_range_test()
	{
	struct timeval* tp=(struct timeval*)malloc(sizeof(struct timeval));
	struct timeval* tv=(struct timeval*)malloc(sizeof(struct timeval));
	tp->tv_sec=0;
	tp->tv_usec=10000000;
	int ret=adjtime(tp,tv);
	free(tp);
	free(tv);
	if(ret==-1 && errno == ERANGE)
		{
	   	INFO_PRINTF1(_L("Test passed."));
	   	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestTimeBlr::clock_gettime_null_test()
	{
	int ret=clock_gettime(CLOCK_REALTIME,NULL);
	if(ret==-1 && errno == EFAULT)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
    else
		{
    	ERR_PRINTF1(_L("Test Failed."));
    	ret = KErrGeneral;
		}
	return ret;
	}
TInt CTestTimeBlr::clock_settime_null_test()
	{
	int ret=clock_settime(CLOCK_REALTIME,NULL);
	if(ret==-1 && errno == EFAULT)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
	    }
	return ret;
	}
	
TInt CTestTimeBlr::clock_settime_range1_test()
	{
	struct timespec* tp=(struct timespec*)malloc(sizeof(struct timespec));
	tp->tv_sec=0;
	tp->tv_nsec=1000000000L;
	int ret=clock_settime(CLOCK_REALTIME,tp);
	free(tp);
	if(ret==-1 && errno == ERANGE)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
	    }
	else
	    {

	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
	    }
	return ret;
	}
TInt CTestTimeBlr::clock_settime_range2_test()
	{
	struct timespec* tp=(struct timespec*)malloc(sizeof(struct timespec));
	tp->tv_sec=0;
	tp->tv_nsec=-1;
	int ret=clock_settime(CLOCK_REALTIME,tp);
	free(tp);
	if(ret==-1 && errno == ERANGE)
		{
    	INFO_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
	    }
	else
	    {
	   	ERR_PRINTF1(_L("Test Failed."));
	   	ret = KErrGeneral;
	    }
	return ret;
	}

TInt CTestTimeBlr::mktime_test_extreme1()
    {
  	TInt ret=KErrGeneral;
  	time_t retval;
	time_t now = time(0);
	struct tm* t = gmtime(&now);
	t->tm_mday = 50;
	retval = mktime(t);
	if (retval != -1)
		{
    	ERR_PRINTF1(_L("Test passed."));
    	ret = KErrNone;
		}
	else
		{
    	INFO_PRINTF1(_L("Test failed."));
    	ret = KErrGeneral;
		}
    return ret;

    }


static TInt  ConvertTimeToSecsAndBack(tm& time)

    {
    tzset();     
    char *before,*after;
    // Convert to seconds since Epoch
    time_t asSeconds = mktime(&time);     
        
    // Print info about tested date
    before = asctime(&time)   ;
    //INFO_PRINTF2(_L("*** Orig: %s"), before);      
       
    
    // Convert back to broken down time

    tm res;        
    tm *brokenDown = 0;
    brokenDown = localtime_r(&asSeconds, &res);  
    after = asctime(brokenDown);  
    //INFO_PRINTF2(_L("*** Back: %s"), asctime(brokenDown));  
    if(!strcmp(before,after))
    	return KErrNone;
    else
    	return KErrGeneral;
        
    
    }

TInt CTestTimeBlr::local_and_mktime()

	{
	
	//INFO_PRINTF1(_L("Time convertin test program\n"));
    // Init few broken down struct for test
    tm localTM;
    localTM.tm_sec = 0;
    localTM.tm_min = 0;
    localTM.tm_hour = 12;
    localTM.tm_mday = 1;
    localTM.tm_mon = 1;
    localTM.tm_year = 2002 - 1900;
    localTM.tm_isdst = -1;        
    
    tm localTM2;
    localTM2.tm_sec = 0;
    localTM2.tm_min = 0;
    localTM2.tm_hour = 12;
    localTM2.tm_mday = 1;
    localTM2.tm_mon = 1;
    localTM2.tm_year = 2012 - 1900;
    localTM2.tm_isdst = -1;        
    
    tm localTM3;
    localTM3.tm_sec = 0;
    localTM3.tm_min = 0;
    localTM3.tm_hour = 12;
    localTM3.tm_mday = 1;
    localTM3.tm_mon = 7;
    localTM3.tm_year = 2002 - 1900;
    localTM3.tm_isdst = -1;    

    
    if(ConvertTimeToSecsAndBack(localTM))
    	return KErrGeneral;
    if(ConvertTimeToSecsAndBack(localTM2))
    	return KErrGeneral;
    if(ConvertTimeToSecsAndBack(localTM3))
    	return KErrGeneral;
  if(ConvertTimeToSecsAndBack(localTM2))
    	return KErrGeneral;  			                   
	return KErrNone;	
	}


