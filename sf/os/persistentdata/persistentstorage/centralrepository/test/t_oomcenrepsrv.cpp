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
// t_oomcenrep.cpp
// 
//

#include "t_cenrep_helper.h"
#include <e32test.h>
#include <f32file.h>
#include <utf.h>
#include "srvsess.h"
#include "sessmgr.h"
#include "srvres.h"
#include "srvreqs.h"
#include "cachemgr.h"
#include "clientrequest.h"
#include "install.h"
#include <bautils.h>

LOCAL_D RFs					TheFs;
LOCAL_D RTest				TheTest (_L ("t_oomcenrep.exe"));

#ifdef __SECURE_DATA__
_LIT(KInstallDirFile,			"c:\\private\\10202BE9\\persists\\installdir.bin");

_LIT(KPersistsFile,				"c:\\private\\10202BE9\\persists\\11111111.cre");
_LIT(KInstallPersistsFile,		"c:\\private\\10202BE9\\persists\\11111110.cre");
_LIT(KPersistsFileNoUpgrade,	"c:\\private\\10202BE9\\bur\\11111111.crn");
_LIT(KPersistsFileUpgraded,		"c:\\private\\10202BE9\\bur\\11111111.cru");

_LIT(KRomUpgradeRev1File,		"z:\\private\\10202BE9\\11111111.txu");
_LIT(KRomUpgradeRev2File,		"z:\\private\\10202BE9\\11111112.txu");
_LIT(KUpgradeFile,				"c:\\private\\10202BE9\\11111111.txt");

_LIT(KInstallOnlyFile,			"z:\\private\\10202BE9\\11111110.cri");
_LIT(KInstallOnlyUpgradeFile,	"z:\\private\\10202BE9\\11111110.cru");
_LIT(KInstallFile,				"c:\\private\\10202BE9\\11111110.cre");

#else
_LIT(KPersistsFiles, 			"c:\\system\\data\\cenrep\\persists\\*.*");
#endif


//Test repositories Uid
const TUid KTestRepositoryUid={0x11111111};
const TUid KCorruptRepositoryUid={0xBADBADBB};

static TUid KCurrentTestUid;

///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions
LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		RDebug::Print(_L("Error\r\n"));
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
#define TESTL(arg) ::CheckL((arg), __LINE__)
#define TEST2L(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)
#define TESTKErrNoneL(aValue) ::CheckL(aValue,0,__LINE__);

/////////////////////////////////////////////////////////////////////////////////////////
class CenrepSrvOOMTest :public CBase
{
public:
	void GetL();
	void FindL();
	void ResetL();
	void NotifyL();
	void SetL();
	void CreateL();
	void DeleteL();
	void MoveL();

