/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "t_certstoreactions.h"
#include "t_certstoreout.h"
#include "t_certstoredefs.h"
#include "t_input.h"
#include <x509cert.h>
#include <wtlscert.h>
#include <securityerr.h>
#include <ecom/ecom.h>
#include "testutilclient.h"
#include <swicertstore.h>

// CCertStoreChangeNotifier ////////////////////////////////////////////////////////////// 
 CCertStoreChangeNotifier* CCertStoreChangeNotifier::NewL(TInt& aNotificationFlag)
 	{
 	CCertStoreChangeNotifier* self = new(ELeave) CCertStoreChangeNotifier(aNotificationFlag);
 	CleanupStack::PushL(self);
 	self->ConstructL();
 	CleanupStack::Pop();
 	return self;
 	}
 CCertStoreChangeNotifier::~CCertStoreChangeNotifier()
 	{
 	Cancel();
	iCertStoreChangeProperty.Close();
 	}
 CCertStoreChangeNotifier::CCertStoreChangeNotifier(TInt& aNotificationFlag): CActive(EPriorityHigh), iNotifiedCounter(aNotificationFlag)
 	{
 	CActiveScheduler::Add(this);
 	}	
 void CCertStoreChangeNotifier::ConstructL()
  	{
 	TInt r=iCertStoreChangeProperty.Attach(KUnifiedCertStorePropertyCat,EUnifiedCertStoreFlag,EOwnerThread);
 	User::LeaveIfError(r);
 	}
 void CCertStoreChangeNotifier::StartNotification()
 	{
 	iCertStoreChangeProperty.Subscribe(iStatus);
 	SetActive();
 	}	
 void CCertStoreChangeNotifier::DoCancel()
 	{
 	iCertStoreChangeProperty.Cancel();
 	}
 void CCertStoreChangeNotifier::RunL()
 	{
 	iNotifiedCounter++;
 	if (iCompleteStatus)
	 	{
	 	 User::RequestComplete(iCompleteStatus, KErrNone);	
	 	}
 	}
void CCertStoreChangeNotifier::SetCompleteStatus(TRequestStatus* aStatus)
	{
	iCompleteStatus=aStatus;
	}

// COpenCertStore //////////////////////////////////////////////////////////////

_LIT(KCOpenCertStore, "COpenCertStore");

COpenCertStore::~COpenCertStore()
	{
	if (iSet)
		{
		switch (iType)
			{
			case EUnifiedCertStore:
				delete iUnifiedCertStore;
				break;
			
			case ESWICertStore:
				iSwiCertStore->Release();
				break;
			
			default:
				User::Panic(KCOpenCertStore, 1);
			}
		}
	}

void COpenCertStore::SetCertStore(CUnifiedCertStore* aCertStore)
	{
	__ASSERT_ALWAYS(!iSet, User::Panic(KCOpenCertStore, 1));
	iType = EUnifiedCertStore;
	iUnifiedCertStore = aCertStore;
	iSet = ETrue;
	}
   
CUnifiedCertStore& COpenCertStore::AsUnifiedCertStore()
	{
	__ASSERT_ALWAYS(iSet && iType == EUnifiedCertStore, User::Panic(KCOpenCertStore, 1));
	return *iUnifiedCertStore;
	}


void COpenCertStore::SetCertStore(CSWICertStore* aCertStore)
	{
	__ASSERT_ALWAYS(!iSet, User::Panic(KCOpenCertStore, 1));
	iType = ESWICertStore;
	iSwiCertStore = aCertStore;
	iSet = ETrue;
	}

CSWICertStore& COpenCertStore::AsSWICertStore()
	{
	__ASSERT_ALWAYS(iSet && iType == ESWICertStore, User::Panic(KCOpenCertStore, 1));
	return *iSwiCertStore;
	}


TCertStoreType COpenCertStore::Type()
	{
	__ASSERT_ALWAYS(iSet, User::Panic(KCOpenCertStore, 1));
	return iType;
	}


