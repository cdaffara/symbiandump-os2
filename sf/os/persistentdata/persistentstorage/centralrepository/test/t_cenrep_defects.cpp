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
#include <cenrepnotifyhandler.h>

#include "e32math.h"
#include "srvparams.h"
#include "srvreqs.h"
#include "srvdefs.h"
#include "transstate.h"
#include "obsrvr_noc.h"
#include "shrepos.h"
#include "install.h"

using namespace NCentralRepositoryConstants;

RTest TheTest(_L("Central Repository Defect Tests"));

_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");

const TUid KUidDEF060843LRepository1    = { 0x00000001 };
const TUid KUidRep1						= { 0x00000100 };
const TUid KUidDEF053500LTestRepository = { 0x00000102 };
const TUid KUidDEF054368TestRepository  = { 0x00000103 };
const TUid KUidINC054688TestRepository  = { 0x100058db };
const TUid KUidDEF054632TestRepository  = { 0x10054632 };
const TUid KUidDEF054633TestRepository  = { 0x10054633 };
const TUid KUidLargeRepository          = { 0xCCCCCC01 };
const TUid KUidDEF055661TestRepository  = { 0x10055661 };
const TUid KUidINC056194TestRepository  = { 0x00056194 };
const TUid KUidDEF057778TestRepository  = { 0x00057778 };
const TUid KUidDEF057470TestRepository 	= { 0x10057470 };
const TUid KUidDEF058900TestRepository 	= { 0x10058900 };
const TUid KUidINC069013TestRepository 	= { 0x00000104 };
const TUid KUidPDEF098500LRepository		= { 0x10098500 };

const TUint32 KRangeDeletePartialKey 			= 0x04010000 ;
const TUint32 KDeleteKeyDoesntExist  			= 0x07000000 ;
const TUint32 KDeleteForbidden     				= 0x03010000 ;
const TUint32 KTransactionRangeDeletePartialKey = 0x02010000 ;
const TUint32 KRangeDeleteMask       			= 0x0F0F0000 ;
const TUint32 KNullKey							= 0x00000000 ;

const TUint32 KNotifyBannedKey					= static_cast<TUint32>(KRequestPending) ;
const TUint32 KNotificationKey1					= 0x80000101;
const TUint32 KNotificationKey2					= 0x80000201;
const TUint32 KNotificationKey3					= 0x80000301;
const TUint32 KNotificationKey4					= 0x80000401;

//
// Repository A
//

const TUint32 KNewInt = 1000;
const TInt KIntValue = 1234;


// Definitions for test DEF116629L()
// The structure to construct a TServerSetting object with a TInt value.
struct ServerSettingStruct
	{
	TUint32 key;
	TUint32 meta;
	TInt value;
	};
// The merge types to be tested
const TMergeType KMergerTypes[] = {ETransactionMerge,ERestoreMerge, ESWIUpgradeMerge, ESWIDowngradeMerge, ERomFlash};


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
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

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-4015
@SYMTestCaseDesc When having multiple connections to a particular repository, starting a
transaction and then killing that connection, should not cause a reset.
@SYMTestPriority High
@SYMTestActions
	Essentially - this deals with multiple connections, the lack of cancelling a transaction,
 	either by shutting the sub-session to the CR server directly (or by a crashing application
 	where all open sub-sessions to CR from that application are closed) and the cache manager
 	inside CR.

   Previously, CR's cache manager would reclaim repository memory regardless of the state
   of the repository (in transaction or not).  However, if a repository is closed and its
   no longer an open repository (cache manager reclaimed the memory), it would not remove
   references to the sub-session for this repository in the transaction queue.  Simply put,
   the sub-session's data would be deleted but there would still be references to that
   deleted data.  When you established a new connection (any operation would cause this) you'd
   walk down an invalid pointer.  Most of the time this would cause the server to crash.

   Now, if you're in an active transaction, the cache manager does not reclaim the data.
   Which means that there will always be valid references in the transaction queue.

   ThreadA                                   CR                        CR Cache Manager
   Connection 1.
   CRepository::NewLC(X) --------------->   EInitialize

   Connection 2.
   CRepository::NewLC(X) --------------->   EInitialize

   Connection 2, Start Transaction
   StartTransaction     ---------------->   ETransactionStart

   Before
   ----------------------------------------------------------------------------------
   Wait 10 seconds

                                                 <--------------------------- Evict(X)

   Connection 2. Close
   delete connection2 ------------------>   EClose (would leave invalid references)

   Connection 1.
   GetInt             ------------------>   EGetInt (would attach to repository, derefence
                                                     invalid pointer, CR would crash)

   -----------------------------------------------------------------------------------
   Now

   Wait 10 seconds

                                                                              Reschedule
                                                  (CR cache manager sees that Repository X
                                                   is in a transaction and just reschedules
                                                   it)
   Connection 2. Close
   delete connection2 --------------------> EClose (memory is now available so it cancels
                                                    the active transaction)

   GetInt             --------------------> EGetInt (attaches to the open repository, all is
                                                     fine)
   delete connection2 --------------------> EClose (repository X has no more observers so
                                                    it is scheduled to have its memory reclaimed
                                                    by CR cache manager)

@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF111734
*/

LOCAL_C void DEF111734L()
	{
	/* first establish a connection. this is important because if there are no observers
	 * on a repository, the repository's shared data is deleted directly */
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidDEF054368TestRepository));

	/* second establish a second connection.  this will be used to start a transaction */
	CRepository* rep2;
	User::LeaveIfNull(rep2 = CRepository::NewLC(KUidDEF054368TestRepository));

	/* establish a transaction, this manipulates the transaction queue for this repository */
	rep2->StartTransaction(CRepository::EConcurrentReadWriteTransaction);

	/* now wait.  this should be plenty of time to allow cache manager in CR to reclaim
	 * the data for this repository */
	User::After(5050000 + 9500000);

	TInt i = 0;
	const TInt KInt1_InitialValue = 100;
	const TUint32 KInt1 = 0x1;

	/* destory the connection - this will cause a "close" operation in CR */
	CleanupStack::PopAndDestroy(rep2);

	/* attempt to reconnect to the repository. */
	TInt r = rep->Get(KInt1, i);

	/* if the fix is not in the build, then CR will crash.
	 * Errcode will be KErrServerTerminated */
	TEST2(r, KErrNone);

	/* just for completion's sake, check the return value */
	TEST(i == KInt1_InitialValue);

	// Close repository

	CleanupStack::PopAndDestroy(rep);
	}


LOCAL_C void DEF053500L()
	{
	CRepository* rep;
	// Open repository with hex key value setting
	User::LeaveIfNull(rep = CRepository::NewLC(KUidDEF053500LTestRepository));
	// Close repository
	CleanupStack::PopAndDestroy(rep);
	}

LOCAL_C void DEF054368L()
	{
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidDEF054368TestRepository));

	TInt i = 0;
	const TInt KInt1_InitialValue = 100;
	const TInt KInt2_InitialValue = 200;
	const TInt KInt3_InitialValue = -100;
	const TInt KInt4_InitialValue = -200;
	const TInt KInt5_InitialValue = 0xc8;

	const TUint32 KInt1 = 0x1;
	const TUint32 KInt2 = 0x2;
	const TUint32 KInt3 = 0x3;
	const TUint32 KInt4 = 0x4;
    const TUint32 KInt5 = 0x5;

	TInt r = rep->Get(KInt1, i);
	TEST2(r, KErrNone);
	TEST(i==KInt1_InitialValue);

	r = rep->Get(KInt2, i);
	TEST2(r, KErrNone);
	TEST(i==KInt2_InitialValue);

	r = rep->Get(KInt3, i);
	TEST2(r, KErrNone);
	TEST(i==KInt3_InitialValue);

	r = rep->Get(KInt4, i);
	TEST2(r, KErrNone);
	TEST(i==KInt4_InitialValue);

	r = rep->Get(KInt5, i);
	TEST2(r, KErrNone);
	TEST(i==KInt5_InitialValue);

	// Close repository
	CleanupStack::PopAndDestroy(rep);
	}

LOCAL_C void DEF055680L()
	{
	CRepository* rep;
	// Open repository
	User::LeaveIfNull(rep = CRepository::NewLC(KUidLargeRepository));

	// Find setting whose value the test doesn't have permission to look at
	RArray<TUint32> foundIds;
	TInt r = KErrNone;
	r = rep->FindL(0x810100, 0xFFFFFFFF, foundIds);
	TEST2(r, KErrNone);
	TEST2(foundIds.Count(), 1);

	TInt val;
	r = rep->Get(foundIds[0], val);
	TEST2(r, KErrPermissionDenied);
	foundIds.Reset();

	r = rep->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);

	//Reset repository to remove any changes made during this test
	r = rep->Reset();
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(rep);

	}

LOCAL_C void DEF054632L()
	{
	CRepository* rep;

	//----------Testing for correct default policy behaviour---------------
	/*---------------------10054632.txt------------------------------------
	[platsec]
	cap_rd=ReadDeviceData cap_wr=WriteDeviceData
	0x3 0x4 cap_wr=NetworkServices
	[Main]
	0x1 int 1 0 cap_rd=CommDD
	0x2 int 2 1 cap_wr=CommDD
	0x3 int 3 0
	0x4 int 4 0
	0x5 int 5 0 cap_rd=CommDD
	---------------------------------------------------------------------*/
	User::LeaveIfNull(rep = CRepository::NewLC(KUidDEF054632TestRepository));

	//testing single policies
	TInt err=KErrNone;
	const TInt key_2=2;
	const TInt key_1=1;
	TInt value=0;

	err=rep->Get(key_2,value);
	TEST2(err,KErrNone);

	err=rep->Set(key_1,value);
	TEST2(err,KErrNone);

	//testing range policies
	const TInt key_3=3;
	err=rep->Get(key_3,value);
	TEST2(err,KErrNone);

	CleanupStack::PopAndDestroy(rep);

	//------Testing for correct policy behaviour with no default policies-----
	/*--------------------------10054633.txt----------------------------------
	[platsec]
	0x3 0x4 cap_rd=CommDD
	[Main]
	0x1 int 1 0 cap_rd=CommDD
	0x2 int 2 1 cap_wr=CommDD
	0x3 int 3 0
	0x4 int 4 0
	------------------------------------------------------------------------*/
	User::LeaveIfNull(rep=CRepository::NewLC(KUidDEF054633TestRepository));

	//testing single policies
	err=rep->Get(key_1,value);
	TEST2(err,KErrPermissionDenied);
	err=rep->Set(key_1,value);
	TEST2(err,KErrPermissionDenied);

	err=rep->Set(key_2,value);
	TEST2(err,KErrPermissionDenied);
	err=rep->Get(key_2,value);
	TEST2(err,KErrPermissionDenied);

	//testing range policies
	err=rep->Get(key_3,value);
	TEST2(err,KErrPermissionDenied);
	const TInt key_4=4;
	err=rep->Set(key_4,value);
	TEST2(err,KErrPermissionDenied);
	err=rep->Get(key_4,value);
	TEST2(err,KErrPermissionDenied);

	CleanupStack::PopAndDestroy(rep);
	}