	CenrepSrvOOMTest();
	~CenrepSrvOOMTest();
	static CenrepSrvOOMTest* NewL();
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	void DoHeapRepositoryContentCheckL();
#endif
private:
	void ConstructL();
public:
	CServerRepository* iServerRepo;
	CSessionNotifier* iSessionNotif;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CenrepSwiOOMTest :public CBase
{
public:
	void UpgradeROMRev1L(TBool aIsSetup);
	void UpgradeROMRev2L(TBool aIsSetup);
	void UninstallROMUpgradeL(TBool aIsSetup);

	void InstallL(TBool aIsSetup);
	void UpgradeInstallL(TBool aIsSetup);
	void UninstallL(TBool aIsSetup);

	CenrepSwiOOMTest();
	~CenrepSwiOOMTest();
	static CenrepSwiOOMTest* NewL();
private:
	void ConstructL();
	CCentRepSWIWatcher* iSwiWatcher;
public:
};
//////////////////////////////////////////////////////////////////////////////////////////////////

CenrepSrvOOMTest::CenrepSrvOOMTest(){}

CenrepSrvOOMTest::~CenrepSrvOOMTest()
	{
	if (iServerRepo)
		{
		if (iSessionNotif)
			{
			iServerRepo->Close();
			}
		delete iServerRepo;
		}
	if (iSessionNotif)
		delete iSessionNotif;

	// Cache must be disabled here. Otherwise, if any idle repositories exists, they will
	// still be open (have their pointers in the iOpenRepositories list) and the list will leak.
	TServerResources::iCacheManager->DisableCache();
	TServerResources::iObserver->CloseiOpenRepositories();
	}

CenrepSrvOOMTest* CenrepSrvOOMTest::NewL()
	{
	CenrepSrvOOMTest* self=new (ELeave)CenrepSrvOOMTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CenrepSrvOOMTest::ConstructL()
	{
	iServerRepo=new (ELeave)CServerRepository();
	iSessionNotif=new (ELeave) CSessionNotifier();

	iServerRepo->OpenL(KCurrentTestUid,*iSessionNotif);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////

CenrepSwiOOMTest::CenrepSwiOOMTest(){}

CenrepSwiOOMTest::~CenrepSwiOOMTest()
	{
	delete iSwiWatcher;
	}

CenrepSwiOOMTest* CenrepSwiOOMTest::NewL()
	{
	CenrepSwiOOMTest* self=new (ELeave)CenrepSwiOOMTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CenrepSwiOOMTest::ConstructL()
	{
	iSwiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);
	}

//Getting various information and security policy from the repository
//TInt 				CServerRepository::Get(TUint32 aId,T& aVal)
//TServerSetting* 	CServerRepository::GetSetting(TUint32 aId)
void CenrepSrvOOMTest::GetL()
	{
	TInt err=KErrNone;
	TInt keyInt=0;
	TServerSetting* srvsetting;
	//----------Getting the TInt(1-15)-----------------------------
	for (TInt i=1;i<=15;i++)
		{
		err=iServerRepo->Get(i,keyInt);
		//Remember the policy check is done at the session level
		TESTKErrNoneL(err);
		TESTL(i==keyInt);
		srvsetting=iServerRepo->GetSetting(i);
		TESTL(srvsetting->Key()==static_cast<TUint32>(i));
		}
	//---------Getting the TReal(16-19)---------------------------
	TReal keyValue;
	err=iServerRepo->Get(16,keyValue);
	TESTKErrNoneL(err);
	TESTL(keyValue==10.1010);
	srvsetting=iServerRepo->GetSetting(16);
	TESTL(srvsetting->Key()==16);

	err=iServerRepo->Get(19,keyValue);
	TESTKErrNoneL(err);
	TESTL(keyValue==13.1313);

	//----------Getting the String(20-23)-----------------------
	TBuf8<50> keyString;
	_LIT(KFourteen,"fourteen");
	err=iServerRepo->Get(20,keyString);
	TESTKErrNoneL(err);
	//Even ascii(8 bytes) are stored as 16 bytes!!!
	TPtr16 str16((TUint16*) keyString.Ptr(),keyString.Length()/2,keyString.Length()/2);
	TESTL(str16.Compare(KFourteen)==0);
	srvsetting=iServerRepo->GetSetting(20);
	TESTL(srvsetting->Key()==20);

	//---------Getting the String8(0x79)------------------------
	TBuf8<50> keyString8;
	_LIT8(KString8,"string8");
	err=iServerRepo->Get(0x79,keyString8);
	TESTKErrNoneL(err);
	TESTL(keyString8.Compare(KString8)==0);
	srvsetting=iServerRepo->GetSetting(0x79);
	TESTL(srvsetting->Key()==0x79);

	//---------Getting the binary(0x82)-------------------------
	TBuf8<50> keyBinary;
	_LIT8(KBinary8,"\x12\x34\xAB\xCD");
	err=iServerRepo->Get(0x82,keyBinary);
	TESTKErrNoneL(err);
	//temporarily added to solve coverage problem in WINSCW
#ifdef __EPOC32__
	TESTL(keyBinary.Compare(KBinary8)==0);
#endif
	srvsetting=iServerRepo->GetSetting(0x82);
	TESTL(srvsetting->Key()==0x82);

	//----------Getting individual policy-----------------------
	//Current Exe has caps AllFiles+WriteDeviceData+ReadDeviceData
	RThread currentThread;
	TSecurityPolicy secPolicy;
	//default policy
	secPolicy=iServerRepo->GetDefaultReadAccessPolicy();
	TESTL(secPolicy.CheckPolicy(currentThread)==1);
	secPolicy=iServerRepo->GetDefaultWriteAccessPolicy();
	TESTL(secPolicy.CheckPolicy(currentThread)==1);

	//check settings policies
	//0x2 int 2 1 cap_rd=CommDD cap_wr=WriteDeviceData
	//0x19 int 25 0 //defined in range policies with cap_rd=NetworkServices
	secPolicy=iServerRepo->GetReadAccessPolicy(2);
	TESTL(secPolicy.CheckPolicy(currentThread)==0);
	secPolicy=iServerRepo->GetWriteAccessPolicy(2);
	TESTL(secPolicy.CheckPolicy(currentThread)==1);
	secPolicy=iServerRepo->GetReadAccessPolicy(25);
	TESTL(secPolicy.CheckPolicy(currentThread)==0);

	}

// now that write operations must be done in transactions, setting up this helper
// function to perform single Sets for the purpose of this test.
template<class T>
static TInt RepositorySingleSetL(CServerRepository& aRep, TUint32 aKey, const T& aValue)
	{
	aRep.StartTransaction(EConcurrentReadWriteTransaction);
	aRep.CleanupCancelTransactionPushL();
	aRep.TransactionSetL(aKey, aValue);
	CleanupStack::Pop();
	TUint32 keyInfo;
	return User::LeaveIfError(aRep.CommitTransaction(keyInfo));
	};

//Setting OOM testing
void CenrepSrvOOMTest::SetL()
	{
	TInt ret=KErrNone;
	TInt intValue=0;
	TReal realValue=0;

	//---------------SetL-----------------------------------
	//Setting an integer key
	ret = RepositorySingleSetL(*iServerRepo, 0x60, 600);
	TESTKErrNoneL(ret);
	ret=iServerRepo->Get(0x60,intValue);
	TESTL(intValue== 600);

	//Setting a real key
	ret = RepositorySingleSetL(*iServerRepo, 0x66, 99.99);
	TESTKErrNoneL(ret);
	ret=iServerRepo->Get(0x66,realValue);
	TESTL(realValue==99.99);

	//Setting a string key
	//Even ascii(8 bytes) are stored as 16 bytes!!!
	_LIT8(KString,"sixhundred");
	TBuf8<50> stringChangeValue=KString();
	ret = RepositorySingleSetL(*iServerRepo, 0x69, stringChangeValue);
	TESTKErrNoneL(ret);

	TBuf8<50> keyString;
	ret=iServerRepo->Get(0x69,keyString);
	TESTKErrNoneL(ret);
	TESTL(keyString.Compare(KString)==0);
	}

// now that write operations must be done in transactions, setting up this helper
// function to perform single Creates for the purpose of this test.
template<class T>
static TInt RepositorySingleCreateL(CServerRepository& aRep, TUint32 aKey, const T& aValue, TUint32* aMeta)
	{
	aRep.StartTransaction(EConcurrentReadWriteTransaction);
	aRep.CleanupCancelTransactionPushL();
	aRep.TransactionCreateL(aKey, aValue, aMeta);
	CleanupStack::Pop();
	TUint32 keyInfo;
	return User::LeaveIfError(aRep.CommitTransaction(keyInfo));
	};

void CenrepSrvOOMTest::CreateL()
	{
	TInt ret=KErrNone;
	TInt intValue=0;
	TReal realValue=0;

	//--------------CreateL-----------------------------------
	//Creating an integer key
	TInt intRetValue;
	intValue=9000;
	RepositorySingleCreateL(*iServerRepo, 0x90, intValue, NULL);
	TESTKErrNoneL(ret);
	ret=iServerRepo->Get(0x90,intRetValue);
	TESTKErrNoneL(ret);
	TESTL(intRetValue==intValue);

	//Creating a real key
	TReal realRetValue;
	realValue=33.3333;
	RepositorySingleCreateL(*iServerRepo, 0x92, realValue, NULL);
	TESTKErrNoneL(ret);
	ret=iServerRepo->Get(0x92,realRetValue);
	TESTKErrNoneL(ret);
	TESTL(realValue==realRetValue);

	//Creating a string key
	_LIT8(KStringValue,"creatingkey");
	TBuf8<50> stringCreateValue=KStringValue();
	RepositorySingleCreateL(*iServerRepo, 0x93, stringCreateValue, NULL);
	TESTKErrNoneL(ret);
	}

// now that write operations must be done in transactions, setting up this helper
// function to perform single Creates for the purpose of this test.
static TInt RepositorySingleDeleteL(CServerRepository& aRep, TUint32 aKey)
	{
	aRep.StartTransaction(EConcurrentReadWriteTransaction);
	aRep.CleanupCancelTransactionPushL();
	aRep.TransactionDeleteL(aKey);
	CleanupStack::Pop();
	TUint32 keyInfo;
	return User::LeaveIfError(aRep.CommitTransaction(keyInfo));
	};

void CenrepSrvOOMTest::DeleteL()
	{
	TInt ret=KErrNone;
	//--------------Delete-----------------------------------
	//Find the settings 0x10A-0x10C to ensure it is still there
	RSettingPointerArray matchingArray;
	TUint32 partialId=0x100;
	TUint32 idMask=0xFFFFFFF0;
	ret=iServerRepo->FindSettings(partialId,idMask,matchingArray);
	if (ret==KErrNoMemory)
		{
		matchingArray.Close();
		User::LeaveNoMemory();
		}

	TESTKErrNoneL(ret);
	TESTL(matchingArray.Count()==3);
	matchingArray.Close();

	//Deleting settings 0x10A to 0x10C
	for (TInt i=0x10A;i<=0x10C;i++)
		{
		RepositorySingleDeleteL(*iServerRepo, i);
		TESTKErrNoneL(ret);
		}
	//After deleting try to find the persistent settings again
	ret=iServerRepo->FindSettings(partialId,idMask,matchingArray);
	if (ret==KErrNoMemory)
		{
		matchingArray.Close();
		User::LeaveNoMemory();
		}
	TESTKErrNoneL(ret);
	TESTL(matchingArray.Count()==0);
	matchingArray.Close();

	//-------------DeleteRange---------------------------------
	//Deleting settings 0x1 to 0xF
	TClientRequest dummyrequest;
	TUint32 errId=0;
	partialId=0;
	idMask=0xFFFFFFF0;
	ret=iServerRepo->FindSettings(partialId,idMask,matchingArray);
	if (ret==KErrNoMemory)
		{
		matchingArray.Close();
		User::LeaveNoMemory();
		}
	TESTKErrNoneL(ret)	;
	TESTL(matchingArray.Count()==15);
	matchingArray.Close();

	//Deleting settings using the DeleteRange
	dummyrequest.SetParam(0,partialId);
	dummyrequest.SetParam(1,idMask);
	dummyrequest.SetPolicyCheck(ETrue);

	// write operation must take place in a transaction
	iServerRepo->StartTransaction(EConcurrentReadWriteTransaction);
	iServerRepo->CleanupCancelTransactionPushL();
	iServerRepo->TransactionDeleteRangeL(dummyrequest,errId);
	CleanupStack::Pop();
	TUint32 keyInfo;
	User::LeaveIfError(iServerRepo->CommitTransaction(keyInfo));

	//Now try to find the key being deleted
	ret=iServerRepo->FindSettings(partialId,idMask,matchingArray);
	if (ret==KErrNoMemory)
		{
		matchingArray.Close();
		User::LeaveNoMemory();
		}
	TESTKErrNoneL(ret);
	TESTL(matchingArray.Count()==0);
	matchingArray.Close();
	}

//Setting existing key value then follow by commit
void CenrepSrvOOMTest::MoveL()
	{
	RSettingPointerArray matchingArray;
	TUint32 idMask=0xFFFFFFF0;

	/*---------------MoveL-------------------------------------
	TUint32 sourcePartialId = aMessage.Int0();
	TUint32 targetPartialId = aMessage.Int1();
	TUint32 idMask = aMessage.Int2();
	*/
	TClientRequest dummyrequest;
	TUint32 errId=0;
	TUint32 sourcePartialId=0x110;
	TUint32 targetPartialId=0x120;
	idMask=0xFFFFFFF0;

	TKeyFilter srcKeyIdentifier = {sourcePartialId, idMask};
	TPckg<TKeyFilter> pSrcIdentifier(srcKeyIdentifier);
	TKeyFilter tgtKeyIdentifier = {targetPartialId, idMask};
	TPckg<TKeyFilter> pTgtIdentifier(tgtKeyIdentifier);

	//First check to ensure the target key before move does not exist
	User::LeaveIfError(iServerRepo->FindSettings(targetPartialId,idMask,matchingArray));
	TESTL(matchingArray.Count()==0);
	matchingArray.Close();

	//moving from 0x110(0x11B,0x11C,0x11E) to 0x120
	dummyrequest.SetParam(0, &pSrcIdentifier);
	dummyrequest.SetParam(1, &pTgtIdentifier);
	dummyrequest.SetPolicyCheck(ETrue);

	// write operation must take place in a transaction
	iServerRepo->StartTransaction(EConcurrentReadWriteTransaction);
	iServerRepo->CleanupCancelTransactionPushL();
	iServerRepo->TransactionMoveL(dummyrequest,errId);
	CleanupStack::Pop();
	TUint32 keyInfo;
	User::LeaveIfError(iServerRepo->CommitTransaction(keyInfo));

	//Now try to find the key being moved
	User::LeaveIfError(iServerRepo->FindSettings(targetPartialId,idMask,matchingArray));
	TESTL(matchingArray.Count()==3);
	matchingArray.Close();
	}

//Finding keys from the settings
//TInt FindSettings(TUint32 aPartialId,TUint32 aIdMask,RSettingPointerArray& aMatches)
//Guarantees the heap free in aMatches if this function fail
void CenrepSrvOOMTest::FindL()
	{
	TInt ret=KErrNone;
	RSettingPointerArray foundIdArray;
	TUint32 partialId=0;
	TUint32 idMask=0;
	//-----------Finding settings array using partial id & mask------

	//------------------Real type---------------------------------
	//0x42,0x44,0x45,0x48
	partialId=0x40;
	idMask=0xFFFFFFF0;
	User::LeaveIfError(iServerRepo->FindSettings(partialId,idMask,foundIdArray));
	TESTKErrNoneL(ret);
	TESTL(foundIdArray.Count()==4);
	foundIdArray.Close();

	//-----------------String type-------------------------------
	//0x51,0x54,0x5B
	partialId=0x50;
	idMask=0xFFFFFFF0;
	User::LeaveIfError(iServerRepo->FindSettings(partialId,idMask,foundIdArray));
	TESTKErrNoneL(ret);
	TESTL(foundIdArray.Count()==3);
	foundIdArray.Close();

	//--------------------Int type----------------------------------------------------
	partialId=0x30;
	idMask=0xFFFFFFF0;
	//This should return only 0x30,0x34,0x35,0x39,0x3B( 5 items)
	User::LeaveIfError(iServerRepo->FindSettings(partialId,idMask,foundIdArray));
	TESTKErrNoneL(ret);
	TESTL(foundIdArray.Count()==5);

	//----------------Find comparison using EEqual & ENotEqual------------------------
	TInt searchValue=100;
	TClientRequest dummyrequest;
	RArray<TUint32> idArray;
	//Set the policycheck to always pass
	dummyrequest.SetPolicyCheck(ETrue);
	//Comparison using Equal
	TRAP(ret,iServerRepo->FindCompareL(foundIdArray,searchValue,EEqual,idArray));
	if (ret==KErrNoMemory)
		{
		//do not need to reset idArray as it is done inside the function itself when it returns not KErrNone
		foundIdArray.Close();
		User::LeaveNoMemory();
		}
	TESTL(idArray.Count()==2);
	TESTL((idArray[0]==0x30 && idArray[1]==0x34) || (idArray[0]==0x34 && idArray[1]==0x30));
	idArray.Close();
	//Comparison using ENotEqual
	TRAP(ret,iServerRepo->FindCompareL(foundIdArray,searchValue,ENotEqual,idArray));
	if (ret==KErrNoMemory)
		{
		//do not need to reset idArray as it is done inside the function itself when it returns not KErrNone
		foundIdArray.Close();
		User::LeaveNoMemory();
		}
	TESTL(idArray.Count()==3);
	idArray.Close();
	foundIdArray.Close();

	}

//Resetting settings
void CenrepSrvOOMTest::ResetL()
	{
	TInt ret=KErrNone;
	TInt retValue=0;
	TReal realValue=0;

	//-------------Single key reset----------------------------

	ret = RepositorySingleSetL(*iServerRepo, 1, 500);
	TESTKErrNoneL(ret);
	ret=iServerRepo->Get(1,retValue);
	TESTL(retValue==500);

	//Resetting individual settings
	ret=iServerRepo->ResetL(1);
	TESTKErrNoneL(ret);

	//Check for value once being reset
	ret=iServerRepo->Get(1,retValue);
	TESTL(retValue==1);

	//------------All keys reset------------------------------
	ret = RepositorySingleSetL(*iServerRepo, 17, 3.1343424);
	TESTKErrNoneL(ret);

	//Reset all settings from Rom
	User::LeaveIfError(iServerRepo->ResetAllL());
	TESTKErrNoneL(ret);

	//Check for value once all being reset

	ret=iServerRepo->Get(17,realValue);
	TESTL(realValue==11.1111);
	}

void CenrepSrvOOMTest::NotifyL()
	{
	TInt err=KErrNone;

	//addding individual requests
	for (TInt i=0;i<10;i++)
		{
		TClientRequest dummyRequest;
		User::LeaveIfError(iSessionNotif->AddRequest(i,dummyRequest));
		}

	//adding group requests
	for (TInt i=0;i<10;i++)
		{
		TClientRequest dummyRequest;
		TUint32 partialId=100*i;
		TUint32 idMask=0xFFFFFFF0;
		User::LeaveIfError(iSessionNotif->AddRequest(partialId,idMask,dummyRequest));
		}

	//cancelling individual requests
	User::LeaveIfError(iSessionNotif->CancelRequest(5));
	//Check to ensure that it has been deleted so calling cancel again will return KErrNotFound
	err=iSessionNotif->CancelRequest(5);
	TESTL(err==KErrNotFound);

	//cancelling group requests
	User::LeaveIfError(iSessionNotif->CancelRequest(500,0xFFFFFFF0));
	err=iSessionNotif->CancelRequest(500,0xFFFFFF0);
	TESTL(err==KErrNotFound);

	//Finally cancel ALL requests
	iSessionNotif->CancelAllRequests();

	}

//Deletes the CServerRepository object properly
LOCAL_C void ReleaseRepository(TAny* aServerRepository)
	{
	CServerRepository* serverRepository = static_cast<CServerRepository*>(aServerRepository);

	TServerResources::iCacheManager->DisableCache();
	TServerResources::iObserver->CloseiOpenRepositories();
	delete serverRepository;
	TServerResources::iCacheManager->EnableCache();
	}

//Opening a repository and closing the repository
LOCAL_C void CreateDeleteL(TBool /*aOOMMode*/)
{
	CServerRepository* serverRepo=new (ELeave)CServerRepository();
	TCleanupItem cleanupItem(&ReleaseRepository, serverRepo);
	CleanupStack::PushL(cleanupItem);

	CSessionNotifier* sessNotif=new (ELeave)CSessionNotifier();
	CleanupStack::PushL(sessNotif);

// 	test access to a valid repository
	serverRepo->OpenL(KCurrentTestUid,*sessNotif);
	serverRepo->Close();

	CleanupStack::PopAndDestroy(sessNotif);
	CleanupStack::PopAndDestroy(1);
}

LOCAL_C void CreateDeleteCorruptL(TBool /*aOOMMode*/)
{
	CServerRepository* serverRepo=new (ELeave)CServerRepository();
	TCleanupItem cleanupItem(&ReleaseRepository, serverRepo);
	CleanupStack::PushL(cleanupItem);

	CSessionNotifier* sessNotif=new (ELeave)CSessionNotifier();
	CleanupStack::PushL(sessNotif);

// 	test access to a corrupt repository
	// trap KErrCorrupt
	TRAPD(err, serverRepo->OpenL(KCorruptRepositoryUid,*sessNotif));
	// leave if not KErrCorrupt
	User::LeaveIfError((err == KErrCorrupt) ? KErrNone : err);
	serverRepo->Close();

	CleanupStack::PopAndDestroy(sessNotif);
	CleanupStack::PopAndDestroy(1);
}

void CenrepSwiOOMTest::UpgradeROMRev1L(TBool aIsSetup)
	{
	if(aIsSetup)
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Copy rev 1 file into install dir & Reset read-only bit
		User::LeaveIfError(fm->Copy(KRomUpgradeRev1File, KUpgradeFile));
		User::LeaveIfError(fm->Attribs(KUpgradeFile,0,KEntryAttReadOnly,TTime(0)));
		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		iSwiWatcher->HandleSWIEventL(ESASwisInstall | ESASwisStatusSuccess);
		}
	}

LOCAL_C void ModifyTimeStampL(RFs &fs, TDesC *fileName, TTime aTime)
	{
	// Reset read-only bit
	User::LeaveIfError(fs.SetAtt(*fileName, 0, KEntryAttReadOnly));
	TTimeIntervalHours interval(1);
	TTime newTime=aTime+interval;
	User::LeaveIfError(fs.SetModified(*fileName, newTime));
	}


void CenrepSwiOOMTest::UpgradeROMRev2L(TBool aIsSetup)
	{
	if(aIsSetup)
		{
		// Set up rev 1
		UpgradeROMRev1L(ETrue);
		UpgradeROMRev1L(EFalse);

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Get modification time
		TTime time;
		TBuf<50> fileName(KUpgradeFile);
		fs.Modified(fileName, time);

		// Copy rev2 file into install dir & Reset read-only bit
		User::LeaveIfError(fm->Copy(KRomUpgradeRev2File, KUpgradeFile));
		User::LeaveIfError(fm->Attribs(KUpgradeFile,0,KEntryAttReadOnly,TTime(0)));
		// Modify timestamp to cause upgrade
		ModifyTimeStampL(fs,&fileName, time);

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		iSwiWatcher->HandleSWIEventL(ESASwisInstall | ESASwisStatusSuccess);
		}

	}

void CenrepSwiOOMTest::UninstallROMUpgradeL(TBool aIsSetup)
	{
	if(aIsSetup)
		{
		// Install rev 2
		UpgradeROMRev2L(ETrue);
		UpgradeROMRev2L(EFalse);

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Delete file from install dir
		User::LeaveIfError(fm->Attribs(KUpgradeFile,0,KEntryAttReadOnly,TTime(0)));
		TInt err=fs.Delete(KUpgradeFile);
		if((err!=KErrNone)&&(err!=KErrNotFound))
			User::Leave(err);

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		iSwiWatcher->HandleSWIEventL(ESASwisUninstall | ESASwisStatusSuccess);
		}
	}

void CenrepSwiOOMTest::InstallL(TBool aIsSetup)
	{
	if(aIsSetup)
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Copy rev 1 file into install dir & Reset read-only bit
		User::LeaveIfError(fm->Copy(KInstallOnlyFile, KInstallFile));
		User::LeaveIfError(fm->Attribs(KInstallFile,0,KEntryAttReadOnly,TTime(0)));

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		iSwiWatcher->HandleSWIEventL(ESASwisInstall | ESASwisStatusSuccess);
		}
	}

void CenrepSwiOOMTest::UpgradeInstallL(TBool aIsSetup)
	{
	if(aIsSetup)
		{
		// Install file
		InstallL(ETrue);
		InstallL(EFalse);

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Get modification time
		TTime time;
		TBuf<50> fileName(KInstallFile);
		fs.Modified(fileName, time);

		// Copy upgrade file into install dir & Reset read-only bit
		User::LeaveIfError(fm->Copy(KInstallOnlyUpgradeFile, KInstallFile));
		User::LeaveIfError(fm->Attribs(KInstallFile,0,KEntryAttReadOnly,TTime(0)));
		// Modify timestamp to cause upgrade
		ModifyTimeStampL(fs,&fileName, time);

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		iSwiWatcher->HandleSWIEventL(ESASwisInstall | ESASwisStatusSuccess);
		}
	}

void CenrepSwiOOMTest::UninstallL(TBool aIsSetup)
	{
	if(aIsSetup)
		{
		// Install upgrade
		UpgradeInstallL(ETrue);
		UpgradeInstallL(EFalse);

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Delete file from install dir
		User::LeaveIfError(fm->Attribs(KInstallFile,0,KEntryAttReadOnly,TTime(0)));
		TInt err=fs.Delete(KInstallFile);
		if((err!=KErrNone)&&(err!=KErrNotFound))
			User::Leave(err);

		// Create a cre persists file, doesn't matter what's in it, it should be deleted
		User::LeaveIfError(fm->Copy(KPersistsFileUpgraded, KInstallPersistsFile));
		User::LeaveIfError(fm->Attribs(KInstallPersistsFile,0,KEntryAttReadOnly,TTime(0)));

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		iSwiWatcher->HandleSWIEventL(ESASwisUninstall | ESASwisStatusSuccess);
		}
	}

LOCAL_C void StartupUpgradeL(TBool aIsSetup)
{
	if(aIsSetup)
		{
		// Set up files for test
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Clean out files
		TInt err=fs.Delete(KInstallDirFile);
		if((err!=KErrNone)&&(err!=KErrNotFound))
			User::Leave(err);

		// Cause directory listing with no files to be written
		CCentRepSWIWatcher*	swiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);
		delete swiWatcher;

		User::LeaveIfError(fm->Copy(KPersistsFileNoUpgrade, KPersistsFile));
		User::LeaveIfError(fm->Attribs(KPersistsFile,0,KEntryAttReadOnly,TTime(0)));

		User::LeaveIfError(fm->Copy(KRomUpgradeRev1File, KUpgradeFile));
		User::LeaveIfError(fm->Attribs(KUpgradeFile,0,KEntryAttReadOnly,TTime(0)));

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		CCentRepSWIWatcher*	swiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);
		delete swiWatcher;
		}
}

