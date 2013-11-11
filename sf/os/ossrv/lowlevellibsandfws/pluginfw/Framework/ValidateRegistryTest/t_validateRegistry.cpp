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
// t_validateRegistrty.cpp
// This file contains the code to test the feature of validate registry for ECom when only a
// RSC file is removed. It is for DEF073338: ECom Incorrectly Validates Registry using DLL Existence
// 
//

#include <e32test.h>
#include <e32panic.h>
#include <f32file.h>
#include <bautils.h>
#include "../EcomTestUtils/EcomTestUtils.h"

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#define UNUSED_VAR(a) a = a

// RAM Only RSC and DLL for testing purpose.
_LIT(KEComPluginDLL,		"Z:\\RAMOnly\\DefectPlugin.dll");
_LIT(KEComPluginRSC,		"Z:\\RAMOnly\\DefectPlugin.rsc");

_LIT(KEComPluginDLLOnC,			"C:\\sys\\bin\\DefectPlugin.dll");
_LIT(KEComPluginRSCOnC,			"C:\\Resource\\Plugins\\DefectPlugin.rsc");
// This Rsc file is just used to trigger a ECom rediscovery.
_LIT(KEComInvalidRscOnZ, "z:\\RAMOnly\\InvalidSIDPlugin.rsc");
_LIT(KEComInvalidRscOnC, "c:\\resource\\plugins\\InvalidSIDPlugin.rsc");

const TInt KOneSecond = 1000000;

LOCAL_D RTest test(_L("t_validateRegistry.exe"));

/**
Kill Ecom Server for testing purposes
*/
static void KillEComServerL()
	{
	//Need to ensure that the EComServer process is killed before even starting this test by using
   	//the EComTestUtils library
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAPD(error, EComTestUtils::KillProcessL(KEComServerProcessName));
   	UNUSED_VAR(error);
	}

/**
Copies the Plugins to specific folder for testing purpose
*/
LOCAL_C void CopyPlugin()
	{
	test.Printf(_L("\nCopying plugins into C drive... "));
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginDLL, KEComPluginDLLOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginRSC, KEComPluginRSCOnC));
	test(err==KErrNone);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);
	}

/**
Deletes Plugin from the RAM for cleanup purpose
*/
LOCAL_C void DeletePlugin()
	{
	test.Printf(_L("\nRemoving plugins to clean environment... "));
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginDLLOnC));
	test(err==KErrNone || err==KErrNotFound);
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginRSCOnC));
	test(err==KErrNone || err==KErrNotFound);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1489
@SYMTestCaseDesc		Tests to ensure that only the RSC file is deleted, then the corresponding
						DLL should be deleted from the registry.

@SYMTestPriority		High
@SYMTestActions			First kill the ECom Server, then copy the plugin to C drive.
						Next call REComSession::ListImplementations() to start up ecom server
						and check the discovery result. delete the plugin RSC file, which
						triggers ecom rediscovery.
						Now call REComSession::ListImplementations() and check if the
						plugin is deleted.
@SYMTestExpectedResults	The test must not fail.
*/

/**
The following plugin will be used for test and copied to the C drive.

Interface UID		DLL	UID		Imp. UID	Version		DllFile                                  RSCFile
------------------------------------------------------------------------------------------------------------------------------------------
0x102797A1		0x102797A0	0x102797A2	      1			C:\\sys\\bin\\DefectPlugin.dll      C:\\resource\\plugins\\DefectPlugin.RSC

**/

LOCAL_C void ValidateRegistryAgainstRscL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1489 "));
	__UHEAP_MARK;
	// before strating the test, kill the EcomServer which is probably running from
	// the previous tests.
	KillEComServerL();
	DeletePlugin();
	// copy the plugin to C drive.
	CopyPlugin();

	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;
	// normal discovery, ECom should discover the plugin
	REComSession::ListImplementationsL(interfaceUid, implArray);

	test(implArray.Count()==1);
	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();
	// imp. uid
	test(implUid.iUid == 0x102797A2);
	// version
	test(version == 1);
	// C drive
	test(drive == EDriveC);

	// ECom Server is running.
	// Delete the plugin RSC file but not the corresponding DLL file, ECom rediscovery is triggered.
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComPluginRSCOnC));
	test(err==KErrNone);

	// Give ECOM a chance to do the rediscovery.
	// Otherwise ListImplementationsL could fail.
	User::After(KOneSecond * 3);
	REComSession::ListImplementationsL(interfaceUid, implArray);
	// Since the RSC file is deleted, the corresponding DLL should be deleted
	// from the ECom registry.
	test(implArray.Count()==0);

	//clean up
	implArray.ResetAndDestroy();
	REComSession::FinalClose();
	KillEComServerL();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1490
