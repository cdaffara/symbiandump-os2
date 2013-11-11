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


#include "CCreateKey.h"
#include "tokenserverdebug.h"
#include <bigint.h>
#include <securityerr.h>

//	Wrapper class because we differ from crypto on what a DH key is
class CSimpleDHKey : public CBase
	{
	public:
		static CSimpleDHKey* NewL(TInt aSize);
		~CSimpleDHKey();
	public:
		inline RInteger& DHKey() {return (iKey);};
	private:
		CSimpleDHKey() {};
		void ConstructL(TInt aSize);
	private:
		RInteger iKey;
	};

CSimpleDHKey* CSimpleDHKey::NewL(TInt aSize)
	{
	CSimpleDHKey* me = new (ELeave) CSimpleDHKey();
	CleanupStack::PushL(me);
	me->ConstructL(aSize);
	CleanupStack::Pop(me);
	return (me);
	}

void CSimpleDHKey::ConstructL(TInt aSize)
	{
	iKey = RInteger::NewRandomL(aSize - 1);							
	}

CSimpleDHKey::~CSimpleDHKey()
	{
	iKey.Close();
	}

CKeyCreator::CKeyCreator()
	: CActive(EPriorityStandard), 
	iAction(EIdle)
	{
	CActiveScheduler::Add(this);
	}

CKeyCreator::~CKeyCreator()
	{
	Cancel(); 

	iCreatorThread.LogonCancel(iStatus);
	iCreatorThread.Close();

	delete iCreateData;
	}

//	Spin a thread to create an appropriate key, if successful, left on CleanupStack
void CKeyCreator::DoCreateKeyAsync(CKeyInfo::EKeyAlgorithm aAlgorithm, TInt aSize, TRequestStatus& aStatus)
	{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	iStatus = KRequestPending;
		
	TInt err = KErrNone;
	
	if ( (aSize <= 0) ||
		 (aAlgorithm==CKeyInfo::EInvalidAlgorithm) || 
		 ((aAlgorithm!=CKeyInfo::ERSA) && (aAlgorithm!=CKeyInfo::EDSA) && (aAlgorithm!=CKeyInfo::EDH)) )
		{
		err = KErrKeyAlgorithm;
		}
	if(err == KErrNone)
		{
		iCreateData = new CKeyCreatorData(aAlgorithm, aSize);
		if(iCreateData == NULL)
			{
			err = KErrNoMemory;
			}
		}
	else
		{
		User::RequestComplete(iClientStatus, err);
		return;
		}
	
	//	OK, ready to start the async operation...do it in RunL
	iAction = EReadyToCreateKey;
	
	SetActive();
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, err);
	}	

//	HERE'S THE THREAD TO CREATE THE KEY
//	Code cannot leave in here, but not as many traps as it looks
/*static*/ TInt CKeyCreator::CreatorThreadEntryPoint(TAny* aParameters)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		User::Exit(KErrNoMemory);

#ifdef _DEBUG
	TokenServerDebug::PauseOOMTest();
#endif
	
	ASSERT(aParameters);
	TInt result = KErrNone;
	CKeyCreatorData* createData = static_cast<CKeyCreatorData*>(aParameters);
	switch (createData->iKeyAlgorithm)
		{
		case(CKeyInfo::ERSA):
			{//	Currently, CRT signing is not supported, in case the key is to be used
			//	for such, create a standard (private) key as part of the pair
				TRAP(result, createData->iKey.iRSAKey = CRSAKeyPair::NewL(createData->iSize));
			}
			break;
		case (CKeyInfo::EDSA):
			{
				TRAP(result, createData->iKey.iDSAKey = CDSAKeyPair::NewL(createData->iSize));
			}
			break;
		case (CKeyInfo::EDH):
			{//	Generate a number that's less than N. The snag is that
			//	we don't know what N is. We do know that it'll be of a
			//	particular size, so we can safely generate any number
			//	with less than iSize digits
				TRAP(result, createData->iKey.iDHKey = CSimpleDHKey::NewL(createData->iSize));				
			}
			break;
		default:
			ASSERT(EFalse);
			result = KErrArgument;
		}

	#ifdef _DEBUG
		TokenServerDebug::ResumeOOMTest();
	#endif
	delete cleanup;
	User::Exit(result);
	return (KErrNone);
	}