LOCAL_C void StartupDowngradeL(TBool aIsSetup)
{
	if(aIsSetup)
		{
		StartupUpgradeL(ETrue);
		StartupUpgradeL(EFalse);

		// Set up files for test
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TInt err=fs.Delete(KUpgradeFile);
		if((err!=KErrNone)&&(err!=KErrNotFound))
			User::Leave(err);

		CleanupStack::PopAndDestroy(); // fs
		}
	else
		{
		CCentRepSWIWatcher*	swiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);
		delete swiWatcher;
		}
}

LOCAL_C void StartupUninstallL(TBool aIsSetup)
{
	if(aIsSetup)
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);

		// Copy rev 1 file into install dir & Reset read-only bit
		User::LeaveIfError(fm->Copy(KInstallOnlyFile, KInstallFile));
		User::LeaveIfError(fm->Attribs(KInstallFile,0,KEntryAttReadOnly,TTime(0)));

		// Cause directory listing to be written and file to be installed
		CCentRepSWIWatcher*	swiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);
		delete swiWatcher;

		// Create a persists file that will be deleted
		User::LeaveIfError(fm->Copy(KPersistsFileUpgraded, KInstallPersistsFile));
		User::LeaveIfError(fm->Attribs(KInstallPersistsFile,0,KEntryAttReadOnly,TTime(0)));

		TInt err=fs.Delete(KInstallFile);
		if((err!=KErrNone)&&(err!=KErrNotFound))
			User::Leave(err);

		CleanupStack::PopAndDestroy(2); // fs and fm
		}
	else
		{
		CCentRepSWIWatcher*	swiWatcher = CCentRepSWIWatcher::NewL(TServerResources::iFs);
		delete swiWatcher;
		}
}

