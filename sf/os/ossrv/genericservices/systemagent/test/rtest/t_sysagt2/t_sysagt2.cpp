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
// The main purpose of this application is to test
// starting of SysAgt2 server and testing RProperty::Get()/Set() methods on
// protected by capability properties.
// 
//

#include <e32test.h>
#include <e32debug.h>
#include <e32property.h>
#include <sacls.h>
#include <e32const.h>
#include <saclsdefines.h>
#include "SaPrivate.h"
#include <saclscommon.h>
static RTest TheTest(_L("T_SysAgt2"));

_LIT(KSysAgent2ServerName, "SysAgt2Svr");

//
//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)
//
//

struct keyAssociation 
{
	TUid iCategory;
	TUint iProperty;
	TBool iIsInt;
};

/**
 * These are keys which are defined by SystemAgent2
 * which belong to other subsystems.
 */

keyAssociation KSaExternalKeys[]=
	{
	KUidSystemCategory,KSAUidJavaInstallKeyValue,ETrue, 			// Java Install Key
	KUidSystemCategory,KSAUidSoftwareInstallKeyValue,ETrue,		// Software Install Key
	KUidSystemCategory,KUidSwiLatestInstallation,ETrue,
	KUidSystemCategory,KUidJmiLatestInstallation,ETrue,
	KUidSystemCategory,KUidUnifiedCertstoreFlag,ETrue,
	KUidSystemCategory,KUidBackupRestoreKey,ETrue,
	KSAPosIndicatorCategory,KSAPosIntGpsHwStatus,ETrue,
	KSAPosLastKnownLocationCategory,KSAPosLastKnownLocation,EFalse
	};

TBool IsProcessRunning(const TDesC& aProcessName)
	{
	TFullName name;
	TBool IsProcessRunning(EFalse);
	TBuf<64> pattern(aProcessName);
	TInt length = pattern.Length();
	pattern += _L("*");
	TFindProcess procFinder(pattern);

	while(procFinder.Next(name) == KErrNone)
		{
		if(name.Length() > length)
			{//If found name is a string containing aProcessName string.
			TChar c(name[length]);
			if(c.IsAlphaDigit() || c == TChar('_') || c == TChar('-'))
				{//If the found name is other valid application name starting with aProcessName string.
				RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
				continue;
				}
			}
		RProcess proc;
		if(proc.Open(name) == KErrNone)
			{
			if (EExitKill == proc.ExitType())
			    {
			    RDebug::Print(_L("\"%S\" process killed.\n"), &name);
			    proc.Close();
			    IsProcessRunning = EFalse;
			    }
			 else
			    {
			    IsProcessRunning = ETrue;
			    RDebug::Print(_L("\"%S\" process is running.\n"), &name);
			    }

			if(IsProcessRunning)
				{
				RDebug::Print(_L("Waiting additional time...\n"), &name);

				User::After(1000000);

				if (EExitKill == proc.ExitType())
					{
					RDebug::Print(_L("\"%S\" process now killed.\n"), &name);
			    	IsProcessRunning = EFalse;
					}

				proc.Close();
				}
			}


		}
	return IsProcessRunning;
	}

//
//

/**
@SYMTestCaseID          SYSLIB-SYSAGENT2-CT-1254
@SYMTestCaseDesc	    Tests for SysAgt2 server
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for RProperty::Set() and RProperty::Get() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void RunTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SYSAGENT2-CT-1254 StartSysAgt2() test 1 "));
	TInt err = ::StartSysAgt2();
	TEST(err == KErrNone);

	//proves that server does not run
	err = IsProcessRunning(KSysAgent2ServerName);
    TEST2(err,EFalse);

	TheTest.Next(_L("Capability test"));
	//The access to KUidPhonePwr, KUidSIMStatus, KUidNetworkStatus, KUidNetworkStrength,
	//KUidChargerStatus, KUidBatteryStrength, KUidCurrentCall capabilities is protected
	//by different capabilities. T_SAgtTest has only ReadDeviceData. Then, all attempts
	//to write/read to/from protected capabilities must fail.
    if(PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) &&
       PlatSec::IsCapabilityEnforced(ECapabilityWriteDeviceData))
    	{
		err = RProperty::Set(KUidSystemCategory, KUidPhonePwr.iUid, 1);
		TEST2(err, KErrPermissionDenied);

		err = RProperty::Set(KUidSystemCategory, KUidSIMStatus.iUid, 1);
		TEST2(err, KErrPermissionDenied);

		err = RProperty::Set(KUidSystemCategory, KUidNetworkStatus.iUid, 1);
		TEST2(err, KErrPermissionDenied);

		err = RProperty::Set(KUidSystemCategory, KUidNetworkStrength.iUid, 1);
		TEST2(err, KErrPermissionDenied);

		err = RProperty::Set(KUidSystemCategory, KUidChargerStatus.iUid, 1);
		TEST2(err, KErrPermissionDenied);

		err = RProperty::Set(KUidSystemCategory, KUidBatteryStrength.iUid, 1);
		TEST2(err, KErrPermissionDenied);

		err = RProperty::Set(KUidSystemCategory, KUidCurrentCall.iUid, 1);
		TEST2(err, KErrPermissionDenied);
    	}

	TheTest.Next(_L("Capability test 2"));
    //If all 48 predefined properties were registered successfully (by SysAgent server),
    //the test must be able to read their values.
    const TInt KPropertyCount = 48;
	TUid saUid = TUid::Uid(KUidPhonePwrValue);
    for(TInt i=0;i<KPropertyCount;++i)
	    {
        TInt val = 0;
	    err = RProperty::Get(KUidSystemCategory, saUid.iUid, val);
        TEST2(err, KErrNone);
	    ++saUid.iUid;
        }

	TheTest.Next(_L("Other defined Properties test"));
	// This test checks that the other publish and subscribe properties
	// have been defined by SystemAgent2. These are properties which are used
	// in other subsystems, mainly defined by this component due to PlatSec
	// restrictions requiring WriteUserData to define a property.
	for (TInt i=0; i < sizeof(KSaExternalKeys) / sizeof(KSaExternalKeys[0]); ++i)
		{
		if(KSaExternalKeys[i].iIsInt)
			{
			TInt propertyValue=0;
			err = RProperty::Get(KSaExternalKeys[i].iCategory, KSaExternalKeys[i].iProperty, propertyValue);
			}
		else
			{
			TBuf8<512> propertyValue;
			err = RProperty::Get(KSaExternalKeys[i].iCategory, KSaExternalKeys[i].iProperty, propertyValue);
			}
		

		// We expect the property to be defined, but we cannot make any assumptions
		// about what value it will have since it could have been changed before
		// the test gets here.
		TEST(err==KErrNone);
		}

	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	__UHEAP_MARK;

	TheTest.Title();
	TRAPD(err, ::RunTestsL());
	TheTest(err == KErrNone);
	TheTest.End();
	TheTest.Close();

	delete tc;

	__UHEAP_MARKEND;
	User::Heap().Check();
	return KErrNone;
	}
