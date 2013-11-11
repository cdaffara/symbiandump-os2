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


#include "keytool_engine.h"
#include "keytool_controller.h"
#include "keytool_commands.h"

#include <keytool.rsg>


/*static*/ CKeyToolEngine* CKeyToolEngine::NewLC(CKeyToolController* aController)
	{
	CKeyToolEngine* self = new (ELeave) CKeyToolEngine(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/* static */ CKeyToolEngine* CKeyToolEngine::NewL(CKeyToolController* aController)
	{
	CKeyToolEngine* self = CKeyToolEngine::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}
	
CKeyToolEngine::CKeyToolEngine(CKeyToolController* aController) : CActive(EPriorityNormal)
	{
	iCurrentAction = EIdle;
	iController = aController;
	iInitialized = EFalse;
	}
	
CKeyToolEngine::~CKeyToolEngine()
	{
	Cancel();	
	delete iHandler;	
	delete iKeyStore;		
	iFs.Close();
	if (iActiveStarted)
		{
		delete iScheduler;
		}	
	iInitialized = EFalse;
#ifdef KEYTOOL
	delete iMigrateStoreHandler;
#endif // KEYTOOL
	}
	
void CKeyToolEngine::ConstructL()
	{
	iActiveStarted = EFalse;
	iScheduler = CActiveScheduler::Current();
	if (!iScheduler)
		{
		iActiveStarted =  ETrue;
		iScheduler = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);	
		}
	
	User::LeaveIfError(iFs.Connect());
	
	iKeyStore = CUnifiedKeyStore::NewL(iFs);
	
	CActiveScheduler::Add(this);	
	}

void CKeyToolEngine::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}
		
	switch (iState)
		{
		case EInitialise:
			{
			iInitialized = ETrue;
			iHandler->DoCommandL(*iKeyStore, iParam);		
			iState = EDone;
			}
			break;
		case EDone:
			{
			}
			break;
		default:
			{
			User::Panic(_L("Keytool Engine - Illegal state"), 0);
			}
		}
	}
	
TInt CKeyToolEngine::RunError(TInt aError)
	{	
	CActiveScheduler::Stop();	
	
	switch (iCurrentAction)
		{
		case EList:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_LIST, aError));				
			}
			break;
		case EImport:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_IMPORT, aError));				
			}
			break;
		default:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_UNKNOWN, aError));				
			}
		}	
	return KErrNone;
	}

	
void CKeyToolEngine::DoCancel()
	{
	CActiveScheduler::Stop();
	}


//\\//\\//\\//\\////\\//\\//\\//\\////\\//\\//\\//\\//
//\\//\\//\\//\\// Business methods //\\//\\//\\//\\//
//\\//\\//\\//\\////\\//\\//\\//\\////\\//\\//\\//\\//
	
void CKeyToolEngine::InitializeL()
	{
	if (!iInitialized)
		{
		iState = EInitialise;
		iKeyStore->Initialize(iStatus);	
		SetActive();		
		}
	else 
		{
		iHandler->DoCommandL(*iKeyStore, iParam);		
		iState = EDone;		
		}
	}

	
void CKeyToolEngine::ListL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	iParam = aParam;
	iCurrentAction = EList;
	delete iHandler;
	iHandler = NULL;
	iHandler = CKeytoolList::NewL(iController);	
	InitializeL();
	}
	
void CKeyToolEngine::ImportL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	
	if (!aParam->iDefault)
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_KEYFILE);				
		User::Leave(KErrArgument);
		}	

	iParam = aParam;
	iCurrentAction = EImport;
	delete iHandler; // Reentrant call
	iHandler = NULL;
	iHandler = CKeytoolImport::NewL(iController);	
	InitializeL();
	}

void CKeyToolEngine::RemoveL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	
	if (!aParam->iDefault)
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_REMOVE);				
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = ERemove;
	iHandler = CKeytoolRemove::NewL(iController);	
	InitializeL();
	}

	
void CKeyToolEngine::DisplayUsageL(CKeyToolParameters* aParam)
	{
	iHandler = CKeytoolUsage::NewL(iController);	
	iHandler->DoCommandL(*iKeyStore, aParam);
	}

void CKeyToolEngine::ListStoresL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	iParam = aParam;
	iCurrentAction = EList;
	iHandler = CKeyToolListStores::NewL(iController);	
	InitializeL();
	}

void CKeyToolEngine::SetPolicyL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	
	if (!aParam->iDefault)
		{
		iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SUSERFAIL);				
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = ESetPolicy;
	iHandler = CKeytoolSetPolicy::NewL(iController);	
	InitializeL();
	}

#ifdef KEYTOOL

void CKeyToolEngine::MigrateStoreL(CKeyToolParameters* aParams)
	{
	iParam = aParams;
	iMigrateStoreHandler = CKeytoolMigrateStore::NewL(aParams);	
	iMigrateStoreHandler->DoCommandL();
	}

#endif // KEYTOOL