// Type definition for pointer to function
// Used for functions that can't use CenrepSrvOOMTest::ConstructL
typedef void (*FuncPtrL) (TBool);
/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMNoServReposL( FuncPtrL atestFuncL, const TDesC& aTestDesc, TBool aOOMMode)
	{
	TheTest.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		//Initializing the server resources
		TServerResources::InitialiseL ();
		//Clear any files in the persist directory
		CleanupCDriveL();

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		(*atestFuncL)(ETrue);

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (*atestFuncL)(EFalse));
		if (err!=KErrNoMemory)
			TESTKErrNoneL(err);

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		TEST2L(startProcessHandleCount, endProcessHandleCount);
		TEST2L(startThreadHandleCount, endThreadHandleCount);

		//Freeing the server resources
		TServerResources::Close();

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TESTKErrNoneL(err);
 	if (aOOMMode)
		TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

// Type definition for pointer to member function.
// Used in calling the CRegistryDataTest member function for testing.
typedef void (CenrepSrvOOMTest::*ClassFuncPtrL) (void);
/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
@param		aOOMMode to enable/disable the OOM environment
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc,TBool aOOMMode)
	{
	TheTest.Next(aTestDesc);

	TInt err=KErrNone;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		//Initializing the server resources
		TServerResources::InitialiseL ();

		//Clear any files in the persist directory
		CleanupCDriveL();

		CenrepSrvOOMTest* theTest=CenrepSrvOOMTest::NewL();
		CleanupStack::PushL(theTest);

		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (theTest->*testFuncL)());

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::ENone, 0);

		if (err!=KErrNoMemory)
			TESTKErrNoneL(err);

		CleanupStack::PopAndDestroy(theTest);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST2L(startProcessHandleCount, endProcessHandleCount);
		TEST2L(startThreadHandleCount, endThreadHandleCount);

		//Freeing the server resources
		TServerResources::Close();
		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TESTKErrNoneL(err);
 	if (aOOMMode)
		TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

