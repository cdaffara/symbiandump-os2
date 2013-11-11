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
* class MCTToken;
* class MCTTokenType;
*
*/


#include <ct/mcttoken.h>
//#include <ct/mcttokeninterface.h>
#include <mctcertstore.h>

#include "certtool_commands.h"
#include "certtool_controller.h"
#include <mctwritablecertstore.h>

/*static*/ CCertToolListStores* CCertToolListStores::NewLC(CCertToolController* aController)
	{
	CCertToolListStores* self = new (ELeave) CCertToolListStores(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
/*static*/ CCertToolListStores* CCertToolListStores::NewL(CCertToolController* aController)
	{
	CCertToolListStores* self = CCertToolListStores::NewLC(aController);
	CleanupStack::Pop(self);
	return self;	
	}
	
CCertToolListStores::CCertToolListStores(CCertToolController* aController) : CCertToolCommand(aController)
	{
	
	}
		
CCertToolListStores::~CCertToolListStores()
	{
	Cancel();
	}
	
void CCertToolListStores::ConstructL()
	{
	
	}

void CCertToolListStores::DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam)
	{
	TInt storeCount = aCertStore.CertStoreCount();
	TInt writableStoreCount = aCertStore.WritableCertStoreCount();
	TInt readableStoreCount = aCertStore.ReadOnlyCertStoreCount();

	TFileName store,wStore,rStore;
	store.Format(_L( "Total Number of Stores: %d"), storeCount);
	iController->DisplayErrorL(store, aParam->iPageWise);
	wStore.Format(_L( "Total Number of Writable Stores: %d"), writableStoreCount);
	iController->DisplayErrorL(wStore, aParam->iPageWise);
	rStore.Format(_L( "Total Number of Read only Stores: %d"), readableStoreCount);
	iController->DisplayErrorL(rStore, aParam->iPageWise);
	
	TInt i = 0;
	for (i = 0; i < writableStoreCount; i++)
		{
		MCTToken& token = (aCertStore.WritableCertStore(i)).Token();
		const TDesC& tmp = token.Label();
		TFileName msg;
		msg.Format(_L("[%d] Store Label: %S"), i, &tmp);	
		iController->DisplayErrorL(msg, aParam->iPageWise);
		const TDesC& tmp1 = token.Information(MCTToken::EVersion);
		const TDesC& tmp2 = token.Information(MCTToken::ESerialNo);
		const TDesC& tmp3 = token.Information(MCTToken::EManufacturer);

		msg.Format(_L("Version: %S \t Serial Number: %S \t Manufacturer: %S Store Type: ReadWrite"), &tmp1, &tmp2, &tmp3);	
		iController->DisplayErrorL(msg, aParam->iPageWise);
		}
	
	for (TInt j = 0; j < readableStoreCount; j++)
		{
		MCTToken& token = (aCertStore.ReadOnlyCertStore(j)).Token();
		TFileName msg;
		const TDesC& tmp = token.Label();
		msg.Format(_L("[%d] Store Label: %S"), i++, &tmp);	
		iController->DisplayErrorL(msg, aParam->iPageWise);
		const TDesC& tmp1 = token.Information(MCTToken::EVersion);
		const TDesC& tmp2 = token.Information(MCTToken::ESerialNo);
		const TDesC& tmp3 = token.Information(MCTToken::EManufacturer);

		msg.Format(_L("Version: %S \t Serial Number: %S \t Manufacturer: %S Store Type: Read only"), &tmp1, &tmp2, &tmp3);	
		iController->DisplayErrorL(msg, aParam->iPageWise);
		}
	CActiveScheduler::Stop();		
	}
	
void CCertToolListStores::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}	
	}
	
void CCertToolListStores::DoCancel()
	{
	
	}