/**
This test will be checking the robustness of the modified array structure
for the single policies, range policies.
*/
LOCAL_C void INC054688L()
	{
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidINC054688TestRepository));

	//--------------CHECKING THE SINGLE POLICIES ARRAY----------------------
	//The ini file has been structured so that the key will be equal to the
	//key value so we can test it using a loop.
	//and also to check some policy checking, we set all the even entries to
	//be non accessible due to insufficient capabilities
	//The key tested for the single policies are from 1 to 24
	const TInt KMaxKeyEntry=24;
	TInt err=KErrNone;
	TInt key_value=0;
	for (TInt i=1;i<=KMaxKeyEntry;i++)
		{
		err=rep->Get(i,key_value);
		//even entries alway not accessible due to insufficient caps
		if (i%2==0)
			TEST2(err,KErrPermissionDenied);
		else
			{
			TEST2(err,KErrNone);
			TEST2(i,key_value);
			}
		}

	//--------------CHECKING THE RANGE POLICIES ARRAY----------------------
	//The range policies in the [platsec] section are arranged in key pairs
	//e.g 25-26,27-28.For testing purpose, the odd pairs(e.g 25-26 is the
	//first pair) will require capabilities that the test lacks off hence
	//causing KErrPermissionDenied when trying to read those range.
	//The key tested for the arary policies are from 25-38 and the value
	//matches the key

	const TInt startRangeKey=25;
	const TInt KMaxRangeKey=38;
	TInt numberOfPairs=(KMaxRangeKey-startRangeKey+1)/2;
	TInt key_value1=0;
	TInt err1=KErrNone;
	for (TInt j=0;j<numberOfPairs;j++)
		{
		//Get first key of the pair
		err=rep->Get(startRangeKey+(2*j),key_value);
		//Get second key of the pair
		err1=rep->Get(startRangeKey+(2*j)+1,key_value1);
		//start from j=0(first pair)
		if ((j+1)%2==0)
			{
			TEST2(err,KErrNone);
			TEST2(err1,KErrNone);
			TEST2(startRangeKey+(2*j),key_value);
			TEST2(startRangeKey+(2*j)+1,key_value1);
			}
		//only odd pairs are non-accessible due to insufficient caps
		else
			{
			TEST2(err,KErrPermissionDenied);
			TEST2(err1,KErrPermissionDenied);
			}
		}

	//Before closing the repository, modify one of the key to see whether
	//the policy array behaves similarly after a rewrite of the ini file
	const TInt keyToChange=3;
	const TInt modifiedValue=3;
	err=rep->Set(keyToChange,modifiedValue);
	TEST2(err,KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(rep);
	User::After(KGeneralDelay);

	//Reopen the repository again after that minor update which will cause
	//all the entry in the ini file to be reversed
	User::LeaveIfNull(rep = CRepository::NewLC(KUidINC054688TestRepository));

	//---------RECHECKING THE POLICY ARRAY AFTER UPDATE--------------------
	for (TInt i=1;i<=KMaxKeyEntry;i++)
		{
		err=rep->Get(i,key_value);
		//even entries alway not accessible due to insufficient caps
		if (i%2==0)
			TEST2(err,KErrPermissionDenied);
		else
			{
			TEST2(err,KErrNone);
			TEST2(i,key_value);
			}
		}


	//---------RECHECKING THE RANGE POLICY ARRAY AFTER UPDATE-------------
	for (TInt j=0;j<numberOfPairs;j++)
		{
		//Get first key of the pair
		err=rep->Get(startRangeKey+(2*j),key_value);
		//Get second key of the pair
		err1=rep->Get(startRangeKey+(2*j)+1,key_value1);
		//start from j=0(first pair)
		if ((j+1)%2==0)
			{
			TEST2(err,KErrNone);
			TEST2(err1,KErrNone);
			TEST2(startRangeKey+(2*j),key_value);
			TEST2(startRangeKey+(2*j)+1,key_value1);
			}
		//only odd pairs are non-accessible due to insufficient caps
		else
			{
			TEST2(err,KErrPermissionDenied);
			TEST2(err1,KErrPermissionDenied);
			}
		}

	CleanupStack::PopAndDestroy(rep);
	}

LOCAL_C void DEF055661L()
	{
	/*--------------------10055661.txt----------------------------------------
	[platsec]
	cap_rd=ReadDeviceData cap_wr=AlwaysFail
	0x3 0x4 cap_wr=NetworkServices
	[Main]
	0x1 int 1 0 cap_rd=AlwaysPass
	0x2 int 2 1 cap_rd=AlwaysFail
	0x3 int 3 0
	0x4 int 4 0 cap_rd=AlwaysFail
	0x5 int 5 0 cap_wr=AlwaysPass
	-------------------------------------------------------------------------*/
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidDEF055661TestRepository));

	TInt err=KErrNone;
	TInt keyValue=0;

	//Key 1
	err=rep->Get(1,keyValue);
	TEST2(err,KErrNone);
	TEST2(keyValue,1);
	err=rep->Set(1,keyValue);
	TEST2(err,KErrPermissionDenied);

	//Key 2
	err=rep->Get(2,keyValue);
	TEST2(err,KErrPermissionDenied);
	err=rep->Set(2,keyValue);
	TEST2(err,KErrPermissionDenied);

	//Key 3
	err=rep->Get(3,keyValue);
	TEST2(err,KErrNone);
	TEST2(keyValue,3);
	err=rep->Set(3,keyValue);
	TEST2(err,KErrPermissionDenied);

	//Key 4
	err=rep->Get(4,keyValue);
	TEST2(err,KErrPermissionDenied);
	err=rep->Set(4,keyValue);
	TEST2(err,KErrPermissionDenied);

	//Key 5
	err=rep->Get(5,keyValue);
	TEST2(err,KErrNone);
	err=rep->Set(5,keyValue);
	TEST2(err,KErrNone);

	CleanupStack::PopAndDestroy(rep);
	}

LOCAL_C void DEF055267L()
	{
	CRepository* rep;
	// Open repository
	User::LeaveIfNull(rep = CRepository::NewLC(KUidLargeRepository));

	TInt r;
	TUint32 errorId;
	// The following loop creates enough settings to blow the server heap
	// when it's 1 M. If it passes it proves that the server heap is > 1M
	// Increasing the server heap allows the messaging test to pass
	_LIT(KString, "Long test string !!!!!!!!!!!");
	const TInt KNumTransactions=25;
	const TInt KNumSettingsInTrans=100;
	for(TInt numTransactions=0; numTransactions<KNumTransactions;numTransactions++)
		{
		r = rep->StartTransaction(CRepository::EReadWriteTransaction);
		TEST2(r, KErrNone);
		for(TInt numSettingsInTrans=0; numSettingsInTrans<KNumSettingsInTrans; numSettingsInTrans++)
			{
			r = rep->Create((numTransactions*KNumSettingsInTrans)+numSettingsInTrans, KString);
			TEST2(r, KErrNone);
			}
		r = rep->CommitTransaction(errorId);
		TEST2(r, KErrNone);
		}

	//Reset repository to remove any changes made during this test
	r = rep->Reset();
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(rep);
	}