MCertStore& COpenCertStore::CertStore()
	{
	__ASSERT_ALWAYS(iSet, User::Panic(KCOpenCertStore, 1));
	
	MCertStore* result = NULL;

	switch (iType)
		{
		case EUnifiedCertStore:
			result = iUnifiedCertStore;
			break;
			
		case ESWICertStore:
			result = iSwiCertStore;
			break;
			
		default:
			User::Panic(KCOpenCertStore, 1);
		}

	return *result;
	}

// CSharedData /////////////////////////////////////////////////////////////////

CSharedData::~CSharedData()
	{
	iCertStores.ResetAndDestroy();
	iCertStores.Close();
	DeleteCertificateAppInfoManager();
	REComSession::FinalClose();
	}

void CSharedData::InitCertificateAppInfoManagerL()
	{
	ASSERT(!iCertificateAppInfoManager);
	User::LeaveIfError(iFs.Connect());
	iCertificateAppInfoManager = CCertificateAppInfoManager::NewL(iFs, ETrue);
	}

void CSharedData::DeleteCertificateAppInfoManager()
	{
	if (iCertificateAppInfoManager)
		{
		delete iCertificateAppInfoManager;
		iCertificateAppInfoManager = NULL;
		iFs.Close();
		}
	}

RPointerArray<COpenCertStore>& CSharedData::CertStores()
	{
	return iCertStores;
	}

/////////////////////////////////////////////////////////////////////////////////
//CCertStoreTestAction base class
/////////////////////////////////////////////////////////////////////////////////

CCertStoreTestAction::~CCertStoreTestAction()
	{
	}

CCertStoreTestAction::CCertStoreTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

CSharedData& CCertStoreTestAction::CertStoreSharedData()
	{
	if (!SharedData())
		{
		SetSharedData(new CSharedData);
		}

    CSharedData* data = static_cast<CSharedData*>(SharedData());
	ASSERT(data); // panic if out of memory
	return *data;
	}

void CCertStoreTestAction::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	// Ensure shared data is already created to prevent panic in oom test
	CertStoreSharedData();
	
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CCertStoreTestAction::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

CCertificateAppInfoManager* CCertStoreTestAction::TheCertificateAppInfoManager()
	{
	return CertStoreSharedData().iCertificateAppInfoManager;
	}

void CCertStoreTestAction::InitTheCertificateAppInfoManagerL()
	{
	CertStoreSharedData().InitCertificateAppInfoManagerL();
	}

void CCertStoreTestAction::DeleteTheCertificateAppInfoManager()
	{
	CertStoreSharedData().DeleteCertificateAppInfoManager();
	}

RPointerArray<COpenCertStore>& CCertStoreTestAction::CertStores()
	{
	return CertStoreSharedData().CertStores();
	}

TInt CCertStoreTestAction::CertStoreCount()
	{
	return CertStores().Count();
	}

TCertStoreType CCertStoreTestAction::CertStoreType(TInt aIndex)
	{
	return CertStores()[aIndex]->Type();
	}

MCertStore& CCertStoreTestAction::CertStore(TInt aIndex)
	{
	return CertStores()[aIndex]->CertStore();
	}

void CCertStoreTestAction::AddCertStoreL(CUnifiedCertStore* aCertStore)
	{
	COpenCertStore* openStore = new (ELeave) COpenCertStore;
	CleanupStack::PushL(openStore);
  	User::LeaveIfError(CertStores().Append(openStore));
	CleanupStack::Pop(openStore);
	openStore->SetCertStore(aCertStore); // takes ownership
	}

CUnifiedCertStore& CCertStoreTestAction::UnifiedCertStore(TInt aIndex)
	{
	return CertStores()[aIndex]->AsUnifiedCertStore();
	}


void CCertStoreTestAction::AddCertStoreL(CSWICertStore* aCertStore)
	{
	COpenCertStore* openStore = new (ELeave) COpenCertStore;
	CleanupStack::PushL(openStore);
	User::LeaveIfError(CertStores().Append(openStore));
	CleanupStack::Pop(openStore);
	openStore->SetCertStore(aCertStore); // takes ownership
	}

