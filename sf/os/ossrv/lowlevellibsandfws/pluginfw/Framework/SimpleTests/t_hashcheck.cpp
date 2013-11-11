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
// Contains tests to exercise the hash checking feature for removable drives
// 
//


#include <ecom/ecom.h>
#include "../EcomTestUtils/EcomTestUtils.h"
#include <hash.h>
#include <swi/swispubsubdefs.h>
#include "../Example/EComHashExample.h"
#include "../EcomTestUtils/TPropertyManager.h"

using namespace Swi;

LOCAL_D RTest test(_L("t_hashcheck.exe"));

LOCAL_D CTrapCleanup* 	  TheTrapCleanup 	 = NULL;

LOCAL_D CActiveScheduler* TheActiveScheduler = NULL;

LOCAL_D RFs					TheFs;

#define UNUSED_VAR(a) a = a

// Implementaion IDs used for testing
const TUid KUidTestInterface = {0x10009E34};
const TUid KUidTestImplementation = {0x10009E35};

// Plugins used in tests.
_LIT(KEComHashExampleDllOnZ, "z:\\RAMOnly\\EComHashExample.dll");
_LIT(KEComHashExampleRscOnZ, "z:\\RAMOnly\\EComHashExample.rsc");

_LIT(KEComAllRSCFilesName,					"\\Resource\\Plugins\\*.rsc");
_LIT(KEComRscDirName,						"\\Resource\\Plugins");

#if defined(__WINSCW__) // X: on emulator
_LIT(KEComHashExampleDllOnRemovableDrive, "X:\\sys\\bin\\EComHashExample.dll");
_LIT(KEComHashExampleRscOnRemovableDrive, "X:\\resource\\plugins\\EComHashExample.rsc");
#else // E: on hardware
_LIT(KEComHashExampleDllOnRemovableDrive, "E:\\sys\\bin\\EComHashExample.dll");
_LIT(KEComHashExampleRscOnRemovableDrive, "E:\\resource\\plugins\\EComHashExample.rsc");
#endif

// hash files
_LIT(KEComTempHashFileOnC, "c:\\EComTempHashFile.dll");
_LIT(KEComTempCorruptHashFileOnC, "c:\\EComTempCorruptHashFile.dll");
_LIT(KEComHashExampleHashFileOnRemovableDrive, "c:\\sys\\hash\\EComHashExample.dll");
_LIT(KEComCorruptHash, "12345678912345678900");

const TInt KHashFileReadSize = 1024*8;

void CreateTempHashFileL()
	{
	// Create valid hash file for the DLL to be used during this test.
	TInt readsize = KHashFileReadSize;
	HBufC8* block0 = HBufC8::NewLC(readsize);
	TPtr8 fileblock0(block0->Des());
	CSHA1* hasher=CSHA1::NewL();
	CleanupStack::PushL(hasher);

	RFile file;
       CleanupClosePushL(file);
	User::LeaveIfError(file.Open(TheFs, KEComHashExampleDllOnZ, EFileRead));

	TInt size;
	User::LeaveIfError(file.Size(size));
	TInt offset=0;
	do  {
		if((size - offset) < readsize)
			readsize = (size - offset);
		User::LeaveIfError(file.Read(offset, fileblock0, readsize));
		hasher->Update(fileblock0);
		offset+=readsize;
		}
	while(offset < size);

	CleanupStack::PopAndDestroy(1); // file

	TBuf8<SHA1_HASH> hash;
	hash=hasher->Final();

	CleanupStack::PopAndDestroy(2); // block0, hasher

	// write hash to file
	RFile tempHashFile;
       CleanupClosePushL(tempHashFile);
      	User::LeaveIfError(tempHashFile.Replace(TheFs, KEComTempHashFileOnC, EFileWrite));
	tempHashFile.Write(hash);
	CleanupStack::PopAndDestroy(1); // tempHashFile
    }

void DeleteTempHashFileL()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComTempHashFileOnC));
	test(err == KErrNone);
	}

void CreateTempCorruptHashFileL()
	{
	// write corrupt hash to file
	TBuf8<SHA1_HASH> hash;
	hash.Append(KEComCorruptHash);
	RFile tempHashFile;
       CleanupClosePushL(tempHashFile);
      	User::LeaveIfError(tempHashFile.Replace(TheFs, KEComTempCorruptHashFileOnC, EFileWrite));
	tempHashFile.Write(hash);
	CleanupStack::PopAndDestroy(1); // tempHashFile
	}

