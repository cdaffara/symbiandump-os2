// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains code to test the plugin upgrade feature of ecom as required by CR 65BCA3.
// 
//

#include <e32test.h>
#include <e32panic.h>
#include <f32file.h>
#include <bautils.h>
#include "../EcomTestUtils/EcomTestUtils.h"
#include "Discoverer.h"
#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins

// RAM Only RSC and DLL
_LIT(KEComPluginDLL,		"Z:\\RAMOnly\\DefectPlugin.dll");
_LIT(KEComPluginRSC,		"Z:\\RAMOnly\\DefectPlugin.rsc");

_LIT(KEComPluginDLLOnC,			"C:\\sys\\bin\\DefectPlugin.dll");
_LIT(KEComPluginRSCOnC,			"C:\\Resource\\Plugins\\DefectPlugin.rsc");
_LIT(KClearEComResourceDir,		"C:\\Resource\\");
_LIT(KClearEComSysDir,			"C:\\Sys\\");

LOCAL_D RTest test(_L("Drive Mount Test"));

/**
Copies the Plugins to specific folder for testing purpose
*/
LOCAL_C void CopyPlugin()
	{
	test.Printf(_L("\nCopying plugins into C drive... \n"));
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginDLL, KEComPluginDLLOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginRSC, KEComPluginRSCOnC));
	test(err==KErrNone);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	WAIT_FOR3s;
	}

/**
Deletes Plugin from the RAM for cleanup purpose
*/
LOCAL_C void DeletePlugin()
	{
	test.Printf(_L("\nRemoving plugins to clean environment... \n"));
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginDLLOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginRSCOnC));
	test(err==KErrNone);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	WAIT_FOR3s;
	}

/**
Removes the Plugin folder together with its sub-folders
*/
LOCAL_C void RemoveDir()
	{
	//Remove plugins from the folders before deleting the directory or else
	//error -14 will be returned on the h2!
	DeletePlugin();
	TInt err=KErrNone;
	test.Printf(_L("Removing 'Resource' & 'Sys' folders... \n"));
	TRAP(err, EComTestUtils::FileManDeleteDirL(KClearEComResourceDir));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManDeleteDirL(KClearEComSysDir));
	test(err==KErrNone);

	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	WAIT_FOR3s;
	}


/**
Prepares test environment, before the tests get started. This is to ensure that ecom.lang is created,
which mustn't change during the test run. Otherwise, the test (although it will pass), won't test
the mounting of a drive.
*/
LOCAL_C void PrepareTestEnvironmentL()
	{
	test.Printf(_L("preparing test environment\n"));
	ECOMTU_KILL_SERVER_L(test);
	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;
	//start ecom session, which will create ecom.lang
	REComSession::ListImplementationsL(interfaceUid, implArray);
	implArray.ResetAndDestroy();
	//kill server
	REComSession::FinalClose();
	ECOMTU_KILL_SERVER_L(test);

	//
	//environment ready for the tests to start
	}


/**
Cleans up the test environment after the test...
*/
LOCAL_C void FinalCleanup()
	{
	DeletePlugin(); //remove plugins from the folders
	}


/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1446
@SYMTestCaseDesc		Tests to ensure that the registry index gets updated when mounting
						a drive at boot time
@SYMTestPriority		High
@SYMTestActions			Copy plugin into c-drive.
						Call REComSession::ListImplementations() and check if implementation
						on C drive exists.
@SYMTestExpectedResults	The test must not fail.
*/
LOCAL_C void MountFromBootL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1446 "));
	__UHEAP_MARK;

	//Copy plugins into C drive
	CopyPlugin();

	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and will be discovered by ECOM during startup.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	0x102797A1		0x102797A0	0x102797A2	1			C:\\..\\DefectPlugin.dll

	**/

	// Now start ecom discovery and get implementations for IF UID 0x102797A1
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
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

	//destroy array
	implArray.ResetAndDestroy();
	//kill server
	REComSession::FinalClose();
	ECOMTU_KILL_SERVER_L(test);
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1448
@SYMTestCaseDesc		Tests to ensure that the registry index gets updated when unmounting
						a drive at boot time
