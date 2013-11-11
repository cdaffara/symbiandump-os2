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
// Contains tests with invalid/missing plugin DLLs
// 
//


#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#include "../EcomTestUtils/EcomTestUtils.h"

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

LOCAL_D RTest test(_L("t_plugindiscovery.exe"));

LOCAL_D CTrapCleanup* 	  TheTrapCleanup 	 = NULL;

LOCAL_D CActiveScheduler* TheActiveScheduler = NULL;

LOCAL_D RFs					TheFs;

// Implementaion ID used for testing
const TUid KUidTestImplementation = {0x101F847C};


//It is used by some test methods which are called two times:
//from normal test and from OOM test.
static void LeaveIfErrNoMemory(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		REComSession::FinalClose();
		User::Leave(aError);
		}
	}


// Plugins used in tests.
_LIT(KEComInvalidDllOnZ, "z:\\RAMOnly\\InvalidSIDPlugin.dll");
_LIT(KEComInvalidRscOnZ, "z:\\RAMOnly\\InvalidSIDPlugin.rsc");
_LIT(KEComInvalidDllOnC, "c:\\sys\\bin\\InvalidSIDPlugin.dll");
_LIT(KEComInvalidRscOnC, "c:\\resource\\plugins\\InvalidSIDPlugin.rsc");

_LIT(KEComExample5RscOnZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KEComExample5RscOnC, "c:\\resource\\plugins\\EComExample5.rsc");
_LIT(KEComRomRslvrExampleRscOnCRomLocation, "z:\\RAMOnly\\EComRomRslvrExampleOnC.RSC");
_LIT(KEComRomRslvrExampleRscOnCRamLocation, "c:\\resource\\plugins\\EComRomRslvrExampleOnZ.RSC");

// Plugins used in plugins directory removing test
_LIT(KEComDefectPluginDLLOnZ,			"Z:\\RAMOnly\\DefectPlugin.dll");
_LIT(KEComDefectPluginRSCOnZ,			"Z:\\RAMOnly\\DefectPlugin.rsc");
_LIT(KEComDefectPluginDLLOnC,			"C:\\sys\\bin\\DefectPlugin.dll");
_LIT(KEComDefectPluginRSCOnC,			"C:\\Resource\\Plugins\\DefectPlugin.rsc");
_LIT(KEComResourcePluginDirNameOnC,		"C:\\Resource\\Plugins\\");
_LIT(KEComResourcePluginDirNameOffOnC,	"C:\\Resource\\PluginsNameOff\\");

/**
@SYMTestCaseID SYSLIB-ECOM-CT-0030
@SYMTestCaseDesc Test that an orphaned resource file does not create a registry
				 entry.
@SYMTestPriority High
@SYMTestActions  EComExample5.rsc file copied to C: drive.
				 Check call to REComSession::CreateImplementationL() via test
				 CExampleInterface class fails to return the implementation.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3846
*/
LOCAL_C void TestOrphanedRscFileL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0030 "));
	__UHEAP_MARK;

	// Copy plugin
	TRAPD(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample5RscOnC));
	ignoreErr = ignoreErr;

	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExample5RscOnZ, KEComExample5RscOnC));
	test(err == KErrNone);

	// The reason for the folowing delay is:
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that an active object,
	// which scans plugin directories. So the discovering service is asynchronous.
	// We have to wait some time until it finishes.
	// Otherwise CreateImplementationsL could fail to create requested implementations.
	WAIT_FOR3s;

	CExampleInterface* interfaceimpl = NULL;
	TRAP(err, interfaceimpl = CExampleInterface::NewL2(KUidTestImplementation));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNotFound);
	CleanupStack::PushL(interfaceimpl);

	CleanupStack::PopAndDestroy(interfaceimpl);
	REComSession::FinalClose();

	// Delete plugin
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample5RscOnC));
	test(err == KErrNone);
	__UHEAP_MARK;
	}


