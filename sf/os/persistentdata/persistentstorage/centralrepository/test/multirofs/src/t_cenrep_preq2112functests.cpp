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
//

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include "t_cenrep_PatchData.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Defect Tests"));
const TUid KUidKeyspace1    = { 0x10286550 };
const TUid KUidKeyspace2    = { 0x10286551 };
const TUid KUidKeyspace3    = { 0x10286552 };
const TUid KUidKeyspace4    = { 0x10286553 };
const TUid KUidKeyspace5    = { 0x10286554 };
const TUid KUidKeyspace6    = { 0x10286555 };
const TUid KUidKeyspace7    = { 0x10286556 };
const TUid KUidKeyspace8    = { 0x10286557 };
const TUid KUidKeyspace9    = { 0x10286558 };
const TUid KUidKeyspace10    = { 0x10286559 };
const TUid KUidKeyspace11    = { 0x1028655a };

const TInt KMultiRofsPanicGlobalOverride = 9;
const TInt KEMultiRofsPanicTypeOveride = 10;
const TInt KEMultiRofsPanicIllegalRofs = 11;

_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");

//Test macroses and functions
LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}

struct TExitDetails
{
TExitCategoryName iCategory;
TExitType iExitType;
TInt iReason;
};

typedef void (*TestFunction)();

TInt TestFunctionLauncher(TAny* aTestFunction)
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
   		TestFunction function = (TestFunction)aTestFunction;

__UHEAP_MARK;
 		TRAP(r,function());
__UHEAP_MARKEND;

   		delete cleanup;
   		}
   	return r;
	}

TExitDetails LaunchTestThread(const TDesC& aThreadName, TestFunction aFunction)
	{
	RThread thread;
	thread.Create(aThreadName, &TestFunctionLauncher, KDefaultStackSize, NULL, (TAny*)aFunction);

	TRequestStatus threadStat;
	thread.Logon(threadStat);

	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);

	thread.Resume();
	User::WaitForRequest(threadStat);

	User::SetJustInTime(jit);

	TExitDetails exitDetails;
	exitDetails.iCategory = thread.ExitCategory();
	exitDetails.iReason = thread.ExitReason();
	exitDetails.iExitType = thread.ExitType();

	thread.Close();

	return exitDetails;
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

const TInt KKeyspace6Setting1Value = 1;
const TInt KKeyspace6Setting2Value = 3;
const TUint32 KKeyspace6Setting3Meta = 0xa;
const TUint32 KKeyspace6Setting4Meta = 0xc;

const TInt KKeyspace7Setting1Value = 7;
const TReal KKeyspace7Setting2Value = 10.7;
_LIT(KKeyspace7Setting3Value, "test 7");

const TInt KKeyspace8Setting1Value = 3;

const TInt KKeyspace9Setting1Value = 1;
const TInt KKeyspace9Setting2Value = 2;

const TInt KKeyspace10Setting1Value = 3;
const TReal KKeyspace10Setting2Value = 10.3;
_LIT(KKeyspace10Setting3Value, "test 3");

const TInt KKeyspace11Setting1Value = 2;
const TReal KKeyspace11Setting2Value = 10.1;
_LIT(KKeyspace11Setting3Value, "test 2");

