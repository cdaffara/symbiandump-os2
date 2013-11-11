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
// This file contains code to test the EcomCachedDriveInfo class.
// 
//

#include <e32test.h>
#include <f32file.h>
#include <ecom/ecomerrorcodes.h>
#include "EComPatchDataConstantv2.h"
#include "DriveInfo.h"
#include "EComInternalErrorCodes.h"
#define UNUSED_VAR(a) (a = a)

LOCAL_D RTest TheTest(_L("t_driveinfo"));
LOCAL_D RFs	TheFs;
static TInt IteratorPanicTest(TAny* aFuncCode);

const TInt KPanicIndexOutOfBound = 133;
_LIT(KTestFolder,			"C:\\TestTemp\\");

enum TIteratorFunctionToTest
	{
	EIterFunctionDriveUnit,
	EIterFunctionDriveNumber,
	EIterFunctionIsReadOnlyInternal,
	EIterFunctionIsRemovable
	};

class TDriveInfo_StateAccessor 
	{
public:
	static CEComCachedDriveInfo* GetCachedDriveInfoL(RFs& aFs, TUint32 aDrvMask);
	static void EComDrvFlagsL(TInt aDriveNum,
							  TUint32& aDrvFlags,
							  const CEComCachedDriveInfo& aCachedDriveInfo);
	};

/**
Because this class is friend of CEComCachedDriveInfo, it can call the
private constructor of CEComCachedDriveInfo to make object instance
with drive disabled mask different from the patchable constant.
@param aFs Connected RFs session.
@param aDrvMask The discovery disabled drive mask to pass to
	CEComCachedDriveInfo.
@return fully constructed CEComCachedDriveInfo. Caller owns the pointer.
@leave KErrNoMemory if system out of memory.
*/
CEComCachedDriveInfo* TDriveInfo_StateAccessor::GetCachedDriveInfoL(
	RFs&	aFs,
	TUint32 aDrvMask)
	{
	// Set this bool to false otherwise ConstructL will do nothing.
	CEComCachedDriveInfo::iInitialized = EFalse;

	CEComCachedDriveInfo* ptr = new (ELeave) CEComCachedDriveInfo();
	CleanupStack::PushL(ptr);
	ptr->ConstructL(aFs, aDrvMask);
	CleanupStack::Pop();
	return ptr;
	}

/**
static method

Retrieve the flag word stored by CEComCachedDriveInfo about a given drive.
@param aDriveNum the drive of interest
@param aDrvFlags output parameter to return the drive attributes
@param aCachedDriveInfo the object instance to access.
@leave KErrNotFound if no such drive
*/
void TDriveInfo_StateAccessor::EComDrvFlagsL(TInt aDriveNum,
											TUint32& aDrvFlags,
											const CEComCachedDriveInfo& aCachedDriveInfo)
	{
	for (TInt i = 0; i <= aCachedDriveInfo.iLastIndex; i++)
		{
		if (aCachedDriveInfo.iDriveAttr[i].iDrvNumber == aDriveNum)
			{
			aDrvFlags = aCachedDriveInfo.iDriveAttr[i].iFlags;
			return;
			}
		}

	User::Leave(KErrNotFound);
	}

//Test macroses and functions
LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
#define TESTL(arg) ::CheckL((arg), __LINE__)


