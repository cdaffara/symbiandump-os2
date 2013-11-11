// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <centralrepository.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <hal.h>

RTest 				TheTest(_L("t_cenrep_perf test"));
CRepository*		TheRepository = NULL;
const TUid 			KTestCenRepUid = {0xCCCCCC03};
_LIT(KFileName, "c:\\private\\10202be9\\persists\\cccccc03.cre");

///////////////////////////////////////////////////////////////////////////////////////

// Shared test data 

enum TValType {EIntVal, ERealVal, EBinaryVal, EStringVal, EMetaVal};
enum TTestType {ESetTest, ECreateTest, EFindEqTest, EFindNeqTest, EResetTest};

// int
const TUint32 KIntKey = 42011136ul;
const TUint32 KNewIntKey = 2374040320ul;
const TUint32 KNewIntKey2 = 2374040576ul;
const TInt KOldIntVal = 10;
const TInt KNewIntVal = KOldIntVal + 1;
const TUint32 KIntMeta = 0;

// real
const TUint32 KRealKey = 2374041088ul;
const TUint32 KNewRealKey = 2374041344ul;
const TReal KOldRealVal = 0.1;
const TReal KNewRealVal = KOldRealVal + 1.0;

// binary
const TUint32 KBinKey = 42141952ul;
const TUint32 KNewBinKey = 2374040832ul;
_LIT8(KOldBinVal, "\x44\x00\x69\x00\x61\x00\x6c\x00\x4f\x00\x75\x00\x74\x00\x49\x00\x53\x00\x50\x00");
_LIT8(KNewBinVal, "\x44\x00\x69\x00\x61\x00\x6c\x00\x4f\x00\x75\x00\x74\x00\x49\x00\x53\x00\x50\x01");

// string
const TUint32 KStrKey = 2374041600ul;
const TUint32 KNewStrKey = 2374041856ul;
_LIT(KOldStrVal, "hello");
_LIT(KNewStrVal, "goodbye");


// FindL range data
const TUint32 KFindPartialKey = 0x08440000ul;
const TUint32 KFindMask = 0x0FFF0000ul;
const TInt KFindKeyCount = 15;


///////////////////////////////////////////////////////////////////////////////////////

TInt DeleteCreFile()
	{
	RFs fs;
	fs.Connect();
	TInt err = fs.Delete(KFileName);
	fs.Close();

	// it's fine if the file or path wasn't found as there's nothing to 
	// delete so return KErrNone
	return (err == KErrNotFound || err == KErrPathNotFound) ? KErrNone : err;
	}

///////////////////////////////////////////////////////////////////////////////////////

void DestroyTestEnv()
	{
	delete TheRepository;

	// delete the CRE file to clear out any changes made during the test
	DeleteCreFile(); 
	}

///////////////////////////////////////////////////////////////////////////////////////

//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Test failure. Boolean expression evaluates to false.\r\n"));
		TheTest(EFalse, aLine);
		}
	}
