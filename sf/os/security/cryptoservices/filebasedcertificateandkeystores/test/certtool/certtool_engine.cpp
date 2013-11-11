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


#include "certtool_engine.h"
#include "certtool_controller.h"
#include "keytool_controller.h"


/*static*/ CCertToolEngine* CCertToolEngine::NewLC(CCertToolController* aController)
	{
	CCertToolEngine* self = new (ELeave) CCertToolEngine(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CCertToolEngine* CCertToolEngine::NewL(CCertToolController* aController)
	{
	CCertToolEngine* self = CCertToolEngine::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}
	
CCertToolEngine::CCertToolEngine(CCertToolController* aController) : CActive(EPriorityNormal)
	{
	iCurrentAction = EIdle;
	iController = aController;
	}
	
CCertToolEngine::~CCertToolEngine()
	{
	Cancel();	
	delete iHandler;		
	delete iKeyHandler;
	delete iKeyController;

	delete iKeyStore;
	iFsKeyStore.Close();	
	delete iCertStore;		
	iFs.Close();			
	delete iScheduler;
	}
	
void CCertToolEngine::ConstructL()
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	User::LeaveIfError(iFs.Connect());
	
	iCertStore = CUnifiedCertStore::NewL(iFs, ETrue);

	User::LeaveIfError(iFsKeyStore.Connect());

	iKeyStore = CUnifiedKeyStore::NewL(iFsKeyStore);
	
	CActiveScheduler::Add(this);	
	}

void CCertToolEngine::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}
		
	switch (iState)
		{
		case EInitialiseKeyStore:
			{
			iKeyHandler->DoCommandL(*iKeyStore, iParam);		
			iState = EDone;
			}
			break;		
		case EInitialise:
			{
			iHandler->DoCommandL(*iCertStore, iParam);		
			iState = EDone;
			}
			break;
		case EDone:
			{
			}
			break;
		default:
			{
			User::Panic(_L("Certtool Engine - Illegal state"), 0);
			}
		}
	}
	
TInt CCertToolEngine::RunError(TInt aError)
	{	
	CActiveScheduler::Stop();	
	
	switch (iCurrentAction)
		{
		case EList:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_LIST, aError));			
			}
			break;
		case EImport:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_IMPORT, aError));						
			}
			break;
		default:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_UNKNOWN, aError));			
			}
		}	
	return KErrNone;
	}

	
void CCertToolEngine::DoCancel()
	{
	//CActiveScheduler::Stop();
	}


//\\//\\//\\//\\////\\//\\//\\//\\////\\//\\//\\//\\//
//\\//\\//\\//\\// Business methods //\\//\\//\\//\\//
//\\//\\//\\//\\////\\//\\//\\//\\////\\//\\//\\//\\//
	
void CCertToolEngine::ListL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	iParam = aParam;
	iCurrentAction = EList;
	delete iHandler;
	iHandler = NULL;
	iHandler = CCertToolList::NewL(iController);	
	iState = EInitialise;
	iCertStore->Initialize(iStatus);	
	SetActive();
	}
	
void CCertToolEngine::ListStoresL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	iParam = aParam;
	iCurrentAction = EList;
	iHandler = CCertToolListStores::NewL(iController);	
	iState = EInitialise;
	iCertStore->Initialize(iStatus);	
	SetActive();
	}
	
void CCertToolEngine::ImportPrivateL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	
	iKeyController = CKeyToolController::NewL(iController->GetView());
	if (!aParam->iDefault)
		{
		iKeyController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_KEYFILE);	
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = EImport;
	iKeyHandler = CKeytoolImport::NewL(iKeyController);	
	iState = EInitialiseKeyStore;
	iKeyStore->Initialize(iStatus);	
	SetActive();
	}

