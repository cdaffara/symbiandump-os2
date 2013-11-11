// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Ost Performance Tests API Test Object
//



/**
 @file te_perfapicall.cpp
 @internalTechnology
 @prototype
*/


#include "te_perfapicall.h"
#include "te_apirunconfig.h"
#include "te_perftracetester.h"
#include "te_lightlogger.h"
#include "te_instrumentationpoints.h" //for TUptTraceCalls::IsKernelTrace
#include "te_perfstackinfo.h"


CApiCallTest::CApiCallTest()
	{
	}

CApiCallTest::~CApiCallTest()
	{
	}


/** This user-side method uses takes the parameters passed by the user to carry out a single API call test run
@param aApiId is the API call method or API printf method, passed as an enum. (NB: enum is different for user vs kernel printf calls, and for static and nonstatic etc.)
@param aUptPc is the TBool specifying the Program Counter parameter to pass to the UTrace API
@param aUptContext is the TBool specifying the Context parameter to pass to the UTrace API
@param aSizeOfUptData is the size in bytes of the Data field being used in the relevant API calls containing a data field, passed as a TInt
@param aTestPrimary is the boolean specifying whether to test the Primary filter setting (ETrue does test the primary filter, EFalse doesn't)
@param aTestSecondary is the boolean specifying whether to test the Secondary filter setting (ETrue does test the secondary filter, EFalse doesn't)
@param aTestUTraceULoggerMatch is the boolean specifying whether the filter settings in ULogger match the filter parameters assigned to the Trace points (ETrue matches, EFalse doesn't match)
@param aTestMinimumExecution is the boolean specifying whether to test the minimum execution time for the API call
							ETrue = minimum execution time, i.e. cached trace
							EFalse = typical execution time, i.e. uncached trace
@return ApiTestResult is the API Call test result struct object returned to the calling program, which contains all the test results from the relevent test run
 */
TInt CApiCallTest::DoTestL(const TApiRunConfig& /*aApiRunConfig*/, TApiTestResult& /*aApiTestResult*/)	
	{
	TInt error = KErrNotSupported;

	//create a dummy time variable to be used for the baseline measurement	
	//TUint64 baselinetime = 0;

	//now run tests to fill the results variables
	
	__TEST_LOG("member functions initialised");

	//Sorry, I cut out all the functionality here and stuck it in the methods below...
	//I needed to know where the errors came from...

	//err = HeapUsage(iApiTestResult->iApiHeapUsage);
	__TEST_LOG("all trace ok - stack ERR IS");
	__TEST_LOGNUM(error);

	//return the results of the API call tests to the calling program
	return error;
	}

TInt CApiCallTest::CachedTraceTimeL(TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult)
	{
	TInt error = KErrNotSupported;
	//create a dummy time variable to be used for the baseline measurement	
	TInt32 baselinetime = 0;

	//carry out the APIcall tests as required by the user
	__TEST_LOG("about to carry out cache trace");
	if(aApiRunConfig.iDoMinExecution) 
		{
		CachedTraceBaselineTimeL(aApiRunConfig, baselinetime);
		__TEST_LOG("cache baseline");
		__TEST_LOGNUM(baselinetime);
		error = CachedTraceTimeL(aApiRunConfig, aApiTestResult.iMinimumExecutionTime, baselinetime);
		__TEST_LOG("cache");
		__TEST_LOGNUM(aApiTestResult.iMinimumExecutionTime);
		}
	return error;
	}

TInt CApiCallTest::NonCachedTraceTimeL(TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult)
	{
	TInt error = KErrNotSupported;
	//create a dummy time variable to be used for the baseline measurement	
	TInt32 baselinetime = 0;
	__TEST_LOG("about to carry out non cache trace");
	if(aApiRunConfig.iDoTypicalExecution)
		{
		NonCachedTraceBaselineTimeL(aApiRunConfig, baselinetime);
		__TEST_LOG("noncache baseline");
		__TEST_LOGNUM(baselinetime);
		error = NonCachedTraceTimeL(aApiRunConfig, aApiTestResult.iTypicalExecutionTime, baselinetime);
		__TEST_LOG("noncache");
		__TEST_LOGNUM(aApiTestResult.iTypicalExecutionTime);				
		}
	return error;
	}