@SYMTestCaseDesc		Tests to ensure that if the DLL file is deleted and trigger a ECom rediscovery, then the corresponding
						DLL should be deleted from the registry.

@SYMTestPriority		High
@SYMTestActions			First kill the ECom Server, then copy the plugin to C drive.
						Next call REComSession::ListImplementations() to start up ecom server
						and check the discovery result. delete the plugin DLL file, and copy a RSC file to
						c:\resource\plugins\,which triggers ecom rediscovery.
						Now call REComSession::ListImplementations() and check if the
						plugin is deleted.
@SYMTestExpectedResults	The test must not fail.
*/

/**
The following plugin will be used for test and copied to the C drive.

Interface UID		DLL	UID		Imp. UID	Version		DllFile                                  RSCFile
------------------------------------------------------------------------------------------------------------------------------------------
0x102797A1		0x102797A0	0x102797A2	      1			C:\\sys\\bin\\DefectPlugin.dll      C:\\resource\\plugins\\DefectPlugin.RSC

**/
LOCAL_C void ValidateRegistryAgainstDllL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1490 "));
	__UHEAP_MARK;
	// before strating the test, kill the EcomServer which is probably running from
	// the previous tests.
	KillEComServerL();
	// copy the plugin to C drive.
	CopyPlugin();

	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;
	// normal discovery, ECom should discover the plugin
	REComSession::ListImplementationsL(interfaceUid, implArray);

	test(implArray.Count()==1);
	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();
	// imp. uid
	test(implUid.iUid == 0x102797A2);
	// version
	test(version == 1);
	// C drive
	test(drive == EDriveC);

	// ECom Server is running.
	// delete the plugin DLL file not the RSC file, ECom rediscovery is NOT triggered.
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComPluginDLLOnC));
	test(err==KErrNone);

	// copy a RSC file to C:\resource\plugin\ to cause a ECom rediscovery.
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComInvalidRscOnZ, KEComInvalidRscOnC));
	test(err==KErrNone);
	// Give ECOM a chance to do the rediscovery.
	// Otherwise ListImplementationsL could fail.
	User::After(KOneSecond * 3);

	REComSession::ListImplementationsL(interfaceUid, implArray);
	// Since the DLL file is deleted, the corresponding DLL should be deleted
	// from the ECom registry.
	test(implArray.Count()==0);

	//clean up
	implArray.ResetAndDestroy();
	REComSession::FinalClose();
	KillEComServerL();
	__UHEAP_MARKEND;
	}

typedef void (*ClassFuncPtrL) (void);

/**
Wrapper function to call all test functions

@param		testFuncL pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	//Call the test function
	(*testFuncL)();

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}


LOCAL_C void DoTestsL()
	{
	//don't change the order of the tests!
	__UHEAP_MARK;
	// Basic tests
	DoBasicTestL(&ValidateRegistryAgainstRscL, _L("ValidateRegistryAgainstRscL"));
	DoBasicTestL(&ValidateRegistryAgainstDllL, _L("ValidateRegistryAgainstDllL"));
	__UHEAP_MARKEND;
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("Validate Registry Tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,DoTestsL());
	// Cleanup files
	if(err != KErrNone)
		{
		DeletePlugin();
		REComSession::FinalClose();
		}

	test(err==KErrNone);
	DeletePlugin();
    TRAPD(error,EComTestUtils::FileManDeleteFileL(KEComInvalidRscOnC));
    UNUSED_VAR(error);
	delete scheduler;
	delete cleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(0);
	}
