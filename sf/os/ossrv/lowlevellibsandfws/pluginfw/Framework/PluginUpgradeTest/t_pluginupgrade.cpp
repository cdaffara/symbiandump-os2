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

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins

// RAM Only RSC and DLL
_LIT(KRamOnlyEComUpgradeExample1DLL,		"Z:\\RAMOnly\\EComUpgradeExample1.dll");
_LIT(KRamOnlyEComUpgradeExample1RSC,		"Z:\\RAMOnly\\EComUpgradeExample1.rsc");

_LIT(KRamOnlyEComUpgradeExample2DLL,		"Z:\\RAMOnly\\EComUpgradeExample2.dll");
_LIT(KRamOnlyEComUpgradeExample2RSC,		"Z:\\RAMOnly\\EComUpgradeExample2.rsc");

_LIT(KRamOnlyEComUpgradeExample3DLL,		"Z:\\RAMOnly\\EComUpgradeExample3.dll");
_LIT(KRamOnlyEComUpgradeExample3RSC,		"Z:\\RAMOnly\\EComUpgradeExample3.rsc");

_LIT(KEComUpgradeExample1DLLOnC,			"C:\\sys\\bin\\EComUpgradeExample1.dll");
_LIT(KEComUpgradeExample1RSCOnC,			"C:\\Resource\\Plugins\\EComUpgradeExample1.rsc");

_LIT(KEComUpgradeExample2DLLOnC,			"C:\\sys\\bin\\EComUpgradeExample2.dll");
_LIT(KEComUpgradeExample2RSCOnC,			"C:\\Resource\\Plugins\\EComUpgradeExample2.rsc");

_LIT(KEComUpgradeExample3DLLOnC,			"C:\\sys\\bin\\EComUpgradeExample3.dll");
_LIT(KEComUpgradeExample3RSCOnC,			"C:\\Resource\\Plugins\\EComUpgradeExample3.rsc");

_LIT(KRamOnlyEComUpgradeROExample1DLL,		"Z:\\RAMOnly\\EComUpgradeROExample1.dll");
_LIT(KRamOnlyEComUpgradeROExample1RSC,		"Z:\\RAMOnly\\EComUpgradeROExample1.rsc");

_LIT(KRamOnlyEComUpgradeROExample2DLL,		"Z:\\RAMOnly\\EComUpgradeROExample2.dll");
_LIT(KRamOnlyEComUpgradeROExample2RSC,		"Z:\\RAMOnly\\EComUpgradeROExample2.rsc");

_LIT(KRamOnlyEComUpgradeROExample3DLL,		"Z:\\RAMOnly\\EComUpgradeROExample3.dll");
_LIT(KRamOnlyEComUpgradeROExample3RSC,		"Z:\\RAMOnly\\EComUpgradeROExample3.rsc");

_LIT(KEComUpgradeROExample1DLLOnC,			"C:\\sys\\bin\\EComUpgradeROExample1.dll");
_LIT(KEComUpgradeROExample1RSCOnC,			"C:\\Resource\\Plugins\\EComUpgradeROExample1.rsc");

_LIT(KEComUpgradeROExample2DLLOnC,			"C:\\sys\\bin\\EComUpgradeROExample2.dll");
_LIT(KEComUpgradeROExample2RSCOnC,			"C:\\Resource\\Plugins\\EComUpgradeROExample2.rsc");

_LIT(KEComUpgradeROExample3DLLOnC,			"C:\\sys\\bin\\EComUpgradeROExample3.dll");
_LIT(KEComUpgradeROExample3RSCOnC,			"C:\\Resource\\Plugins\\EComUpgradeROExample3.rsc");

const TInt KOneSecond = 1000000;
const TInt KInterfaceUid = 0x10009DBA;
const TInt KRomOnlyInterfaceUid = 0x10009DBD;
const TInt KImplementationUid = 0x10009DBB;
const TInt KRomOnlyImplementationUid = 0x10009DBE;