TInt CApiCallTest::StackUsageL(TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult)
	{
	__TEST_LOG("about to carry out stack usage trace");
	TInt error = StackUsageL(aApiRunConfig, aApiTestResult.iRunResults.iApiStackUsage);
	if(aApiTestResult.iRunResults.iApiStackUsage == KErrNotSupported && !error)
		error = KErrNotSupported;
	return error;
	}

/** This user-side method is used to ensure that the cache has been flushed for calculating the API typical execution times
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CApiCallTest::FlushTheCache()
	{
	//Put in empty call of certain size to flush the cache 
	TInt err=0;
	/*
#ifndef __WINSCW__				
	err=Te_Flush_The_Cache();
#endif
*/
	return err;
	}

/** This user-side method calculates the minimum execution baseline time taken for a single commented API call.
@param aMinimumExecutionBaselineTime is the minimum execution baseline time taken for the commented API call, calculated in nanoseconds
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CApiCallTest::CachedTraceBaselineTimeL(TApiRunConfig& aApiRunConfig, TInt32& aMinimumExecutionBaselineTime) 
	{
	//****NB: update this routine to something more maintainable
	TUptTraceTester timer;
	TInt error = KErrNone;

	TApiRunResults apiRunResults;
	TRunConfigurer::Init(apiRunResults);
	TInt originalApiId = aApiRunConfig.iApiId;
	if(TUptTraceCalls::IsKernelTrace(originalApiId))
		aApiRunConfig.iApiId = UPTKernelBaseline;
	else
		aApiRunConfig.iApiId = UPTBaseline;
	aApiRunConfig.iDoTrace=EFalse;
	aApiRunConfig.iDoStack=EFalse;
	aApiRunConfig.iDoFlush=EFalse;
	
	error = timer.TimeTraceL(aApiRunConfig, apiRunResults);
	
	aApiRunConfig.iApiId = originalApiId;
	aMinimumExecutionBaselineTime = apiRunResults.iApiTraceTime;
	return error;	
	
	}
	
/** This user-side method calculates the minimum execution time taken for a single API call.
@param aMinimumExecutionTime is the minimum execution time taken for the API call, calculated in nanoseconds
@param aMinimumExecutionBaselineTime is the minimum execution baseline time taken for commented API call, calculated in nanoseconds
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */	
TInt CApiCallTest::CachedTraceTimeL(TApiRunConfig& aApiRunConfig, TInt32& aMinimumExecutionTime, TInt32& aMinimumExecutionBaselineTime)
	{ 
	//NB!!! any changes to this routine and you have to update the baseline calculation as well
	TUptTraceTester timer;

	TInt err =KErrNone;
	TApiRunResults apiRunResults;
	TRunConfigurer::Init(apiRunResults);
	aApiRunConfig.iDoTrace=ETrue;
	aApiRunConfig.iDoStack=EFalse;
	aApiRunConfig.iDoFlush=EFalse;

	err=timer.TimeTraceL(aApiRunConfig, apiRunResults);
	
	aMinimumExecutionTime = apiRunResults.iApiTraceTime - aMinimumExecutionBaselineTime;

	return err;	
	}

/** This user-side method calculates the typical execution baseline time taken for a single commented API call.
@param aTypicalExecutionBaselineTime is the typical execution baseline time taken for the API call, calculated in nanoseconds
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */	
TInt CApiCallTest::NonCachedTraceBaselineTimeL(TApiRunConfig& aApiRunConfig, TInt32& aTypicalExecutionBaselineTime)
	{
	//****NB: update this routine to something more maintainable
	TUptTraceTester timer;
	
	TInt error = KErrNone;

	TApiRunResults apiRunResults;
	TRunConfigurer::Init(apiRunResults);
	TInt originalApiId = aApiRunConfig.iApiId;
	if(TUptTraceCalls::IsKernelTrace(originalApiId))
		aApiRunConfig.iApiId = UPTKernelBaseline;
	else
		aApiRunConfig.iApiId = UPTBaseline;
	aApiRunConfig.iDoTrace = EFalse;
	aApiRunConfig.iDoStack = EFalse;
	aApiRunConfig.iDoFlush = ETrue;

	error=timer.TimeTraceL(aApiRunConfig, apiRunResults);
	
	aApiRunConfig.iApiId = originalApiId;
	aTypicalExecutionBaselineTime = apiRunResults.iApiTraceTime;
	return error;	
	}
	