LOCAL_C void INC056194L()
	{
	CRepository* repository;
	// Open repository
	User::LeaveIfNull(repository = CRepository::NewLC(KUidINC056194TestRepository));

	const TUint32 KInt1 = 1;
	TRequestStatus intStatus;

	TInt r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(KInt1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	r = repository->NotifyRequest(KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancelAll();
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	r = repository->NotifyRequest(0, KInt1, intStatus);
	TEST2(r, KErrNone);
	r = repository->NotifyCancel(0, KInt1);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	RThread thisThread;
	TEST(thisThread.RequestCount()==0);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void DEF057145L()
	{
	TInt err=KErrNone;
	const TUid testUid={0x10057145};
	RArray<TUint32> idArray;
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(testUid));

	//Start Transaction
	err=repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(err,KErrNone);

	//Before creating new entries get the number of persistent settings
	err=repository->FindL(0,0xFFFFF000,idArray);
	TEST2(err,KErrNone);
	TInt persistentSettingsCount=idArray.Count();
	idArray.Close();

	//Try creating new entries during transaction between the two extreme points
	//ROM persistent file has low key of 1 and high key of 201
	//We try creating new entries in between during transaction
	for (TInt i=100;i<150;i++)
		{
		err=repository->Create(i,i);
		TEST2(err,KErrNone);
		}

	//Now check the merge with the persistent settings is fine by checking
	//the array count after the merge.
	err=repository->FindL(0x000,0xFFFFF000,idArray);
	TEST2(err,KErrNone);
	TEST(idArray.Count()==(persistentSettingsCount+50));
	idArray.Close();

	TUint32 errId;
	err=repository->CommitTransaction(errId);
	TEST2(err,KErrNone);

	idArray.Close();
	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void DEF057778L()
	{
	TInt r;
	CRepository* repository;
	// Open repository
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF057778TestRepository));

	const TUint32 KInt14=14;
	const TInt KInt14NewValue=0;

	r=repository->Set(KInt14,KInt14NewValue);
	TEST2(r, KErrNone);

	// Reset setting that has a single policy
	r=repository->Reset(KInt14);
	TEST2(r, KErrNone);

	// Attempt to access and write setting
	r=repository->Set(KInt14,KInt14NewValue);
	TEST2(r, KErrNone);

	// Reset entire repository
	r=repository->Reset();
	TEST2(r, KErrNone);

	// Attempt to access and write setting
	r=repository->Set(KInt14,KInt14NewValue);
	TEST2(r, KErrNone);

	// Reset entire repository
	r=repository->Reset();
	TEST2(r, KErrNone);

	// Close repository
	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void DEF057999L()
 	{

 	TInt err=KErrNone;
 	TInt retValue=0;
 	const TUid testUid={0x10057145};
 	RArray<TUint32> idArray;

  	CRepository* repository;
 	User::LeaveIfNull(repository = CRepository::NewLC(testUid));

  	//--------------Start Transaction------------------------------------
 	err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

 	//--------------Create follow by delete-----------------------------
  	err=repository->Create(3400,3400);
 	TEST2(err,KErrNone);
 	err=repository->Delete(3400);
 	TEST2(err,KErrNone);
 	err=repository->Delete(3400);
 	TEST2(err,KErrNotFound);

	//--------------Deleting existing romkey-----------------------------
  	err=repository->Delete(200);
  	TEST2(err,KErrNone);

   	err=repository->FindL(200,0xFFFFFFFF,idArray);
  	TEST2(err,KErrNotFound);

 	//--------------Commit Transaction------------------------------------
   	TUint32 errId;
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);

  	idArray.Close();
   	CleanupStack::PopAndDestroy(repository);

	//--------------Reopen and check the deleted keys are gone-------------
   	User::LeaveIfNull(repository = CRepository::NewLC(testUid));

	//Check for the key created and deleted during transaction
	err=repository->FindL(3400,0xFFFFFFFF,idArray);
	TEST2(err,KErrNotFound);

	//Check for the rom key deleted during transaction
   	err=repository->Get(200,retValue);
	TEST2(err,KErrNotFound);

	//--------------Start Transaction------------------------------------
 	err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

 	//--------------Delete follow by create-----------------------------
  	err=repository->Delete(201);
 	TEST2(err,KErrNone);
 	err=repository->Create(201, 201);
 	TEST2(err,KErrNone);

 	//--------------Commit Transaction------------------------------------
   	err=repository->CommitTransaction(errId);
   	TEST2(err,KErrNone);

	err=repository->Get(201,retValue);
	TEST2(err,KErrNone);
	TEST2(retValue, 201);

 	CleanupStack::PopAndDestroy(repository);
   	}

LOCAL_C void DEF057522L()
	{
	TInt r=KErrNone;
	CRepository* repository;
	TUid testUidRepository={0x10057522};

	//Opening a repository with Max number of capabilities supported should not return KErrCorrupt
	User::LeaveIfNull(repository = CRepository::NewLC(testUidRepository));

	//Test reading some of the settings with the max capabilities defined
	TInt retVal=0;
	r=repository->Get(17,retVal);
	TEST2(r,KErrNone);
	r=repository->Set(17,retVal);
	TEST2(r,KErrNone);

	r=repository->Get(18,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(18,retVal);
	TEST2(r,KErrNone);

	//Test that AlwaysPass and AlwaysFail behave as expected when not followed by sid
	r=repository->Get(21,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(21,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Get(22,retVal);
	TEST2(r,KErrNone);
	r=repository->Set(22,retVal);
	TEST2(r,KErrNone);


	//Test that AlwaysPass and AlwaysFail behave as expected when followed by sid
	r=repository->Get(19,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(19,retVal);
	TEST2(r,KErrNone);

	//Additional test cases
	//Test single policies with max 3 caps for both read and write
	r=repository->Get(28,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(28,retVal);
	TEST2(r,KErrNone);

	//Test single policies with max 7 caps for both read and write
	r=repository->Get(33,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(33,retVal);
	TEST2(r,KErrPermissionDenied);

	//Test range policies with max 3 caps when followed by sid
	r=repository->Get(201,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(201,retVal);
	TEST2(r,KErrNone);

	//Test range policies with max 7 caps not with sid
	r=repository->Get(300,retVal);
	TEST2(r,KErrPermissionDenied);
	r=repository->Set(300,retVal);
	TEST2(r,KErrPermissionDenied);

	//Test behaviour when single policies are specified with just sid
	r=repository->Get(400,retVal);
	TEST2(r,KErrNone);
	r=repository->Set(400,retVal);
	TEST2(r,KErrPermissionDenied);		// default read cap for sid 0 is AlwaysFail

	r=repository->Get(401,retVal);
	TEST2(r,KErrNone);					// default write cap for sid 0 is AlwaysPass
	r=repository->Set(401,retVal);
	TEST2(r,KErrNone);

	//Test that default policies are picked up when no policies are specified
	r=repository->Get(500,retVal);
	TEST2(r,KErrNone);

	r=repository->Set(500,retVal);
	TEST2(r,KErrPermissionDenied);

	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void DEF057470L()
	{

	TInt r=KErrNone;
	TUint32 errId=0;
	CRepository* repository;

	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF057470TestRepository));

	//-------------- 1. Simple Range Delete Operation-----------------------------
	// Confirm that keys exist
	RArray<TUint32> foundIds;
	r = repository->FindL(KRangeDeletePartialKey, KRangeDeleteMask, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==5);
	foundIds.Reset();
	// Delete a set of keys
	r = repository->Delete(KRangeDeletePartialKey, KRangeDeleteMask, errId) ;
	TEST2(r, KErrNone);
	// Confirm that deleted keys do not exist
	r = repository->FindL(KRangeDeletePartialKey, KRangeDeleteMask, foundIds);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();
	CleanupStack::PopAndDestroy(repository);
	// Try to find keys again just to double check whether the changes have persisted
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF057470TestRepository));
	r = repository->FindL(KRangeDeletePartialKey, KRangeDeleteMask, foundIds);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();


	//--------------- 2. Range Delete Operation where keys don't exist--------------
	// Confirm that keys don't exist.
	r = repository->FindL(KDeleteKeyDoesntExist, KRangeDeleteMask, foundIds);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();
	// Attempt delete and check result
	r = repository->Delete(KDeleteKeyDoesntExist, KRangeDeleteMask, errId) ;
	TEST2(r, KErrNotFound);
	errId &= KRangeDeleteMask ;
	TEST2(errId, KDeleteKeyDoesntExist) ;


	//-------3. Range Delete Operation where client doesn't have write capabilities for key range.----
	// Attempt delete and check result
	r = repository->Delete(KDeleteForbidden, KRangeDeleteMask, errId) ;
	TEST2(r, KErrPermissionDenied);
	errId &= KRangeDeleteMask ;
	TEST2(errId, KDeleteForbidden) ;
	// Close repository
	CleanupStack::PopAndDestroy(repository);

	//------ 4. Range Delete Operation which deletes all the keys and Resets the entire repository back its original state.
	TInt numberOfIds;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidLargeRepository));
	// Find how many keys are originally present
	r = repository->FindL(KNullKey, KNullKey, foundIds);
	TEST2(r, KErrNone);
	numberOfIds = foundIds.Count();
	TEST(foundIds.Count()!=0);
	foundIds.Reset();
	// Range delete all the keys in the repository
	r = repository->Delete(KNullKey, KNullKey, errId);
	TEST2(r, KErrNone);
	// Check all the keys have been deleted from cache
	r = repository->FindL(KNullKey, KNullKey, foundIds);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();
	// Close and re-open the repository to check if the deleted keys have persisted.
	CleanupStack::PopAndDestroy(repository);	
	// Existance of caching functionality invalidates some tests and
	// makes them fail, so kill the server to force clearing the cache
	r = KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);
	User::LeaveIfNull(repository = CRepository::NewLC(KUidLargeRepository));
	// Check all the deleted keys have persisted.
	r = repository->FindL(KNullKey, KNullKey, foundIds);
	TEST2(r, KErrNotFound);
	TEST(foundIds.Count()==0);
	foundIds.Reset();
	// Reset repository back to its original state from ROM
	r = repository->Reset();
	TEST2(r, KErrNone);
	// Check that number of keys is same as when we started.
	r = repository->FindL(KNullKey, KNullKey, foundIds);
	TEST2(r, KErrNone);
	TEST(foundIds.Count()==numberOfIds);
	foundIds.Reset();
	// Close the repository
	CleanupStack::PopAndDestroy(repository);

	//**********************Delete Range within Transacrions**********************
	//--------- 5. Range Delete within a transaction where key's don't exist.------------
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF057470TestRepository));
	// Begin read write transaction.
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction);
	r = repository->Delete(KDeleteKeyDoesntExist, KRangeDeleteMask, errId) ;
	TEST2(r, KErrNotFound);
	// Check that transaction hasn't failed.
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction);
	// Fail transaction.
	repository->FailTransaction();
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction | EFailedBit);
	repository->RollbackTransaction();
	r = TransactionState(repository);
	TEST2(r, ENoTransaction);


	//---------- 6. Range delete within a transaction where client doesn't have capabilities to write to key range.
	// Begin read write transaction.
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);
	r = repository->Delete(KDeleteForbidden, KRangeDeleteMask, errId) ;
	TEST2(r, KErrPermissionDenied);
	errId &= KRangeDeleteMask ;
	TEST2(errId, KDeleteForbidden) ;
	// check that transaction has failed.
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction | EFailedBit);
	// Rollback the failed transaction.
	repository->RollbackTransaction();
	r = TransactionState(repository);
	TEST2(r, ENoTransaction);


	//---------- 7. Range delete while not in transaction fails other session's transaction with KErrLocked.
	// Open another repository
	CRepository* repository2;
	User::LeaveIfNull(repository2 = CRepository::NewLC(KUidDEF057470TestRepository));
	// Begin read write transaction
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);
	r = TransactionState(repository);
	TEST2(r, EReadWriteTransaction);
	// Delete keys in the first session.
	r = repository->Delete(KTransactionRangeDeletePartialKey, KRangeDeleteMask, errId) ;
	TEST2(r, KErrNone);
	// Delete keys in the second session.
	r = repository2->Delete(KTransactionRangeDeletePartialKey, KRangeDeleteMask, errId) ;
	TEST2(r, KErrNone);
	// first session's transaction should have failed with KErrLocked
	TInt state = TransactionState(repository);
	TEST2(state, EReadWriteTransaction | EFailedBit);
	// Commit transaction should report failure reason:
	TUint32 keyInfo;
	r = repository->CommitTransaction(keyInfo);
	TEST2(r, KErrLocked);
	TEST(keyInfo == KUnspecifiedKey);
	r = TransactionState(repository);
	TEST2(r, ENoTransaction);
	// Close the repositories
	CleanupStack::PopAndDestroy(repository2);


	//----------- 8. Range Delete within a Transaction and Commit.-----------------------
	//Before doing this we need to reset from the ROM first to restore the deleted keys in
	//non transaction test earlier
	r=repository->Reset();
	TEST2(r,KErrNone);
	r = repository->FindL(KRangeDeletePartialKey, KRangeDeleteMask, foundIds);
	TEST2(r,KErrNone);
	TEST(foundIds.Count()==5);
	foundIds.Reset();
	// Begin read write transaction
	r = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(r, KErrNone);
	r = repository->Delete(KTransactionRangeDeletePartialKey, KRangeDeleteMask, errId) ;
	TEST2(r, KErrNone);
	// Commit transaction
	r = repository->CommitTransaction(errId);
	TEST2(r, KErrNone);
	//The number of keys changed should be equal to the number of keys deleted
	TEST(errId==5);

	//Reset repository to remove any changes made during this test
	r=repository->Reset();
	TEST2(r, KErrNone);

	// Close the repositories
	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void INC058229L()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	// Open repository to ensure server is running
	CRepository* rep;
	User::LeaveIfNull(rep = CRepository::NewLC(KUidLargeRepository));
	// Close repository
	CleanupStack::PopAndDestroy(rep);

	// Remove install dir
	TInt r = fm->RmDir(KCInstallDir);
	if(r!=KErrNone && r!=KErrNotFound && r!=KErrPathNotFound)
		User::Leave(r);
	// Wait so that watcher will see directory is gone
	User::After(KGeneralDelay);

	// Kill server so that next test will reconnect and recreate resources
	r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	// Open repository to ensure directories are restored
	User::LeaveIfNull(rep = CRepository::NewLC(KUidLargeRepository));
	// Close repository
	CleanupStack::PopAndDestroy(3);	// rep, fs, fm
	}

LOCAL_C void DEF058900L()
	{

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF058900TestRepository));
	TRequestStatus intStatus;
	RThread thisThread;

	//
	// Basic notification to test notification for one banned key
	//
	TInt r = repository->NotifyRequest(KNotifyBannedKey, intStatus);
	TEST2(r, KErrNone);
	TEST(intStatus == KRequestPending);

	// First change to setting should cause notification
	r = repository->Set(KNotifyBannedKey, 100);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);

	//
	// Group notification to test notification when the banned key is in a range
	//
	r = repository->NotifyRequest(KNullKey, KNullKey, intStatus);
	TEST2(r, KErrNone);

	r = repository->Set(KNotifyBannedKey, 50);
	TEST2(r, KErrNone);
	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
	TEST(thisThread.RequestCount()==0);

	//
	// Group notification to test notification when many keys including the banned key change
	// i.e. a repository wide reset
	//
	// First change values for alot of the keys before a reset.
	r = repository->Set(KNotificationKey1, 80);
	TEST2(r, KErrNone);
	r = repository->Set(KNotifyBannedKey, 50);
	TEST2(r, KErrNone);
	r = repository->Set(KNotificationKey2, 70);
	TEST2(r, KErrNone);
	r = repository->Set(KNotificationKey3, 60);
	TEST2(r, KErrNone);
	r = repository->Set(KNotificationKey4, 30);
	TEST2(r, KErrNone);

	r = repository->NotifyRequest(KNullKey, KNullKey, intStatus); // Request notification
																  // for all keys in repository.
	TEST2(r, KErrNone);

	//Reset the whole repository to original settings
	r = repository->Reset();
	TEST2(r, KErrNone);

	User::WaitForAnyRequest();
	TEST(intStatus==KUnspecifiedKey);
	TEST(thisThread.RequestCount()==0);


	CleanupStack::PopAndDestroy(repository);
	}

LOCAL_C void DEF061087L()
	{
	TInt r=KErrNone;
	TUid testUid={0x10061087};
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(testUid));

	/*--------------Scenario 1---------------------
	100 300 cap_wr=ReadDeviceData
	100 200 cap_wr=TCB */
	r=repository->Create(150,100);
	TEST2(r,KErrPermissionDenied);
	r=repository->Create(201,201);
	TEST2(r,KErrNone);

	/*--------------Scenario 2---------------------
	400 500 cap_wr=ReadDeviceData
	400 500 cap_wr=TCB */
	r=repository->Create(500,100);
	TEST2(r,KErrPermissionDenied);

	/*--------------Scenario 3---------------------
	0x600 0x700 cap_wr=ReadDeviceData
	# Odd entry from 600-700
	0x601 mask=0xF01 cap_wr=TCB*/
	r=repository->Create(0x601,601);
	TEST2(r,KErrPermissionDenied);
	r=repository->Create(0x657,647);
	TEST2(r,KErrPermissionDenied);
	//0x602 will be even so it will check agains the next matched range policies
	//which is 0x600 0x700 cap_wr=ReadDeviceData
	r=repository->Create(0x602,600);
	TEST2(r,KErrNone);

	/*--------------Scenario 4---------------------
	# protected data
	0x00000100 mask=0x00000100 cap_wr=NetworkControl
	# private data
	0x000000F0 mask=0x00000080 cap_rd=ReadDeviceData */
	r=repository->Create(0x01000100,123);
	TEST2(r,KErrPermissionDenied);
	r=repository->Create(0x03450700,123);
	TEST2(r,KErrPermissionDenied);
	r=repository->Create(0x00450080,123);
	TEST2(r,KErrNone);
	r=repository->Create(0x06450081,123);
	TEST2(r,KErrNone);


	CleanupStack::PopAndDestroy(repository);
	}


LOCAL_C void DEF060843L()
    {
    CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF060843LRepository1));

    _LIT8(KString12_InitialValue, "string");
    TPtrC8 p8(KString12_InitialValue);
    TInt len8 = p8.Length();

    _LIT(KString11_InitialValue, "string");
    TPtrC p16(KString11_InitialValue);
    TInt len16 = p16.Length();

    //setting ids in a test repository
    const TUint32 KReal1 = 2;
    const TUint32 KString11 = 11;
    const TUint32 KString12 = 12;

    TInt r;
    TInt i(0);

    TBuf<15> str;
	TInt strLen;

	TBuf8<15> str8;
	TInt strLen8;

    //================read real using int ========================
    r = repository->Get(KReal1, i);
	TEST2(r, KErrArgument);
	TEST(i==0);

	//================read TDes16 using TDes8=====================
    r = repository->Get(KString11, str8);
	TEST2(r, KErrNone);

	//8-bit descriptor holding the same data as 16-bit descriptor
	//should twice as long as 16-bit des.
	strLen8 = str8.Length();
	TEST2(strLen8,len16*2);

    const TUint16* p16a  = (TUint16*)str8.Ptr();
    const TUint16* p16b = p16.Ptr();
    //compares content of descriptors byte by byte
    r = Mem::Compare(p16a,strLen8/2,p16b,len16);
    TEST2(r,KErrNone);

    //=================read TDes8 using TDes16=====================
    r = repository->Get(KString12, str);
    TEST2(r, KErrNone);

    //16-bit descriptor holding the same data as 8-bit descriptor
    //should be half a length of a 8-bit desc
    strLen = str.Length();
    TEST2(strLen,len8/2);

	const TUint8* p8a = (TUint8*)str.Ptr();
    const TUint8* p8b = p8.Ptr();
    //compares content of descriptors byte by byte
    r = Mem::Compare(p8a,strLen*2,p8b,len8);
    TEST2(r,KErrNone);

	//================writting to TDes16 using TDes8===============
	r = repository->Set(KString11,KString12_InitialValue);
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF060843LRepository1));
	r = repository->Get(KString11, str8);
	strLen8 = str8.Length();
	TEST2(strLen8,len8);
	TEST(str8==KString12_InitialValue);

	//================writting to TDes8 using TDes16===============
	r = repository->Set(KString12,KString11_InitialValue);
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy(repository);
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF060843LRepository1));
	r = repository->Get(KString12, str);
	strLen = str.Length();
	TEST2(strLen,len16);
	TEST(str==KString11_InitialValue);
    CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1433