// Type definition for pointer to member function.
// Used in calling the CRegistryDataTest member function for testing.
typedef void (CenrepSwiOOMTest::*ClassSwiFuncPtrL) (TBool);
/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
@param		aOOMMode to enable/disable the OOM environment
*/
LOCAL_C void DoOOMSwiTestL(ClassSwiFuncPtrL aTestFuncL, const TDesC& aTestDesc,TBool aOOMMode)
	{
	TheTest.Next(aTestDesc);

	TInt err=KErrNone;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		//Initializing the server resources
		TServerResources::InitialiseL ();
		//Clear any files in the persist directory
		CleanupCDriveL();

		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CenrepSwiOOMTest* theTest=CenrepSwiOOMTest::NewL();
		CleanupStack::PushL(theTest);

		// Set up test
		(theTest->*aTestFuncL)(ETrue);

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (theTest->*aTestFuncL)(EFalse));

		if (aOOMMode)
			__UHEAP_SETFAIL(RHeap::ENone, 0);

		if (err!=KErrNoMemory)
			TESTKErrNoneL(err);

		CleanupStack::PopAndDestroy(theTest);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST2L(startProcessHandleCount, endProcessHandleCount);
		TEST2L(startThreadHandleCount, endThreadHandleCount);

		//Freeing the server resources
		TServerResources::Close();
		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TESTKErrNoneL(err);
 	if (aOOMMode)
		TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
