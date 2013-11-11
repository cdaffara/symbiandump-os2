// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EComRomOnlyExampleOnC/10009DB6 on C:
// EComRomOnlyExampleOnZ/10009DB5 on Z:
// The tests work by checking that the correct implementations are chosen from two plug-ins, one on C:, one on Z:
// Implementation:		On C:					On Z:								Expected result:
// 10009DCA			version 2				version 1, not ROM-only				ECom selects version 2 on C:
// 10009DCB			version 2, ROM-only	f	version 1, not ROM-only				ECom selects version 1 on Z:
// 10009DCC			version 1				version 2, not ROM-only				ECom selects version 2 on Z:
// 10009DCD			version 2				version 1, ROM-only					ECom selects version 1 on Z:
// 10009DCE			version 1				version 2, ROM-only					ECom selects version 2 on Z:
// 10009DCF			version 3				version 1 & version 2, ROM-only		ECom selects version 2 on Z:
// 
//


#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#include "../EcomTestUtils/EcomTestUtils.h"

const TUid KUidInterface = {0x10009DC9};
const TInt KUidImplementationAValue = 0x10009DCA;
const TInt KUidImplementationBValue = 0x10009DCB;
const TInt KUidImplementationCValue = 0x10009DCC;
const TInt KUidImplementationDValue = 0x10009DCD;
const TInt KUidImplementationEValue = 0x10009DCE;
const TInt KUidImplementationFValue = 0x10009DCF;
const TInt KOneSecond = 1000000;

LOCAL_D RTest test(_L("t_romonly.exe"));

_LIT(KEComPluginOnZ, "z:\\RAMOnly\\EComRomOnlyExampleOnC.dll");

_LIT(KEComPluginOnC, "c:\\sys\\bin\\EComRomOnlyExampleOnC.dll");
_LIT(KEComPluginRscOnZ, "z:\\RAMOnly\\EComRomOnlyExampleOnC.rsc");
_LIT(KEComPluginRscOnC, "c:\\resource\\plugins\\EComRomOnlyExampleOnC.rsc");

class TheTest
	{
public:
	void StartTestL();
	};

void CopyFiles()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComPluginOnZ, KEComPluginOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginRscOnZ, KEComPluginRscOnC));
	test(err==KErrNone);
	}

void CleanupFiles()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComPluginOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginRscOnC));
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0665
@SYMTestCaseDesc	    Tests for REComSession::ListImplementationsL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Checks that the correct implementations are chosen from two plug-ins, one on C:, one on Z:
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestImplementationsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0665 TestImplementationsL "));

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
	test(availCount == 6);

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
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationCValue:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationDValue:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationEValue:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementationFValue:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;
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

	__UHEAP_MARKEND;
	}

void TheTest::StartTestL()
	{
	TestImplementationsL();
	}

TInt DoTestsL()
	{
	// Should any tests leave past the lowest level???
	TheTest theT;
	theT.StartTestL();
	return KErrNone;
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