CSWICertStore& CCertStoreTestAction::SWICertStore(TInt aIndex)
	{
	return CertStores()[aIndex]->AsSWICertStore();
	}


void CCertStoreTestAction::RemoveCertStore(TInt aIndex)
	{
	COpenCertStore* openStore = CertStores()[aIndex];
	TBool delCertstore = EFalse;
	
	if (openStore->Type() == ESWICertStore)
			{
			delCertstore = ETrue;
			}
			
	CertStores().Remove(aIndex);
	delete openStore;
	
	if (delCertstore)
			{
			RTestUtilSession testutil;
			User::LeaveIfError(testutil.Connect());
			CleanupClosePushL(testutil);
			
			TDriveUnit sysDrive (RFs::GetSystemDrive());
			TDriveName driveName(sysDrive.Name());			
			TBuf<64> certstoreFile (driveName);
			certstoreFile.Append(_L("\\Resource\\SwiCertstore\\"));
			testutil.RmDir(certstoreFile);

			CleanupStack::PopAndDestroy(&testutil);
			}
	}
/////////////////////////////////////////////////////////////////////////////////
//CSubscriberAction base class
/////////////////////////////////////////////////////////////////////////////////
CSubscriberAction::~CSubscriberAction()
	{
	if (iNotifier)
		delete iNotifier;
	}
CSubscriberAction::CSubscriberAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:CCertStoreTestAction(aFs, aConsole, aOut)
	{	
	}
void CSubscriberAction::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;
	TPtrC8 ptr1 = Input::ParseElement(aTestActionSpec.iActionBody, KChangeNotifiedStart, KChangeNotifiedEnd, pos, err);
	if (ptr1 != KNullDesC8)
		{
		TLex8 lexi(ptr1);
		TInt val;
		TInt ret=lexi.Val(val);
		if (val != 0)
			{
			iNotificationSubscribed=1;
			}
		}		
	}