@SYMTestCaseDesc Central repository panics the client when trying to read 8-bit descriptor with uneven length
@SYMTestPriority High
@SYMTestActions  Read 8-bit descriptor with uneven length
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF INC069013
*/
LOCAL_C void INC069013L()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidINC069013TestRepository));

	_LIT(KString16, "16BitString");
	_LIT8(KString8Odd, "8BitOddString");
	_LIT8(KString8Even, "8BitEvenString");

	const TUint32 KString16ID = 1;
	const TUint32 KString8OddID = 2;
	const TUint32 KString8EvenID = 3;

	TBuf<20> buf;
	TInt r = KErrNone;
	TInt actualLength = 0;

	//test for string stored as 16 bit
	r = repository->Get(KString16ID, buf);
	TEST2(r, KErrNone);

	r = repository->Get(KString16ID, buf, actualLength);
	TEST2(r, KErrNone);
	TEST2(actualLength, KString16().Length());

	//test for string stored as 8 bit with odd length
	r = repository->Get(KString8OddID, buf);
	TEST2(r, KErrNone);

	r = repository->Get(KString8OddID, buf, actualLength);
	TEST2(r, KErrNone);
	TEST2(actualLength, (KString8Odd().Length() + 1) / 2);

	//test for string stored as 8 bit with even length
	r = repository->Get(KString8EvenID, buf);
	TEST2(r, KErrNone);

	r = repository->Get(KString8EvenID, buf, actualLength);
	TEST2(r, KErrNone);
	TEST2(actualLength, (KString8Even().Length() + 1) / 2);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1434
@SYMTestCaseDesc Central repository doesn't handle corrupt .cre files correctly.
@SYMTestPriority High
@SYMTestActions  Open corrupt files from ROM and persists
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF070731
*/
const TUid KUidCorruptEOF = { 0x00000e0f };
const TUid KUidCorrupt    = { 0x00000bad };

_LIT(KCRep1File,                                "c:\\private\\10202BE9\\persists\\00000100.cre");
_LIT(KZCorruptEOFFile,                          "z:\\private\\10202BE9\\00000e0f.cre");
_LIT(KZCorruptFile,                             "z:\\private\\10202BE9\\00000bad.cre");

LOCAL_C void DEF070731L()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	CRepository* rep;

	// Open ROM cre file with EOF corruption
	TheTest.Printf(_L("\nOpen ROM cre file with EOF corruption\n"));
	TRAPD(errH, rep=CRepository::NewLC(KUidCorruptEOF));
	TEST2(errH, KErrCorrupt);

	// Open corrupt ROM file
	TheTest.Printf(_L("\nOpen corrupt ROM file\n"));
	TRAPD(errS,rep=CRepository::NewLC(KUidCorrupt));
	TEST2(errS, KErrCorrupt);

	// Edit repos with new setting
	TheTest.Printf(_L("\nEdit repos with new setting\n"));
	User::LeaveIfNull(rep = CRepository::NewLC(KUidRep1));
	TInt r = rep->Create(KNewInt, KIntValue);
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy(rep);

	// Reopen repos to read from persists file
	User::LeaveIfNull(rep = CRepository::NewLC(KUidRep1));
	TInt newInt;
	r=rep->Get(KNewInt, newInt);
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy(rep);

	// Existance of caching functionality invalidates some tests and
	// makes them fail, so kill the server to force clearing the cache
	r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	// Copy corrupt EOF file into persists
	TheTest.Printf(_L("\nCopy corrupt EOF file into persists dir\n"));
	User::LeaveIfError(fm->Copy(KZCorruptEOFFile, KCRep1File));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCRep1File,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	// Open repos, should find corrupt file and use ROM file
	User::LeaveIfNull(rep = CRepository::NewLC(KUidRep1));
	// If we can't find KNewInt, this proves that we are using ROM file
	r=rep->Get(KNewInt, newInt);
	TEST2(r, KErrNotFound);
	// Test that persists file is deleted
	TheTest.Printf(_L("\nCheck corrupt EOF file deleted from persists dir\n"));
	TEST2 (BaflUtils::FileExists (fs, KCRep1File), EFalse);
	// Close repository
	CleanupStack::PopAndDestroy(rep);

	// Existance of caching functionality invalidates some tests and
	// makes them fail, so kill the server to force clearing the cache
	r=KillProcess(KCentralRepositoryServerName);
	TEST2(r,KErrNone);

	// Copy corrupt file into persists
	TheTest.Printf(_L("\nCopy corrupt file into persists dir\n"));
	User::LeaveIfError(fm->Copy(KZCorruptFile, KCRep1File));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCRep1File,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	// Open repos, should find corrupt file and use ROM file
	User::LeaveIfNull(rep = CRepository::NewLC(KUidRep1));
	// Test that file is deleted
	TheTest.Printf(_L("\nCheck corrupt file deleted from persists dir\n"));
	TEST2 (BaflUtils::FileExists (fs, KCRep1File), EFalse);

	// Close repository
	CleanupStack::PopAndDestroy(3);// rep, fs,fm

	}

// Helper function for DEF084700L
LOCAL_C void ConnectAndCommitSuicideL(void)
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF060843LRepository1));

	RThread currentThread;
	currentThread.Kill(KErrDied);

	// Should never get executed
	CleanupStack::PopAndDestroy(repository);
	}

// Helper function for DEF084700L
LOCAL_C TInt SuicidalThread(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAP_IGNORE(ConnectAndCommitSuicideL());

	// Should never get executed
	delete cleanup;
	return KErrNone;
	}

// Helper function for DEF084700L
LOCAL_C void ConnectAndDisconnectL(void)
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF060843LRepository1));

	CleanupStack::PopAndDestroy(repository);
	}

// Helper function for DEF084700L
LOCAL_C TInt WellBehavedThread(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAP_IGNORE(ConnectAndDisconnectL());

	delete cleanup;
	return KErrNone;
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1683
@SYMTestCaseDesc UIF automated tests crash H4 board
@SYMTestPriority High
@SYMTestActions  Open a repository, create a thread, open same repository from that thread, kill the thread
				 before closing repository, update a setting from main body (trigger notifications). Do the same
				 thing for a thread which terminates normally for making sure the previous behaviour is maintained
@SYMTestExpectedResults The test must not panic or fail.
@SYMDEF DEF084700
*/
LOCAL_C void DEF084700L()
	{
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidDEF060843LRepository1));

	RThread testThread;
	_LIT(KThreadName1, "SuicidalTestThread");

	testThread.Create(KThreadName1, SuicidalThread, KDefaultStackSize, KMinHeapSize, 0x100000, NULL);

	TRequestStatus requestStatus;
	// Request notification when the thread kills itself
	testThread.Logon(requestStatus);
	// Let the thread execute
	testThread.Resume();

	// Wait for suicide
	User::WaitForRequest(requestStatus);
	// Make sure it's dead
	TEST2(requestStatus.Int(), KErrDied);

	const TUint32 KInt1 = 0x1;
	TInt i, r = KErrNone;

	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, i+1);
	// Fails here with KErrServerTerminated before fix because server crashes
	TEST2(r, KErrNone);

	// Create another thread which accesses the repository but terminates normally
	_LIT(KThreadName2, "WellBehavedTestThread");

	testThread.Create(KThreadName2, WellBehavedThread, KDefaultStackSize, KMinHeapSize, 0x100000, NULL);

	// Request notification when the thread terminates normally
	testThread.Logon(requestStatus);
	// Let the thread execute
	testThread.Resume();

	// Wait for old age
	User::WaitForRequest(requestStatus);
	// Make sure the cause of death is natural
	TEST2(requestStatus.Int(), KErrNone);

	r = repository->Get(KInt1, i);
	TEST2(r, KErrNone);
	r = repository->Set(KInt1, i+1);

	TEST2(r, KErrNone);

	//Reset repository to remove any changes made during this test
	r = repository->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1862
@SYMTestCaseDesc [AQP] CommitTransaction returns incorrect error code if Find~L methods run OOM
@SYMTestPriority High
@SYMTestActions  Open a repository, start a transaction, then simulate a OOM failure in a TRAPPED
				 FindL, this will result in client side error and this error should be the returned
				 error of CommitTransaction.
@SYMTestExpectedResults The test must not panic or fail.
@SYMDEF DEF089945
*/
LOCAL_C void DEF089945L()
	{
 	TInt err=KErrNone;
 	const TUid testUid={0x10057145};
 	RArray<TUint32> idArray;

	CRepository* repository;
 	User::LeaveIfNull(repository = CRepository::NewLC(testUid));

 	//-----------SIMULATE CLIENT SIDE TRANSACTION ERROR------------------------------
 	err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

 	//set the next allocation to fail in the FindL method
	__UHEAP_FAILNEXT(1);
   	TRAP(err,repository->FindL(1,0xFFFFFFFF,idArray));
 	__UHEAP_TOTAL_RESET;
 	//uheap failure only happens in debug build
#ifdef _DEBUG
  	TEST2(err,KErrNoMemory);
#else
	TEST2(err,KErrNone);
#endif

   	TUint32 errId;
   	//ensure that the returned code from Commit is same as the last failure error in client
   	err=repository->CommitTransaction(errId);
#ifdef _DEBUG
   	TEST2(err,KErrNoMemory);
   	TEST2(errId,KUnspecifiedKey);
#else
   	TEST2(err,KErrNone);
#endif
   	idArray.Reset();

   	//----------SIMULATE CLIENT SIDE TRANSACTION ERROR FOLLOWED BY ADDITIONAL OPERATION-----
 	err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);
 	//set the next allocation to fail in the FindL method
	__UHEAP_FAILNEXT(1);
   	TRAP(err,repository->FindL(1,0xFFFFFFFF,idArray));
 	__UHEAP_TOTAL_RESET;
#ifdef _DEBUG
  	TEST2(err,KErrNoMemory);
#else
	TEST2(err,KErrNone);
#endif

  	//now try to execute another operation following the failure
  	TReal realValue;
 	err=repository->Set(1,realValue);
#ifdef _DEBUG
 	TEST2(err,KErrAbort);
#else
	TEST2(err,KErrArgument);
#endif

    err=repository->CommitTransaction(errId);
    //confirms that the error returned is the last failure in client side
#ifdef _DEBUG
   	TEST2(err,KErrNoMemory);
   	TEST2(errId,KUnspecifiedKey);
#else
	TEST2(err,KErrArgument);
#endif
	idArray.Reset();

	//---------SIMULATE SERVER SIDE ERROR FOLLOWED BY CLIENT SIDE ERROR-------------------
 	err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 	TEST2(err,KErrNone);

 	//server error
 	err=repository->Set(1,realValue);
 	TEST2(err,KErrArgument);

 	//client error
 	__UHEAP_FAILNEXT(1);
 	TInt ret=KErrNone;
   	TRAP(err,ret=repository->FindL(1,0xFFFFFFFF,idArray));
 	__UHEAP_TOTAL_RESET;
  	TEST2(ret,KErrAbort);

    err=repository->CommitTransaction(errId);
    //confirms that the error returned is the last failure in server side
   	TEST2(err,KErrArgument);
   	TEST2(errId,1);
	idArray.Reset();

   	//clean up stuff
   	idArray.Close();
   	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID 	 SYSLIB-CENTRALREPOSITORY-CT-3242
@SYMTestCaseDesc Memory card: Backup to memory card causes phone freeze.
@SYMTestPriority High
@SYMTestActions  Open a repository with a corrupt ini file and check that
				 the call to CRepository::NewLC leaves with KErrCorrupt and does not panic.