@SYMTestPriority		High
@SYMTestActions			Pluginfolder (resource) is removed (to simulate the removal of an mmc card)
						Call REComSession::ListImplementations() and check if implementation
						on C drive exists.
@SYMTestExpectedResults	The test must not fail.
*/
LOCAL_C void UnmountFromBootL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1448 "));
	__UHEAP_MARK;
	/**
	The following plugin is added to the C drive and discovered by ECOM. The Plugin will be dismounted
	and trigger a re-discovery.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	0x102797A1		0x102797A0	0x102797A2	1			C:\\..\\DefectPlugin.dll

	**/
	RemoveDir();

	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;

	// Get implementations for IF UID 0x102797A1
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	test(implArray.Count()==0);

	//destroy array
	implArray.ResetAndDestroy();

	//kill server
	REComSession::FinalClose();
	ECOMTU_KILL_SERVER_L(test);
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1449
@SYMTestCaseDesc		Tests to ensure that the registry index gets updated when mounting
						a drive after boot time
@SYMTestPriority		High
@SYMTestActions			First clean the c-drive by removing the plugin folder
						Next call REComSession::ListImplementations() with any z-drive plugin,
						to start up ecom server and copy the plugins back into the c-drive, which
						triggers ecom discovery.
						Now call REComSession::ListImplementations() and check if implementation
						on C drive exists.
@SYMTestExpectedResults	The test must not fail.
*/
LOCAL_C void MountAfterBootL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1449 "));
	__UHEAP_MARK;
	//start ecom server and prepare test environment...
	CopyPlugin();
	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;
	REComSession::ListImplementationsL(interfaceUid, implArray);
	RemoveDir();

	/**
	The following plugin will now be added to the C drive and ECOM will do a rediscovery and discover it.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	0x102797A1		0x102797A0	0x102797A2	1			C:\\..\\DefectPlugin.dll

	**/
	//ECom server is running... Now test for mount after boot time.
	CopyPlugin();
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

	//destroy array
	implArray.ResetAndDestroy();
	REComSession::FinalClose();
	ECOMTU_KILL_SERVER_L(test);
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1450
@SYMTestCaseDesc		Tests to ensure that the registry index gets updated when unmounting
						a drive after boot time
@SYMTestPriority		High
@SYMTestActions			Remove plugin folders to unmount and call REComSession::ListImplementations()
						to check if implementation on C drive exists.
@SYMTestExpectedResults	The test must not fail.
*/
LOCAL_C void UnmountAfterBootL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1450 "));
	__UHEAP_MARK;
	//start ecom server and prepare test environment...
	CopyPlugin();
	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;
	REComSession::ListImplementationsL(interfaceUid, implArray);

	/**
	The following plugin is added to the C drive and discovered by ECOM. The Plugin will be dismounted
	and trigger a re-discovery.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	0x102797A1		0x102797A0	0x102797A2	1			C:\\..\\DefectPlugin.dll

	**/
	//Ecom server still running...
	RemoveDir();

	// Get implementations for IF UID 0x102797A1
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	test(implArray.Count()==0);

	//destroy array
	implArray.ResetAndDestroy();

	REComSession::FinalClose();
	ECOMTU_KILL_SERVER_L(test);
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
	PrepareTestEnvironmentL();
	// Basic tests
	DoBasicTestL(&MountFromBootL, _L("MountFromBoot"));
	DoBasicTestL(&UnmountFromBootL, _L("UnmountFromBoot"));
	DoBasicTestL(&MountAfterBootL, _L("MountAfterBoot"));
	DoBasicTestL(&UnmountAfterBootL, _L("UnmountAfterBoot"));
	FinalCleanup();
	__UHEAP_MARKEND;
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("Drive Mount Tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,DoTestsL());
	// Cleanup files
	if(err != KErrNone)
		{
		FinalCleanup();
		REComSession::FinalClose();
		}

	test(err==KErrNone);

	delete scheduler;
	delete cleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(0);
	}