CRSAKeyPair* CKeyCreator::GetCreatedRSAKey()
	{
	//	Check algorithm is as expected, return NULL if no key or wrong type
	if ( (!iCreateData) || (CKeyInfo::ERSA!=iCreateData->iKeyAlgorithm) )
		return (NULL);
	else
		return (iCreateData->iKey.iRSAKey);
	}

CDSAKeyPair* CKeyCreator::GetCreatedDSAKey()
	{
	//	Check algorithm is as expected, return NULL if no key or wrong type
	if ( (!iCreateData) || (CKeyInfo::EDSA!=iCreateData->iKeyAlgorithm) )
		return (NULL);
	else
		return (iCreateData->iKey.iDSAKey);
	}

void CKeyCreator::GetCreatedDHKey(RInteger& aDHKey)
	{
	ASSERT(iCreateData);
	ASSERT(CKeyInfo::EDH==iCreateData->iKeyAlgorithm);		
	aDHKey = iCreateData->iKey.iDHKey->DHKey();
	}

void CKeyCreator::DoCancel()
	{
	//	Only do the cancel if in the middle of creating a key.  Kill the thread.
	if (iAction!=EIdle)
		{
		TExitType exitType = iCreatorThread.ExitType();
		if (EExitPending==exitType)	//	Still alive, so kill it
			{
			iCreatorThread.Kill(KErrCancel);
			}
		iAction = EIdle;
		}

	ASSERT(iClientStatus);
	User::RequestComplete(iClientStatus, KErrCancel);
	}

void CKeyCreator::RunL()
	{
	ASSERT(iClientStatus);
	User::LeaveIfError(iStatus.Int());
	
	switch (iAction)
		{
		case (EReadyToCreateKey):
			{	
			// Spin off the thread and pass it the parameter data, then stand by
			// INC118634
			// To be safe, we should use anonymous threads because naming a thread means anybody could have opened a handle on the thread,
			// most likely system applications which want to know about panicing threads. So next thread creation will fail with KErrAlreadyExist(-11).
			User::LeaveIfError(iCreatorThread.Create(KNullDesC, CreatorThreadEntryPoint, KDefaultStackSize, NULL, (TAny*)iCreateData));
			iStatus = KRequestPending;
			iCreatorThread.Logon(iStatus);
			iAction = ECreatedKey;
			SetActive();
			iCreatorThread.Resume();
			}
		break;
		
		case (ECreatedKey):
			{//	Notify the caller
			ASSERT(iClientStatus);
			// May be OOM creating logon, in which case we should kill thread
			if (iStatus.Int() == KErrNoMemory)
				{
				TExitType exitType = iCreatorThread.ExitType();
				if (EExitPending==exitType)	//	Still alive, so kill it
					iCreatorThread.Kill(KErrNone);
				}
			
			User::RequestComplete(iClientStatus, iStatus.Int());
			iAction = EIdle;
			}
		break;
		default:
			ASSERT(EFalse);
		}
	}

TInt CKeyCreator::RunError(TInt anError)
	{
	if (iClientStatus)
		User::RequestComplete(iClientStatus, anError);

	return (KErrNone);
	}



CKeyCreator::CKeyCreatorData::CKeyCreatorData(CKeyInfo::EKeyAlgorithm aAlgorithm, TInt aSize)
	:iSize(aSize), 
	iKeyAlgorithm(aAlgorithm)
	{}

CKeyCreator::CKeyCreatorData::~CKeyCreatorData()
	{
	if (iKeyAlgorithm==CKeyInfo::ERSA)
		delete iKey.iRSAKey;
	else if (iKeyAlgorithm==CKeyInfo::EDSA)
		delete iKey.iDSAKey;
	else if (iKeyAlgorithm==CKeyInfo::EDH)
		delete iKey.iDHKey;
	}

