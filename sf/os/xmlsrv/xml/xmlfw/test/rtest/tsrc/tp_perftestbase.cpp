// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic performance test framework implementation. 
// 
//

/**
 @file 
 @internalComponent
*/

#include <e32base.h>
#include <f32file.h>
#include <hal.h>
#include <e32test.h>
#include <bacline.h>

#include "tp_perftestbase.h"

/**
The log directory.
*/
_LIT(KPTestOutput,       "c:\\logs\\");

/**
Global test object
*/
RTest test(_L("Perfomance Testing"));

/**
Class constructor.

@param aAPIName Name of the API method to test
@param aAPIPtr Pointer to the API method
@param aMaxDuration The maximum duration of the performance test for that API method
*/
TApiRecord::TApiRecord(const TDesC8& aAPIName, TFuncPtr aAPIPtr, TUint64 aMaxDuration)
	{
	iAPIName = aAPIName;
	iAPIPtr = aAPIPtr;
	iMaxDuration = aMaxDuration;
	}

/**
Set method for API pointer attribute

@param aAPIPtr Pointer to API method
*/
void TApiRecord::SetAPIPtr(TFuncPtr aAPIPtr)
	{
	iAPIPtr = aAPIPtr;
	}
	
/**
Set method for API name attribute

@param aAPIName Function name
*/
void TApiRecord::SetAPIName(TDesC8& aAPIName)
	{
	iAPIName = aAPIName;
	}

/**
Get method for API name attribute

@return API name string
*/
const TDesC8& TApiRecord::APIName()
	{
	return iAPIName;
	}
	
/**
Get method for API method pointer

@return API method pointer
*/
TFuncPtr TApiRecord::APIPtr()
	{
	return iAPIPtr;
	}
	
/**
Set method for max duration attribute

@param aMaxDuration The maximum duration of the performance test for that API method
*/
void TApiRecord::SetMaxDuration(TUint64 aMaxDuration)
	{
	iMaxDuration = aMaxDuration;
	}
	
/**
Get method for max duration attribute

@return The maximum duration of the performance test for that API method
*/
TUint64 TApiRecord::MaxDuration()
	{
	return iMaxDuration;
	}
		
/**
Class consturctor. 
*/
CPerformanceTests::CPerformanceTests(): iOutputFile(NULL)
	{
	
	}
	
/**
Second phase constructor. 
Creates adequate directory and output file for logging purposes.

@param aTestName Test name string
*/	
void CPerformanceTests::ConstructL(const TDesC& aTestName)
	{
	ParserComandLineL();
	if (iOutputFile != NULL)
		{
		HBufC* fileName = HBufC::NewLC(KPTestOutput().Length() + iOutputFile->Length());
		TPtr ptr(fileName->Des());
		
		User::LeaveIfError(session.Connect());
		
		// create the directory if not existing
		if (!session.IsValidName(KPTestOutput()))
			{
			session.MkDirAll(KPTestOutput());
			}
		
		ptr = KPTestOutput;
		ptr.Append(*iOutputFile);
		
		//create the file
		User::LeaveIfError(output.Replace(session, *fileName, EFileWrite ));
		CleanupStack::PopAndDestroy(fileName);
		}
	else
		{
		test.Start(aTestName);
		test.Title();
		}
	}

/**
Destructor. 
*/
CPerformanceTests::~CPerformanceTests()
	{
	while (list.Count() > 0)
		{
		delete list[list.Count()-1];
		list.Remove(list.Count()-1);
		}
	if (iOutputFile != NULL)
		{
		delete iOutputFile;
		}
	list.Reset();
	if (iOutputFile)
		{
		output.Flush();
		output.Close();
		session.Close();
		}
	else
		{
		test.End();
		test.Close();
		}
	
	}

/**
Runs all the required tests. 
*/
void CPerformanceTests::RunTestsL()
	{
	GetMethodListL(list);
	iRepNum = GetNumberOfRepetition();
	PrepareTestsL();
	
	for (TInt i = 0 ; i < list.Count();i++)
		{
		TApiRecord* apiRecord = list[i];
		TestApi(*apiRecord);
		}
	}

/**
Tests particular API method. 
Logs the perfomance benchmarks to the output file. 

@param aApi The API record 
*/
void CPerformanceTests::TestApi(TApiRecord& aApi)
	{
	TUint64 total = 0;
	TUint64 average = 0;
	TUint64 firstCall = 0;
	TBuf8<255> string;
	TInt freq;
	
	if (iOutputFile)
		{
		output.Write(aApi.APIName());
		}
	
	HAL::Get(HAL::EFastCounterFrequency, freq);
		
	//first call time verification	
	iEndTime = 0;
	iStartTime = 0;
	aApi.APIPtr()();
	
	//error checking
	if (iEndTime == 0 || iStartTime == 0)
		{
		// user test method did not call either ApiTestStart or ApiTestEnd method
		User::Panic(_L("Xml Framework Error"), KErrGeneral);
		}
	
	// calculate and convert to miliseconds
	firstCall = (1000000*(iEndTime - iStartTime))/freq; 

	for (TInt i = 0; i < iRepNum; i++)
		{
		// initialize members
		iEndTime = 0;
		iStartTime = 0;
		
		//method call
		aApi.APIPtr()();
		
		//error checking
		if (iEndTime == 0 || iStartTime == 0)
			{
			// user test method did not call either ApiTestStart or ApiTestEnd method
			User::Panic(_L("Xml Framework Error"), KErrGeneral);
			}
		// time counting
		total += iEndTime - iStartTime;
		}
	// calculate and convert to miliseconds
	// Fast counter frequency already in MHz
	total =  (1000000*total)/freq;
	average = total/iRepNum;
	if (iOutputFile)
		{
		string.Format(_L8("\n  First call time: %Ld ms\n  Number of repetitions: %d\n    Time: %Ld ms\n    Average: %Ld ms\n    Max duration: %Ld ms\n\n"),
					firstCall,
					iRepNum,
					total,
					average,
					aApi.MaxDuration());
		TPtrC8 des(string);
		output.Write(des);
		}
	else
		{
#ifdef EABI
		test(total < aApi.MaxDuration());
#else
		test(ETrue);
#endif // EABI
		TBuf<100> temp;
		temp.Copy(aApi.APIName());
		test.Printf(_L("\n %S - Performance testing results\n\t First call time: %Ld ms\n\t Number of repetitions: %d\n\t Time: %Ld ms \n\t Average: %Ld ms\n\t Max duration: %Ld ms \n"),
		&temp, firstCall, iRepNum, total, average, aApi.MaxDuration());
		}
	}
/**
Stores the start time of Api call.
ApiTestStart and ApiTestEnd should enlosed to particular API call. 
*/
void CPerformanceTests::ApiTestStart()
	{
	iStartTime = User::FastCounter();
	}
	
/**
Stores the end time of Api call.
ApiTestStart and ApiTestEnd should enlosed to particular API call. 
*/
void CPerformanceTests::ApiTestEnd()
	{
	iEndTime = User::FastCounter();
	}

/**
Parses the command line 
*/
void CPerformanceTests::ParserComandLineL()
	{
	CCommandLineArguments* pCmd = CCommandLineArguments::NewLC();
	if (pCmd->Count() == 3)
		{
		if (pCmd->Arg(1).CompareF(_L("-l")) == 0)
			{
			iOutputFile = pCmd->Arg(2).AllocL();
			}	
		}
	
	CleanupStack::PopAndDestroy(pCmd);
	}