/////////////////////////////////////////////////////////////////
//CInitialiseCertStore::
///////////////////////////////////////////////////////////////////
CTestAction* CInitialiseCertStore::NewL(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut,
										const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CInitialiseCertStore::NewLC(aFs, 
		aConsole, aOut,	aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CInitialiseCertStore::NewLC(RFs& aFs,
										 CConsoleBase& aConsole, 
										 Output& aOut, 
										 const TTestActionSpec& aTestActionSpec)
	{
	CInitialiseCertStore* self = new(ELeave) CInitialiseCertStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CInitialiseCertStore::~CInitialiseCertStore()
	{
	delete iNewUnifiedCertStore;
 	iFilterOrdering.Close();
 	iExpectedOrderingResult.Close();	
	}

CInitialiseCertStore::CInitialiseCertStore(RFs& aFs, 
										   CConsoleBase& aConsole, 
										   Output& aOut)

	:CCertStoreTestAction(aFs, aConsole, aOut), iState(ENew)
	{
	}

void CInitialiseCertStore::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 ptr = 
		Input::ParseElement(aTestActionSpec.iActionBody, KModeStart, KModeEnd, pos, err);
	if (ptr == KNullDesC8)
		{
		User::Leave(KErrNotFound);
		}

	if (ptr == _L8("read"))
		{
		iOpenedForWrite = EFalse;
		}
	else if (ptr == _L8("write"))
		{
		iOpenedForWrite = ETrue;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	pos = 0;
	err = KErrNone;
	TPtrC8 ptr1 = Input::ParseElement(aTestActionSpec.iActionBody, KOrderingFilterStart, KOrderingFilterEnd, pos, err);
	if (ptr1 != KNullDesC8)
		{
		TInt lastPos=0;
		TInt startPos=0;
		TLex8 lexi;
		do 
			{
			startPos=lastPos;
			while (lastPos<ptr1.Length() && ptr1[lastPos]!=',')
				{
				lastPos++;
				}
			TPtrC8 uidInA(&ptr1[startPos], lastPos-startPos);
			TLex8 lexi(uidInA);
			TInt32 uidInD=0;
			TInt err=lexi.Val(uidInD);
			User::LeaveIfError(iFilterOrdering.Append(uidInD));
			lastPos++;
			}
		while (lastPos < ptr1.Length());	
		}
	pos = 0;
	err = KErrNone;		
	TPtrC8 ptr2 = Input::ParseElement(aTestActionSpec.iActionResult, KOrderingResultStart, KOrderingResultEnd, pos, err);	
	if (ptr2 != KNullDesC8)
		{
		TInt lastPos=0;
		TInt startPos=0;
		TLex8 lexi;
		do 
			{
			startPos=lastPos;
			while (lastPos<ptr2.Length() && ptr2[lastPos]!=',')
				{
				lastPos++;
				}
			TPtrC8 uidInA(&ptr2[startPos], lastPos-startPos);
			TLex8 lexi(uidInA);
			TInt32 uidInD=0;
			TInt err=lexi.Val(uidInD);
			User::LeaveIfError(iExpectedOrderingResult.Append(uidInD));
			lastPos++;
			}
		while (lastPos < ptr2.Length());	
		}		
	pos = 0;
	err = KErrNone;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CInitialiseCertStore::PerformAction(TRequestStatus& aStatus)
	{
	if (aStatus != KErrNone)
		{
		iState = EFinished;
		}
	
	switch (iState)
		{
		case ENew:
			__ASSERT_DEBUG(!iNewUnifiedCertStore, User::Panic(_L("CInitialiseCertStore"), 1));
 			iNewUnifiedCertStore = CUnifiedCertStore::NewL(iFs, iOpenedForWrite, iFilterOrdering);
 			
 			iNewUnifiedCertStore->Initialize(aStatus);		
			iState = EAppend;
			break;

		case EAppend:
			{	
			AddCertStoreL(iNewUnifiedCertStore);
			iNewUnifiedCertStore = 0; // we don't own this any more
			if (iFilterOrdering.Count()==0)
				{
				iState = EFinished;
				}
			else
				{
				iState = ECheckOrder;
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);		
			}
			break;

		case ECheckOrder:
			{
			TInt ret=KErrNone;
			TInt count = UnifiedCertStore().CertStoreCount();
			MCTCertStore* p=NULL;
			TInt j=-1;
			TInt32 previous=0;
			TInt32 current=0;			
			for (TInt i=0;i<count;i++)
				{
				p=&(UnifiedCertStore().CertStore(i));
				current = p->Token().TokenType().Type().iUid;
				if (previous != current)
					{
					j++;
					previous = current;
					}
				TInt32 l1=iExpectedOrderingResult[j];
				if (current != iExpectedOrderingResult[j])
					{
					ret=KErrNotFound;
					break;
					}
				}
			if (count<iExpectedOrderingResult.Count())
				{
				ret=KErrNotFound;
				}
			iState = EFinished;
			TRequestStatus* status = &aStatus;		
			User::RequestComplete(status, ret);
			}
			break;
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			if (aStatus == KErrNoMemory)
				{	
				iState = ENew;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CInitialiseCertStore::PerformCancel()
	{
	switch (iState)
		{
		case EAppend:
			ASSERT(iNewUnifiedCertStore);
			iNewUnifiedCertStore->CancelInitialize();
			break;

		default:
			// do nothing
			break;
		}
	}

void CInitialiseCertStore::Reset()
	{
	iState = ENew;
	delete iNewUnifiedCertStore;
	iNewUnifiedCertStore = 0;
	}

void CInitialiseCertStore::DoReportAction()
	{
	iOut.writeString(_L("Initializing certstore manager..."));
	iOut.writeNewLine();
	}

void CInitialiseCertStore::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tstore manager initialised successfully\n"));
			iOut.writeString(_L("\tstore manager initialised successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else if (aError == KErrInUse)
			{
			iConsole.Write(_L("\tstore manager initialised error : KErrInUse\n"));
			iOut.writeString(_L("\tstore manager initialization error : KErrInUse"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

/////////////////////////////////////////////////////////////////
//COnlyCreateCertStore::
///////////////////////////////////////////////////////////////////

CTestAction* COnlyCreateCertStore::NewL(RFs& aFs,
										CConsoleBase& aConsole, 
										Output& aOut, 
										const TTestActionSpec& aTestActionSpec)
	{
	COnlyCreateCertStore* self = new(ELeave) COnlyCreateCertStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

COnlyCreateCertStore::COnlyCreateCertStore(RFs& aFs, 
										   CConsoleBase& aConsole, 
										   Output& aOut)
	:CCertStoreTestAction(aFs, aConsole, aOut), iState(EInit)
	{
	}

void COnlyCreateCertStore::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void COnlyCreateCertStore::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EInit:
			{
			__ASSERT_DEBUG(!iNewUnifiedCertStore, User::Panic(_L("CInitialiseCertStore"), 1));
			TRAPD(err, iNewUnifiedCertStore = CUnifiedCertStore::NewL(iFs, ETrue));
				if (err != KErrNone)
					{
					iState = EFinished;
					TRequestStatus* status = &aStatus;
					User::RequestComplete(status, err);
					}
				else
					{
					AddCertStoreL(iNewUnifiedCertStore);
					iNewUnifiedCertStore = 0; // we don't own this any more
					iState = EFinished;
					TRequestStatus* status = &aStatus;
					User::RequestComplete(status, err);
					}
			}
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			if (aStatus == KErrNoMemory)
				{	
				iState = EInit;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void COnlyCreateCertStore::PerformCancel()
{
}

void COnlyCreateCertStore::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("COnlyCreateCertStore::Reset()"), 1));
	}

void COnlyCreateCertStore::DoReportAction()
	{
	iOut.writeString(_L("Only creating certstore manager..."));
	iOut.writeNewLine();
	}

void COnlyCreateCertStore::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tstore manager created successfully\n"));
			iOut.writeString(_L("\tstore manager created successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else if (aError == KErrInUse)
			{
			iConsole.Write(_L("\tstore manager creation error : KErrInUse\n"));
			iOut.writeString(_L("\tstore manager creation error : KErrInUse"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////
// CDeleteCertStore
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CDeleteCertStore::NewL(RFs& aFs, 
									CConsoleBase& aConsole, 
									Output& aOut,
									const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDeleteCertStore::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CDeleteCertStore::NewLC(RFs& aFs, 
									 CConsoleBase& aConsole, 
									 Output& aOut,
									 const TTestActionSpec& aTestActionSpec)
	{
	CDeleteCertStore* self = new(ELeave) CDeleteCertStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDeleteCertStore::~CDeleteCertStore()
	{
	}

CDeleteCertStore::CDeleteCertStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CCertStoreTestAction(aFs, aConsole, aOut), iState(EDelete)
	{
	}

void CDeleteCertStore::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CDeleteCertStore::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EDelete:
			{
			TInt err = KErrNotFound;
			TInt count = CertStoreCount();
			if (count)
				{
				RemoveCertStore(count - 1);
				err = KErrNone;
				}
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
			}
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			if (aStatus == KErrNoMemory)
				{	
				iState = EDelete;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CDeleteCertStore::PerformCancel()
	{
	iState = EDelete;
	}

void CDeleteCertStore::Reset()
	{
	}

void CDeleteCertStore::DoReportAction()
	{
	iOut.writeString(_L("Deleting certstore manager..."));
	iOut.writeNewLine();
	}

void CDeleteCertStore::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tstore manager deleted successfully\n"));
			iOut.writeString(_L("\tstore manager deleted successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Write(_L("\tstore manager deleted failed\n"));
			iOut.writeString(_L("\tstore manager deleted failed"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

CSetAppsAndTrust::~CSetAppsAndTrust()
	{
	delete iFilter;
	iCertInfos.Close();
	}

void CSetAppsAndTrust::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EGetCAEntries:
			iState = ESetAppTrust;
			GetCerts(aStatus);
			break;

		case ESetAppTrust:
			{
            // Find the certificate we want to set the trust settings for
            // and edit its trust settings
            iState = EFinished;    
            TInt iEnd =  iCertInfos.Count();
            for (TInt i = 0; i < iEnd; i++)
                {
                if (iCertInfos[i]->Label() == iLabel)
                    {
 					if (iNotificationSubscribed)
 						{
 						if (!iNotifier)
 							{
 							iNotifier = CCertStoreChangeNotifier::NewL(iNotifierFlag);
 							iNotifier->StartNotification();
 							}
 						iState = ECheckNotification;
 						}
 					else
 						{
 						iState = EFinished;
 						}    
                    DoSetAppTrust(*iCertInfos[i], aStatus);
                    return;
                    }
                }
            TRequestStatus* status = &aStatus;
            User::RequestComplete(status, KErrNotFound);
            }
			break;

 		case ECheckNotification:
 			{
 			iState = EFinished;
 			if (iNotifierFlag)
	 			{
 				TRequestStatus* status = &aStatus;
 				User::RequestComplete(status, KErrNone);
	 			}
			else
				{
				iNotifier->SetCompleteStatus(&aStatus);
				}
 			}
 			break;
		case EFinished:
			{
			if (!iNotifierFlag && iNotificationSubscribed)
				{
				iResult = EFalse;
				}
			else
				{
				if (aStatus == iExpectedResult)
					{
					iResult = ETrue;
					}
				else
					{
					iResult = EFalse;
					}				
				}
            
            iFinished = ETrue;
            
            TRequestStatus* status = &aStatus;
            User::RequestComplete(status, aStatus.Int());
			}
			break;
			
		default:
			User::Invariant();
			break;
		}
	}

void CSetAppsAndTrust::PerformCancel()
	{
	switch (iState)
		{
		case ESetAppTrust:
			CertStore().CancelList();
			break;
		case ECheckNotification:
		case EFinished:
			DoPerformCancel();
			break;

		default:
			// do nothing
			break;
		}
	}

void CSetAppsAndTrust::Reset()
	{
    iState = EGetCAEntries;
	iCertInfos.Close();
	}

CSetAppsAndTrust::CSetAppsAndTrust(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CSubscriberAction(aFs, aConsole, aOut), iState(EGetCAEntries)
	
	{
	}

void CSetAppsAndTrust::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSubscriberAction::ConstructL(aTestActionSpec);
	iFilter = CCertAttributeFilter::NewL();

	// Let derived class do its own parsing
	}

void CSetAppsAndTrust::GetCerts(TRequestStatus& aStatus)
	{
	CertStore().List(iCertInfos, *iFilter, aStatus);
	}

void CSetAppsAndTrust::SetCertLabel(const TDesC8& aLabel)
	{
	iLabel.Copy(aLabel);
	}

CTestAction* CSetApplications::NewL(RFs& aFs, 
									CConsoleBase& aConsole,
									Output& aOut,
									const TTestActionSpec& aTestActionSpec)
	{
	CSetApplications* self = new(ELeave) CSetApplications(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CSetApplications::~CSetApplications()
	{
	iApplications.Close();
	}

CSetApplications::CSetApplications(RFs& aFs, CConsoleBase& aConsole, 
								   Output& aOut)
: CSetAppsAndTrust(aFs, aConsole, aOut)
	{
	}

void CSetApplications::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSetAppsAndTrust::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	SetCertLabel(Input::ParseElement(aTestActionSpec.iActionBody,
		KCertLabelStart, KCertLabelEnd, pos, err));
	AppendUid(Input::ParseElement(aTestActionSpec.iActionBody, KUIDStart, KUIDEnd, pos, err));
	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CSetApplications::AppendUid(const TDesC8& aUid)
	{
	TLex8 lex(aUid);

	TInt err = KErrNone;
	while (err == KErrNone)
		{
		TUid u ;
		err = lex.Val(u.iUid);
		if (err == KErrNone)
			{
			lex.SkipSpace();
			User::LeaveIfError(iApplications.Append(u));
			}
		}
	}

void CSetApplications::DoSetAppTrust(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus)
	{
	UnifiedCertStore().SetApplicability(aCertInfo, iApplications, aStatus);
	}

void CSetApplications::DoPerformCancel()
	{
	UnifiedCertStore().CancelSetApplicability();
	}

void CSetApplications::DoReportAction()
	{
	iOut.writeString(_L("Setting Application settings..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iLabel);
	iOut.writeNewLine();
	iOut.writeString(_L("\tApplications = "));
	TInt count = iApplications.Count();
	for (TInt i = 0; i < count; i++)
		{
		iOut.writeNum(iApplications[i].iUid);
		iOut.writeString(_L(" "));
		}
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CSetApplications::DoCheckResult(TInt /*aError*/)
	{
	if (iFinished)
		{
		if (iResult)
			{
			iConsole.Write(_L("\tapplications set successfully\n"));
			iOut.writeString(_L("\tapplications set successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Write(_L("\tapplications set failed\n"));
			iOut.writeString(_L("\tapplications set failed"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

CTestAction* CSetTrusters::NewL(RFs& aFs,
								CConsoleBase& aConsole,
								Output& aOut,
								const TTestActionSpec& aTestActionSpec)
	{
	CSetTrusters* self = new(ELeave) CSetTrusters(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CSetTrusters::~CSetTrusters()
	{
	}


CSetTrusters::CSetTrusters(RFs& aFs, CConsoleBase& aConsole, 
						   Output& aOut)
: CSetAppsAndTrust(aFs, aConsole, aOut)
	{
	}

void CSetTrusters::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSetAppsAndTrust::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	SetCertLabel(Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart, KCertLabelEnd, pos, err));
	SetTrusted(Input::ParseElement(aTestActionSpec.iActionBody, KTrustersStart, KTrustersEnd, pos, err));
	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CSetTrusters::SetTrusted(const TDesC8& aTrusted)
	{
	TLex8 lex(aTrusted);

	TInt err = KErrNone;
	while (err == KErrNone)
		{
		TInt val;
		err = lex.Val(val);
		if (err == KErrNone)
			{
			iTrusted = static_cast<TBool>(val);
			}
		}
	}

void CSetTrusters::DoSetAppTrust(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus)
	{
	UnifiedCertStore().SetTrust(aCertInfo, iTrusted, aStatus);
	}

void CSetTrusters::DoPerformCancel()
	{
	UnifiedCertStore().CancelSetTrust();
	}

void CSetTrusters::DoReportAction()
	{
	iOut.writeString(_L("Setting trust..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iLabel);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CSetTrusters::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (iResult && aError == KErrNone)
			{
			iConsole.Write(_L("\ttrust set successfully\n"));
			iOut.writeString(_L("\ttrust set successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Write(_L("\ttrust set failed\n"));
			iOut.writeString(_L("\ttrust set failed"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

CTestAction* CGetTrusters::NewL(RFs& aFs,
								CConsoleBase& aConsole,
								Output& aOut,
								const TTestActionSpec& aTestActionSpec)
	{
	CGetTrusters* self = new(ELeave) CGetTrusters(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CGetTrusters::~CGetTrusters()
	{
	iCertInfos.Close();
	iTrusters.Close();
	iExpectedTrusters.Close();
	delete iFilter;
	}

void CGetTrusters::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EGetCAEntries:
			iState = EGetTrusters;
			GetCerts(aStatus);
			break;

		case EGetTrusters:
			{
            TInt end =  iCertInfos.Count();
            TBool calledCertStore = EFalse;
            for (TInt i = 0; i < end; i++)
                {
                if (iCertInfos[i]->Label() == iLabel)
                    {
                    CertStore().Trusted(*iCertInfos[i], iTrust, aStatus);
                    calledCertStore = ETrue;
                    break;
                    }
                }
            iState = EFinished;
            if (EFalse==calledCertStore)
				{//	None with the appropriate label
                TRequestStatus* status = &aStatus;
                User::RequestComplete(status, KErrNotFound);
				}
			}
			break;
            
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
            if (aStatus == iExpectedResult)
                {

                if (iTrust == iExpectedTrusters[0].iUid)
                    {
                    iResult = ETrue;
                    }
                else
                    {
                    iResult = EFalse;
                    }
                }
            else
                {
                iResult = EFalse;
                }

            iFinished = ETrue;				
			}
			break;
            
		default:
			break;
		}
	}

void CGetTrusters::PerformCancel()
	{
	switch (iState)
		{
		case EGetTrusters:
			CertStore().CancelList();
			break;

		case EFinished:
			CertStore().CancelTrusted();
			break;

		default:
			break;
		}
	}

void CGetTrusters::Reset()
	{
    iCertInfos.Close();
    iState = EGetCAEntries;
	}

CGetTrusters::CGetTrusters(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CCertStoreTestAction(aFs, aConsole, aOut), iState(EGetCAEntries)
	{
	}

void CGetTrusters::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	iFilter = CCertAttributeFilter::NewL();
	TInt err = KErrNone;
	TInt pos = 0;
	SetCertLabel(Input::ParseElement(aTestActionSpec.iActionBody,
		KCertLabelStart, KCertLabelEnd, pos, err));
	
	// Set the expected result
	pos = 0;
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	
	SetExpectedTrusters(Input::ParseElement(aTestActionSpec.iActionResult, KTrustersStart,
		KTrustersEnd, pos, err));
		
	}

void CGetTrusters::GetCerts(TRequestStatus& aStatus)
	{
	CertStore().List(iCertInfos, *iFilter, aStatus);
	}

void CGetTrusters::SetCertLabel(const TDesC8& aLabel)
	{
	iLabel.Copy(aLabel);
	}

void CGetTrusters::SetExpectedTrusters(const TDesC8& aExpectedTrusters)
	{
	TLex8 lex(aExpectedTrusters);
	TInt err = KErrNone;
	while (err == KErrNone)
		{
		TUid uid;
		err = lex.Val(uid.iUid);
		if (err == KErrNone)
			{
			lex.SkipSpace();
			User::LeaveIfError(iExpectedTrusters.Append(uid));
			}
		}
	}

void CGetTrusters::DoReportAction()
	{
	iOut.writeString(_L("Getting trust settings..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iLabel);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CGetTrusters::DoCheckResult(TInt /*aError*/)
	{
	if (iFinished)
		{
		iConsole.Printf(_L("\ttrust Setting : "));
		iOut.writeString(_L("\tTrust Setting: "));
		iOut.writeString(_L("\t\t"));
		iConsole.Printf(_L("%D \n"), iTrust);
		iOut.writeNum(iTrust);
		iOut.writeString(_L(" "));
		iOut.writeNewLine();
		iOut.writeNewLine();

		iConsole.Printf(_L("\texpected Trust Setting: "));
		iOut.writeString(_L("\tExpected Trust Setting: "));
		iOut.writeNewLine();
		iOut.writeString(_L("\t\t"));
		iConsole.Printf(_L("%D \n"), iExpectedTrusters[0].iUid);
		iOut.writeNum(iExpectedTrusters[0].iUid);
		iOut.writeString(_L(" "));
		iOut.writeNewLine();
		iOut.writeNewLine();
		if (iResult)
			{
			iConsole.Printf(_L("\tTrust retrieved successfully\n"));
			iOut.writeString(_L("\tTrust retrieved successfully"));
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Printf(_L("\tTrust retrieved failed\n"));
			iOut.writeString(_L("\tTrust retrieved failed"));
			iOut.writeNewLine();
			}
		iOut.writeNewLine();

		}
	}