void CenrepSrvOOMTest::DoHeapRepositoryContentCheckL()
	{
	CServerRepository* srv=iServerRepo;
	//check setting and its meta
	TServerSetting* setting=NULL;

	setting=srv->GetSetting(1);
	TESTL(setting->iKey==1);
	TESTL(setting->iMeta==0x80000010);
	//points to global default policy here
	TESTL(setting->iAccessPolicy->LowKey()==KUnspecifiedKey);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==1);

	setting=srv->GetSetting(2);
	TESTL(setting->iKey==2);
	TESTL(setting->iMeta==0xA0000010);
	//points to global default policy here
	TESTL(setting->iAccessPolicy->LowKey()==KUnspecifiedKey);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==1);

	setting=srv->GetSetting(3);
	TESTL(setting->iKey==3);
	TESTL(setting->iMeta==0x800000FF);
	//points to global default policy here
	TESTL(setting->iAccessPolicy->LowKey()==KUnspecifiedKey);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==1);

	setting=srv->GetSetting(4);
	TESTL(setting->iKey==4);
	TESTL(setting->iMeta==0x80000010);
	TESTL(setting->iAccessPolicy->LowKey()==4);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==0);

	setting=srv->GetSetting(5);
	TESTL(setting->iKey==5);
	TESTL(setting->iMeta==0xC0000063);
	//points to global default policy here
	TESTL(setting->iAccessPolicy->LowKey()==KUnspecifiedKey);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==1);

	setting=srv->GetSetting(6);
	TESTL(setting->iKey==6);
	TESTL(setting->iMeta==0x90000010);
	TESTL(setting->iAccessPolicy->LowKey()==6);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==0);

	setting=srv->GetSetting(7);
	TESTL(setting->iKey==7);
	TESTL(setting->iMeta==0x80000010);
	TESTL(setting->iAccessPolicy->LowKey()==7);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==0);

	setting=srv->GetSetting(9);
	TESTL(setting->iKey==9);
	TESTL(setting->iMeta==0x80000010);
	TESTL(setting->iAccessPolicy->LowKey()==KUnspecifiedKey);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==1);

	setting=srv->GetSetting(10);
	TESTL(setting->iKey==10);
	TESTL(setting->iMeta==0x80000010);
	TESTL(setting->iAccessPolicy->LowKey()==10);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==0);

	setting=srv->GetSetting(300);
	TESTL(setting->iKey==300);
	TESTL(setting->iMeta==0x900003E7);
	TESTL(setting->iAccessPolicy->LowKey()==KUnspecifiedKey);
	TESTL(setting->iAccessPolicy->HighKey()==1);
	TESTL(setting->iAccessPolicy->KeyMask()==1);
	}

