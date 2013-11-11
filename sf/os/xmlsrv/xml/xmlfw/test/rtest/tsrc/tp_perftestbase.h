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
// Generic performance test framework
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef TP_FRAMEWORK_H
#define TP_FRAMEWORK_H

#include <e32std.h>
#include <f32file.h>

/**
Typedef for method pointer
*/
typedef TAny (*TFuncPtr)();

/** 
The class purpose is to store all necessary details of API method to call. 
*/
class TApiRecord
	{
	public:
		TApiRecord(const TDesC8& aAPIName, TFuncPtr aAPIPtr, TUint64 aMaxDuration);
		

	public:
		void SetAPIPtr(TFuncPtr aAPIPtr);
		const TDesC8& APIName();
		
		void SetAPIName(TDesC8& aAPIName);
		TFuncPtr APIPtr();
		
		void SetMaxDuration(TUint64 aMaxDuration);
		TUint64 MaxDuration();
		
	private:
		/**
		API method name
		*/
		TBuf8<100> iAPIName;
		
		/**
		API method pointer
		*/
		TFuncPtr iAPIPtr;
		
		/**
		The maximum duration of the performance test for that API method
		*/
		TUint64 iMaxDuration;
	};
	
/**
Typedef to simplify usage of the Api record list.
*/
typedef RPointerArray<TApiRecord> RApiList;

/** 
The base clase for API performance testing. 
The class users should inherit from this class to use the performance test functionality. 

The user needs to provide a number of test methods, which should be a friend to the class. 
Each of this method should have a call to ApiTestStart and ApiTestEnd methods, which 
should enclose particular functionality (API method) for performance testing. It allows to 
performance test only requried functionality leaving out of test scope the test preparation 
and post test activity.

Here is the test method prototype:
void TestAPIMethod()
	{
		// test preparation here
	apiTest->ApiTestStart();
		// call to the API method
	apiTest->ApiTestEnd();
		// post test activity
	}

The framework will call all test methods required number of times for perfromance testing. 
The test results will be shown on the console and logged into result file in "C:\Log" directory. 
*/
class CPerformanceTests : public CBase
	{

public:
	void RunTestsL();
	~CPerformanceTests();

public: 
	/**
	User should implement GetMethodList to create full list of require API method to test
	*/
	virtual void GetMethodListL(RApiList& aApiList) = 0;
	
	/**
	User should implement GetNumberOfRepetition to inform the framework about the number
	of repetitions for performance testing. 
	*/
	virtual TInt GetNumberOfRepetition() = 0;
	
	/**
	User should implement this method for some general purpose test preparation. 
	*/
	virtual void PrepareTestsL() = 0;
	
private:
	void TestApi(TApiRecord& aApi);

protected:
	void ConstructL(const TDesC& aTestName);
	CPerformanceTests();
	void ApiTestStart();
	void ApiTestEnd();
	void ParserComandLineL();
			
private:
	/** The list of methods to perform the test on */
	RApiList list; 		

	/** Output log file handle */
	RFile output; 		
	
	/** RF session for logging */
	RFs session;	
	
	/** Start time of API call */
	TUint64 iStartTime; 

	/** End time of API call */
	TUint64 iEndTime;    

	/** Number of required repetition */
	TInt iRepNum;

	/** Name of outpu file */
	HBufC* iOutputFile;
	};

#endif // TP_FRAMEWORK_H