@SYMTestExpectedResults The call to CRepository::NewLC should return with KErrCorrupt and
				the test must not panic or fail.
@SYMDEF PDEF098500
*/
LOCAL_C void PDEF098500()
	{
	CRepository* repository = NULL;

	TRAPD(err,repository = CRepository::NewLC(KUidPDEF098500LRepository));

	//We expect the above call to leave with KErrCorrupt so repository should be NULL
	TEST(repository == NULL);
	TEST2(err, KErrCorrupt);

	//No PopAndDestroy required as above call is expected to leave

	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1884
@SYMTestCaseDesc DEF093855: CenRep cancels transactions if they exceed 5 seconds...
@SYMTestPriority High
@SYMTestActions  Open a repository, start a transaction, then wait more than default cache timeout
				 to let the normal eviction clean out the cache, call a function to reload the repository,
				 check that the transaction is still valid, rollback the transaction to check the
				 repository unload/reload during an open transaction doesn't cause a panic when transaction is rolled back.
@SYMTestExpectedResults The test must not panic or fail.
@SYMDEF DEF093855
*/
LOCAL_C void DEF093855L()
	{
 	TInt err=KErrNone;

	// make sure no items are in the cache
 	KillProcess(KCentralRepositoryServerName);

	// connect
	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidRep1));

	// Begin concurrent read write transaction
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	TInt val;
	const TInt KIntTestValue = 10;
	// get test value, make sure it is KIntTestValue
    err = repository->Get(0x02010100,val);
	TEST2(err, KErrNone);
	TEST2(val, KIntTestValue);

    // change the value in transaction
	TInt r = repository->Set(0x02010100, KIntValue);
	TEST2(r, KErrNone);

	// wait for the repository to be removed from memory
	CleanupRepositoryCache();

	// check that the transaction is not aborted, still in memory and keeps the value set during the transaction
    err = repository->Get(0x02010100,val);
	TEST2(err, KErrNone);
	TEST2(val, KIntValue);

	// rollback to check if RollbackTransaction works, and to reset the value to KIntTestValue
    repository->RollbackTransaction();

	// Begin another transaction
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	// get test value, make sure it is still KIntTestValue (successfully rolled back)
    err = repository->Get(0x02010100,val);
	TEST2(err, KErrNone);
	TEST2(val, KIntTestValue);

    // change the value in transaction
	r = repository->Set(0x02010100, KIntValue);
	TEST2(r, KErrNone);

	// wait for the repository to be removed from memory
	CleanupRepositoryCache();

	// check that the transaction is not aborted, still in memory and keeps the value set during the transaction
    err = repository->Get(0x02010100,val);
	TEST2(err, KErrNone);
	TEST2(val, KIntValue);

	// commit to check if CommitTransaction works
	TUint32 keyInfo;
    repository->CommitTransaction(keyInfo);
	TEST2(err, KErrNone);
	TEST2(keyInfo, 1);

	// check that the value is set during the transaction (in shared rep settings)
    err = repository->Get(0x02010100,val);
	TEST2(err, KErrNone);
	TEST2(val, KIntValue);

	// close repository
	CleanupStack::PopAndDestroy(repository);

	// make sure the repository is unloaded from cache
	KillProcess(KCentralRepositoryServerName);

	// reopen the repository
	User::LeaveIfNull(repository = CRepository::NewLC(KUidRep1));

	// get test value, to check if it had been successfully persisted
    err = repository->Get(0x02010100,val);
	TEST2(err, KErrNone);
	TEST2(val, KIntValue);

    // reset the setting back to the original value
	r = repository->Set(0x02010100, KIntTestValue);
	TEST2(r, KErrNone);

	//Reset repository to remove any changes made during this test
	r = repository->Reset();
	TEST2(r, KErrNone);

	// close repository
	CleanupStack::PopAndDestroy(repository);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-3477
@SYMTestCaseDesc PDEF106507: Central repository doesn't handle corruption of the installdir.bin file properly
@SYMTestPriority High
@SYMTestActions  Replaces installdir.bin file with a corrupted version, then shuts down the server and restarts it (with CRepository::NewL)  This causes the file to be re-read, and as it is corrupt it should get deleted and re-created.  The test checks that the server does not panic.
@SYMTestExpectedResults The Server should not panic when trying to read the corrupt file, and should return KErrNone
@SYMDEF PDEF106507
*/
LOCAL_C void PDEF106507L()
	{
	_LIT(KInstallDirFile, "C:\\private\\10202be9\\persists\\installdir.bin");
	_LIT(KInstallDirFileBad1, "Z:\\private\\10202be9\\installdir_corrupt1.bin");
	_LIT(KInstallDirFileBad2, "Z:\\private\\10202be9\\installdir_corrupt2.bin");
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	// copy first corrupt file into centrep private directory (this file has data area corrupted)
	TheTest.Printf(_L("\nCopy first corrupt installdir.bin file into persists dir\n"));
	User::LeaveIfError(fm->Copy(KInstallDirFileBad1, KInstallDirFile));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KInstallDirFile,0,KEntryAttReadOnly,TTime(0)));

	// Kill server so that next test will restart and internalize file
	TInt error = KillProcess(KCentralRepositoryServerName);
	TEST2(error, KErrNone);

	// Start server and check it does not panic
	CRepository* repository = NULL;
	TRAP(error, repository = CRepository::NewL(KUidRep1));
	TEST2(error, KErrNone);
	delete repository;

	// test the second corrupted file (this file has header information corrupted)
	TheTest.Printf(_L("\nCopy second corrupt installdir.bin file into persists dir\n"));
	User::LeaveIfError(fm->Copy(KInstallDirFileBad2, KInstallDirFile));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KInstallDirFile,0,KEntryAttReadOnly,TTime(0)));

	// Kill server so that next test will reconnect and recreate resources
	error = KillProcess(KCentralRepositoryServerName);
	TEST2(error, KErrNone);

	// Start server and check it does not panic
	TRAP(error, repository = CRepository::NewL(KUidRep1));
	TEST2(error, KErrNone);
	delete repository;

	CleanupStack::PopAndDestroy(2); //fs, fm
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-3517
@SYMTestCaseDesc R3.2 stability problems: CentralRepositorySrv crash
@SYMTestPriority High
@SYMTestActions  Attempt to open a .cre file which has a corrupt UID. Ensure that
				 the error is dealt with properly.
@SYMTestExpectedResults The test must not fail.
@SYMDEF INC108803
*/

_LIT(KZCorruptFile1, "z:\\private\\10202BE9\\101f8764.cre");
_LIT(KCCorruptFile1, "c:\\private\\10202BE9\\persists\\101f8764.cre");
_LIT(KCCorruptFile2, "c:\\private\\10202BE9\\persists\\101f8765.cre");

const TUid KUidCorruptUid1 	= { 0x101f8764 };
const TUid KUidCorruptUid2 	= { 0x101f8765 };

LOCAL_C void INC108803L()
	{

	TInt err = KErrNone;

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	CRepository* rep = NULL;

	// Attempt to open a repository from a .cre file which has a corrupt UID
	// The .cre file exists on the Z drive only. This test ensures that
	// the corruption of the .cre file is detected and that the correct error
	// code, KErrCorrupt, is returned to the client.
	// This test will
	// 1. Look on c drive, be unable to find the file
	// 2. Look in z drive, find the file with the specified UID,
	// determine that the file is corrupt and return KErrCorupt.

	TheTest.Printf(_L("\nAttempt to open ROM cre file with UID corruption\n"));
	TRAP(err, rep = CRepository::NewLC(KUidCorruptUid1); CleanupStack::Pop(rep););
	TEST2(err, KErrCorrupt);

	// Copy the corrupt .cre file into the persists directory for the next test
 	User::LeaveIfError(fm->Copy(KZCorruptFile1, KCCorruptFile1 ));
 	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCCorruptFile1,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);

	// Attempt to open a repository which has a .cre file with a corrupt UID.
	// The .cre file exists on the C and Z drives. (This test ensures that the
	// corrupt .cre file on the C Drive is deleted).
	// This test will
	// 1.Look in the C Drive, find the file with the specified UID
	// determine that the file is corrupt and delete the file
	// 2. Look in the Z Drive, find the file with the specified UID,
	// determine that the file is corrupt and return KErrCorupt.

 	TheTest.Printf(_L("\nAttempt to open persists cre file with UID corruption\n"));
	TRAP (err, rep = CRepository::NewLC(KUidCorruptUid1); CleanupStack::Pop(rep););
	TEST2(err, KErrCorrupt);
	TheTest.Printf(_L("\nCheck corrupt file deleted from persists dir\n"));
	TEST2 (BaflUtils::FileExists (fs, KCCorruptFile1), EFalse);

 	// Copy a second corrupt .cre file into the persists directory
 	// (Note that an associated ini file
 	// z:\\private\\10202BE9\\101f8765.txt exists and is not corrupt)

 	User::LeaveIfError(fm->Copy(KZCorruptFile1, KCCorruptFile2));
 	TEST2 (BaflUtils::FileExists (fs, KCCorruptFile2), ETrue);
 	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCCorruptFile2,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);

 	// Attempt to open a corrupt repository. This will fail and the .cre file on the
 	// C Drive will be deleted. Fall-back to the Z drive occurs. The associated ini file
 	// is found.
 	// Attempt to use the resulting repository by using a Reset operation. No panics should
 	// occur.
 	TheTest.Printf(_L("\nAttempt to open corrupt persists cre file, re-create from txt file\n"));
	TRAP (err, rep = CRepository::NewLC(KUidCorruptUid2); CleanupStack::Pop(rep););
	TEST2 (BaflUtils::FileExists (fs, KCCorruptFile2), EFalse);
 	TEST2(err, KErrNone);
  	ASSERT(rep != NULL);
	rep->Reset();

 	// Clean up any files created on the C Drive
 	CleanupFileFromCDriveL(KUidCorruptUid1);
 	CleanupFileFromCDriveL(KUidCorruptUid2);

	delete rep;
	CleanupStack::PopAndDestroy(2, &fs); // fm, fs
	}


/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-3545
@SYMTestCaseDesc The corrupted Cache ini file should cause CentralRepositorySrv panic in debug mode
				 but keep running with the default values in release mode.
@SYMTestPriority High
@SYMTestActions  Kill CentralRepositorySrv and restart it with the corrupted Cache ini file.
				 Ensure that:
				 . in debug mode, the client will fail to create a CRepository pointer because
				 the server has panicked.
				 . in release mode, the client will be able to create a CRepository pointer because
				 the server keeps running with the default values.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF109390
*/

_LIT(KZCorruptIniFile, "z:\\private\\10202BE9\\cache_corrupt.ini");
_LIT(KCCorruptIniFile, "c:\\private\\10202BE9\\centrep.ini");
_LIT(KZRepFile, 	 "z:\\private\\10202BE9\\00000001.txt");
_LIT(KCRepFile, 	 "c:\\private\\10202BE9\\persists\\00000001.txt");

const TUid KTestRepUid 	= { 0x00000001};