LOCAL_D RTest TheTest(_L("Plugin Upgrade Test"));

/**
Test macroes and functions
*/
static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

/**
Kill Ecom Server for testing purposes
*/
static void KillEComServerL()
	{
	//Need to ensure that the EComServer process is killed before even starting this test by using
   	//the EComTestUtils library
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAPD(error, EComTestUtils::KillProcessL(KEComServerProcessName));
   	error=error;
	}

class RPluginUpgradeTest
	{
public:
	static void UpgradePluginSameNameLowerVersionL(TBool aIsRomOnly);
	static void UpgradePluginDifferentNameLowerVersionL(TBool aIsRomOnly);
	static void UpgradePluginSameNameHigherVersionL(TBool aIsRomOnly);
	static void UpgradePluginDifferentNameHigherVersionL(TBool aIsRomOnly);
	static void UpgradePluginSameNameSameVersionL(TBool aIsRomOnly);
	static void UpgradePluginDifferentNameSameVersionL(TBool aIsRomOnly);
	};

/**
Copies the Plugins to specific folder for testing purpose
*/
LOCAL_C void CopyPluginSameNameLowerVersion(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample1DLL, KEComUpgradeROExample2DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample1RSC, KEComUpgradeROExample2RSCOnC));
		TEST2(err, KErrNone);
		}
	else
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample1DLL, KEComUpgradeExample2DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample1RSC, KEComUpgradeExample2RSCOnC));
		TEST2(err, KErrNone);
		}
	}

LOCAL_C void CopyPluginDifferentNameLowerVersion(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample1DLL, KEComUpgradeROExample1DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample1RSC, KEComUpgradeROExample1RSCOnC));
		TEST2(err, KErrNone);
		}
	else
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample1DLL, KEComUpgradeExample1DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample1RSC, KEComUpgradeExample1RSCOnC));
		TEST2(err, KErrNone);
		}
	}

LOCAL_C void CopyPluginSameNameHigherVersion(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample3DLL, KEComUpgradeROExample2DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample3RSC, KEComUpgradeROExample2RSCOnC));
		TEST2(err, KErrNone);
		}
	else
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample3DLL, KEComUpgradeExample2DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample3RSC, KEComUpgradeExample2RSCOnC));
		TEST2(err, KErrNone);
		}
	}

LOCAL_C void CopyPluginDifferentNameHigherVersion(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample3DLL, KEComUpgradeROExample3DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample3RSC, KEComUpgradeROExample3RSCOnC));
		TEST2(err, KErrNone);
		}
	else
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample3DLL, KEComUpgradeExample3DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample3RSC, KEComUpgradeExample3RSCOnC));
		TEST2(err, KErrNone);
		}
	}

LOCAL_C void CopyPluginSameNameSameVersion(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample2DLL, KEComUpgradeROExample2DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample2RSC, KEComUpgradeROExample2RSCOnC));
		TEST2(err, KErrNone);
		}
	else
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample2DLL, KEComUpgradeExample2DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample2RSC, KEComUpgradeExample2RSCOnC));
		TEST2(err, KErrNone);
		}
	}

LOCAL_C void CopyPluginDifferentNameSameVersion(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample2DLL, KEComUpgradeROExample3DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeROExample2RSC, KEComUpgradeROExample3RSCOnC));
		TEST2(err, KErrNone);
		}
	else
		{
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample2DLL, KEComUpgradeExample3DLLOnC));
		TEST2(err, KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComUpgradeExample2RSC, KEComUpgradeExample3RSCOnC));
		TEST2(err, KErrNone);
		}
	}

inline LOCAL_C void ResetEcom()
	{
	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteUpgradeExample1Plugins(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeROExample1DLLOnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeROExample1RSCOnC));
		}
	else
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeExample1DLLOnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeExample1RSCOnC));
		}
	}