void Check2(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

// leaves if it can't successfully delete the cre file if it exists
void CreateTestEnvL()
	{
	// delete the CRE file to clear out any changes leftover from previous test runs
	User::LeaveIfError(DeleteCreFile());
	
	TRAPD(err, TheRepository = CRepository::NewL(KTestCenRepUid));
	TEST2(err, KErrNone);
	}


//Prints aTicks parameter (converted to us)
void PrintStats(TUint32 aStartTicks, TUint32 aEndTicks)
	{
	static TInt freq = 0;
	if(freq == 0)
		{
		TEST2(HAL::Get(HAL::EFastCounterFrequency, freq), KErrNone);
		}
	TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
	if(diffTicks < 0)
		{
		diffTicks = KMaxTUint32 + diffTicks + 1;
		}
	const TInt KMicroSecIn1Sec = 1000000;
	TInt32 us = (diffTicks * KMicroSecIn1Sec) / freq;
	TheTest.Printf(_L("####Execution time: %d us\r\n"), us);
	}

///////////////////////////////////////////////////////////////////////////////////////

// Utility functions containing code common to multiple test cases

/**
 * Common code for calling Get() with old values
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal, EStringVal or EMetaVal-- any other value will return KErrArgument).
 * @param aResult bool indicating whether the comparison was successful
 * @return err code from Get() 
 */
TInt GetOldVal(TValType aValType, TBool& aResult)
	{
	TInt intVal(0);
	TReal realVal(0.0);
	TBuf8<100> binVal;
	TBuf16<100> strVal;
	TUint32 metaVal(0);
	
	TInt err(KErrNone);

	switch (aValType)
		{
		case EIntVal:
			err = TheRepository->Get(KIntKey, intVal);
			aResult = (intVal == KOldIntVal); 
			break;
			
		case ERealVal:
			err = TheRepository->Get(KRealKey, realVal);
			aResult = (realVal == KOldRealVal); 
			break;

		case EBinaryVal:
			err = TheRepository->Get(KBinKey, binVal);
			aResult = (binVal == KOldBinVal); 
			break;

		case EStringVal:
			err = TheRepository->Get(KStrKey, strVal);
			aResult = (strVal == KOldStrVal); 
			break;

		case EMetaVal:
			err = TheRepository->GetMeta(KIntKey, metaVal);
			aResult = (metaVal == KIntMeta);
			break;
			
		default:
			err = KErrArgument;
			break;
		}
	return err;
	}


/**
 * Common code for calling Get() with new values
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal or EStringVal -- any other value will return KErrArgument).
 * @param aTestType enum defining whether Set() or Create() is being tested
 * 	(ESetTest or ECreateTest -- anything else will return KErrArgument). 
 * @param aResult bool indicating whether the comparison was successful
 * @return err code from Get() 
 */
TInt GetNewVal(TValType aValType, TTestType aTestType, TBool& aResult)
	{
	// check correct test type
	if (aTestType != ECreateTest && aTestType != ESetTest)
		{
		return KErrArgument;
		}
	
	TInt intVal(0);
	TReal realVal(0.0);
	TBuf8<100> binVal;
	TBuf16<100> strVal;
	
	TInt err(KErrNone);

	switch (aValType)
		{
		case EIntVal:
			err = TheRepository->Get(
					(aTestType == ECreateTest ? KNewIntKey : KIntKey), intVal);
			aResult = (intVal == KNewIntVal); 
			break;
			
		case ERealVal:
			err = TheRepository->Get(
					(aTestType == ECreateTest ? KNewRealKey : KRealKey), realVal);
			aResult = (realVal == KNewRealVal); 
			break;

		case EBinaryVal:
			err = TheRepository->Get(
					(aTestType == ECreateTest ? KNewBinKey : KBinKey), binVal);
			aResult = (binVal == KNewBinVal); 
			break;

		case EStringVal:
			err = TheRepository->Get(
					(aTestType == ECreateTest ? KNewStrKey : KStrKey), strVal);
			aResult = (strVal == KNewStrVal); 
			break;

		case EMetaVal:
			// no meta testing on new setting so fall through
			
		default:
			err = KErrNotFound;
			break;
		}
	return err;
	}


/**
 * Common code for calling Set()
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal or EStringVal -- any other value will return KErrArgument).
 * @return err code from Set() 
 */
TInt SetNewVal(TValType aValType)
	{
	TInt err(KErrNone);

	switch (aValType)
		{
		case EIntVal:
			err = TheRepository->Set(KIntKey, KNewIntVal);
			break;
			
		case ERealVal:
			err = TheRepository->Set(KRealKey, KNewRealVal);
			break;

		case EBinaryVal:
			err = TheRepository->Set(KBinKey, KNewBinVal);
			break;

		case EStringVal:
			err = TheRepository->Set(KStrKey, KNewStrVal);
			break;

		case EMetaVal:
			// no meta testing on new setting so fall through
			
		default:
			err = KErrArgument;
			break;
		}
	return err;
	}


/**
 * Common code for calling Create()
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal or EStringVal -- any other value will return KErrArgument).
  * @return err code from Create() 
 */
TInt CreateNewSetting(TValType aValType)
	{
	TInt err(KErrNone);

	switch (aValType)
		{
		case EIntVal:
			err = TheRepository->Create(KNewIntKey, KNewIntVal);
			break;
			
		case ERealVal:
			err = TheRepository->Create(KNewRealKey, KNewRealVal);
			break;

		case EBinaryVal:
			err = TheRepository->Create(KNewBinKey, KNewBinVal);
			break;

		case EStringVal:
			err = TheRepository->Create(KNewStrKey, KNewStrVal);
			break;

		case EMetaVal:
			// no meta testing on new setting so fall through
			
		default:
			err = KErrArgument;
			break;
		}
	return err;
	}


/**
 * Common code for calling Reset()
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal or EStringVal -- any other value will return KErrArgument).
  * @return err code from Reset() 
 */
TInt ResetSetting(TValType aValType)
	{
	TInt err(KErrNone);

	switch (aValType)
		{
		case EIntVal:
			err = TheRepository->Reset(KIntKey);
			break;
			
		case ERealVal:
			err = TheRepository->Reset(KRealKey);
			break;

		case EBinaryVal:
			err = TheRepository->Reset(KBinKey);
			break;

		case EStringVal:
			err = TheRepository->Reset(KStrKey);
			break;

		case EMetaVal:
			// no meta testing on new setting so fall through
			
		default:
			err = KErrArgument;
			break;
		}
	return err;
	}


/**
 * Common code for calling FindL()
 * 
 * @param aStart Out parameter for start timer value
 * @param aEnd Out parameter for end timer value
 * @param aFound Out parameter for number of settings found
 * @return err code from FindL()
 */
TInt FindRangeL(TInt& aStart, TInt& aEnd, TInt& aFound)
	{
	RArray<TUint32> keys;
	CleanupClosePushL(keys);
	
	aStart = User::FastCounter();
	TInt err = TheRepository->FindL(KFindPartialKey, KFindMask, keys);
	aEnd = User::FastCounter();
	
	aFound = keys.Count();
	
	CleanupStack::PopAndDestroy(&keys);

	return err;
	}


/**
 * Common code for performing all the Get() tests
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal, EStringVal or EMetaVal -- any other value will fail with KErrArgument).
 * @param aUseTransaction bool instructing whether to use a read-mode transaction or not
 */
void DoGetTest(TValType aValType, TBool aUseTransaction = EFalse)
	{
	TInt err(KErrNone);
	TInt result(EFalse);
	TUint32 keyInfo(0);

	TUint32 start = User::FastCounter();
	
	// start transaction, if required
	err = aUseTransaction ? TheRepository->StartTransaction(CRepository::EReadTransaction) : err;
	TEST2(err, KErrNone);

	err = GetOldVal(aValType, result);
	TEST2(err, KErrNone);

	// end transaction, if required
	err = aUseTransaction ? TheRepository->CommitTransaction(keyInfo) : err;
	TEST2(err, KErrNone);

	TUint32 end = User::FastCounter();
	
	TEST(result);
	
	PrintStats(start, end);
	}


/**
 * Common code for performing the Set() and Reset() tests for all datatypes
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal or EStringVal -- any other value will fail with KErrArgument).
 * @param aTestType enum defining whether Set() or Reset() should be timed
 */
void DoSetResetTest(TValType aValType, TTestType aTestType)
	{
	TInt err(KErrNone);
	TInt result(EFalse);

	// Check we get the old expected value
	err = GetOldVal(aValType, result);
	TEST2(err, KErrNone);
	TEST(result);

	TUint32 start(0);
	TUint32 end(0);

	// Set the new value
	start = aTestType == ESetTest ? User::FastCounter() : start;
	err = SetNewVal(aValType);
	end = aTestType == ESetTest ? User::FastCounter() : end;
	TEST2(err, KErrNone);

	// Test we get the new value to check it's worked
	err = GetNewVal(aValType, ESetTest, result);
	TEST2(err, KErrNone);
	TEST(result);

	// Restore the old value
	start = aTestType == EResetTest ? User::FastCounter() : start;
	err = ResetSetting(aValType);
	end = aTestType == EResetTest ? User::FastCounter() : end;
	TEST2(err, KErrNone);
	
	// Check reset's worked
	err = GetOldVal(aValType, result);
	TEST2(err, KErrNone);
	TEST(result);
	
	PrintStats(start, end);
	}


/**
 * Common code for performing all the Create() tests
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal, EStringVal, EMetaVal -- any other value will fail with KErrArgument).
 */
void DoCreateTest(TValType aValType)
	{
	TInt err(KErrNone);
	TInt result(EFalse);

	TUint32 start(0);
	TUint32 end(0);

	// Create the new setting
	start = User::FastCounter();
	err = CreateNewSetting(aValType);
	end = User::FastCounter();
	TEST2(err, KErrNone);

	// Test we get the right value from the new setting to check it's worked
	err = GetNewVal(aValType, ECreateTest, result);
	TEST2(err, KErrNone);
	TEST(result);

	PrintStats(start, end);
	}


/**
 * Common code for performing all the FindEqL() and FindNeqL() tests
 * 
 * @param aValType enum indicating which data type to test
 * 	(EIntVal, ERealVal, EBinaryVal, EStringVal, EMetaVal -- any other value will fail with KErrArgument).
 * @param aTestType enum defining whether FindEqL() or FindNeqL() should be timed
 * 	(EFindEqTest or EFindNeqTest -- anything else will leave with KErrArgument). 
 */
void DoFindEqNeqTestL(TValType aValType, TTestType aTestType)
	{
	const TUint32 KPartialKey(0x8D800000);
	const TUint32 KMask(0xFFF00000);
	const TInt KFindNum(1); 
	const TInt KNotFindNum(15);
	
	const TInt KIntFindVal(9999);
	const TReal KRealFindVal(999.9);
	_LIT8(KBinFindVal, "\xab\xcd\x99\x99");
	_LIT(KStrFindVal, "abcd9999");

	RArray<TUint32> results;
	CleanupClosePushL(results);
	
	TInt expectedCount(0);
	switch(aTestType)
		{
		case EFindEqTest:
			expectedCount = KFindNum;
			break;

		case EFindNeqTest:
			expectedCount = KNotFindNum;
			break;

		default:
			// wrong test type passed in
			User::Leave(KErrArgument);
		}

	TInt err(KErrNone);
	
	TUint32 start = User::FastCounter();
	switch (aValType)
		{
		case EIntVal:
			err = (aTestType == EFindEqTest) ?
					TheRepository->FindEqL(KPartialKey, KMask, KIntFindVal, results) :
					TheRepository->FindNeqL(KPartialKey, KMask, KIntFindVal, results);
			break;
			
		case ERealVal:
			err = (aTestType == EFindEqTest) ?
					TheRepository->FindEqL(KPartialKey, KMask, KRealFindVal, results) :
					TheRepository->FindNeqL(KPartialKey, KMask, KRealFindVal, results);
			break;

		case EBinaryVal:
			err = (aTestType == EFindEqTest) ?
					TheRepository->FindEqL(KPartialKey, KMask, KBinFindVal, results) :
					TheRepository->FindNeqL(KPartialKey, KMask, KBinFindVal, results);
			break;

		case EStringVal:
			err = (aTestType == EFindEqTest) ?
					TheRepository->FindEqL(KPartialKey, KMask, KStrFindVal, results) :
					TheRepository->FindNeqL(KPartialKey, KMask, KStrFindVal, results);
			break;

		case EMetaVal:
			// no meta testing on new setting so fall through
			
		default:
			err = KErrArgument;
			break;
		}
	TUint32 end = User::FastCounter();
	
	// Test there was no error and that we found the expected number
	TEST2(err, KErrNone);
	TEST(results.Count() == expectedCount);

	CleanupStack::PopAndDestroy(&results);

	PrintStats(start, end);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// TEST CASES

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-UT-4047
@SYMTestCaseDesc		CRepository::Get(TUint32 aKey, TInt& aValue) - performance test
						The test measures the time needed for retrieving of a single integer setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Get(TUint32 aKey, TInt& aValue) - performance test
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF128986
*/
void GetIntTest()
	{
	DoGetTest(EIntVal);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4087
@SYMTestCaseDesc		CRepository::Get(TUint32 aKey, TReal& aValue) - performance test
						The test measures the time needed for retrieving of a single real setting,
						witht the Get() wrapped in a read-mode transaction.
@SYMTestPriority		High
@SYMTestActions			CRepository::Get(TUint32 aKey, TReal& aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void GetRealTest()
	{
	DoGetTest(ERealVal, ETrue);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-UT-4048
@SYMTestCaseDesc		CRepository::Get(TUint32 aKey, TDes8& aValue) - performance test
						The test measures the time needed for retrieving of a single 8-bit string setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Get(TUint32 aKey, TDes8& aValue) - performance test
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF128986
*/
void GetBinaryTest()
	{
	DoGetTest(EBinaryVal);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4088
@SYMTestCaseDesc		CRepository::Get(TUint32 aKey, TDes8& aValue) - performance test
						The test measures the time needed for retrieving of a single 8-bit string setting,
						with the Get() wrapped in a read-mode transaction.
@SYMTestPriority		High
@SYMTestActions			CRepository::Get(TUint32 aKey, TDes8& aValue)
						CRepository::StartTransaction(TTransactionMode aMode)
						CRepository::CommitTransaction(TUint32 &aKeyInfo)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void GetBinaryInTransactionTest()
	{
	DoGetTest(EBinaryVal, ETrue);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4089
@SYMTestCaseDesc		CRepository::Get(TUint32 aKey, TDesC16& aValue) - performance test
						The test measures the time needed for retrieving of a single 16-bit string setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Get(TUint32 aKey, TDesC16& aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void GetStringTest()
	{
	DoGetTest(EStringVal, ETrue);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4090
@SYMTestCaseDesc		CRepository::GetMeta(TUint32 aKey, TUint32& aMeta) - performance test
						The test measures the time needed for retrieving metadata for a single setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::GetMeta(TUint32 aKey, TUint32& aMeta)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void GetMetaTest()
	{
	DoGetTest(EMetaVal, ETrue);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4091
@SYMTestCaseDesc		CRepository::Set(TUint32 aKey, TInt aValue) - performance test
						The test measures the time needed for setting a single integer setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Set(TUint32 aKey, TInt aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void SetIntTest()
	{
	DoSetResetTest(EIntVal, ESetTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4092
@SYMTestCaseDesc		CRepository::Set(TUint32 aKey, TReal aValue) - performance test
						The test measures the time needed for setting a single real setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Set(TUint32 aKey, TReal aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void SetRealTest()
	{
	DoSetResetTest(ERealVal, ESetTest);
	}

	
/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4093
@SYMTestCaseDesc		CRepository::Set(TUint32 aKey, TDes8& aValue) - performance test
						The test measures the time needed for setting a single 8-bit string setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Set(TUint32 aKey, TDes8& aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void SetBinaryTest()
	{
	DoSetResetTest(EBinaryVal, ESetTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4094
@SYMTestCaseDesc		CRepository::Set(TUint32 aKey, TDesC16& aValue) - performance test
						The test measures the time needed for setting a single 16-bit string setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Set(TUint32 aKey, TDesC16& aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void SetStringTest()
	{
	DoSetResetTest(EStringVal, ESetTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4095
@SYMTestCaseDesc		CRepository::Create(TUint32 aKey, TInt aValue) - performance test
						The test measures the time needed for creating a new single integer setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Create(TUint32 aKey, TInt aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void CreateIntTest()
	{
	DoCreateTest(EIntVal);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4096
@SYMTestCaseDesc		CRepository::Create(TUint32 aKey, TReal aValue) - performance test
						The test measures the time needed for creating a new single real setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Create(TUint32 aKey, TReal aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void CreateRealTest()
	{
	DoCreateTest(ERealVal);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4097
@SYMTestCaseDesc		CRepository::Create(TUint32 aKey, TDesC8& aValue) - performance test
						The test measures the time needed for creating a new single 8-bit string setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Create(TUint32 aKey, TDesC8& aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void CreateBinaryTest()
	{
	DoCreateTest(EBinaryVal);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4098
@SYMTestCaseDesc		CRepository::Create(TUint32 aKey, TDesC16& aValue) - performance test
						The test measures the time needed for creating a new single 16-bit string setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Create(TUint32 aKey, TDesC16& aValue)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void CreateStringTest()
	{
	DoCreateTest(EStringVal);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-UT-4049
@SYMTestCaseDesc		CRepository::FindL(TUint32 aPartialKey, TUint32 aMask, RArray<TUint32>& aFoundKeys) - performance test
						The test measures the time needed for retrieving of an array of keys matching the function arguments.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindL(TUint32 aPartialKey, TUint32 aMask, RArray<TUint32>& aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMDEF					DEF128986
*/
void FindTestL()
	{
	TInt start(0);
	TInt end(0);
	TInt found(0);
	
	TInt err = FindRangeL(start, end, found);
		
	TEST2(err, KErrNone);
	TEST2(found, KFindKeyCount);

	PrintStats(start, end);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4099
@SYMTestCaseDesc		CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TInt aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find an int setting in an "equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TInt aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindEqIntTestL()
	{
	DoFindEqNeqTestL(EIntVal, EFindEqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4100
@SYMTestCaseDesc		CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TReal aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find a real setting in an "equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TReal aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindEqRealTestL()
	{
	DoFindEqNeqTestL(ERealVal, EFindEqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4101
@SYMTestCaseDesc		CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TDesC8& aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find an 8-bit string setting in an "equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TDesC8& aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindEqBinaryTestL()
	{
	DoFindEqNeqTestL(EBinaryVal, EFindEqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4102
@SYMTestCaseDesc		CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TDesC16& aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find an 16-bit string setting in an "equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask, TDesC16& aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindEqStringTestL()
	{
	DoFindEqNeqTestL(EStringVal, EFindEqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4103
@SYMTestCaseDesc		CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TInt aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find an int setting in an "not equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TInt aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindNeqIntTestL()
	{
	DoFindEqNeqTestL(EIntVal, EFindNeqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4104
@SYMTestCaseDesc		CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TReal aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find a real setting in an "not equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TReal aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindNeqRealTestL()
	{
	DoFindEqNeqTestL(ERealVal, EFindNeqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4105
@SYMTestCaseDesc		CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TDesC8& aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find an 8-bit string setting in an "not equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TDesC8& aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindNeqBinaryTestL()
	{
	DoFindEqNeqTestL(EBinaryVal, EFindNeqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4106
@SYMTestCaseDesc		CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TDesC16& aValue, RArray< TUint32 > &aFoundKeys) - performance test
						The test measures the time to find an 16-bit string setting in an "not equals" search on a range.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask, TDesC16& aValue, RArray< TUint32 > &aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void FindNeqStringTestL()
	{
	DoFindEqNeqTestL(EStringVal, EFindNeqTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4107
@SYMTestCaseDesc		Performance test that measures the time taken to retrieve data to rebuild a table of ints with 22 rows 
						and 8 columns, using successive calls of Find() with subsequent calls to Get for each key retrieved.
@SYMTestPriority		High
@SYMTestActions			CRepository::FindL(TUint32 aPartialKey, TUint32 aMask, RArray<TUint32>& aFoundKeys)
						CRepository::Get(TUint32 aPartialKey, TUint32 aMask, RArray<TUint32>& aFoundKeys)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void RetrieveTableTestL()
	{
	TUint32 partialKeyData[] = {
		0x03950000, 0x03980000, 0x039A0000, 0x039E0000, 0x03A80000, 0x03A90000,
		0x03AA0000, 0x03AB0000, 0x03B50000, 0x03B60000, 0x03B70000, 0x03B80000,
		0x03B90000, 0x03BA0000, 0x03BB0000, 0x03BC0000, 0x03BD0000, 0x03BE0000,
		0x03BF0000, 0x03C00000, 0x03C10000, 0x03C20000
		};
	const TInt KNumRows(22); // same as number of partialKeys in array above
	const TInt KNumCols(8); // expected number of columns per row
	const TUint32 KMask(0xFFFF0000); // match top 2 bytes

	RArray<TUint32> rowKeys(KNumCols); // set granularity to num cols
	CleanupClosePushL(rowKeys);
	
	TUint32 start = User::FastCounter();

	for (TInt i(0); i < KNumRows; ++i)
		{
		// fetch all keys representing columns/cells in the row 
		// matching the key range defined by the partial key
		TInt findErr = TheRepository->FindL(partialKeyData[i], KMask, rowKeys);
		TEST2(findErr, KErrNone);
		
		const TInt KColCount = rowKeys.Count();
		TEST(KColCount == KNumCols);
		
		// retrieve data for each column/cell represented 
		// by a key from the row key range 
		TInt value(KErrNotFound);
		for (TInt j(0); j < KColCount; ++j)
			{
			TInt getErr = TheRepository->Get(rowKeys[j], value);
			TEST2(getErr, KErrNone);
			
			// data for these cells are 0 or 1
			TEST(value == 0 || value == 1);
			}

		// clear our keys array for next row
		rowKeys.Reset();
		}

	TUint32 end = User::FastCounter();
	PrintStats(start, end);

	CleanupStack::PopAndDestroy(&rowKeys);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4108
@SYMTestCaseDesc		CRepository::Move(TUint32 aSourcePartialKey, TUint32 aTargetPartialKey, TUint32 aMask, TUint32 &aErrorKey) - performance test
						The test measures the time needed for moving a single integer setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Move(TUint32 aSourcePartialKey, TUint32 aTargetPartialKey, TUint32 aMask, TUint32 &aErrorKey)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void MoveTest()
	{
	TUint32 errorKey(0);
	
	// Move a setting to a new key
	TUint32 start = User::FastCounter();
	TInt err = TheRepository->Move(KNewIntKey, KNewIntKey2, 0xFFFFFFFF, errorKey);
	TUint32 end = User::FastCounter();
	TEST2(err, KErrNone);

	// Test we get the right value from the new location
	TInt val(0);
	err = TheRepository->Get(KNewIntKey2, val);
	TEST2(err, KErrNone);
	TEST2(val, KNewIntVal);

	// Test we get the KErrNotFound from the old location
	err = TheRepository->Get(KNewIntKey, val);
	TEST2(err, KErrNotFound);
	
	PrintStats(start, end);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4109
@SYMTestCaseDesc		CRepository::Delete(TUint32 aKey) - performance test
						The test measures the time needed for deleting a single setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Delete(TUint32 aKey)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void DeleteTest()
	{
	// Test the setting is there and we get the right value 
	TInt val(0);
	TInt err = TheRepository->Get(KNewIntKey2, val);
	TEST2(err, KErrNone);
	TEST2(val, KNewIntVal);

	// Delete the setting
	TUint32 start = User::FastCounter();
	err = TheRepository->Delete(KNewIntKey2);
	TUint32 end = User::FastCounter();
	TEST2(err, KErrNone);

	// Test we get the KErrNotFound from the key now
	err = TheRepository->Get(KNewIntKey2, val);
	TEST2(err, KErrNotFound);
	
	PrintStats(start, end);
	}

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4110
@SYMTestCaseDesc		CRepository::Delete(TUint32 aKey) - performance test
						The test measures the time needed for deleting a single setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Delete(TUint32 aKey)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void DeleteRangeTestL()
	{
	TInt start(0);
	TInt end(0);
	TInt found(0);
	
	// Check we can find the settings in the range
	// (ignore timing data here as we're not timing find)
	TInt err = FindRangeL(start, end, found);
	TEST2(err, KErrNone);
	TEST2(found, KFindKeyCount);

	// Delete the setting
	TUint32 errorKey(0);
	start = User::FastCounter();
	err = TheRepository->Delete(KFindPartialKey, KFindMask, errorKey);
	end = User::FastCounter();
	TEST2(err, KErrNone);

	// Check we *can't* find the settings in the range now they've been deleted
	err = FindRangeL(start, end, found);
	TEST2(err, KErrNotFound);
	
	PrintStats(start, end);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4111
@SYMTestCaseDesc		CRepository::Reset(TUint32 aKey) - performance test
						The test measures the time needed for resetting a single integer setting.
@SYMTestPriority		High
@SYMTestActions			CRepository::Reset(TUint32 aKey)
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void ResetTest()
	{
	DoSetResetTest(EIntVal, EResetTest);
	}


/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4112
@SYMTestCaseDesc		CRepository::Reset() - performance test
						The test measures the time needed for resetting the whole keyspace.
@SYMTestPriority		High
@SYMTestActions			CRepository::Reset()
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2505
@SYMREQ					REQ13142
*/
void ResetAllTest()
	{
	// Check we can get the value of the newly created binary setting
	TBool result;
	TInt err = GetNewVal(EBinaryVal, ECreateTest, result);
	TEST2(err, KErrNone);
	TEST(result);
	
	// Reset the whole keyspace
	TInt start = User::FastCounter();
	TheRepository->Reset();
	TInt end = User::FastCounter();
	TEST2(err, KErrNone);

	// Check we *can't* get the value of the newly created binary setting
	// since it shouldn't exist anymore after the Reset()
	err = GetNewVal(EBinaryVal, ECreateTest, result);
	TEST2(err, KErrNotFound);
	PrintStats(start, end);
	}

///////////////////////////////////////////////////////////////////////////////////////

// MAIN

void DoTestsL()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4047 Get Int test"));
	GetIntTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4087 Get Real test"));
	GetRealTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4048 Get Binary test"));
	GetBinaryTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4088 Get Binary in Transaction test"));
	GetBinaryInTransactionTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4089 Get String test"));
	GetStringTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4090 GetMeta test"));
	GetMetaTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4091 Set Int test"));
	SetIntTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4092 Set Real test"));
	SetRealTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4093 Set Binary test"));
	SetBinaryTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4094 Set String test"));
	SetStringTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4095 Create Int setting test"));
	CreateIntTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4096 Create Real setting test"));
	CreateRealTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4097 Create Binary setting test"));
	CreateBinaryTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4098 Create String setting test"));
	CreateStringTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4049 Find test"));
	FindTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4099 FindEq Int test"));
	FindEqIntTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4100 FindEq Real test"));
	FindEqRealTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4101 FindEq Binary test"));
	FindEqBinaryTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4102 FindEq String test"));
	FindEqStringTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4103 FindNeq Int test"));
	FindNeqIntTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4104 FindNeq Real test"));
	FindNeqRealTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4105 FindNeq Binary test"));
	FindNeqBinaryTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4106 FindNeq String test"));
	FindNeqStringTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4107 Retrieve structured data table test"));
	RetrieveTableTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4108 Move test"));
	MoveTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4109 Delete test"));
	DeleteTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4110 Delete test"));
	DeleteRangeTestL();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4111 Reset Single test"));
	ResetTest();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4112 Reset All test"));
	ResetAllTest();
	}


TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	TRAPD(err, CreateTestEnvL());
	TEST2(err, KErrNone);

	TRAP(err, DoTestsL());
	DestroyTestEnv();
	TEST2(err, KErrNone);
	
	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