const TInt KKeyOne = 1;
const TInt KKeyTwo = 2;
const TInt KKeyThree = 3;
const TInt KKeyFour = 4;
const TInt KKeyFive = 5;
const TInt KKeySix = 6;

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4053
@SYMTestCaseDesc Test that settings information can be selectively overridden in a ROFS layer
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286550.  This keyspace is represented by two txt files.  The
keyspace file names in the source have been mangled in a similar way to that which would be
expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the value of each setting specified in ROFS 1 has been
modified to the values specified in ROFS 1 and that each value not specified in ROFS 1 remains
as it was defined in the CORE layer.
@SYMREQ REQ11283
*/
LOCAL_C void REQ11283L()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4053 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace1));

	TInt settingValueInt = 0;
	TReal settingValueReal = 0.0;
	TBuf<20> settingValueStr;
	TUint32 settingMeta = 0;
	TInt rtn = 0;

	//check every single setting and meta and access policy
	User::LeaveIfError(rep->Get(1,settingValueInt));
	TEST(settingValueInt==1);
	User::LeaveIfError(rep->GetMeta(1,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(2,settingValueInt));
	TEST(settingValueInt==3);
	User::LeaveIfError(rep->GetMeta(2,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(3,settingValueReal));
	TEST(settingValueReal==3.1);
	User::LeaveIfError(rep->GetMeta(3,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(4,settingValueReal));
	TEST(settingValueReal==4.2);
	User::LeaveIfError(rep->GetMeta(4,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(5,settingValueStr));
	TEST(settingValueStr.Compare(_L("test 1"))==0);
	User::LeaveIfError(rep->GetMeta(5,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(6,settingValueStr));
	TEST(settingValueStr.Compare(_L("test 3"))==0);
	User::LeaveIfError(rep->GetMeta(6,settingMeta));
	TEST(settingMeta==0);

	//check value and meta combination
	User::LeaveIfError(rep->Get(7,settingValueInt));
	TEST(settingValueInt==8);
	User::LeaveIfError(rep->GetMeta(7,settingMeta));
	TEST(settingMeta==88);
	User::LeaveIfError(rep->Get(8,settingValueReal));
	TEST(settingValueReal==8.2);
	User::LeaveIfError(rep->GetMeta(8,settingMeta));
	TEST(settingMeta==888);
	User::LeaveIfError(rep->Get(9,settingValueStr));
	TEST(settingValueStr.Compare(_L("test 4"))==0);
	User::LeaveIfError(rep->GetMeta(9,settingMeta));
	TEST(settingMeta==888);

	//check value and platsec combination
	rtn=rep->Get(10,settingValueInt);
	TEST(rtn==KErrPermissionDenied);
	rtn=rep->Get(11,settingValueReal);
	TEST(rtn==KErrPermissionDenied);
	rtn=rep->Get(12,settingValueStr);
	TEST(rtn==KErrPermissionDenied);
	User::LeaveIfError(rep->Get(13,settingValueInt));
	TEST(settingValueInt==14);
	User::LeaveIfError(rep->GetMeta(13,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(14,settingValueInt));
	TEST(settingValueInt==15);
	User::LeaveIfError(rep->GetMeta(14,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Get(15,settingValueInt));
	TEST(settingValueInt==16);
	User::LeaveIfError(rep->GetMeta(15,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Set(15,150));
	User::LeaveIfError(rep->Get(16,settingValueInt));
	TEST(settingValueInt==16);
	User::LeaveIfError(rep->GetMeta(16,settingMeta));
	TEST(settingMeta==0);
	rtn=rep->Set(16,160);
	TEST(rtn==KErrPermissionDenied);


	//range meta
	User::LeaveIfError(rep->Get(0x20,settingValueInt));
	TEST(settingValueInt==16);
	User::LeaveIfError(rep->GetMeta(0x20,settingMeta));
	TEST(settingMeta==100);
	User::LeaveIfError(rep->Get(0x21,settingValueInt));
	TEST(settingValueInt==17);
	User::LeaveIfError(rep->GetMeta(0x21,settingMeta));
	TEST(settingMeta==101);

	//range platsec
	User::LeaveIfError(rep->Get(0x30,settingValueInt));
	TEST(settingValueInt==18);
	User::LeaveIfError(rep->GetMeta(0x30,settingMeta));
	TEST(settingMeta==0);
	rtn=rep->Set(0x30,0x31);
	TEST(rtn==KErrPermissionDenied);
	User::LeaveIfError(rep->Get(0x31,settingValueInt));
	TEST(settingValueInt==19);
	User::LeaveIfError(rep->GetMeta(0x31,settingMeta));
	TEST(settingMeta==0);
	User::LeaveIfError(rep->Set(0x31,0x32));

	//new settings
	User::LeaveIfError(rep->Get(0x50,settingValueInt));
	TEST(settingValueInt==50);
	User::LeaveIfError(rep->GetMeta(0x50,settingMeta));
	TEST(settingMeta==505);

	User::LeaveIfError(rep->Get(0x51,settingValueReal));
	TEST(settingValueReal==51.51);
	rtn=rep->Set(0x51,1.234);
	TEST(rtn==KErrPermissionDenied);

	User::LeaveIfError(rep->Get(0x52,settingValueStr));
	TEST(settingValueStr.Compare(_L("test 6"))==0);

	CleanupStack::PopAndDestroy(rep);
	}

/**
Ensure that an attempt to open keyspace 0x10286551, which will attempt to override
the global owner settings, causes the client attempting to open these keyspaces to panic with
EMultiRofsPanicGlobalOverride.
*/
LOCAL_C void PatchGlobalOwnerL()
	{
	CRepository* rep=NULL;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace2));
	CleanupStack::PopAndDestroy(rep);
	}

/**
Ensure that an attempt to open keyspace 0x10286552, which will attempt to override
the global metadata settings, causes the client attempting to open these keyspaces to panic with
EMultiRofsPanicGlobalOverride.
*/
LOCAL_C void PatchGlobalMetaL()
	{
	CRepository* rep=NULL;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace3));
	CleanupStack::PopAndDestroy(rep);
	}

/**
Ensure that an attempt to open keyspace 0x10286553, which will attempt to override
the global access control settings, causes the client attempting to open these keyspaces to panic with
EMultiRofsPanicGlobalOverride.
*/
LOCAL_C void PatchGlobalAccessControlL()
	{
	CRepository* rep=NULL;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace4));
	CleanupStack::PopAndDestroy(rep);
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4054
@SYMTestCaseDesc Attempt to modify policy settings in the keyspace from the ROFS layer
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286553.  This keyspace is represented by two txt files.  The
keyspace file names in the source have been mangled in a similar way to that which would be
expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that an attempt to override the global access control settings
will causes the client attempting to open these keyspaces to panic with EMultiRofsPanicGlobalOverride.
@SYMREQ REQ11400
        REQ11402
*/
LOCAL_C void PatchGlobalValues()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4054 " ));

	TExitDetails exitDetail;

	TheTest.Next(_L("doesn't support overriding of global owner policy"));
	exitDetail=LaunchTestThread(_L("global owner policy override"),&PatchGlobalOwnerL);
	TEST(exitDetail.iExitType==EExitPanic);
	TEST(exitDetail.iCategory.Compare(_L("CENREPCLI"))==0);
	TEST(exitDetail.iReason==KMultiRofsPanicGlobalOverride);

	TheTest.Next(_L("doesn't support overriding of global meta data policy"));
	exitDetail=LaunchTestThread(_L("global meta data policy override"),&PatchGlobalMetaL);
	TEST(exitDetail.iExitType==EExitPanic);
	TEST(exitDetail.iCategory.Compare(_L("CENREPCLI"))==0);
	TEST(exitDetail.iReason==KMultiRofsPanicGlobalOverride);

	TheTest.Next(_L("doesn't support overriding of global access control policy"));
	exitDetail=LaunchTestThread(_L("global access control policy override"),&PatchGlobalAccessControlL);
	TEST(exitDetail.iExitType==EExitPanic);
	TEST(exitDetail.iCategory.Compare(_L("CENREPCLI"))==0);
	TEST(exitDetail.iReason==KMultiRofsPanicGlobalOverride);
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4055
@SYMTestCaseDesc Ensure that the type of a setting cannot be overridden
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286554.  This keyspace is represented by two txt files.  The
keyspace file names in the source have been mangled in a similar way to that which would be
expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that an attempt to open keyspace 0x10286554 which will attempt
to override the type of a setting causes a EMultiRofsPanicTypeOveride panic.
@SYMREQ REQ11283
*/
LOCAL_C void PatchTypeL()
	{
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace5));
	CleanupStack::PopAndDestroy(rep);
	}