/** Check CEComCachedDriveInfo has the correct attributes for the given drive
and the iterator will return the drive if discovery is not disabled.
*/
LOCAL_C void VerifyDrvAttributeL(const TInt aDriveNum, 
						  		TUint32 aDisableMask,
							  	const CEComCachedDriveInfo& aCachedDriveInfo)
	{
	TEComCachedDriveInfoIterator iter(aCachedDriveInfo);

	TDriveInfo driveInfo;
	User::LeaveIfError(TheFs.Drive(driveInfo, aDriveNum));

	if (0 == driveInfo.iDriveAtt)
		{
		// Drive not exist, i.e. drive letter not in-used

		TESTL( !iter.SetPos(aDriveNum) );
		return;
		}

	TUint32 expectedAttr = 0;
	if ((driveInfo.iDriveAtt & KDriveAttInternal) &&
		(driveInfo.iMediaAtt & KMediaAttWriteProtected))
		{
		// Drive is ROnly internal which cannot be disabled.
		expectedAttr = EEComDrvAttrReadOnlyInternal;
		}
	else
		{
		TUint32 drvBitMask = 1;
		if ((drvBitMask << aDriveNum) & aDisableMask ||
			driveInfo.iDriveAtt & KDriveAttSubsted ||
			driveInfo.iDriveAtt & KDriveAttRemote)
			{
			expectedAttr |= EEComDrvAttrNoDiscovery;
			}
		
		if (driveInfo.iDriveAtt & KDriveAttRemovable)
			{
			expectedAttr |= EEComDrvAttrRemovable;
			}
		
		if (0 == (driveInfo.iDriveAtt & KDriveAttRom))
			{
			expectedAttr |= EEComDrvAttrWritable;
			}
		}

	// Test iterator does not return disabled drives.
	TBool found = EFalse;
	for (iter.First(); iter.InRange() && !found; iter.Next())
		{
		if (iter.DriveNumber() == aDriveNum)
			{
			found = ETrue;
			}
		}

	TBool expectedFound = !(expectedAttr & EEComDrvAttrNoDiscovery);
	if (found != expectedFound)
		{
		TheTest.Printf(_L("Error drive %d, expected att 0x%X, iter found %d"), aDriveNum, expectedAttr, found);
		}
	TESTL(expectedFound == found);

	// verify drive attributes
	TUint32 actualAttr = 0;
	TDriveInfo_StateAccessor::EComDrvFlagsL(aDriveNum, actualAttr,
		aCachedDriveInfo);
	if (actualAttr != expectedAttr)
		{
		TheTest.Printf(_L("Error drive %d, expected att 0x%X, got 0x%X"), aDriveNum, expectedAttr, actualAttr);
		}
	TESTL(actualAttr == expectedAttr);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3536
@SYMTestCaseDesc		Disable/enable each drive and verify CEComCachedDriveInfo
	has correct attribute for each drive and the iterator will not return
	drives that are disabled, subst or remote.
@SYMTestPriority		High
@SYMTestActions			Instantiate CEComCachedDriveInfo with each drive
	disabled in turn. Verify the attribute and iterator operation on the drive.
	Instantiate CEComCachedDriveInfo with all drive enable. Verify
	attribute and iterator operation on each drive.
@SYMTestExpectedResults	CEComCachedDriveInfo has the expected attributes for
	each drive whethe the drive is enabled or disabled. The iterator will only
	return the drive is the drive is enabled.
@SYMCR					CR1049
*/
LOCAL_C void DriveMaskTestL()
	{
	CEComCachedDriveInfo* cachedDriveInfo;

	// Disable each drive in turn to check that disable works as expected.
	TInt i;
	for (i = EDriveA; i <= EDriveZ; i++)
		{
		TUint32 disableMask = 1 << i;
		cachedDriveInfo = TDriveInfo_StateAccessor::GetCachedDriveInfoL(TheFs, disableMask);
		CleanupStack::PushL(cachedDriveInfo);

		// Check CEComCachedDriveInfo has the expected value.
		VerifyDrvAttributeL(i, disableMask, *cachedDriveInfo);

		// Test CEComCachedDriveInfo::DriveIsReadOnlyInternalL and 
		// DriveIsRemovableL leaving. They should be used on drives that
		// are known to be valid, e.g. drive extracted from the path
		// of a discovered DLL.
		// Since C: is disabled, CEComCachedDriveInfo will leave instead
		// of answering true or false (because the answer is misleading).
		if (i == EDriveC)
			{
			TRAPD(err, cachedDriveInfo->DriveIsReadOnlyInternalL(i) );
			TESTL(err == KEComErrDriveNotFound);

			TRAP(err, cachedDriveInfo->DriveIsRemovableL(i) );
			TESTL(err == KEComErrDriveNotFound);
			}

		CleanupStack::PopAndDestroy(cachedDriveInfo);
		}

	// Test enable case.
	// Make sure the disable mask is zero.
	TESTL(KDiscoveryDisabledDriveList == 0);

	cachedDriveInfo = TDriveInfo_StateAccessor::GetCachedDriveInfoL(TheFs,0);
	CleanupStack::PushL(cachedDriveInfo);

	for (i = EDriveA; i < KMaxDrives; i++)
		{
		VerifyDrvAttributeL(i, 0, *cachedDriveInfo);
		}

	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3537
@SYMTestCaseDesc		Test the CEComCachedDriveInfo and its iterator classes
	handles substituted drives correctly.
@SYMTestPriority		High
@SYMTestActions			Create a substituted drive, instantiate the cached drive info and verify that 
						the sustitued drive is not in the valid drive list.
@SYMTestExpectedResults	Substituted drive is not in the valid drive list.
@SYMCR					CR1049
*/
LOCAL_C void SubstitutedDriveTestL()
	{
	//Create c:\TestTemp folder
	TInt err = TheFs.MkDir(KTestFolder);
	//Create substituted drive L:, it maps to C:\TestTemp\ folder
	err = TheFs.SetSubst(KTestFolder,EDriveL);
	TESTL(err==KErrNone);

	//Verify that L Drive is not in the valid list.	
	CEComCachedDriveInfo* cachedDriveInfo =
		TDriveInfo_StateAccessor::GetCachedDriveInfoL(TheFs,0);
	CleanupStack::PushL(cachedDriveInfo);

	VerifyDrvAttributeL(EDriveL, 0, *cachedDriveInfo);

	CleanupStack::PopAndDestroy(cachedDriveInfo);

	// undo subst
	err = TheFs.SetSubst(KNullDesC, EDriveL);
	TESTL(err==KErrNone);

	err = TheFs.RmDir(KTestFolder);
	TESTL(err==KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3539
@SYMTestCaseDesc		Test the various methods exposed by TEComCachedDriveInfoIterator class.
@SYMTestPriority		High
@SYMTestActions			For each drive returned by the iterator, call all the
	getter methods.
@SYMTestExpectedResults	No leave or panic occur.
@SYMCR					CR1049
*/
LOCAL_C void ExerciseIterator()
	{
	CEComCachedDriveInfo* cachedDriveInfo = CEComCachedDriveInfo::NewL(TheFs);
	CleanupStack::PushL(cachedDriveInfo);

	TEComCachedDriveInfoIterator iter(*cachedDriveInfo);

	for (iter.Last(); iter.InRange(); iter.Prev())
		{
		TDriveNumber drvNum = iter.DriveNumber();
		TDriveUnit drvUnit = iter.DriveUnit();

		// A trivial test just to use the returned objects.
		TESTL(drvNum == drvUnit);

		TBool b = iter.DriveIsReadOnlyInternal();
		UNUSED_VAR(b);

		b = iter.DriveIsRemovable();
		UNUSED_VAR(b);
		}

	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
Intended Usage	: Capture the PANIC that occurs in the thread.
@param			: aName The name to be assigned to this thread. 
@param			: aFunction The function which causes the panic.
*/
LOCAL_C void ThreadPanicTest(const TDesC& aName,TThreadFunction aFunction)
	{
	TheTest.Next(aName);
	TRequestStatus threadStatus;
	RThread thread;
	TBool jit;
	jit=User::JustInTime();
	User::SetJustInTime(EFalse);

	for (TInt i = EIterFunctionDriveUnit; i <= EIterFunctionIsRemovable; i++)
		{
		TIteratorFunctionToTest func = static_cast<TIteratorFunctionToTest>(i);
		TInt err=thread.Create(aName,aFunction,KDefaultStackSize,KMinHeapSize,KMinHeapSize, &func);
		TESTL(err==KErrNone);
	
		thread.Logon(threadStatus);
		thread.Resume();
		User::WaitForRequest(threadStatus);

		//Now check why the thread Exit
		TExitType exitType = thread.ExitType();
		TInt exitReason = thread.ExitReason();
		TheTest.Printf(_L("PanicTest: exitType %d, reason %d\n"), exitType, exitReason);

		TESTL(exitType == EExitPanic);
		TESTL(exitReason == KPanicIndexOutOfBound);
		thread.Close();
		}

	User::SetJustInTime(jit);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3540
@SYMTestCaseDesc		Test the TEComCachedDriveInfoIterator class will panic if object instance of this class access out of bound elements.
@SYMTestPriority		High
@SYMTestActions			Cause the iterator to panic by accessing out of range
	cell.
@SYMTestExpectedResults	Panic occur.
@SYMCR					CR1049
*/
LOCAL_C void DoIteratorPanicTestL(TIteratorFunctionToTest aFuncCode)
	{
	CEComCachedDriveInfo* cachedDriveInfo = CEComCachedDriveInfo::NewL(TheFs);
	CleanupStack::PushL(cachedDriveInfo);

	TEComCachedDriveInfoIterator iter(*cachedDriveInfo);
	// Run the iterator out of range.
	for (iter.Last(); iter.InRange(); iter.Prev())
		{
		}

	// Access the getter function to trigger panic.
	switch (aFuncCode)
		{
		case EIterFunctionDriveUnit:
			{
			TDriveUnit d = iter.DriveUnit();
			break;
			}
		case EIterFunctionDriveNumber:
			{
			TDriveNumber d = iter.DriveNumber();
			break;
			}
		case EIterFunctionIsReadOnlyInternal:
			{
			TBool f = iter.DriveIsReadOnlyInternal();
			break;
			}
		case EIterFunctionIsRemovable:
			{
			TBool f = iter.DriveIsRemovable();
			break;
			}
		default:
			// do nothing and the test will fail.
			break;
		}

	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1485
@SYMTestCaseDesc		Tests EcomCachedDriveInfo class.
@SYMTestPriority			High
@SYMTestActions			Test the various methods exposed by EcomCachedDriveInfo,
						 ensuring that the returned values are correct.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF				 DEF073919
*/
LOCAL_C void SystemDriveTestL()
	{
	CEComCachedDriveInfo* cachedDriveInfo = CEComCachedDriveInfo::NewL(TheFs);
	CleanupStack::PushL(cachedDriveInfo);

	TDriveNumber driveSys = RFs::GetSystemDrive();
	TESTL( !cachedDriveInfo->DriveIsReadOnlyInternalL(driveSys) );
	TESTL(  cachedDriveInfo->DriveIsWritableL(driveSys) );

	// The old EcomCachedDriveInfo class has DriveIsRemoteL and
	// DriveIsSubstL methods. To verify that system drive is neither
	// remote nor subst, use TEComCachedDriveInfoIterator::SetPos().
	
	TEComCachedDriveInfoIterator iter(*cachedDriveInfo);
	TESTL( iter.SetPos(driveSys) );

	// Test Z: drive property
	TESTL( cachedDriveInfo->DriveIsReadOnlyInternalL(EDriveZ) );
	
	CleanupStack::PopAndDestroy(cachedDriveInfo);
	}

/** Setup the TRAP harness to invoke DoIteratorPanicTestL
*/
TInt IteratorPanicTest(TAny* aFuncCode)
	{
	CTrapCleanup* threadcleanup = CTrapCleanup::New();
	TIteratorFunctionToTest* funcCode = static_cast<TIteratorFunctionToTest*>(aFuncCode);
	TRAPD(ret, DoIteratorPanicTestL(*funcCode)); 

	delete threadcleanup;
	return ret;
	}

typedef void (*ClassFuncPtrL) (void);
/**
Test under OOM conditions.
This is a wrapper function to call all test functions.
@param	aTestFunctionL	 Pointer to test function.
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL aTestFunctionL, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);
	TInt err = KErrNone;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
	
		TRAP(err, aTestFunctionL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);
		
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount; 
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		TESTL(startProcessHandleCount == endProcessHandleCount);
		TESTL(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TESTL(err==KErrNone);
	TheTest.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
Wrapper function to call all test functions
@param		aTestFunctionL pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL aTestFunctionL, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);
	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	aTestFunctionL();
	
	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TESTL(startProcessHandleCount == endProcessHandleCount);
	TESTL(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}
	
LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	User::LeaveIfError(TheFs.Connect());
	CleanupClosePushL(TheFs);

	// normal tests
	DoBasicTestL(&DriveMaskTestL, _L("Drive Mask Test."));
	DoBasicTestL(&SubstitutedDriveTestL, _L("Substituted Drive Test."));
	DoBasicTestL(&ExerciseIterator, _L("Getter Test."));
	DoBasicTestL(&SystemDriveTestL, _L("System Drive Test."));

	// panic tests
	ThreadPanicTest(_L("Iterator Panic Testing"),IteratorPanicTest);
	
	// OOM tests
	DoOOMTestL(&DriveMaskTestL, _L("OOM Test for Drive Mask Test."));
	DoOOMTestL(&ExerciseIterator, _L("OOM Test for Getter."));
	DoOOMTestL(&SystemDriveTestL, _L("OOM Test for System Drive Test."));
	
	CleanupStack::PopAndDestroy();
	
	__UHEAP_MARKEND;
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
		
	TheTest.Title();
	TheTest.Start(_L("Start Drive Info Tests."));
	
	User::LeaveIfError (TheFs.Connect ());
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,DoTestsL());
	TESTL(err==KErrNone);
	
	delete scheduler;
	delete cleanup;
	
	TheFs.Close();
	
	TheTest.End();
	TheTest.Close();
	
	__UHEAP_MARKEND;
	return(0);
	}