inline LOCAL_C void DeleteUpgradeExample2Plugins(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeROExample2DLLOnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeROExample2RSCOnC));
		}
	else
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeExample2DLLOnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeExample2RSCOnC));
		}
	}

inline LOCAL_C void DeleteUpgradeExample3Plugins(TBool aIsRomOnly)
	{
	TInt err=KErrNone;
	if(aIsRomOnly)
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeROExample3DLLOnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeROExample3RSCOnC));
		}
	else
		{
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeExample3DLLOnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComUpgradeExample3RSCOnC));
		}
	}

inline LOCAL_C void CheckDefaultPluginOnZ(TBool aIsRomOnly)
	{
	RImplInfoPtrArray implArray;
	TUid interfaceUid;

	if(aIsRomOnly)
		{
		/**
		The following Plugin exists on the Z drive and will be discovered by ECOM during startup.

		Interface UID   DLL	UID     Imp. UID    Version    DllFile
		---------------------------------------------------------------------------------------
		0x10009DBD      0x10009DBC  0x10009DBE  2          Z:\\..\\EComUpgradeExample2.dll
		**/
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		/**
		The following Plugin exists on the Z drive and will be discovered by ECOM during startup.

		Interface UID   DLL	UID     Imp. UID    Version    DllFile
		---------------------------------------------------------------------------------------
		0x10009DBA      0x10009DB9  0x10009DBB  2          Z:\\..\\EComUpgradeExample2.dll
		**/
		interfaceUid = TUid::Uid(KInterfaceUid);
		}

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();

	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);

	// imp. uid
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		}
	// version
	TEST2(version, 2);
	// Z drive
	TEST2(drive, EDriveZ);

	implArray.ResetAndDestroy();
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1596
@SYMTestCaseDesc		Tests to ensure that an implementation will not be upgraded when the
						upgrading DLL/RSC name is the same as the DLL/RSC name to be
						upgraded but has a lower version.