/**
@SYMTestCaseID SYSLIB-ECOM-CT-1404
@SYMTestCaseDesc Test that an orphaned resource file does not create a registry entry.
			and that other valid implementations are discovered instead.
@SYMTestPriority High
@SYMTestActions  Copy EComRomRslvrExampleOnC to the plug-ins directory on the C drive, without the associated DLL
	Because the DLL is not there, Ecom should fall back to the implementations on the Z drive in EComRomRslvrExampleOnZ
@SYMTestExpectedResults ECom plugins with invalid DLLs fall back to the correct implementations.
@SYMPREQ	PREQ1192
*/
LOCAL_C void TestOrphanedRscFileFallBackL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1404 "));
	__UHEAP_MARK;

	// Copy plugin resource file and NOT the associated DLL
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComRomRslvrExampleRscOnCRomLocation, KEComRomRslvrExampleRscOnCRamLocation));
	test(err == KErrNone);

	// The reason for the folowing delay is:
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that an active object,
	// which scans plugin directories. So the discovering service is asynchronous.
	// We have to wait some time until it finishes.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	WAIT_FOR3s;

	TUid romRslvrExampleInterfaceUid = {0x10009DC8};
	RImplInfoPtrArray ifArray;
	TRAP(err, REComSession::ListImplementationsL(romRslvrExampleInterfaceUid, ifArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	// Go through the implementations for the given interface, and find the ones that match up with
	// EComRomRslvrExampleRscOnZ (and not EComRomRslvrExampleRscOnC becuase it's DLL is missing)
	// EComRomRslvrExampleOnZ contains the implementation 0x10009DC7v2 and 0x10009DC6v1 (these should be found)
	// EComRomRslvrExampleOnC contains the implementation 0x10009DC7v1 and 0x10009DC6v2 (these should NOT be returned becuase of the missing DLL)
	TBool foundRightImplementation1 = EFalse;
	TBool foundRightImplementation2 = EFalse;
	for(TInt index = 0; index < ifArray.Count(); ++index)
		{
		TUid uid = ifArray[index]->ImplementationUid();
		TInt version = ifArray[index]->Version();
		TInt drive = ifArray[index]->Drive();
		if (uid.iUid == 0x10009DC6)
			{
			test(version == 1);
			test(drive==EDriveZ);
			foundRightImplementation1 = ETrue;
			}
		if (uid.iUid == 0x10009DC7)
			{
			test(version == 2);
			test(drive==EDriveZ);
			foundRightImplementation2 = ETrue;
			}
		}
	test(foundRightImplementation1 && foundRightImplementation2);

	ifArray.ResetAndDestroy();
	REComSession::FinalClose();

	// Delete plugin
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRomRslvrExampleRscOnCRamLocation));
	test(err == KErrNone);
	__UHEAP_MARK;
	}


/**
@SYMTestCaseID SYSLIB-ECOM-CT-0031
@SYMTestCaseDesc Test that a plugin whose SID does not match the value in it's
				 resource file will not be installed in the registry.
@SYMTestPriority High
@SYMTestActions  InvalidSIDPlugin dll and rsc files copied to C: drive.
				 Verify plugin is not registered by checking
				 ListImplementationsL returns zero plugins for this interface.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3846
*/
LOCAL_C void TestInvalidSIDPluginL()
	{
	// InvalidSIDPlugin .dll and .rsc are copied from EComExample5.dll and
	// HeapTestImpl.rsc.

	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0031 "));
	__UHEAP_MARK;

	// Copy plugins
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComInvalidDllOnZ, KEComInvalidDllOnC));
	test(err == KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComInvalidRscOnZ, KEComInvalidRscOnC));
	test(err == KErrNone);

	// The reason for the folowing delay is:
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that an active object,
	// which scans plugin directories. So the discovering service is asynchronous.
	// We have to wait some time until it finishes.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	WAIT_FOR3s;

	TUid ifUid = {0x101FE392};  // HeapTestImpl.rsc interface_uid
	RImplInfoPtrArray ifArray;
	REComSession::ListImplementationsL(ifUid, ifArray);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 0);

	ifArray.ResetAndDestroy();
	REComSession::FinalClose(); // Don't want leaks outside the test

	// Cleanup plugins
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComInvalidDllOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComInvalidRscOnC));

	__UHEAP_MARK;
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1858
@SYMTestCaseDesc Test that rediscovery of removing and adding "\Resource\Plugins" directory
					works properly.
@SYMTestPriority High
@SYMTestActions  Removes "\Resource\Plugins" directory on C: drive, check that
					rediscovery	works properly by verifying plugin's registration.
				 Adds "\Resource\Plugins" directory on C: drive, check that rediscovery works
				 	fine by verifying plugin's registration.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF088454