/** This user-side method calculates the typical execution time taken for a single API call.
@param aTypicalExecutionTime is the typical execution time taken for the API call, calculated in nanoseconds
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */	
TInt CApiCallTest::NonCachedTraceTimeL(TApiRunConfig& aApiRunConfig, TInt32& aTypicalExecutionTime, TInt32& aTypicalExecutionBaselineTime)
	{
	
	TUptTraceTester timer;
	
	TInt err =KErrNone;
	TApiRunResults apiRunResults;
	TRunConfigurer::Init(apiRunResults);
	aApiRunConfig.iDoTrace=ETrue;
	aApiRunConfig.iDoStack=EFalse;
	aApiRunConfig.iDoFlush=ETrue;

	err=timer.TimeTraceL(aApiRunConfig, apiRunResults);

	aTypicalExecutionTime = apiRunResults.iApiTraceTime - aTypicalExecutionBaselineTime;

	return err;	
	
	}

/*A test function available for use by TestStackUsage()
 * */
void foo()
	 {
	 volatile int gt[100]; //expect 400 bytes stack use - i.e. 100 words of 4bytes each
		for(int i=0; i<100;++i)
			gt[i] = i;
	 }
	 

/** This calculates the typical stack usage for a single API call.
 * This is only done on hardware, but this is taken care of in the
 * actual stack usage methods. For emulator the result will always be 0.
 *  
 * @return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CApiCallTest::StackUsageL(TApiRunConfig& aApiRunConfig, TInt& aApiStackUsage)
	{
	/*
	PSEUDOCODE 
	1) initiate a variable forced to the cleanup stack
			static volatile int foo = 0;
	2) initiate a pointer to this variable
			int* p=&foo;
	3) now fill the stack from this point to 0K with e.g. DEADBEEF bitpattern
		memid=(foo, p, 0xDEADBEEF);
	4) (note will wind the pointer on to p = p & 0xFFFC000 (0k)
	5) take a copy of this bitmap pattern (memcopy (foo));
	6) take pointer back to foo
	7) execute UTrace(...)
	8) take a copy of the new bitmap patterm (memcopy (foo));
	9) compare the bitmap pattern from 5) and 9) to give stack usage
	10)	verify this result with Lauterbach
	*/
	TUptTraceTester tracecall;

	TInt err=KErrNone;
	TApiRunResults apiRunResults;
	TRunConfigurer::Init(apiRunResults);
	aApiRunConfig.iDoTrace=ETrue;
	aApiRunConfig.iDoStack=ETrue;
	aApiRunConfig.iDoFlush=EFalse; //not really needed but kept for consistency


	err = tracecall.SendTraceL(aApiRunConfig, apiRunResults);
	aApiStackUsage = apiRunResults.iApiStackUsage;
	return err;
	}

/** This user-side method calculates the typical heap usage for a single API call. (Expected to be zero, but included for sanity)
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CApiCallTest::HeapUsage(TInt& aApiHeapUsage)
	{
	aApiHeapUsage=-1;
	return KErrNone;	
	}
	
/** This sanity test method is executed at the start of the test run to verify that UPT methods in
this class are stable before any of the performance tests are carried out 
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */
TInt CApiCallTest::Test()
	{
	TInt err=TestStackUsage();
	return err;
	}

/** This sanity test method is executed in Test() to verify that UPT StackUsageL() method in
this class are stable before any of the stack size performance tests are carried out 
@return KErrNone if command was prepared correctly and system wide error code otherwise.
 */	
TInt CApiCallTest::TestStackUsage()
	{
	TInt err=KErrNone;
#ifndef __WINSCW__
	err=KErrGeneral;
	TInt32 stackBase=0;
	TInt32 before=0;
	TInt fooSize=400;
	TBool doStack=ETrue;
	
	//TTeUptStackInfo::MarkInitialStack(doStack, stackBase, before);
	__MARK_INITIAL_STACK(doStack, stackBase, before);
	foo();
//	TTeUptStackInfo::ReturnStackSize(doStack, stackBase, before, fooSize);
	__RETURN_STACK_SIZE(doStack, stackBase, before, fooSize);
	
	if(fooSize==400)
		err=KErrNone;
	else
		err=fooSize;
#endif
			
	return err;
	}





