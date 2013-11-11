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
// Example CTestStep derived implementation
//



/**
 @file te_perfsanity.cpp
 @internalTechnology
*/
#include "te_perfsanity.h"
#include "te_perfcsvgenerator.h"
#include "te_perfresultscollector.h"
#include "te_perfapicall.h"
#include "te_perfplugin.h"
#include "te_ostv2integsuite_defs.h"
#include "te_perfsanitytimer.h"
#include "te_d32perfdrv.h"

CPerformanceSanityWrapper::CPerformanceSanityWrapper()
/**
 * Constructor
 */
	{
	}

CPerformanceSanityWrapper::~CPerformanceSanityWrapper()
/**
 * Destructor
 */
	{
	}

CPerformanceSanityWrapper* CPerformanceSanityWrapper::NewLC()
	{
	CPerformanceSanityWrapper* self = new (ELeave)CPerformanceSanityWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPerformanceSanityWrapper* CPerformanceSanityWrapper::NewL()
	{
	CPerformanceSanityWrapper* self=CPerformanceSanityWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CPerformanceSanityWrapper::ConstructL()
	{
	}

TAny* CPerformanceSanityWrapper::GetObject()
	{
	return NULL;
	}

TBool CPerformanceSanityWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	if(BlockResult()==EPass)
		{
		INFO_PRINTF1(_L("Testing stack usage functions"));	
		//test CApiCallTest class 
		CApiCallTest apicalltest;
   		TInt stackerr=apicalltest.Test();	 
   		if(stackerr!=KErrNone)	
   			{
   			SetBlockResult(EFail);
			INFO_PRINTF2(_L("Sanity test failed on Stack usage, with returned stack usage of %d, should be 400"), stackerr);	
   			}
   		else
			INFO_PRINTF1(_L("stack usage test passed"));
		
		/*
		INFO_PRINTF1(_L("Testing CSV generator functions"));	
		//test CUptCsvGenerator class 
   		CUptCsvGenerator csvtest;
   		TInt csverr=csvtest.Test();	 
   		if(csverr!=KErrNone)	
   			{
   			SetBlockResult(EFail);
			INFO_PRINTF2(_L("Sanity test failed on CSV Generator, with error code %d"), csverr);	
   			}
   		else
			INFO_PRINTF1(_L("CSV Generator Sanity test passed"));	
			*/
		}
		
	if(BlockResult()==EPass)
		{
		SetBlockResult(TestTimerL());
		}
	
	//run this no matter if its failed or not..., just to get some statistics out.
	
	//temporarily tagged out this test as it is causing a test timeout
	//can be reinstated as soon as the device driver has been fully implemented
	
	//SanityTestTimedUtraceL();
	

	return ETrue;
	}


TVerdict CPerformanceSanityWrapper::TestTimerL()
	{
	INFO_PRINTF1(_L("Testing User side timer functions"));	
	TBool passed = EFalse;
	TInt error = KErrNone;

	INFO_PRINTF1(_L("Testing User side timer functions #1"));	

	//test CUptTimer class 
	TTestTimer timer;
	TUint32 userTime = 0;
	passed = timer.TestUserTimerL(userTime);	 

	INFO_PRINTF1(_L("Testing User side timer functions #2"));	
	if(!passed)	
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("ERROR: Sanity test failed on User side timer, with time %d"), userTime);
		}
	else
		INFO_PRINTF2(_L("User side timer Sanity test passed with time %d"), userTime);
	
	
	//Kernel side
	
	INFO_PRINTF1(_L("Testing Kernel side timer functions"));
	RUptUTrace utracedriver;
	error = utracedriver.Open();
	TUint32 kernelTime = 0;
	if(!error)
		{
		passed = utracedriver.SanityTestTimer(kernelTime);
		}
	utracedriver.Close();

	if(!passed)	
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("ERROR: Sanity test failed on Kernel side timer, with time %d"), kernelTime);	
		}
	else
		{
		INFO_PRINTF2(_L("Kernel side timer Sanity test passed with time %d"), kernelTime);
		}
	
	if(passed)
		{ 
		//kernel not yet implemented at the moment
		
		/*
		TReal variation = (TReal) userTime / 10;
		if(kernelTime < userTime - variation || kernelTime > userTime + variation)
			{
			SetBlockResult(EFail);
			INFO_PRINTF1(_L("ERROR: Sanity test failed because the kernel and the user times where too different"));				
			}
		else
			INFO_PRINTF1(_L("Sanity test passed, the kernel and the user times where whithin a 10% range"));
			*/
		}
	if(!passed)
		{
		INFO_PRINTF3(_L("The times are supposed to be between %d-%d ns"), KMinFooTime, KMaxFooTime);
		INFO_PRINTF1(_L("This only applies to h4 for reference board, or the equivalent to a duo core PC for emulator, if you run it on any other hardware the range needs to be corrected."));
		}
		
	return BlockResult();
	}