/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4060
@SYMTestCaseDesc Test that new settings can be added in subsequent ROFS layers
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286558.  This keyspace is represented by two txt files.  The
keyspace file names in the source have been mangled in a similar way to that which would be
expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the final value of the setting is as specified in the final
ROFS layer that touched that setting.
@SYMREQ REQ11398
*/
LOCAL_C void REQ11398L()
	{
	TheTest.Next(_L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4060 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace9));
	TInt settingValueInt = 0;

	// tests that the core value is not modified
	rep->Get(KKeyOne, settingValueInt);
	TEST2(settingValueInt, KKeyspace9Setting1Value);

	// tests that the core value is modified by the ROFS layer
	rep->Get(KKeyTwo, settingValueInt);
	TEST2(settingValueInt, KKeyspace9Setting2Value);

	CleanupStack::PopAndDestroy(rep);
	}


/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4056
@SYMTestCaseDesc Test to confirm that keyspace files of mixed types (txt and cre) can be used
to make up a keyspace
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286555.  This keyspace is represented by one cre file and one
txt files.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the final value of the setting is as specified in the final
ROFS layer that touched that setting.
@SYMREQ REQ11282
*/
LOCAL_C void MixedFileTypesL()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4056 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace6));
	TInt settingValueInt = 0;
	TBuf<20> settingValueStr;
	TUint32 settingMeta = 0;
	TInt rtn = 0;

	// tests that the core value is not modified
	rtn = rep->Get(KKeyOne, settingValueInt);
	TEST2(settingValueInt, KKeyspace6Setting1Value);

	// tests that the core value is modified by the ROFS layer
	rtn = rep->Get(KKeyTwo, settingValueInt);
	TEST2(settingValueInt, KKeyspace6Setting2Value);

	// tests that the core metadata value is not modified
	rep->GetMeta(KKeyThree, settingMeta);
	TEST2(settingMeta, KKeyspace6Setting3Meta);

	// tests that the core metadata value is modified by the ROFS layer
	rep->GetMeta(KKeyFour, settingMeta);
	TEST2(settingMeta, KKeyspace6Setting4Meta);

	// tests that the platsec details have not been modified
	rtn = rep->Get(KKeyFive, settingValueStr);
	TEST2(rtn, KErrNone);

	// tests that the platsec details were modified in the ROFS layer
	rtn = rep->Get(KKeySix, settingValueStr);
	TEST2(rtn, KErrPermissionDenied);

	CleanupStack::PopAndDestroy(rep);
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4057
@SYMTestCaseDesc Test that settings information can be selectively overridden on many ROFS layers
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286556.  This keyspace is represented by seven
txt files.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the final values of the settings are as specified in the final ROFS layer.
@SYMREQ REQ11282
        REQ11401