LOCAL_C void DEF109390L()
	{
 	TInt err(KErrNone);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);

	// Copy the corrupted Cache ini file to install directory.
	TheTest.Printf(_L("\nCopy the corrupted Cache ini file to install directory.\n"));
 	User::LeaveIfError(fm->Copy(KZCorruptIniFile, KCCorruptIniFile));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCCorruptIniFile,0,KEntryAttReadOnly,TTime(0)));

	// Copy a good repository file to persist directory to verify whether the cenrep server is
	// able to start up.
	TheTest.Printf(_L("\nCopy the repository file to persist directory.\n"));
 	User::LeaveIfError(fm->Copy(KZRepFile, KCRepFile));
	// Reset read-only bit
	User::LeaveIfError(fm->Attribs(KCRepFile,0,KEntryAttReadOnly,TTime(0)));

	// Kill server so that next test will restart it.
	err=KillProcess(KCentralRepositoryServerName);
	TEST2(err,KErrNone);

	User::After(KGeneralDelay);

	CRepository* rep = NULL;
	TRAP(err, rep = CRepository::NewLC(KTestRepUid); CleanupStack::PopAndDestroy(rep););

	#ifdef _DEBUG
		// in debug mode, CRepository::NewLC should leave with leave code KErrGeneral.
		TheTest.Printf(_L("Debug mode err = %d.\n"),err);
		TEST2(err, KErrGeneral);
	#else
		// in release mode, CRepository::NewLC should return a valid CRepository pointer successfully.
		TheTest.Printf(_L("Release mode err = %d.\n"),err);
		TEST2(err, KErrNone);
	#endif

 	// Clean up the repository files created on the C Drive
 	CleanupFileFromCDriveL(KTestRepUid);

 	// Clean up the corrupted ini file on the C Drive
 	User::LeaveIfError(fm->Delete(KCCorruptIniFile));

 	// pop the cleanup stack for fm & fs
	CleanupStack::PopAndDestroy(2, &fs);

	// Kill the Cenrep server so that it can it can be restarted by the client
	// without the corrupted Cache ini file.
	err=KillProcess(KCentralRepositoryServerName);
	TEST2(err,KErrNone);
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-4024
@SYMTestCaseDesc
Test that the UID compare function works properly with large UID's.  Since
CObservable::CompareTUidValues() is a private function and cannot be accessed
directly, CObservable::AddObserverL()and CObservable::FindConnectedRepository()
are used to infer the correct operation.
@SYMTestPriority High
@SYMTestActions
-Compare a large positive UID and a very negative UID
-Compare a large positive UID and a large positive UID
-Compare a very negative UID and a very negative UID
-Compare two very large but equivalent UID's.
@SYMTestExpectedResults All UID's added to a sorted list can be found later, regardless of size and sign.
@SYMDEF DEF116043
@SYMUnit CObservable::CompareTUidValues()
*/
LOCAL_C void DEF116043L()
	{
	const TUid KLargePositive1 = {0x7FFFFFFF};
	const TUid KLargePositive2 = {0x7FFFFFFE};
	const TUid KLargeNegative = {0x8FFFFFFF};

	CObservable* ob = CObservable::NewLC();

	// Add a variety of UID's to work with, including large positives, a very negative,
	// and duplicates
	ob->AddObserverL(KLargePositive1, NULL);
	ob->AddObserverL(KLargeNegative, NULL);
	ob->AddObserverL(KLargePositive2, NULL);
	ob->AddObserverL(KLargePositive1, NULL);

	// Look for a large positive and expect a non-negative index
	TEST(ob->FindConnectedRepository(KLargePositive1) >= 0);

	// Look for another large positive and expect a non-negative index
	TEST(ob->FindConnectedRepository(KLargePositive2) >= 0);

	// Look for a very negative and expect a non-negative index.
	// This will fail under 'UID1 - UID2' implementations.
	TEST(ob->FindConnectedRepository(KLargeNegative) >= 0);

	CleanupStack::PopAndDestroy(ob);
	}
	
/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4081
@SYMTestCaseDesc
Test that the Observer compare function works properly with large UID's
@SYMTestPriority High
@SYMTestActions The test will insert a set of pointers with very big and small unsigned address
and ensure that the correct order is inserted in the list
@SYMTestExpectedResults the pointer is inserted in the correct order
@SYMDEF DEF132807
*/	
LOCAL_C void DEF132807L()
	{
	CObservable* ob = CObservable::NewLC();
	
	const TUid KReposUid={0x87654321};
	
	ob->AddObserverL(KReposUid,(CServerRepository*)0x7FFFFFFF);
	ob->AddObserverL(KReposUid,(CServerRepository*)0x7FFFFFFE);
	ob->AddObserverL(KReposUid,(CServerRepository*)0x8FFFFFFF);
	ob->AddObserverL(KReposUid,(CServerRepository*)0x00000001);		

	TEST(ob->iObservers.Count()==4);
	CObservable::TRepositoryObserverInfo pos1=ob->iObservers[0];
	TEST(pos1.iRepositoryPointer==(CServerRepository*)0x00000001);
	CObservable::TRepositoryObserverInfo pos2=ob->iObservers[1];
	TEST(pos2.iRepositoryPointer==(CServerRepository*)0x7FFFFFFE);	
	CObservable::TRepositoryObserverInfo pos3=ob->iObservers[2];
	TEST(pos3.iRepositoryPointer==(CServerRepository*)0x7FFFFFFF);	
	CObservable::TRepositoryObserverInfo pos4=ob->iObservers[3];
	TEST(pos4.iRepositoryPointer==(CServerRepository*)0x8FFFFFFF);	
	CleanupStack::PopAndDestroy(ob);
	}

//a test class notify handler
//the notify handler will check the value for different test case
static TInt currentTestCaseID=0;

class CTestNotifyHandler : public CBase, public MCenRepNotifyHandlerCallback
	{
public:
	static CTestNotifyHandler* NewL()
		{
		return new (ELeave)CTestNotifyHandler;
		}
	~CTestNotifyHandler(){}
	// Notification handlers
	void HandleNotifyInt(TUint32 aId, TInt aNewValue)
		{
		if (currentTestCaseID==0)
			return;
		else if(currentTestCaseID==1)
			TEST(aId==1 && aNewValue==208);
		}
	void HandleNotifyReal(TUint32 aId, TReal /*aNewValue*/)
		{
		if (currentTestCaseID==2)
			{
			TEST(aId==1);
			CActiveScheduler::Stop();
			}
		}
	void HandleNotifyString(TUint32 /*aId*/, const TDesC16& /*aNewValue*/)
		{
		}
	void HandleNotifyBinary(TUint32 /*aId*/, const TDesC8& /*aNewValue*/)
		{
		}
	void HandleNotifyGeneric(TUint32 /*aId*/)
		{
		}
	void HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* /*aHandler*/)
		{
		if (currentTestCaseID==0)
			return;
		else if (currentTestCaseID==1)
			TEST((TInt)aId==-11 && error==KErrArgument);
		else if (currentTestCaseID==2)
			TEST(aId==1 && error==KErrArgument);
		else if (currentTestCaseID==3)
			TEST(aId== 10000 && error==KErrPermissionDenied);
		
		CActiveScheduler::Stop();
		}
private:
	CTestNotifyHandler(){}
	};

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-4035
@SYMTestCaseDesc Notify Handling in centralrepository under different error conditions
@SYMTestPriority High
@SYMTestActions Testing the CRepository::NotifyRequest under different error condition
				Testing the CCenrepNotifierHandler under different error condition