TBool CPerformanceSanityWrapper::SanityTestTimedUtraceL()
	{
	TTestTimer timer;
	TUint32 userSanityTime = 0;
	TUint32 userSanityLongTime = 0;
	TUint32 userUtraceTime = 0;
	
	timer.TestUserTimerL(userSanityTime);
	timer.TestUserLongTimerL(userSanityLongTime);
	timer.TestUTraceUserTimerL(userUtraceTime);

	TUint32 kernelSanityTime = 0;
//	TUint32 kernelSanityLongTime = 0;
	TUint32 kernelUTraceTime = 0;
	
	TInt iters = 10;
	const TInt min = 10;//just to store fastest time
	const TInt max = 11;//to store slowes time
	TUint32 user[max + 1] = {0};
	TUint32 kernel[max + 1] = {0};
	TUint32 ulong[max + 1] = {0};
	TUint32 klong[max + 1] = {0};
	user[min] = 0xFFFFFF;//prime it to a large number so we can change it later...
	kernel[min] = 0xFFFFFF;
	ulong[min] = 0xFFFFFF;
	klong[min] = 0xFFFFFF;

	RUptUTrace utracedriver;
	TInt error = utracedriver.Open();	
	if(!error)
		{
		utracedriver.SanityTestTimer(kernelSanityTime);
		//utracedriver.SanityTestLongTimer(kernelSanityLongTime);//Can't do for kernel side as it takes too long and panics the kernel
		utracedriver.SanityUtraceTestTimer(kernelUTraceTime);
		
		
		//Test for variation between the different foos
		//I want to find out what the variation is, is it static 0.5us, or is it 10% variable, or what?
		for(TInt i = 0; i < iters; i++)
			{
			timer.TestUserTimerL(user[i]);
			timer.TestUserLongTimerL(ulong[i]);
			utracedriver.SanityTestTimer(kernel[i]);
			//utracedriver.SanityTestLongTimer(klong[i]);
			}

		}
	utracedriver.Close();
	
	
	//find fastest/slowest times
	for(TInt i = 0; i < iters; i++)
		{
		//fastest time
		if(user[i] < user[min])
			user[min] = user[i];
		if(ulong[i] < ulong[min])
			ulong[min] = ulong[i];
		if(kernel[i] < kernel[min])
			kernel[min] = kernel[i];
		if(klong[i] < klong[min])
			klong[min] = klong[i];
		//slowest time
		if(user[i] > user[max])
			user[max] = user[i];
		if(ulong[i] > ulong[max])
			ulong[max] = ulong[i];
		if(kernel[i] > kernel[max])
			kernel[max] = kernel[i];
		if(klong[i] > klong[max])
			klong[max] = klong[i];
		}

	INFO_PRINTF1(_L("Comparing foo times called from different places"));
	INFO_PRINTF1(_L("\t\t\tSanity, Sanity long, UPT"));
	INFO_PRINTF4(_L("User side, ns:\t %d, \t %d, \t%d"), userSanityTime, userSanityLongTime, userUtraceTime);	
	INFO_PRINTF3(_L("Kernel side, ns:\t %d, \tN/A, \t%d"), kernelSanityTime, kernelUTraceTime);
	INFO_PRINTF3(_L("The time's are supposed to range between %d-%d for h4 for the normal sanity test"), KMinFooTime, KMaxFooTime);
	INFO_PRINTF2(_L("Number of iterations averaged over: %d"), KNumberOfTraceCallIterations+1);

	INFO_PRINTF1(_L("Checking what the variation is"));
	TInt variation = user[max] - user[min];
	INFO_PRINTF5(_L("User sanity spans: \t\t%d-%d ns, variation: %d ns, percent: %f2"), user[min], user[max], variation, 100*(TReal)((TReal)variation/((TReal)user[min]+(TReal)variation/(TReal)2)));
	variation = ulong[max] - ulong[min];
	INFO_PRINTF5(_L("User sanity long spans: \t%d-%d ns, variation: %d ns, percent: %f2"), ulong[min], ulong[max], variation, 100*(TReal)((TReal)variation/((TReal)ulong[min]+(TReal)variation/(TReal)2)));
	variation = kernel[max] - kernel[min];
	INFO_PRINTF5(_L("kernel sanity spans: \t\t%d-%d ns, variation: %d ns, percent: %f2"), kernel[min], kernel[max], variation, 100*(TReal)((TReal)variation/((TReal)kernel[min]+(TReal)variation/(TReal)2)));
	variation = klong[max] - klong[min];
	INFO_PRINTF1(_L("kernel sanity long spans: N/A as interrupts in DFCQ causes kernel panic, i.e. call is running for too long."));
	INFO_PRINTF2(_L("Number of iterations averaged over: %d"), (iters+1));
	
	//havent actually tested anything... not yet anyway
	return ETrue;
	}

// eof