*/
LOCAL_C void REQ11401L()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4057 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace7));
	TInt settingValueInt = 0;
	TReal settingValueReal = 0.0;
	TBuf<20> settingValueStr;
	TInt valsequal = 0;

	//
	rep->Get(KKeyOne, settingValueInt);
	TEST2(settingValueInt, KKeyspace7Setting1Value);

	//
	rep->Get(KKeyTwo, settingValueReal);
	valsequal = settingValueReal - KKeyspace7Setting2Value;
	TEST2(valsequal, 0);

	//
	rep->Get(KKeyThree, settingValueStr);
	valsequal = settingValueStr.Compare(KKeyspace7Setting3Value);
	TEST2(valsequal, 0);


	CleanupStack::PopAndDestroy(rep);
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4058
@SYMTestCaseDesc Test that settings information can be selectively overridden on many ROFS layers when
the keyspace is not represented on all ROFS layers
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286557.  This keyspace is represented by three
txt files.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the final values of the settings are as specified in the final ROFS layer.
@SYMREQ REQ11401
*/
LOCAL_C void MissingRofsLayerL()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4058 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace8));
	TInt settingValueInt = 0;

	// tests that the core value is modified
	rep->Get(KKeyOne, settingValueInt);
	TEST2(settingValueInt, KKeyspace8Setting1Value);

	CleanupStack::PopAndDestroy(rep);
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4061
@SYMTestCaseDesc Test that global settings in overriding ROFS layer will not panic the client if the
properties are not modified
@SYMTestPriority High
@SYMTestActions Open keyspace 0x1028655a.  This keyspace is represented by two
txt files.  The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the final value of the setting is as specified in the final ROFS
layer.  Ie that there is no panic with EMultiRofsPanicGlobalOverride.
@SYMREQ REQ11400
*/
LOCAL_C void LegalGlobalOverrideL()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4061 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace11));
	TInt settingValueInt = 0;
	TReal settingValueReal = 0.0;
	TBuf<20> settingValueStr;
	TInt valsequal = 0;

	//
	rep->Get(KKeyOne, settingValueInt);
	TEST2(settingValueInt, KKeyspace11Setting1Value);

	//
	rep->Get(KKeyTwo, settingValueReal);
	valsequal = settingValueReal - KKeyspace11Setting2Value;
	TEST2(valsequal, 0);

	//
	rep->Get(KKeyThree, settingValueStr);
	valsequal = settingValueStr.Compare(KKeyspace11Setting3Value);
	TEST2(valsequal, 0);

	CleanupStack::PopAndDestroy(rep);
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4062
@SYMTestCaseDesc Test that the exattrib=u attribute cannot be incorrectly used in the CORE layer
@SYMTestPriority High
@SYMTestActions Specify exattrib=u for the CORE layer keyspace file in the corresponding IBY file.  
@SYMTestExpectedResults CenRep will panic the client opening the keyspace when the first ROFS layer 
keyspace file is referenced with exattrib=u in the IBY
@SYMREQ REQ11282
*/
LOCAL_C void LegalExAttribUse1L()
	{
	TUid invalid_uid={0xADABADAC};
	CRepository* rep=CRepository::NewL(invalid_uid);
	}
	
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-CT-4059
@SYMTestCaseDesc Test that settings information can be selectively overridden on many ROFS
layers when the keyspace is not represented on the CORE layer
@SYMTestPriority High
@SYMTestActions Open keyspace 0x10286559.  This keyspace is represented by two txt files.
The keyspace file names in the source have been mangled in a similar way to that
which would be expected on the device (eg. abcd1234.txt, abcd1234.txt[01-00], abcd1234.txt[02-00] etc.
@SYMTestExpectedResults Ensure that the final value of the setting is as specified in the final ROFS
layer.  Ie that there is no panic with EMultiRofsPanicGlobalOverride.
@SYMREQ REQ11401
*/
LOCAL_C void NoCoreKeyspaceL()
	{
	TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4059 " ));

	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidKeyspace10));

	TInt settingValueInt = 0;
	TReal settingValueReal = 0.0;
	TBuf<20> settingValueStr;
	TInt valsequal = 0;

	//
	rep->Get(KKeyOne, settingValueInt);
	TEST2(settingValueInt, KKeyspace10Setting1Value);

	//
	rep->Get(KKeyTwo, settingValueReal);
	valsequal = settingValueReal - KKeyspace10Setting2Value;
	TEST2(valsequal, 0);

	//
	rep->Get(KKeyThree, settingValueStr);
	valsequal = settingValueStr.Compare(KKeyspace10Setting3Value);
	TEST2(valsequal, 0);

	CleanupStack::PopAndDestroy(rep);
	}