void DeleteTempCorruptHashFileL()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComTempCorruptHashFileOnC));
	test(err == KErrNone);
	}

void CopyHashFile()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComTempHashFileOnC,
											KEComHashExampleHashFileOnRemovableDrive));
	test(err == KErrNone);
	}

void DeleteHashFile()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComHashExampleHashFileOnRemovableDrive));
	UNUSED_VAR(err);

	// If ECOM server is already running we need to allow some time for re-discovery
	// to complete.
	WAIT_FOR3s;
	}

void CopyCorruptHashFile()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComTempCorruptHashFileOnC,
											KEComHashExampleHashFileOnRemovableDrive));
	test(err == KErrNone);
	}

void CopyPlugins()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComHashExampleDllOnZ, KEComHashExampleDllOnRemovableDrive));
	test(err == KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComHashExampleRscOnZ, KEComHashExampleRscOnRemovableDrive));
	test(err == KErrNone);

	// If ECOM server is already running we need to allow some time for re-discovery
	// to complete.
	WAIT_FOR3s;
	}

void DeletePlugins()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComHashExampleDllOnRemovableDrive));
	UNUSED_VAR(err);
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComHashExampleRscOnRemovableDrive));
	UNUSED_VAR(err);

	// If ECOM server is already running we need to allow some time for re-discovery
	// to complete.
	WAIT_FOR3s;
	}


void DeleteRSCFolderOnDrive(TUint aDriveNum)
	{
	TInt err=KErrNone;
	TDriveUnit aDrive(aDriveNum);

	TBuf<256> resourceFileName;
	resourceFileName.Append(aDrive.Name());
	resourceFileName.Append(KEComAllRSCFilesName);
	TRAP(err, EComTestUtils::FileManDeleteFileL(resourceFileName));

	TBuf<256> resourceDirName;
	resourceDirName.Append(aDrive.Name());
	resourceDirName.Append(KEComRscDirName);
	TRAP(err, EComTestUtils::FileManDeleteDirL(resourceDirName));
	}

TBool IsImplementationListedL()
	{
	RImplInfoPtrArray implArray;
	REComSession::ListImplementationsL(KUidTestInterface, implArray);

	TBool found = EFalse;
	TInt count = implArray.Count();
	while(count)
		{
		count--;
		if(implArray[count]->ImplementationUid() == KUidTestImplementation)
			{
			found = ETrue;
			break;
			}
		}
	REComSession::FinalClose();
	implArray.ResetAndDestroy();
	return found;
	}

TInt IsImplementationCreatedL()
	{
	TUid dtor_ID_Key;
	TAny* ptr = NULL;
	TRAPD(err, ptr = REComSession::CreateImplementationL(KUidTestImplementation, dtor_ID_Key));

	CImplementationHashExample* implPtr = reinterpret_cast <CImplementationHashExample*> (ptr);

	if(err == KErrNone)
		{
		REComSession::DestroyedImplementation(dtor_ID_Key);
		delete implPtr;
		}

	REComSession::FinalClose();
	return err;
	}

void	DoPreInstall()
	{
	// Install in progress
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisInstall);
	CopyHashFile();
	WAIT_FOR1s;

	// Install successful
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisInstall);
	WAIT_FOR3s;

	// Reset
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisNone);
	WAIT_FOR1s;
	}

void	DoCorruptPreInstall()
	{
	// Install in progress
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisInstall);
	CopyCorruptHashFile();
	WAIT_FOR1s;

	// Install successful
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisInstall);
	WAIT_FOR3s;

	// Reset
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisNone);
	WAIT_FOR1s;
	}

void	DoPreUninstall()
	{
	// Uninstall in progress
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisUninstall);
	DeleteHashFile();
	WAIT_FOR1s;

	// Uninstall successful
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisUninstall);
	WAIT_FOR3s;

	// Reset
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisNone);
	WAIT_FOR1s;
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1922
@SYMTestCaseDesc Test that implementation on removable drive is not available
	when no hash file has been installed.