*/
LOCAL_C void TestPluginsDirectoryRemovingL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1858 "));
	__UHEAP_MARK;

	/**
	Add The following plugin to C drive

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	0x102797A1			0x102797A0	0x102797A2		1			C:\\..\\DefectPlugin.dll

	**/
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComDefectPluginDLLOnZ, KEComDefectPluginDLLOnC));
	test(err == KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComDefectPluginRSCOnZ, KEComDefectPluginRSCOnC));
	test(err == KErrNone);

	WAIT_FOR3s;

	TUid interfaceUid={0x102797A1};
	RImplInfoPtrArray implArray;

	// Now start ecom discovery and get implementations for IF UID 0x102797A1
	TRAP(err, REComSession::ListImplementationsL(interfaceUid, implArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	// Expected number of implementations returned
	test(implArray.Count()==1);

	// Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();
	// imp. uid
	test(implUid.iUid == 0x102797A2);
	// version
	test(version == 1);
	// C drive
	test(drive == EDriveC);

	implArray.ResetAndDestroy();

	// Now remove plugins directory
	TRAP(err, EComTestUtils::FileManRenameL(KEComResourcePluginDirNameOnC, KEComResourcePluginDirNameOffOnC));
	test(err == KErrNone);
	TRAP(err, EComTestUtils::FileManDeleteDirL(KEComResourcePluginDirNameOnC));
	test(err == KErrNone);

	TEntry entry;
	err = TheFs.Entry(KEComResourcePluginDirNameOnC, entry);
	// Test the plugins directory is now gone
	test(err == KErrNotFound);

	// Wait EComServer performing rediscovery
	WAIT_FOR3s;

	TRAP(err, REComSession::ListImplementationsL(interfaceUid, implArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	// Test the rediscovery and directory scanning has been performed on renaming off event
	test(implArray.Count()==0);

	implArray.ResetAndDestroy();

	// Now rename plugins directory back
	TRAP(err, EComTestUtils::FileManRenameL(KEComResourcePluginDirNameOffOnC, KEComResourcePluginDirNameOnC));
	test(err == KErrNone);
	TRAP(err, EComTestUtils::FileManDeleteDirL(KEComResourcePluginDirNameOffOnC));
	test(err == KErrNone);

	// Test the plugins directory is added back
	err = TheFs.Entry(KEComResourcePluginDirNameOnC, entry);
	test(err == KErrNone);

	// Wait EComServer performing rediscovery
	WAIT_FOR3s;

	TRAP(err, REComSession::ListImplementationsL(interfaceUid, implArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	// Test the rediscovery and directory scanning has been performed on renaming off event
	test(implArray.Count()==1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();
	test(implUid.iUid == 0x102797A2);
	test(version == 1);
	test(drive == EDriveC);

	implArray.ResetAndDestroy();

	REComSession::FinalClose(); // Don't want leaks outside the test

	// Cleanup plugins
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComDefectPluginDLLOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComDefectPluginRSCOnC));

	__UHEAP_MARK;
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

/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		//Call the test function
		TRAP(err, ((*testFuncL)()));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	test(err == KErrNone);
	test.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}



LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	// Basic tests
	test.Next(_L("Basic Test Suite"));
	test.Start(_L("Basic Test Suite"));
	DoBasicTestL(&TestOrphanedRscFileL, _L("TestOrphanedRscFileL"));
	DoBasicTestL(&TestInvalidSIDPluginL, _L("TestInvalidSIDPluginL"));
	DoBasicTestL(&TestOrphanedRscFileFallBackL, _L("TestOrphanedRscFileFallBackL"));
	DoBasicTestL(&TestPluginsDirectoryRemovingL, _L("TestPluginsDirectoryRemovingL"));
	test.End();

	// OOM tests
	test.Next(_L("Out-of-memory Repeat Tests"));
	test.Start(_L("Out-of-memory Repeat Tests"));
	DoOOMTestL(&TestOrphanedRscFileL, _L("TestOrphanedRscFileL"));
	DoOOMTestL(&TestInvalidSIDPluginL, _L("TestInvalidSIDPluginL"));
	DoOOMTestL(&TestOrphanedRscFileFallBackL, _L("TestOrphanedRscFileFallBackL"));
	DoOOMTestL(&TestPluginsDirectoryRemovingL, _L("TestPluginsDirectoryRemovingL"));
	test.End();

	__UHEAP_MARKEND;
	}


//Initialise the Active Scheduler
//
LOCAL_C void SetupL()
	{
	// Construct and install the Active Scheduler. The Active Schedular is needed
	// by components used by this test as they are ActiveObjects.
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Dual-Mode Discovery Tests"));

	TheTrapCleanup = CTrapCleanup::New();
	TInt err = TheFs.Connect();
	test(err == KErrNone);
	TRAP(err, SetupL());
	test(err == KErrNone);


	// Perform tests.
	TRAP(err,DoTestsL());
	test(err==KErrNone);

	delete TheActiveScheduler;
	TheFs.Close();
	delete TheTrapCleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