LOCAL_C void FuncTestsL()
	{
	TExitDetails exitDetail;	
	//Run on 1 ROFS configuration
	if (KCentrepTestNumberROFS>1)
		{
		REQ11283L();

		TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4062 " ));		
		exitDetail=LaunchTestThread(_L("illegal use of exattrib=U"),&LegalExAttribUse1L);
		TEST(exitDetail.iExitType==EExitPanic);
		TEST(exitDetail.iCategory.Compare(_L("CENREPCLI"))==0);
		TEST(exitDetail.iReason==KEMultiRofsPanicIllegalRofs);
		}

	//Run on 2 ROFS configuration
	if (KCentrepTestNumberROFS>=2)
		{
		PatchGlobalValues();

		TheTest.Next(_L("supports adding of settings"));
		REQ11398L();

		TheTest.Next(_L("supports legal global override"));
		LegalGlobalOverrideL();
		}

	//Run on 3 ROFS configuration
	if (KCentrepTestNumberROFS>=3)
		{
		TheTest.Next(_L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-4055 " ));		
		exitDetail=LaunchTestThread(_L("type override"),&PatchTypeL);
		TEST(exitDetail.iExitType==EExitPanic);
		TEST(exitDetail.iCategory.Compare(_L("CENREPCLI"))==0);
		TEST(exitDetail.iReason==KEMultiRofsPanicTypeOveride);

		TheTest.Next(_L("tests situation where no keyspace file is in CORE layer but there are keyspaces in ROFS"));
		NoCoreKeyspaceL();

		TheTest.Next(_L("tests situation where a keyspace is represented on a number of layers but one layer has been missed out"));
		MissingRofsLayerL();
		}

	//RUn on 7 ROFS configuration
	if (KCentrepTestNumberROFS>=7)
		{
		TheTest.Next(_L("tests multiple ROFS layers"));
		REQ11401L();
		}

	//Run on 4 ROFS configuration
	if (KCentrepTestNumberROFS>=4)
		{
		TheTest.Next(_L("tests mixed binary and text keyspace files"));
		MixedFileTypesL();
		}

	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("PREQ2112 Component Testing"));
	CleanupCDriveL();

	TheTest.Next(_L("Functional Tests"));
	FuncTestsL();

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL();
	
	// As the merged persisted files on C: have been cleaned out, the Central Repository
	// server should be shutdown.
	KillProcess(KCentralRepositoryServerName);

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;

	return 0;
	}