@SYMTestPriority		High
@SYMTestActions			Call REComSession::ListImplementations() and check V1 implementation
						on Z drive exists.
						Copy DLL/RSC with same name and lower version to C drive.
						Call REComSession::ListImplementations() and check V1 implementation
						upgrades on C drive has not upgraded the original implementation on Z drive.
						Restart ECOM (to test subsequent boot condition i.e. loading from dat file
						Call REComSession::ListImplementations() and check V1 implementation
						upgrades on C drive has not upgraded the original implementation on Z drive.
						Check for no crash and memory leaks.
						The same test is repeated for Rom Only drives. And no upgrade in version is
						observed.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF					DEF080682
*/
void RPluginUpgradeTest::UpgradePluginSameNameLowerVersionL(TBool aIsRomOnly)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1596 "));
	__UHEAP_MARK;

	ResetEcom();

	CheckDefaultPluginOnZ(aIsRomOnly);

	TUid interfaceUid;
	if(aIsRomOnly)
		{
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		interfaceUid = TUid::Uid(KInterfaceUid);
		}
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and ECOM will do a rediscovery and
	will not discover it.

	Interface UID   DLL	UID     Imp. UID    Version    DllFile
	---------------------------------------------------------------------------------------
	for ROM only
	0x10009DBD      0x10009DBC  0x10009DBE  1          C:\\..\\EComUpgradeROExample2.dll
	otherwise
	0x10009DBA      0x10009DB9  0x10009DBB  1          C:\\..\\EComUpgradeExample2.dll
	**/

	CopyPluginSameNameLowerVersion(aIsRomOnly);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	// The directory notifier will now detect that a new plugin has been added to the C drive and will cause
	// ECOM to perform a new discovery.

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();

	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}

	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();

	RDebug::Print(_L("After restart...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}

	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// tidy up
	// Remove plugins from C drive
	DeleteUpgradeExample2Plugins(aIsRomOnly);

	// Give ECOM a chance to rediscover after deletion of plugins.
	User::After(KOneSecond * 3);

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID			SYSLIB-ECOM-CT-1597
@SYMTestCaseDesc		Tests to ensure that an implementation will not be upgraded when the
						upgrading DLL/RSC name is different from the DLL/RSC to be upgraded.
						See CR65BCA3 even when the version is lower.
@SYMTestPriority		High
@SYMTestActions			Call REComSession::ListImplementations() and check V2 implementation
						on Z drive exists.
						Copy DLL/RSC with different name and V1 to DLL/RSC to C drive.
						Call REComSession::ListImplementations() and check the original V2
						implementation on Z drive has not been upgraded.
						Check for no crash and memory leaks.
						The same test is repeated for Rom Only drives. And no upgrade in version is
						observed.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF					DEF080682
*/
void RPluginUpgradeTest::UpgradePluginDifferentNameLowerVersionL(TBool aIsRomOnly)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1597 "));
	__UHEAP_MARK;

	ResetEcom();

	CheckDefaultPluginOnZ(aIsRomOnly);

	TUid interfaceUid = TUid::Null();
	if(aIsRomOnly)
		{
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		interfaceUid = TUid::Uid(KInterfaceUid);
		}
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and ECOM will do a rediscovery and attempt to discover it.

	Interface UID   DLL	UID     Imp. UID    Version    DllFile
	----------------------------------------------------------------------------------------
	for ROM only
	0x10009DBD      0x10009DBC  0x10009DBE  1          C:\\..\\EComUpgradeROExample1.dll
	otherwise
	0x10009DBA      0x10009DB9  0x10009DBB  1          C:\\..\\EComUpgradeExample1.dll
	**/

	CopyPluginDifferentNameLowerVersion(aIsRomOnly);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	// The directory notifier will now detect that a new plugin has been added to the C drive and will cause
	// ECOM to perform a new discovery.

	// The upgrade should fail an dthe existing implementation on the Z should remain

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();

	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();

	RDebug::Print(_L("After restart...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// tidy up
	// Remove plugins from C drive
	DeleteUpgradeExample1Plugins(aIsRomOnly);

	// Give ECOM a chance to rediscover after deletion of plugins.
	User::After(KOneSecond * 3);

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	__UHEAP_MARKEND;
	}


/*
@SYMTestCaseID			SYSLIB-ECOM-CT-0286
@SYMTestCaseDesc		Tests to ensure that an implementation can be upgraded when the
						upgrading DLL/RSC name is the same as the DLL/RSC name to be
						upgraded.
						See CR65BCA3.
@SYMTestPriority			High
@SYMTestActions			Call REComSession::ListImplementations() and check V1 implementation
						on Z drive exists.
						Copy DLL/RSC with same name as DLL/RSC to be upgraded to C drive.
						Call REComSession::ListImplementations() and check V3 implementation
						upgrades on C drive has upgraded the original implementation on Z drive.
						Restart ECOM (to test subsequent boot condition i.e. loading from dat file
						Call REComSession::ListImplementations() and check V3 implementation
						upgrades on C drive has upgraded the original implementation on Z drive.
						Check for no crash and memory leaks.
						The same test is repeated for Rom Only drives. And no upgrade in version is
						observed.
@SYMTestExpectedResults	The test must not fail.
@SYMPREQ				PREQ277
@SYMDEF					DEF080682
*/
void RPluginUpgradeTest::UpgradePluginSameNameHigherVersionL(TBool aIsRomOnly)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0286 "));
	__UHEAP_MARK;

	ResetEcom();

	CheckDefaultPluginOnZ(aIsRomOnly);

	TUid interfaceUid;
	if(aIsRomOnly)
		{
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		interfaceUid = TUid::Uid(KInterfaceUid);
		}
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and ECOM will do a rediscovery and discover it.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	for ROM only
	0x10009DBD      0x10009DBC  0x10009DBE  3          C:\\..\\EComUpgradeROExample2.dll
	otherwise
	0x10009DBA      0x10009DB9  0x10009DBB  3          C:\\..\\EComUpgradeExample2.dll
	**/

	CopyPluginSameNameHigherVersion(aIsRomOnly);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	// The directory notifier will now detect that a new plugin has been added to the C drive and will cause
	// ECOM to perform a new discovery.

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();

	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 3);
		TEST2(drive, EDriveC);
		}

	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();

	RDebug::Print(_L("After restart...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 3);
		TEST2(drive, EDriveC);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// tidy up
	// Remove plugins from C drive
	DeleteUpgradeExample2Plugins(aIsRomOnly);

	// Give ECOM a chance to rediscover after deletion of plugins.
	User::After(KOneSecond * 3);

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-0287
@SYMTestCaseDesc		Tests to ensure that an implementation will not be upgraded when the
						upgrading DLL/RSC name is different from the DLL/RSC to be upgraded.
						See CR65BCA3.
@SYMTestPriority			High
@SYMTestActions			Call REComSession::ListImplementations() and check V1 implementation
						on Z drive exists.
						Copy DLL/RSC with different name to DLL/RSC to be upgraded to C drive.
						Call REComSession::ListImplementations() and check the original V2
						implementation on Z drive has not been upgraded.
						Check for no crash and memory leaks.
						The same test is repeated for Rom Only drives. And no upgrade in version is
						observed.
@SYMTestExpectedResults	The test must not fail.
@SYMPREQ				PREQ277
@SYMDEF					DEF080682
*/
void RPluginUpgradeTest::UpgradePluginDifferentNameHigherVersionL(TBool aIsRomOnly)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0287 "));
	__UHEAP_MARK;

	ResetEcom();

	CheckDefaultPluginOnZ(aIsRomOnly);

	TUid interfaceUid;
	if(aIsRomOnly)
		{
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		interfaceUid = TUid::Uid(KInterfaceUid);
		}
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and ECOM will do a rediscovery and attempt to discover it.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	for ROM only
	0x10009DBD      0x10009DBC  0x10009DBE  3          C:\\..\\EComUpgradeROExample3.dll
	otherwise
	0x10009DBA      0x10009DB9  0x10009DBB  3          C:\\..\\EComUpgradeExample3.dll
	**/

	CopyPluginDifferentNameHigherVersion(aIsRomOnly);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	// The directory notifier will now detect that a new plugin has been added to the C drive and will cause
	// ECOM to perform a new discovery.

	// The upgrade should fail an dthe existing implementation on the Z should remain

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();

	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();

	RDebug::Print(_L("After restart...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// tidy up
	// Remove plugins from C drive
	DeleteUpgradeExample3Plugins(aIsRomOnly);

	// Give ECOM a chance to rediscover after deletion of plugins.
	User::After(KOneSecond * 3);

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-0288
@SYMTestCaseDesc		Tests to ensure that an implementation can be upgraded when the
						upgrading DLL/RSC name is the same as the DLL/RSC name to be
						upgraded and also the version is the same.
						See CR65BCA3.
@SYMTestPriority			High
@SYMTestActions			Call REComSession::ListImplementations() and check V1 implementation
						on Z drive exists.
						Copy DLL/RSC with same name as DLL/RSC to be upgraded to C drive.
						Call REComSession::ListImplementations() and check V2 implementation
						upgrades on C drive has upgraded the original implementation on Z drive.
						Check for no crash and memory leaks.
						Restart ECOM (to test subsequent boot condition i.e. loading from dat file
						Call REComSession::ListImplementations() and check V2 implementation
						upgrades on C drive has upgraded the original implementation on Z drive.
						The same test is repeated for Rom Only drives. And no upgrade in version is
						observed.
@SYMTestExpectedResults	The test must not fail.
@SYMPREQ				PREQ277
@SYMDEF					DEF080682
*/
void RPluginUpgradeTest::UpgradePluginSameNameSameVersionL(TBool aIsRomOnly)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0288 "));
	__UHEAP_MARK;

	ResetEcom();

	CheckDefaultPluginOnZ(aIsRomOnly);

	TUid interfaceUid;
	if(aIsRomOnly)
		{
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		interfaceUid = TUid::Uid(KInterfaceUid);
		}
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and ECOM will do a rediscovery and discover it.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	------------------------------------------------------------------------------------------------------------------------------------------
	for ROM only
	0x10009DBD      0x10009DBC  0x10009DBE  2          C:\\..\\EComUpgradeROExample2.dll
	otherwise
	0x10009DBA      0x10009DB9  0x10009DBB  2          C:\\..\\EComUpgradeExample2.dll
	**/

	CopyPluginSameNameSameVersion(aIsRomOnly);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	// The directory notifier will now detect that a new plugin has been added to the C drive and will cause
	// ECOM to perform a new discovery.

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();

	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveC);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();

	RDebug::Print(_L("After restart...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);
	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveC);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// tidy up
	// Remove plugins from C drive
	DeleteUpgradeExample2Plugins(aIsRomOnly);
	// Give ECOM a chance to rediscover after deletion of plugins.
	User::After(KOneSecond * 3);

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-0289
@SYMTestCaseDesc		Tests to ensure that an implementation will not be upgraded when the
						upgrading DLL/RSC name is different from the DLL/RSC to be upgraded
						and also the version is the same.
						See CR65BCA3.
@SYMTestPriority		High
@SYMTestActions			Call REComSession::ListImplementations() and check V2 implementation
						on Z drive exists.
						Copy DLL/RSC with different name, but same version, to DLL/RSC to be
						upgraded to C drive.
						Call REComSession::ListImplementations() and check the original V2
						implementation on Z drive has not been upgraded.
						Check for no crash and memory leaks.
						The same test is repeated for Rom Only drives. And no upgrade in version is
						observed.
@SYMTestExpectedResults	The test must not fail.
@SYMPREQ				PREQ277
@SYMDEF					DEF080682
*/
void RPluginUpgradeTest::UpgradePluginDifferentNameSameVersionL(TBool aIsRomOnly)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0289 "));
	__UHEAP_MARK;

	ResetEcom();

	CheckDefaultPluginOnZ(aIsRomOnly);

	TUid interfaceUid;
	if(aIsRomOnly)
		{
		interfaceUid = TUid::Uid(KRomOnlyInterfaceUid);
		}
	else
		{
		interfaceUid = TUid::Uid(KInterfaceUid);
		}
	RImplInfoPtrArray implArray;

	/**
	The following plugin is now added to the C drive and ECOM will do a rediscovery and attempt to discover it.

	Interface UID		DLL	UID		Imp. UID		Version		DllFile
	----------------------------------------------------------------------------------------
	for ROM only
	0x10009DBD      0x10009DBC  0x10009DBE  2          C:\\..\\EComUpgradeROExample3.dll
	otherwise
	0x10009DBA      0x10009DB9  0x10009DBB  2          C:\\..\\EComUpgradeExample3.dll
	**/

	CopyPluginDifferentNameSameVersion(aIsRomOnly);
	// Give ECOM a chance to discover new plugins.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	// The directory notifier will now detect that a new plugin has been added to the C drive and will cause
	// ECOM to perform a new discovery.

	// The upgrade should fail an dthe existing implementation on the Z should remain

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	TUid implUid = implArray[0]->ImplementationUid();
	TInt version = implArray[0]->Version();
	TInt drive = implArray[0]->Drive();
	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);

	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	// Get implementations for IF UID 0x10009DBA
	REComSession::ListImplementationsL(interfaceUid, implArray);

	//Expected number of implementations returned
	TEST2(implArray.Count(), 1);

	//Check that the implementation uid returned matched the specs above
	implUid = implArray[0]->ImplementationUid();
	version = implArray[0]->Version();
	drive = implArray[0]->Drive();
	RDebug::Print(_L("After new implementation is discovered...\n"));
	RDebug::Print(_L("implUid = 0x%x, version = %d drive = %d \n"), implUid, version, drive);

	if(aIsRomOnly)
		{
		TEST2(implUid.iUid, KRomOnlyImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	else
		{
		TEST2(implUid.iUid, KImplementationUid);
		TEST2(version, 2);
		TEST2(drive, EDriveZ);
		}
	implArray.ResetAndDestroy();

	// close session
	REComSession::FinalClose();

	// tidy up
	// Remove plugins from C drive
	DeleteUpgradeExample3Plugins(aIsRomOnly);
	// Give ECOM a chance to rediscover after deletion of plugins.
	User::After(KOneSecond * 3);

	// close session
	REComSession::FinalClose();

	// kill ecomserver
	KillEComServerL();

	__UHEAP_MARKEND;
	}

LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;


	TheTest.Next(_L("Upgrade rom only plugin with same name and lower version"));
	RPluginUpgradeTest::UpgradePluginSameNameLowerVersionL(ETrue);

	TheTest.Next(_L("Upgrade rom only plugin with different name and lower version"));
	RPluginUpgradeTest::UpgradePluginDifferentNameLowerVersionL(ETrue);

	TheTest.Next(_L("Upgrade rom only plugin with same name and higher version"));
	RPluginUpgradeTest::UpgradePluginSameNameHigherVersionL(ETrue);

	TheTest.Next(_L("Upgrade rom only plugin with different name and higher version"));
	RPluginUpgradeTest::UpgradePluginDifferentNameHigherVersionL(ETrue);

	TheTest.Next(_L("Upgrade rom only plugin with same name and same version"));
	RPluginUpgradeTest::UpgradePluginSameNameSameVersionL(ETrue);

	TheTest.Next(_L("Upgrade rom only plugin with different name and same version"));
	RPluginUpgradeTest::UpgradePluginDifferentNameSameVersionL(ETrue);

	TheTest.Next(_L("Upgrade plugin with same name and lower version"));
	RPluginUpgradeTest::UpgradePluginSameNameLowerVersionL(EFalse);

	TheTest.Next(_L("Upgrade plugin with different name and lower version"));
	RPluginUpgradeTest::UpgradePluginDifferentNameLowerVersionL(EFalse);

	TheTest.Next(_L("Upgrade plugin with same name and higher version"));
	RPluginUpgradeTest::UpgradePluginSameNameHigherVersionL(EFalse);

	TheTest.Next(_L("Upgrade plugin with different name and higher version"));
	RPluginUpgradeTest::UpgradePluginDifferentNameHigherVersionL(EFalse);

	TheTest.Next(_L("Upgrade plugin with same name and same version"));
	RPluginUpgradeTest::UpgradePluginSameNameSameVersionL(EFalse);

	TheTest.Next(_L("Upgrade plugin with different name and same version"));
	RPluginUpgradeTest::UpgradePluginDifferentNameSameVersionL(EFalse);

	__UHEAP_MARKEND;
	}

void CleanupPluginFiles()
	{
	DeleteUpgradeExample1Plugins(EFalse);
	DeleteUpgradeExample2Plugins(EFalse);
	DeleteUpgradeExample3Plugins(EFalse);
	DeleteUpgradeExample1Plugins(ETrue);
	DeleteUpgradeExample2Plugins(ETrue);
	DeleteUpgradeExample3Plugins(ETrue);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Start(_L("Plugin Upgrade Tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	// Cleanup files
	CleanupPluginFiles();

	//run tests
	TRAPD(err,RunTestL());

	// Cleanup files
	if(err != KErrNone)
		{
		CleanupPluginFiles();
		}

	TEST2(err, KErrNone);

	// cleanup session
	REComSession::FinalClose();

	delete scheduler;
	delete cleanup;

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return(0);
	}