LOCAL_C void DoAdditionalCheckingL()
	{
	TServerResources::InitialiseL ();

	CenrepSrvOOMTest* test=CenrepSrvOOMTest::NewL();
	CleanupStack::PushL(test);
	test->DoHeapRepositoryContentCheckL();

	CleanupStack::PopAndDestroy();

	TServerResources::Close();
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4050
@SYMTestCaseDesc Verifying that CRE generated will have the latest CRE version which is currently 2
@SYMTestPriority High
@SYMTestActions  Validating that CRE files generated with post PREQ2112 code will always contain version 2.
				 The unit test will load a txt repository and then modify some settings so that it gets persisted
				 in the persists directory. The test then read the cre files to verify that the version persisted
				 is the latest which is 2.
@SYMTestExpectedResults The correct file version is returned.
@SYMREQ REQ11282
*/
LOCAL_C void DoPersistedVersionCheckingL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4050 Verifying CRE generated will always be version 2 "));
	TServerResources::InitialiseL ();

	const TUid KTestUid={0x88880000};
	KCurrentTestUid=KTestUid;

	CenrepSrvOOMTest* test=CenrepSrvOOMTest::NewL();
	CleanupStack::PushL(test);

	//persist immediately
	test->iServerRepo->CommitChangesL();

	//now check the version of the CRE file
	CHeapRepository* heap=CHeapRepository::NewL(KTestUid);
	CleanupStack::PushL(heap);
	TUint8 creVersion;
	heap->CreateRepositoryFromCreFileL(TServerResources::iFs,_L("c:\\private\\10202be9\\persists\\88880000.cre"),creVersion);
	TESTL(creVersion==KPersistFormatSupportsIndMetaIndicator);

	CleanupStack::PopAndDestroy(2,test);
	TServerResources::Close();
	}
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4051
@SYMTestCaseDesc Server side OOM loading of a multiple repository files
@SYMTestPriority High
@SYMTestActions  Create the server class CServerRepository and used it to load the multi ROFS keyspace.
				 Test are repeated for the following keyspace combination such as TXT/TXT, CRE/TXT or TXT/CRE and CRE/CRE.
				 Test will also verify the resulting merged settings which includes checking
				 individual metadata bits and security policy bits
@SYMTestExpectedResults Server loading of the multi rofs keypsace should not fail and leak memory under OOM condition.
@SYMREQ REQ11282
*/
LOCAL_C void DoOOMMultiRofsTestL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4051 CENREPSRV MultiROFS OOM Test "));

	const TUid KMultiRofsRepositoryUid1={0xFFFF0000};
	const TUid KMultiRofsRepositoryUid2={0xEEEE0000};
	const TUid KMultiRofsRepositoryUid3={0xDDDD0000};
	const TUid KMultiRofsRepositoryUid4={0xCCCC0000};

	//First Testuid=KMultiRofsRepositoryUid
	//Testing the OOM of multi rofs processing
	KCurrentTestUid=KMultiRofsRepositoryUid1;
	DoOOMNoServReposL(&CreateDeleteL, _L("Create Delete OOM Multi ROFS Test"),ETrue);
	DoAdditionalCheckingL();
	KCurrentTestUid=KMultiRofsRepositoryUid2;
	DoOOMNoServReposL(&CreateDeleteL, _L("Create Delete OOM Multi ROFS Test"),ETrue);
	DoAdditionalCheckingL();
	KCurrentTestUid=KMultiRofsRepositoryUid3;
	DoOOMNoServReposL(&CreateDeleteL, _L("Create Delete OOM Multi ROFS Test"),ETrue);
	DoAdditionalCheckingL();
	KCurrentTestUid=KMultiRofsRepositoryUid4;
	DoOOMNoServReposL(&CreateDeleteL, _L("Create Delete OOM Multi ROFS Test"),ETrue);
	DoAdditionalCheckingL();
	}