@SYMTestPriority High
@SYMTestActions	 Copy plugins to removable drive.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is unavailable.
@SYMTestExpectedResults The test must not fail.
@SYMDEF PDEF090578
*/
LOCAL_C void TestNoHashFileInstalledL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1922 "));
	__UHEAP_MARK;

	// Test ListImplementations()
	CopyPlugins();
	TBool implListed = IsImplementationListedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no hash so the implementation is
	// not listed.
	test(!implListed);

	// Test CreateImplementation()
	TInt implCreated = IsImplementationCreatedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be created.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no hash so the implementation is
	// not created.
	test(implCreated == KErrNotFound);

	DeletePlugins();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1923
@SYMTestCaseDesc Test that implementation on removable drive is available
	when a Pre Install occurs exists.
@SYMTestPriority High
@SYMTestActions	 Copy plugins to removable drive.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is unavailable.
				 Emulate pre-install.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is available.
				 Emulate uninstall.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is unavailable.
@SYMTestExpectedResults The test must not fail.
@SYMDEF PDEF090578
*/
LOCAL_C void TestPreInstallL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1923 "));
	__UHEAP_MARK;

	// Only copy plugins - during pre-install plugins exists before the install occurs
	CopyPlugins();

	// Test ListImplementations()
	TBool implListed = IsImplementationListedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no hash because install has not occurred
	// so the implementation is not listed.
	test(!implListed);

	// Test CreateImplementation()
	// No hash so no implementation should  be created.
	TInt implCreated = IsImplementationCreatedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be created.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no hash because install has not occurred
	// so the implementation is not created.
	test(implCreated == KErrNotFound);

	// Do install
	DoPreInstall();

	// Test ListImplementations()
	implListed = IsImplementationListedL();

	_LIT(KMessage3,"Pre-Install 3: List = %d");
	RDebug::Print(KMessage3, implListed);
	// Check implementation
#if defined(__WINSCW__)
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed.
	//
	test(!implListed);
#else
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is a hash so the implementation is
	// listed.
	test(implListed);
#endif

	// Test CreateImplementation()
	implCreated = IsImplementationCreatedL();

	// Check implementation was created
#if defined(__WINSCW__)
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be created.
	//
	test(implCreated == KErrNotFound);
#else
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is a hash so the implementation is
	// created.
	test(implCreated == KErrNone);
#endif

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1924
@SYMTestCaseDesc Test that implementation on removable drive is unavailable
	after an uninstall occurs.
@SYMTestPriority High
@SYMTestActions	 Emulate uninstall.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is unavailable.
@SYMTestExpectedResults The test must not fail.
@SYMDEF PDEF090578
*/
LOCAL_C void TestPreUninstallL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1924 "));
	__UHEAP_MARK;

	// Do uninstall
	DoPreUninstall();

	// Test ListImplementations()
	TBool implListed = IsImplementationListedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no hash so the implementation is
	// not listed.
	test(!implListed);

	// Test CreateImplementation()
	TInt implCreated = IsImplementationCreatedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be created.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no hash so the implementation is
	// not created.
	test(implCreated == KErrNotFound);

	DeletePlugins();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1925
@SYMTestCaseDesc Test that implementation on removable drive is unavailable
	if the hash is corrupted for the DLL.
@SYMTestPriority High
@SYMTestActions	 Emulate pre-install with a corrupted hash file.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is unavailable.
@SYMTestExpectedResults The test must not fail.
@SYMDEF PDEF090578
*/
LOCAL_C void TestCorruptHashL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1925 "));
	__UHEAP_MARK;

	// Only copy plugins - during pre-install plugins exists before the install occurs
	CopyPlugins();

	// Do pre install
	DoCorruptPreInstall();

	// Test ListImplementations()
	TBool implListed = IsImplementationListedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case the hash file is corrupted so the implementation is
	// not listed.
	test(!implListed);

	// Test CreateImplementation()
	TInt implCreated = IsImplementationCreatedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be created.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case the hash file is corrupted so the implementation is
	// not created.
	test(implCreated == KErrNotFound);

	DeletePlugins();
	DeleteHashFile();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-ECOM-CT-1926
@SYMTestCaseDesc Test that implementation on removable drive is available
	when a full Install occurs exists.
@SYMTestPriority High
@SYMTestActions	 Copy plugins to removable drive.
				 Emulate pre-install.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is available.