void CCertToolEngine::SetManagerPolicyL(CKeyToolParameters* aParam)
	{
	Cancel();
	delete iKeyController;
	iKeyController = NULL;
	iKeyController = CKeyToolController::NewL(iController->GetView());
	if (!aParam->iDefault)
		{
		iKeyController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_REMOVE);	
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = ESetPolicy;
	delete iKeyHandler;
	iKeyHandler = NULL;
	iKeyHandler = CKeytoolSetPolicy::NewL(iKeyController);	
	iState = EInitialiseKeyStore;
	delete iKeyStore;
	iKeyStore = NULL;
	iKeyStore = CUnifiedKeyStore::NewL(iFsKeyStore);
	iKeyStore->Initialize(iStatus);	
	SetActive();
	}
	
void CCertToolEngine::RemovePrivateL(CKeyToolParameters* aParam)
	{
	Cancel();
	
	iKeyController = CKeyToolController::NewL(iController->GetView());
	if (!aParam->iDefault)
		{
		iKeyController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_REMOVE);	
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = ERemove;
	iKeyHandler = CKeytoolRemove::NewL(iKeyController);	
	iState = EInitialiseKeyStore;
	User::LeaveIfError(iFsKeyStore.Connect());
	delete iKeyStore;
	iKeyStore = NULL;
	iKeyStore = CUnifiedKeyStore::NewL(iFsKeyStore);
	iKeyStore->Initialize(iStatus);	
	SetActive();
	}

void CCertToolEngine::ImportL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	delete iKeyHandler;
	iKeyHandler = NULL;
	delete iKeyController;
	iKeyController = NULL;
	
	if (!aParam->iDefault)
		{
		iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_CERTFILE);	
		User::Leave(KErrArgument);
		}	

	iParam = aParam;
	iCurrentAction = EImport;
	iHandler = CCertToolAdd::NewL(iController);	
	iState = EInitialise;
	iCertStore->Initialize(iStatus);	
	SetActive();
	}

void CCertToolEngine::RemoveL(CKeyToolParameters* aParam)
	{ 
	Cancel();
	delete iKeyHandler;
	iKeyHandler = NULL;
	delete iKeyController;
	iKeyController = NULL;	
	
	if (!aParam->iDefault)
		{
		iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_REMOVE);			
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = ERemove;
	iHandler = CCertToolRemove::NewL(iController);	
	iState = EInitialise;
	iCertStore->Initialize(iStatus);	
	SetActive();
	}
	
void CCertToolEngine::SetupUidCommandL(CKeyToolParameters* aParam)
	{
	Cancel();
	if (!aParam->iDefault && !aParam->iLabel)
		{
		iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_NOCERT);			
		User::Leave(KErrArgument);
		}	

	if (aParam->iUIDs.Count()==0)
		{
		iController->DisplayLocalisedMsgL(R_CERTTOOL_USAGE_NOAPPS);
		User::Leave(KErrArgument);
		}

	iParam = aParam;
	iCurrentAction = EAddApps;
	iState = EInitialise;
	iCertStore->Initialize(iStatus);	
	SetActive();		
	}
	
void CCertToolEngine::SetAppsL(CKeyToolParameters* aParam)
	{
	iCurrentAction = ESetApps;	
	iHandler = CCertToolSetApps::NewL(iController);	
	SetupUidCommandL(aParam);
	}
	
	
void CCertToolEngine::RemoveAppsL(CKeyToolParameters* aParam)
	{
	iHandler = CCertToolRemoveApps::NewL(iController);	
	SetupUidCommandL(aParam);	
	}
	

void CCertToolEngine::AddAppsL(CKeyToolParameters* aParam)
	{
	iHandler = CCertToolAddApps::NewL(iController);	
	SetupUidCommandL(aParam);
	}
	

	
void CCertToolEngine::DisplayUsageL(CKeyToolParameters* aParam)
	{
	iHandler = CCertToolUsage::NewL(iController);	
	iHandler->DoCommandL(*iCertStore, aParam);
	}