#endif

LOCAL_C void DoOOMTestsL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-LEGACY-T_OOMCENREP-0001 Starting CENREPSRV OOM Test "));
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	//Clear any files in the persist directory
	CleanupCDriveL();

	//First Testuid=KTestRepositoryUid
	KCurrentTestUid=KTestRepositoryUid;

	DoOOMTestL(&CenrepSrvOOMTest::GetL,_L("Get Basic Test"),EFalse);
	DoOOMTestL(&CenrepSrvOOMTest::FindL,_L("FindL Basic Test"),EFalse);
	DoOOMTestL(&CenrepSrvOOMTest::NotifyL,_L("NotifyL Basic Test"),EFalse);
	DoOOMTestL(&CenrepSrvOOMTest::ResetL,_L("ResetL Basic Test"),EFalse);
	// Save file without timestamp
	User::LeaveIfError(fm->Copy(KPersistsFile, KPersistsFileNoUpgrade));
	DoOOMTestL(&CenrepSrvOOMTest::SetL,_L("SetL Basic Test"),EFalse);
	DoOOMTestL(&CenrepSrvOOMTest::CreateL,_L("CreateL Basic Test"),EFalse);
	DoOOMTestL(&CenrepSrvOOMTest::DeleteL,_L("DeleteL Basic Test"),EFalse);
	DoOOMTestL(&CenrepSrvOOMTest::MoveL,_L("MoveL Basic Test"),EFalse);

	//Clear any files in the persist directory
	CleanupCDriveL();

	// Simulate response to SWI rom-upgrade and downgrade events
	DoOOMSwiTestL(&CenrepSwiOOMTest::UpgradeROMRev1L,_L("SwiUpgradeROMRev1L Basic Test"),EFalse);
	// Save file with timestamp
	User::LeaveIfError(fm->Copy(KPersistsFile, KPersistsFileUpgraded));
	DoOOMSwiTestL(&CenrepSwiOOMTest::UpgradeROMRev2L,_L("SwiUpgradeROMRev2L Basic Test"),EFalse);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UninstallROMUpgradeL,_L("SwiUninstallROMUpgradeL Basic Test"),EFalse);

	// Simulate response to SWI new rep install/uninstall event events
	DoOOMSwiTestL(&CenrepSwiOOMTest::InstallL,_L("SwiInstallL Basic Test"),EFalse);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UpgradeInstallL,_L("SwiUpgradeInstallL Basic Test"),EFalse);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UninstallL,_L("SwiUninstallL Basic Test"),EFalse);

	// Simulate SWI events before server startup
	DoOOMNoServReposL(&StartupUpgradeL, _L("Startup Upgrade Basic Test"), EFalse);
	DoOOMNoServReposL(&StartupDowngradeL, _L("Startup Downgrade Basic Test"), EFalse);
	DoOOMNoServReposL(&StartupUninstallL,_L("Startup Uninstall Basic Test"), EFalse);

	//OOM Test aOOMMode=ETrue
	DoOOMNoServReposL(&CreateDeleteL, _L("Create Delete OOM Test"),ETrue);
	DoOOMNoServReposL(&CreateDeleteCorruptL, _L("Create Delete Corrupt OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::GetL,_L("Get OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::FindL,_L("FindL OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::NotifyL,_L("NotifyL OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::ResetL,_L("ResetL OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::SetL,_L("SetL OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::CreateL,_L("CreateL OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::DeleteL,_L("DeleteL OOM Test"),ETrue);
	DoOOMTestL(&CenrepSrvOOMTest::MoveL,_L("MoveL OOM Test"),ETrue);

	//Clear any files in the persist directory
	CleanupCDriveL();

	DoOOMSwiTestL(&CenrepSwiOOMTest::UpgradeROMRev1L,_L("SwiUpgradeROMRev1L OOM Test"),ETrue);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UpgradeROMRev2L,_L("SwiUpgradeROMRev2L OOM Test"),ETrue);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UninstallROMUpgradeL,_L("SwiUninstallROMUpgradeL OOM Test"),ETrue);

	DoOOMSwiTestL(&CenrepSwiOOMTest::InstallL,_L("SwiInstallL OOM Test"),ETrue);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UpgradeInstallL,_L("SwiUpgradeInstallL OOM Test"),ETrue);
	DoOOMSwiTestL(&CenrepSwiOOMTest::UninstallL,_L("SwiUninstallL OOM Test"),ETrue);

	DoOOMNoServReposL(&StartupUpgradeL, _L("Startup Upgrade OOM Test"), ETrue);
	DoOOMNoServReposL(&StartupDowngradeL, _L("Startup Downgrade OOM Test"), ETrue);
	DoOOMNoServReposL(&StartupUninstallL, _L("Startup Uninstall OOM Test"), ETrue);

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	DoPersistedVersionCheckingL();
	DoOOMMultiRofsTestL();
#endif

	// Delete files from bur dir
	User::LeaveIfError(fm->Attribs(KPersistsFileNoUpgrade,0,KEntryAttReadOnly,TTime(0)));
	TInt err=fs.Delete(KPersistsFileNoUpgrade);
	if((err!=KErrNone)&&(err!=KErrNotFound))
		User::Leave(err);

	User::LeaveIfError(fm->Attribs(KPersistsFileUpgraded,0,KEntryAttReadOnly,TTime(0)));
	err=fs.Delete(KPersistsFileUpgraded);
	if((err!=KErrNone)&&(err!=KErrNotFound))
		User::Leave(err);


	//Clear any files in the persist directory
	CleanupCDriveL();

	CleanupStack::PopAndDestroy (2);	// fs and fm
	}


LOCAL_C void MainL()
	{
	TheTest.Title ();
	TheTest.Start (_L("OOM Cenrepserv Test"));

	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	DoOOMTestsL();

	//Clear any files in the persist directory
	CleanupCDriveL();

	TheFs.Close ();

	delete scheduler;

	TheTest.End ();
	TheTest.Close ();
	}

TInt E32Main ()
	{

	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;
	__UHEAP_MARKEND;

	return KErrNone;
	}
