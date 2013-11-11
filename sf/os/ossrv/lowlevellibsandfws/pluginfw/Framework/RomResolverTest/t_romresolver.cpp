// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test assumes the following setup:
// EComRomRslvrExampleOnC on C:
// EComRomRslvrExampleOnZ on Z:
// The tests work by checking that the correct implementations are chosen from two plug-ins, one on C:, one on Z:
// Implementation:		On C:					On Z:								Expected result:
// 10009DC6			version 2				version 1							ROM resolver selects version 2 on C:
// 10009DC7			version 1				version 2							ROM resolver selects version 2 on Z:
// 10009DCF			version 1				-									ROM resolver does not return this
// 
//

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#include "../EcomTestUtils/EcomTestUtils.h"

TUid KUidInterface = {0x10009DC8};
const TInt KUidImplementationAValue = 0x10009DC6;
const TInt KUidImplementationBValue = 0x10009DC7;
const TInt KUidImplementationCValue = 0x10009DCF;
const TInt KOneSecond = 1000000;

LOCAL_D RTest test(_L("t_romresolver.exe"));

_LIT(KEComPlugin2OnZ, "z:\\RAMOnly\\EComRomRslvrExampleOnC.dll");

_LIT(KEComPluginRsc2OnZ, "z:\\RAMOnly\\EComRomRslvrExampleOnC.rsc");
_LIT(KEComPlugin2OnC, "c:\\sys\\bin\\EComRomRslvrExampleOnC.dll");
_LIT(KEComPluginRsc2OnC, "c:\\resource\\plugins\\EComRomRslvrExampleOnC.rsc");


class TheTest
	{
public:
	void StartTestL();
	};

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0663
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for interface implementation data
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDefaultResolverL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0663 TestDefaultResolverL "));

	//
	// Testing a overloaded method for ListImplementationsL
	// ------------------------------------------------------------------
	//

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	RImplInfoPtrArray ifArray;

	TEComResolverParams ResolverParams;
	_LIT8(KImplementationTest,"text/wml");
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);	// Allow wildcard matching

	REComSession::ListImplementationsL(
			KUidInterface,
			ResolverParams,
			KDefaultResolverUid,
			ifArray);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount==3);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"), count+1, info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementationAValue:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationBValue:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationCValue:
				test(info->Version()==1);
				test(info->Drive()==EDriveC);
				break;

			default:
				test.Printf(_L("Unknown implementation UID\n"));
				test(0);
			}
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;								}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0664
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for interface implementation data
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRomResolverL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0664 TestRomResolverL "));

	//
	// Testing a overloaded method for ListImplementationsL
	// ------------------------------------------------------------------
	//

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	RImplInfoPtrArray ifArray;

	TEComResolverParams ResolverParams;
	_LIT8(KImplementationTest,"text/wml");
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetWildcardMatch(ETrue);	// Allow wildcard matching

	REComSession::ListImplementationsL(
			KUidInterface,
			ResolverParams,
			KRomOnlyResolverUid,
			ifArray);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"), count+1, info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementationAValue:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationBValue:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			default:
				test.Printf(_L("Unknown implementation UID\n"));
				test(0);
			}
		}

	// Empty the array of implementations
	test.Printf(_L("Destroying List..."));
	ifArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);
	test.Printf(_L("Great ! No handle mismatch."));

	// Test Ends...

	__UHEAP_MARKEND;								}

LOCAL_C void TestListImplementationL()
	{
	TestDefaultResolverL();
	TestRomResolverL();
	}

void TheTest::StartTestL()
	{
	TestListImplementationL();
	}

TInt DoTestsL()
	{
	// Should any tests leave past the lowest level???
	TheTest theT;
	theT.StartTestL();
	return KErrNone;
	}

void CopyFiles()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComPlugin2OnZ, KEComPlugin2OnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginRsc2OnZ, KEComPluginRsc2OnC));
	test(err==KErrNone);
	}

void CleanupFiles()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComPlugin2OnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginRsc2OnC));
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("Rom resolver tests"));

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();

	CopyFiles();
	//The reason for the folowing delay is:
	//ECOM server could be already started. It means that when we copy some
	//ECOM plugins from Z: to C: drive - ECOM server should look for and
	//find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	//which is an active object. So the discovering service is asynchronous. We have to
	//wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	//requested implementations.
	User::After(KOneSecond * 3);

	TRAPD(err,DoTestsL());

	delete cleanup;

	test(err==KErrNone);

	CleanupFiles();

	test.Next(_L("/n"));
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(0);
	}