@SYMTestExpectedResults The test must not fail.
@SYMDEF PDEF090578
*/
LOCAL_C void TestFullInstallL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1926 "));
	__UHEAP_MARK;

	// Only copy plugins - during pre-install plugins exists before the install occurs
	CopyPlugins();
	// Do install
	DoPreInstall();

	// Test ListImplementations()
	TBool implListed = IsImplementationListedL();

	// Test CreateImplementation()
	TInt implCreated = IsImplementationCreatedL();

#if defined(__WINSCW__)
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed and
	// the implementation will not be created.
	test(!implListed);
	test(implCreated == KErrNotFound);
#else
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is a valid hash so the implementation is
	// listed and created.
	test(implListed);
	test(implCreated == KErrNone);
#endif

	__UHEAP_MARKEND;
	}
/**
@SYMTestCaseID SYSLIB-ECOM-CT-1927
@SYMTestCaseDesc Test that implementation on removable drive is unavailable
	after an uninstall occurs.
@SYMTestPriority High
@SYMTestActions	 Emulate uninstall.
				 Call ListImplementations() and CreateImplementation()
				 Check implementation is unavailable.
@SYMTestExpectedResults The test must not fail.
@SYMDEF PDEF09057
*/
LOCAL_C void TestFullUninstallL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1927 "));
	__UHEAP_MARK;

	//Clean up DLL on remove drive
	DeletePlugins();

	//Clean up the hash file related the DLL
	DoPreUninstall();

	// Test ListImplementations()
	TBool implListed = IsImplementationListedL();

	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be listed.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no plugin and no hash so the implementation
	// is not listed.
	test(!implListed);

	// Test CreateImplementation()
	TInt implCreated = IsImplementationCreatedL();
	// Check implementation
	// On the emulator RLibrary::GetInfo() always returns KErrNotFound for DLL's
	// on the X drive. Because of this the implementation will not be created.
	//
	// On hardware ECOM uses RLoader::CheckLibraryHash() to determine if the hash
	// is available for the DLL. In this case there is no plugins and no hash file so the
	// implementation is not created.
	test(implCreated == KErrNotFound);

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
	__UHEAP_MARK;

//If it is hardware and E: drive deosn't exist, don't run the test.
#if (!defined(__WINSCW__))
	if(!TheFs.IsValidDrive(EDriveE))
		{
		test.Printf(_L("E: drive doesn't exist, the test won't be able to run \n"));
		return;
		}
#endif

	// Basic tests
	test.Next(_L("Basic Test Suite"));
	test.Start(_L("Basic Test Suite"));
	DoBasicTestL(&TestPreInstallL, _L("TestPreInstallL"));
	DoBasicTestL(&TestPreUninstallL, _L("TestPreUninstallL"));

	DoBasicTestL(&TestFullInstallL, _L("TestFullInstallL"));
	DoBasicTestL(&TestFullUninstallL, _L("TestFullUninstallL"));

	DoBasicTestL(&TestNoHashFileInstalledL, _L("TestNoHashFileInstalledL"));
	DoBasicTestL(&TestCorruptHashL, _L("TestCorruptHashL"));
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

	// create hash files
	CreateTempHashFileL();
	CreateTempCorruptHashFileL();

	//Define swinstall property
	PropertyManager::DefineProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue,RProperty::EInt);

	// Initialise swinstall property
	PropertyManager::SetProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, ESASwisNone);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Hash Tests"));

	TheTrapCleanup = CTrapCleanup::New();

	//Delete swinstall property if it already exists
	PropertyManager::DeleteProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue);

	TInt err = TheFs.Connect();
	test(err == KErrNone);
	TRAP(err, SetupL());
	test(err == KErrNone);


	// Perform tests.
	TRAP(err,DoTestsL());
	test(err==KErrNone);

	DeleteTempHashFileL();
	DeleteTempCorruptHashFileL();

	//Delete swinstall property
	PropertyManager::DeleteProperty(KUidSystemCategory, KSAUidSoftwareInstallKeyValue);

#if !defined(__WINSCW__)
	DeleteRSCFolderOnDrive(EDriveE);
#endif

	delete TheActiveScheduler;
	TheFs.Close();
	delete TheTrapCleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