@SYMTestExpectedResults Test should pass and exhibit the expected behaviour
@SYMDEF DEF117987
*/
LOCAL_C void DEF117987L()
	{
	__UHEAP_MARK;

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	const TUid KCDUid = {0xF0000001};
	CRepository* rep=CRepository::NewL(KCDUid);

	TRequestStatus ts,ts1;

	//-----------------single notification test-----------------------
	//Negative testing for Double notification on same setting
	TInt ret=rep->NotifyRequest(1,ts);
	TEST(ret==KErrNone);
	ret=rep->NotifyRequest(1,ts1);
	TEST(ret==KErrNone);
	ret=rep->Set(1,123);
	User::WaitForRequest(ts);
	User::WaitForRequest(ts1);
	TEST(ts==1);
	TEST(ts1==KErrAlreadyExists);
	
	//Negative testing for non-existent key---------------------------
	ts=KRequestPending;
	ret=rep->NotifyRequest(10303,ts);
	TEST(ret==KErrNotFound);
	TEST(ts==KRequestPending);

	//Negative testing for key we dont have permission----------------
	ret=rep->NotifyRequest(10000,ts);
	TEST(ret==KErrPermissionDenied);
	TEST(ts==KRequestPending);

	//---------------------group notification test--------------------------
	ts=KRequestPending;
	ts1=KRequestPending;

	//Negative testing for double notification on the same setting range
	//set partial key 0 and mask 0xFFFFFF00 to filter only 0-255
	ret=rep->NotifyRequest(0,0xFFFFFF00,ts);
	TEST(ret==KErrNone);
	//second notification request on the same partial key and mask should not fail
	ret=rep->NotifyRequest(0,0xFFFFFF00,ts1);
	TEST(ret==KErrNone);
	ret=rep->Set(1,246);
	User::WaitForRequest(ts);
	User::WaitForRequest(ts1);
	TEST(ts==1);
	TEST(ts1==1);
	
	//Negative testing for unknown range------------------------------
	//this will not return error code for range notification as it is intended for
	//to cover keys created later in the future.
	ts=KRequestPending;
	ret=rep->NotifyRequest(100000,0xFFFFFFFF,ts);
	TEST(ret==KErrNone);
	ret=rep->NotifyCancel(100000,0xFFFFFFFF);
	User::WaitForAnyRequest();
	TEST(ret==KErrNone);
	TEST(ts==KUnspecifiedKey);

	//Negative testing for key we dont have permission------------------------------
	ts=KRequestPending;
	ret=rep->NotifyRequest(10000,0xFFFFFFFF,ts);
	TEST(ret==KErrNone);
	User::WaitForRequest(ts);
	TEST(ts==KErrPermissionDenied);

	delete rep;

	//----------------single notification using cenrepnotifhandler-----------
	currentTestCaseID=1;
	rep=CRepository::NewL(KCDUid);
	//create the handler callback
	CTestNotifyHandler* callback=CTestNotifyHandler::NewL();

	//now setup the handler
	CCenRepNotifyHandler* handler=CCenRepNotifyHandler::NewLC(*callback,*rep,CCenRepNotifyHandler::EIntKey,1);
	CCenRepNotifyHandler* handler2=CCenRepNotifyHandler::NewLC(*callback,*rep,CCenRepNotifyHandler::EIntKey,1);

	handler->StartListeningL();
	handler2->StartListeningL();

	ret=rep->Set(1,208);
	TEST(ret==KErrNone);

	CActiveScheduler::Start();
	
	User::After(1000000);
	CleanupStack::PopAndDestroy(2);
	
	delete callback;
	delete rep;

	CleanupStack::PopAndDestroy();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-4034
@SYMTestCaseDesc Centrep notifier handler does not check return codes
@SYMTestPriority High
@SYMTestActions	 Test that error code encountered during handling of notification
				 results in the HandleNotifyError being invoked
@SYMTestExpectedResults Test should pass and exhibit the expected behaviour
@SYMDEF DEF117848
*/
LOCAL_C void DEF117848L()
	{
	__UHEAP_MARK;

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	const TUid KCDUid = {0xF0000001};

	//create the handler callback
	CTestNotifyHandler* callback=CTestNotifyHandler::NewL();

	//----------------notify handle error test case------------------------------
	currentTestCaseID=2;
	CRepository* rep=CRepository::NewL(KCDUid);

	//purposely set the notify handler for a key but specifying the wrong type
	//this will only be detected during the notification
	CCenRepNotifyHandler* handler=CCenRepNotifyHandler::NewLC(*callback,*rep,CCenRepNotifyHandler::ERealKey,1);
	handler->StartListeningL();
	TInt ret=rep->Set(1,199);
	TEST(ret==KErrNone);
	
	CActiveScheduler::Start();	
	//handlenotify error will be called and validate the error code and then stop the scheduler
	handler->StopListening();
	
	CleanupStack::PopAndDestroy();
	
	//test for other type of settings
	handler=CCenRepNotifyHandler::NewLC(*callback,*rep,CCenRepNotifyHandler::EStringKey,1);
	handler->StartListeningL();
	ret=rep->Set(1,209);
	TEST(ret==KErrNone);
	CActiveScheduler::Start();
	handler->StopListening();
	CleanupStack::PopAndDestroy();

	//test for other type of settings
	handler=CCenRepNotifyHandler::NewLC(*callback,*rep,CCenRepNotifyHandler::EBinaryKey,1);
	handler->StartListeningL();
	ret=rep->Set(1,309);
	TEST(ret==KErrNone);
	
	CActiveScheduler::Start();
	handler->StopListening();
	CleanupStack::PopAndDestroy();	
	
	delete callback;
	delete rep;

	CleanupStack::PopAndDestroy();
	
	__UHEAP_MARKEND;			
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-4036
@SYMTestCaseDesc  Central Repository Notification Handler flooded with notifications
@SYMTestPriority High
@SYMTestActions	Testing the CCenrepNotifierHandler when the client does not have
				enough permission when setting up the whole repository notifier.
@SYMTestExpectedResults Test should pass and exhibit the expected behaviour
@SYMDEF DEF118107
*/
LOCAL_C void DEF118107L()
	{
	__UHEAP_MARK;
	
	currentTestCaseID = 3;
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	const TUid KCDUid = {0xF0000001};	
	CRepository* rep=CRepository::NewL(KCDUid);

	//create the handler callback
	CTestNotifyHandler* callback=CTestNotifyHandler::NewL();	
	
	//create a handler to listen to the whole repository
	CCenRepNotifyHandler* handler=CCenRepNotifyHandler::NewLC(*callback,*rep);	
	
	//Start listening as a fail should occur as one of repository entries does not
	//have the correct capabilities
	handler->StartListeningL();
	
	//This causes the handlenotifyerror to be called which checks that 
	//KErrPermissionDenied has occured and stops the scheduler.
	CActiveScheduler::Start();
	handler->StopListening();
	
	//Cleanup
	CleanupStack::PopAndDestroy();
	
	delete callback;
	delete rep;

	CleanupStack::PopAndDestroy();
	
	__UHEAP_MARKEND;		
	}


// This function creates TServerSetting objects with the data in the ServerSettingStruct array and then
// appends them to the specified RSettingsArray object.
LOCAL_C void SetupSettingsArrayL(RSettingsArray& aSettingsArray,const ServerSettingStruct aSettingStructArr[], TInt aSettingStructArrSize)
	{
	for(TInt i = 0; i < aSettingStructArrSize; i++)
		{
		TServerSetting newSetting(aSettingStructArr[i].key);
		TInt error;
		newSetting.SetIntValue(aSettingStructArr[i].value);
		newSetting.SetType(aSettingStructArr[i].meta);
		newSetting.SetMeta(aSettingStructArr[i].meta);

		error = aSettingsArray.OrderedInsert(newSetting);
		
		if(KErrNone != error)
			{
			newSetting.Reset();	// for safety in case of newSetting is not TInt type.
			User::Leave(error);
			}
		}
	}
	
// This function compares 2 RSettingsArray arrays. Panics if they are not identical
LOCAL_C void CompareSettingsArrays(const RSettingsArray& aArr1, const RSettingsArray& aArr2)	
	{
	TEST2(aArr1.Count(), aArr2.Count());
	for(TInt i = 0; i < aArr2.Count(); i++)
		{
		if(aArr1[i].IsDeleted())
			{
			TEST(aArr2[i].IsDeleted());	
			}
		else
			{
			TEST(aArr1[i] == aArr2[i]);				
			}
		}
	}

// This function compares 2 RArray<TUint32> arrays. Panics if they are not identical
LOCAL_C void CompareUint32Arrays(const RArray<TUint32>& aArr1, const RArray<TUint32>& aArr2)	
	{
	TEST2(aArr1.Count(), aArr2.Count());
	for(TInt i = 0; i < aArr2.Count(); i++)
		{
		TEST(aArr1[i] == aArr2[i]);				
		}
	}

// This function calls MergerArray() with the pre-set parameters for each merge type and compares the results to 
// the reference arrays.
// Also used as the OOM test if aOOMMode is ETrue.
LOCAL_C void MergerArrayTestL(const RSettingsArray aPersistRef[], const RSettingsArray aChangeRef[], const RArray<TUint32> aDeletedRef[], TBool aOOMMode)

	{
	// Data used to construct the persistence array for all merge types	
	static const ServerSettingStruct settingStructPersist[] = {{1,0,1}, {3,KMetaBackup,3}, {5,0,5}, {7,0,7}, {9,KMetaDefaultValue,9}, {11,0,11}, {13,KMetaDefaultValue,13}};
	
	// Data used to construct the changes array for merge type: ETransactionMerge. Some are marked as deleted.
	static const ServerSettingStruct settingStructChangeTrans[] = {{1,TServerSetting::EDeleted,1}, {2,TServerSetting::EDeleted,2}, \
		{3,TServerSetting::EDeleted,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,8}, {9,0,10}};
	
	// Data used to construct the changes array for all other merge types than ETransactionMerge. No deleted items.
	static const ServerSettingStruct settingStructChange[] = {{1,0,1}, {2,0,2}, {3,0,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,8}, {9,0,10}};
	
	for(TInt i = 0; i < sizeof(KMergerTypes) / sizeof(TMergeType); i++)
		{
		RSettingsArray persist;
		RSettingsArray change;
		RArray<TUint32> deleted; 
		TInt error = KErrNone ;
		TInt count = 0;
		
		do
			{
			__UHEAP_MARK;
			
			// Sets up arrays passed into MergeArray()
			SetupSettingsArrayL(persist,settingStructPersist, sizeof(settingStructPersist)/sizeof(ServerSettingStruct));
			if( ETransactionMerge == KMergerTypes[i] )
				{
				SetupSettingsArrayL(change,settingStructChangeTrans, sizeof(settingStructChangeTrans)/sizeof(ServerSettingStruct));	
				}
			else
				{
				SetupSettingsArrayL(change,settingStructChange, sizeof(settingStructChange)/sizeof(ServerSettingStruct));	
				}
			
			deleted.InsertInUnsignedKeyOrderL(4);
			
			if(aOOMMode)
				{
				__UHEAP_SETFAIL(RHeap::EFailNext, ++count);
				}
			
			error = persist.MergeArray(change, deleted, KMergerTypes[i]);
			
			if (aOOMMode)
				{
				TEST(KErrNone == error || KErrNoMemory == error);
				__UHEAP_SETFAIL(RHeap::ENone, 0);
				}
			else 
				{
				TEST(KErrNone == error);
				// Verifies affected arrays.
				CompareSettingsArrays(persist, aPersistRef[i]);
				CompareSettingsArrays(change, aChangeRef[i]);
				CompareUint32Arrays(deleted, aDeletedRef[i]);
				} 

			// Resets the arrays.			
			persist.Reset();
			change.Reset();		
			deleted.Reset();
			
			__UHEAP_MARKEND;
			} while(KErrNoMemory == error);
	 	if (aOOMMode)
	 		{
			TheTest.Printf(_L("- MergeArray for type %d succeeded at heap failure rate of %i\n"), KMergerTypes[i], count);	 		
	 		}
		}
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-4037
@SYMTestCaseDesc This test case verifies the re-factored CentralRepository internal merge function (MergeArray).
@SYMTestPriority High
@SYMTestActions	 For each merger type, calls MergeArray() with the pre-defined arrays: this, aChanges, aDeleted, 
                 which would be changed by the function. Verifies these 3 arrays with the reference arrays as well as 
                 the return code after MergeArray() returns.
                 This case also carries out the OOM test for function MergeArray().
@SYMTestExpectedResults MergeArray() should return no error and these 3 arrays should be identical with the reference
                        arrays. In OOM mode, only KErrNoMemory or KErrNone should be returned.
@SYMDEF DEF116629
*/
LOCAL_C void DEF116629L()
	{

	// Data used to construct the reference arrays of each merge type:
	// ETransactionMerge
	const ServerSettingStruct settingStructPersistAfterTrans[] = {{4,0,4}, {5,0,5}, {6,0,6}, {7,0,8}, {9,0,10}, {11,0,11}, {13,0,13}};
	const ServerSettingStruct settingStructChangeAfterTrans[] = {{1,TServerSetting::EDeleted,1}, 
		{3,TServerSetting::EDeleted,3}, {4,0,4}, {6,0,6}, {7,0,8}, {9,0,10}};

	// ERestoreMerge
	const ServerSettingStruct settingStructPersistAfterRest[] = {{1,0,1}, {2,0,2}, {3,KMetaBackup,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,8}, {9,0,10}, {11,0,11}, {13,0,13}};
	const ServerSettingStruct settingStructChangeAfterRest[] = {{2,0,2}, {4,0,4}, {6,0,6}, {7,0,8}, {9,0,10}};
	
	// ESWIUpgradeMerge
	const ServerSettingStruct settingStructPersistAfterSWIUp[] = {{1,0,1}, {2,0,2}, {3,KMetaBackup,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,7}, {9,0,10}, {11,0,11}, {13,0,13}};
	const ServerSettingStruct settingStructChangeAfterSWIUp[] = {{2,0,2}, {4,0,4}, {6,0,6}, {9,0,10}};
			
	// ESWIDowngradeMerge
	const ServerSettingStruct settingStructPersistAfterSWIDown[] = {{1,0,1}, {2,0,2}, {3,KMetaBackup,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,7}, {9,0,10}, {11,0,11}};
	const ServerSettingStruct settingStructChangeAfterSWIDown[] = {{2,0,2}, {4,0,4}, {6,0,6}, {9,0,10}, {13,0,0}};

	// ERomFlash
	const ServerSettingStruct settingStructPersistAfterRomFlash[] = {{1,0,1}, {2,0,2}, {3,KMetaBackup,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,7}, {9,0,10}, {11,0,11}};	
	const ServerSettingStruct settingStructChangeAfterRomFlash[] = {{1,0,1}, {2,0,2}, {3,0,3}, {4,0,4}, {5,0,5}, {6,0,6}, {7,0,8}, {9,0,10}};			
	
	// Reference arrays
	RSettingsArray persistAfter[5];
	RSettingsArray changeAfter[5];
	RArray<TUint32> deletedAfter[5]; 
	
	// Sets up the reference arrays for each merge type:
	// ETransactionMerge
	SetupSettingsArrayL(persistAfter[0],settingStructPersistAfterTrans, sizeof(settingStructPersistAfterTrans)/sizeof(ServerSettingStruct));
	SetupSettingsArrayL(changeAfter[0],settingStructChangeAfterTrans, sizeof(settingStructChangeAfterTrans)/sizeof(ServerSettingStruct));
	deletedAfter[0].InsertInUnsignedKeyOrderL(3);
	
	// ERestoreMerge
	SetupSettingsArrayL(persistAfter[1],settingStructPersistAfterRest, sizeof(settingStructPersistAfterRest)/sizeof(ServerSettingStruct));
	SetupSettingsArrayL(changeAfter[1],settingStructChangeAfterRest, sizeof(settingStructChangeAfterRest)/sizeof(ServerSettingStruct));
	//deletedAfter[1] should be empty because Restore Merge should not delete any setting.

	// ESWIUpgradeMerge
	SetupSettingsArrayL(persistAfter[2],settingStructPersistAfterSWIUp, sizeof(settingStructPersistAfterSWIUp)/sizeof(ServerSettingStruct));
	SetupSettingsArrayL(changeAfter[2],settingStructChangeAfterSWIUp, sizeof(settingStructChangeAfterSWIUp)/sizeof(ServerSettingStruct));
	//deletedAfter[2] should be empty because SWIUpgrade Merge should not delete any setting.

	// ESWIDowngradeMerge
	SetupSettingsArrayL(persistAfter[3],settingStructPersistAfterSWIDown, sizeof(settingStructPersistAfterSWIDown)/sizeof(ServerSettingStruct));
	SetupSettingsArrayL(changeAfter[3],settingStructChangeAfterSWIDown, sizeof(settingStructChangeAfterSWIDown)/sizeof(ServerSettingStruct));
	//deletedAfter[3] should be empty because settings deleted by SWIDowngrade Merge are recorded in aChanges.

	// ERomFlash
	SetupSettingsArrayL(persistAfter[4],settingStructPersistAfterRomFlash, sizeof(settingStructPersistAfterRomFlash)/sizeof(ServerSettingStruct));
	SetupSettingsArrayL(changeAfter[4],settingStructChangeAfterRomFlash, sizeof(settingStructChangeAfterRomFlash)/sizeof(ServerSettingStruct));
	//deletedAfter[4] should be empty because settings deleted by RomFlash Merge are recorded in aChanges.
	
	TheTest.Printf(_L("- Verification test for MergeArray.\n"));
	MergerArrayTestL(persistAfter, changeAfter, deletedAfter, EFalse);
	
	TheTest.Printf(_L("- OOM test for MergeArray.\n"));
	MergerArrayTestL(persistAfter, changeAfter, deletedAfter, ETrue);

	// Resets the reference arrays.		
	for(TInt i = 0; i < sizeof(KMergerTypes) / sizeof(TMergeType); i++)
		{
		persistAfter[i].Reset();
		changeAfter[i].Reset();
		deletedAfter[i].Reset();
		}
	}
/**
@SYMTestCaseID 				PDS-CENTRALREPOSITORY-UT-4046	
@SYMTestCaseDesc 			The centrep server panics when it reads a repository text file where 
							the value of a binary settings is missing.
@SYMTestPriority 			Medium
@SYMTestActions  			- Provide a repository text file for this test which have a binary 
							setting which value is missing. 
 							- Open the repository text file, 
							this should return KErrCorrupt when parsing the repository text file.
@SYMTestExpectedResults 	The test must not fail. 
@SYMDEF 					PDEF126904 
*/    
LOCAL_C void PDEF126904L()
   	{
   	const TUid KUidBinaryValueRepositoryFile ={0x1020506B};
   	const TInt KBinaryValueRepKey = 0xA;
   	TInt binaryValue = 0;
   	CRepository* repository = NULL;
   	
   	TRAPD(res, repository = CRepository::NewL(KUidBinaryValueRepositoryFile));		
   	if (res == KErrNone)
 		{
 		TInt err = KErrNone;
 		CleanupStack::PushL(repository);
 		err = repository->Get(KBinaryValueRepKey, binaryValue);
		User::LeaveIfError(err);
 		CleanupStack::PopAndDestroy(repository);
 		}
 	else if (res == KErrCorrupt)
 		{
 		//This is just to confirm that the err should be KErrCorrupt.
		TheTest.Printf(_L("The error is KErrCorrupt as expected.\n"));
		}
 	else
		{
 		User::Leave(res);
 		}
}	

/**
@SYMTestCaseID 				PDS-CENTRALREPOSITORY-UT-4084	
@SYMTestCaseDesc 			Ensure that the timestamp in a TXT file is always zero when not specified
							instead of arbitrary value
@SYMTestPriority 			Medium
@SYMTestActions  			Create the CHeapRepository object from a TXT keyspace file where no timestamp(legacy)
							section is specified. The timestamp should be set to zero
@SYMTestExpectedResults 	The test must not fail. 
@SYMDEF 					DEF136161 
*/ 
LOCAL_C void DEF136161L()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	//this is a TXT based ROM keyspace
	const TUid KTestUid={0x00000001};

	CHeapRepository* repos=CHeapRepository::NewL(KTestUid);
	CIniFileIn* iniFile;
	TInt ret=CIniFileIn::NewLC(fs,iniFile,_L("z:\\private\\10202be9\\00000001.txt"));
	User::LeaveIfError(ret);
	repos->ReloadContentL(*iniFile);
	
	//check the timestamp is always set to zero
	TEST(repos->TimeStamp()==TTime(0));
	
	CleanupStack::PopAndDestroy(iniFile);
	delete repos;
	
	fs.Close();
	}

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4085
@SYMTestCaseDesc  Central Repository: Trying to configure Central Repository cache with centrep.in 
@SYMTestPriority High
@SYMTestActions	Testing that the SWIWatcher does nto leave when the cache configuration exists within
the private data cate
@SYMTestExpectedResults Test should pass and exhibit the expected behaviour
@SYMDEF PDEF139979
*/
LOCAL_C void PDEF139979L()
    {
    __UHEAP_MARK;   
    CActiveScheduler* s=new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);
        
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    CFileMan* fm = CFileMan::NewL(fs);
    CleanupStack::PushL(fm);    

    TRAPD(err,TServerResources::InitialiseL());
    TEST2(err,KErrNone);    
    
    delete TServerResources::iInstallDirectory;
    _LIT(KInstallFolder,"c:\\private\\12345678\\");
    TServerResources::iInstallDirectory=(KInstallFolder()).AllocL();
    TInt erro=fm->Delete(_L("c:\\private\\12345678\\*.*"));
    //copy centrep.ini file to the install folder
    _LIT(KSource,"z:\\private\\10202be9\\centrepcache.ini0");
    _LIT(KDestination,"c:\\private\\12345678\\centrep.ini");
    TRAP(err,CopyTestFilesL(*fm,KSource(),KDestination()));
    
    CCentRepSWIWatcher* swi=CCentRepSWIWatcher::NewL(fs);
    TRAP(err,swi->FindChangedEntriesL(EFalse));
    TEST2(err,KErrNone);
    
    delete swi;
    TServerResources::Close();
    
    //delete the c file before we exit
    fm->Delete(KDestination());
    
    CleanupStack::PopAndDestroy(3);
    __UHEAP_MARKEND;  
    }

/**
@SYMTestCaseID PDS-CENTRALREPOSITORY-UT-4086
@SYMTestCaseDesc centralrepositorysrv.exe crashes and phone doesn't boot up 
@SYMTestPriority High
@SYMTestActions  Test merging two settings of different type, one type exist for
persist and one type for the new rom, when the type differes, we expect the
persist setting to be completely replaced by the rom.
@SYMTestExpectedResults The test must not panic or fail.
@SYMDEF PDEF141519
*/
LOCAL_C void PDEF141519L()
    {
    __UHEAP_MARK;
    
    RSettingsArray persist;
    RSettingsArray newRom;
    RArray<TUint32> deletedSettings;
    //persist setting
    TServerSetting a(0x10);
    a.SetType(TServerSetting::EInt);
    a.SetIntValue(100);
    persist.OrderedInsertL(a);
    
    //new rom setting
    TServerSetting b(0x10);
    b.SetType(TServerSetting::EString);
    TBuf8<5> buffer(_L8("abc"));
    HBufC8* hbuf=buffer.AllocL();
    b.SetStrValue(hbuf);
    newRom.OrderedInsertL(b);   
    
    User::LeaveIfError(persist.MergeArray(newRom,deletedSettings,ERomFlash));
    TServerSetting* persistEntry=persist.Find(0x10);
    TEST2(persistEntry->Key(),0x10);
    TEST2(persistEntry->Type(),TServerSetting::EString);
    const HBufC8* val=persistEntry->GetStrValue();
    TEST2(val->Compare(buffer),0);
    
    persist.Close();
    newRom.Close();
    
    __UHEAP_MARKEND;
    } 

LOCAL_C void ConnectStartSuicideTransL(void)
    {
    const TUid KLargeReposUid1 ={0xcccccc00};   
    CRepository* repository=CRepository::NewL(KLargeReposUid1);
    repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
    }

LOCAL_C TInt SuicidalTransThread(TAny*)
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(!cleanup)
        return KErrNoMemory;

    TRAP_IGNORE(ConnectStartSuicideTransL());
    //purposely waiting to be killed
    User::WaitForAnyRequest();
    return 0;
    }

