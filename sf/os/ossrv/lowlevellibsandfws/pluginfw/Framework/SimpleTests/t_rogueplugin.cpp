// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <ecom/ecom.h>
#include "Interface.h" // ECOM CExampleInterface class
#include "exampleNine.h" // CRoguePlugin class


//Test utils for copying plugin to C
#include "EcomTestUtils.h"

_LIT(KTestTitle, "DEF094656 Rogue plugin should not override build-in");

LOCAL_D RTest TheTest(_L("Rogue plugin with duplicated Impl. UID"));

LOCAL_D TBool correctTypeCastPassed = EFalse;

_LIT(KRoguePluginDllOnZ,	"Z:\\RAMOnly\\exampleNine.dll");
_LIT(KRoguePluginDllOnC,	"C:\\sys\\bin\\exampleNine.dll");
_LIT(KRoguePluginRscOnZ,	"Z:\\RAMOnly\\exampleNine.rsc");
_LIT(KRoguePluginRscOnC,	"C:\\resource\\plugins\\exampleNine.rsc");

/** Copy the rogue plugin to C: drive
*/
LOCAL_C void CopyPluginsL()
    {
	// Copy the dlls and .rsc files on to RAM
	EComTestUtils::FileManCopyFileL(KRoguePluginDllOnZ, KRoguePluginDllOnC);
	EComTestUtils::FileManCopyFileL(KRoguePluginRscOnZ, KRoguePluginRscOnC);
	// Pause in case ECOM server is already up and running and needs
	// time to activate the scanning timer active object.
	User::After(3000000);
	}

/** Remove resource file and dll copied to C: drive
*/
LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(ignoreErr, EComTestUtils::FileManDeleteFileL(KRoguePluginRscOnC));
	TRAP(ignoreErr, EComTestUtils::RLoaderDeleteFileL(KRoguePluginDllOnC));
	}

/**
Test rogue plugin trying to override a ROM based plugin by
duplicating the legitimate DLL's implementation UID (different
interface UID so that ECOM not treat it as an update).

@SYMTestCaseID			SYSLIB-ECOM-CIT-3161
@SYMTestCaseDesc	    Copy a rogue plugin to C drive. This plugin duplicates the
						implementation UID of a built-in dll. Test if the rogue
						plugin will over shadow the built-in dll.
@SYMTestPriority		High
@SYMTestActions			1. copy the rsc and dll of the rogue plugin to C:
						2. Use the CreateImplementation API which does not specify
						   the i/f UID to instantiate the built-in implementation.
						   Test the instance works normally.
						3. Repeat 2 but instantiate the pointer as the rogue plugin
						   class. Pass NULL as the initialising param in the call
						   to CreateImplementationL. Because the actual plugin returned by
						   ECOM needs this initParam, KERN-EXEC 3 panic occurs.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF					DEF094656
*/
LOCAL_C void DEF094656_TestCaseL()
	{
	// DuplicateImplUidTestL SYSLIB-ECOM-CT-3157 is similar to this testcase.
	// CT-3157 tests higher driver letter > lower driver letter,
	// and lower i/f UID > higher i/f UID.

	// Note that this function is expected to panic. Hence not bother
	// with __UHEAP_MARK.

	//Check if the rogue plugin is in the registry.
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CIT-3161 "));
	RImplInfoPtrArray ifArray;
	REComSession::ListImplementationsL(KRogueInterfaceUid, ifArray);

	TInt count = ifArray.Count();
	TheTest.Printf(_L("Found %d implementations of I/f 0x%X"), count, KRogueInterfaceUid.iUid);

	TheTest(count == 1);
	TheTest(KRogueImplUid == ifArray[0]->ImplementationUid());

	ifArray.ResetAndDestroy();

	TUid dtor_key;
	// Initialisation parameter needed by CImplementationClassOne
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	// First test creating the correct class.
	CExampleInterface* correctDll = reinterpret_cast<CExampleInterface*>(
		REComSession::CreateImplementationL(KRogueImplUid,
											dtor_key,
											&initParams) );
	TheTest(correctDll != NULL);

	TUid testUid = correctDll->ImplId();
	// the example plugins should return the value 10009DC3.
	TheTest(testUid == KRogueImplUid);

	REComSession::DestroyedImplementation(dtor_key);
	delete correctDll;

	// Indicate to thread creator that first stage test passed.
	correctTypeCastPassed = ETrue;

	// This create should crash because the plugin returned by
	// ECOM is really CImplementationClassOne which needs a properly
	// constructed initParam.
	CRoguePlugin* wrongDll = reinterpret_cast<CRoguePlugin*>(
		REComSession::CreateImplementationL(KRogueImplUid,
											dtor_key,
											NULL) );

	// If gets here then someone has changed CImplementationClassOne::NewL
	// or 10009DC3 has been updated by another implementation.
	if (wrongDll)
		{
		REComSession::DestroyedImplementation(dtor_key);
		delete wrongDll;
		}

	REComSession::FinalClose();

	TheTest(EFalse);
	}

LOCAL_C void ThreadMainL()
	{
	CConsoleBase* newConsole = Console::NewL(KTestTitle,
		TSize(KConsFullScreen, KConsFullScreen));

	// Thread creator needs to save the original console because this
	// thread is expected to crash, hence cannot cleanup.
	TheTest.SetConsole(newConsole);
	DEF094656_TestCaseL();
	TheTest.SetConsole(NULL);

	delete newConsole;
	}

LOCAL_C TInt ThreadFunc(TAny*)
	{
	__UHEAP_MARK;
	CTrapCleanup* threadcleanup = CTrapCleanup::New();
	TRAPD(err, ThreadMainL());
	delete threadcleanup;
	__UHEAP_MARKEND;
	return err;
	}

LOCAL_C void RunTestThreadL()
	{
	__UHEAP_MARK;

	CopyPluginsL();

	_LIT(KThreadName, "RoguePluginTest");

	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);

	TheTest.Start(KTestTitle);

	// Save the console because the created thread must use its own
	// console.
	CConsoleBase* savedConsole = TheTest.Console();

	RThread tt;
	TInt err = tt.Create(KThreadName, &ThreadFunc, KDefaultStackSize,
		KMinHeapSize, 0x100000, 0);
	User::LeaveIfError(err);

	TRequestStatus status;
	tt.Logon(status);
	tt.Resume();

	User::WaitForRequest(status);

	// restore console
	TheTest.SetConsole(savedConsole);

	TExitCategoryName exitcategory = tt.ExitCategory();
	TExitType exittype = tt.ExitType();
	TInt exitReason = tt.ExitReason();
	tt.Close();

	TheTest.Printf(_L("Thread exit type %d, reason %d, category %S"), exittype, exitReason, &exitcategory);

	User::SetJustInTime(jit);
	DeleteTestPlugin();

	// Check if tt thread passes this checkpoint
	TheTest(correctTypeCastPassed);

	// Check if tt thread die of KERN-EXEC.
	_LIT(KKernExec, "KERN-EXEC");
	TheTest(exitcategory.CompareF(KKernExec) == 0);

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAP_IGNORE( RunTestThreadL() );

	delete scheduler;
	delete cleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}