LOCAL_C void DEF143352L()
    {
    CleanupCDriveL();    
    __UHEAP_MARK;

    const TUid KLargeReposUid1 ={0xcccccc00};
    //create on in this thread and start transaction too
    CRepository* rep1=CRepository::NewL(KLargeReposUid1);
    User::LeaveIfError(rep1->StartTransaction(CRepository::EConcurrentReadWriteTransaction));
    
    //create thread to connect, start transaction on the same repository as previous
    RThread testThread;
    _LIT(KThreadName1, "Martin");
    testThread.Create(KThreadName1, SuicidalTransThread, KDefaultStackSize, KMinHeapSize, 0x100000, NULL);

    TRequestStatus requestStatus;
    testThread.Logon(requestStatus);
    testThread.Resume();

    //wait for the first client to complete the start transaction
    User::After(1000000);
    
    //opening another big keyspace to force the eviction(cache size is 100K) both repository is about 78K in the heap
    //cccccc03 is exactly the same as cccccc00
    const TUid KLargeReposUid2 ={0xcccccc04};
    CRepository* rep2=CRepository::NewL(KLargeReposUid2);
   
    //now kill the thread we have created
    testThread.Kill(KErrDied);
    User::WaitForRequest(requestStatus);
    TEST2(requestStatus.Int(), KErrDied);
    
    //Wait for the session to be cleaned up
    User::After(1000000);
    
    //now current test thread will retry the same connection
    //at this stage server will panic with kern-exec 3
    CRepository* rep4=CRepository::NewL(KLargeReposUid1);
    
    delete rep1;
    delete rep2;
    delete rep4;
    
    __UHEAP_MARKEND;
    }
	
LOCAL_C void FuncTestsL()
	{
	TheTest.Start(_L("DEF053500 - Central repository integer type key entries cannot handle hex values"));

	DEF053500L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4015 DEF111734 - Central repository crashes when accessing a closed repository with active transaction "));
	DEF111734L();

    TheTest.Next(_L("DEF054368 - MetaData value in CentRep setting is not optional"));
	DEF054368L();

	TheTest.Next(_L("DEF055680 - Central repository find performance could be improved "));
	DEF055680L();

    TheTest.Next(_L("INC054688 - Key based capabilities in ini-file crashes cenrep server "));
	INC054688L();

	TheTest.Next(_L("DEF054632 - CenRep Initialisation default data does not work as expected"));
	DEF054632L();

	TheTest.Next(_L("DEF055661 - Central Repository does not allow AlwaysFail to be set for access control. "));
	DEF055661L();

	TheTest.Next(_L("DEF055267 - Can not insert more than 71 records into WapAccessPoint Table "));
	DEF055267L();

	TheTest.Next(_L("INC056194 - NTT - Central Repository checks DefaultWriteAccessPolicy when cancelling notif "));
	INC056194L();

	TheTest.Next(_L("DEF057145 - CenRepSrv KERN-EXEC3 when running CommDB test harness "));
	DEF057145L();

	TheTest.Next(_L("DEF057778 - Central repository ResetAll can cause user Panic 190 "));
	DEF057778L();

	TheTest.Next(_L("DEF057522 - Centralrepository security policy incorrect behaviour "));
	DEF057522L();

	TheTest.Next(_L("DEF057999 - Creating and deleting the same setting in a transaction doesn't work "));
	DEF057999L();

	TheTest.Next(_L("DEF057470 - Central repository lacks a range delete method "));
	DEF057470L();

    TheTest.Next(_L("INC058229 - CInstallFileWatcher::ReadInstallDirL contains insufficient error handling "));
	INC058229L();

	TheTest.Next(_L("DEF058900 - CentralRepository Notification section document needs update "));
	DEF058900L();

	TheTest.Next(_L("DEF061087 - Central repository cannot handle more than one range policy "));
	DEF061087L();

	TheTest.Next(_L("DEF060843 - [PSAudit] Lack of centralrep. data type checking could cause instability & bugs "));
	DEF060843L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1433 INC069013 - Central repository panics the client when trying to read 8-bit descriptor with uneven length "));
	INC069013L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1434 DEF070731 - Central repository doesn't handle corrupt .cre files correctly. "));
	DEF070731L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1683 DEF084700 - UIF automated tests crash H4 board. "));
	DEF084700L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1862 DEF089945: [AQP] CommitTransaction returns incorrect error code if Find~L methods run OOM "));
	DEF089945L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3242 PDEF098500 - Backup to memory card causes phone freeze "));
	PDEF098500();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1884 DEF093855: CenRep cancels transactions if they exceed 5 seconds... "));
	DEF093855L();
	
	
//	When adding a new defect test, if you decide to reuse a repository that has already been used 
//  in another defect test, make sure you modify the existing test which uses the repository and 
//  add the cache delay after that test closes the repository. e.g. DEF055680L

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3477 PDEF106507 - Currupt installdir.bin file causes server to crash "));
	PDEF106507L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3517 INC108803 - R3.2 stability problems: CentralRepositorySrv crash "));
	INC108803L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3545 DEF109390: Centrep cache ini file corruption should not cause server crash. "));
	DEF109390L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4024 DEF116043: Cenrep doesn't protect against overflows in sorting "));
	DEF116043L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4035 DEF117987: CenrepNotify handler does not leave/return error "));
	DEF117987L();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-4034 DEF117848: Centrep notifier handler does not check return codes "));
	DEF117848L();

	TheTest.Next(_L(" @SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-4036 DEF118107: Central Repository Notification Handler flooded with notifications "));
	DEF118107L();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4037 DEF116629: CentralRepository internal merge function need to be refactored and documented")); 
	DEF116629L();
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4046 PDEF126904  - Ini-file parsing of Symbian provided central repository server crashes"));
	PDEF126904L();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4081 DEF132807 - CObservable::ObserverSortOrder ordering algorithm is wrong "));	
	DEF132807L();	

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4084 DEF136161: CenRep file timestamp is never intialised  "));
	DEF136161L();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4085 PDEF139979: Central Repository: Trying to configure Central Repository cache with centrep.in  "));
  PDEF139979L();

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4086 PDEF141518: centralrepositorysrv.exe crashes and phone doesn't boot up "));
	PDEF141519L(); 

	TheTest.Next(_L("@SYMTestCaseID:PDS-CENTRALREPOSITORY-CT-XXXX DEF143352: CentralRepository server crash in CObservable::RefreshTransactorAccessPolicies "));
	DEF143352L();

	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("Defect tests"));
	CleanupCDriveL();

	FuncTestsL();

	TheTest.Next(_L("Clean out C: files"));
	CleanupCDriveL();

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
	//
	// For the tests to work we need SID policing enforced plus the specific
	// capabilities listed below.
	//
	// These are dependent on the capabilities set in the platform security
	// repository test initialisation file 87654321.txt.  If the content
	// of that file changes then the following clauses may need to be
	// updated.
	//
	if(!PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityNetworkServices) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityDRM) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityLocalServices) ||
			!PlatSec::IsCapabilityEnforced(ECapabilityCommDD))
		{
		TheTest.Start(_L("NOTE: Skipping tests due to incompatible PlatSec enforcement settings"));
		TheTest.End();
		TheTest.Close();
		return 0;
		}

	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
