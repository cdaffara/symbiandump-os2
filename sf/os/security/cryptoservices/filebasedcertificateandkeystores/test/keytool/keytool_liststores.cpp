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



#include <ct/mcttoken.h>

#include "keytool_commands.h"
#include "keytool_controller.h"

/*static*/ CKeyToolListStores* CKeyToolListStores::NewLC(CKeyToolController* aController)
	{
	CKeyToolListStores* self = new (ELeave) CKeyToolListStores(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
/*static*/ CKeyToolListStores* CKeyToolListStores::NewL(CKeyToolController* aController)
	{
	CKeyToolListStores* self = CKeyToolListStores::NewLC(aController);
	CleanupStack::Pop(self);
	return self;	
	}
	
CKeyToolListStores::CKeyToolListStores(CKeyToolController* aController) : CKeyToolCommand(aController)
	{
	
	}
		
CKeyToolListStores::~CKeyToolListStores()
	{
	Cancel();
	}
	
void CKeyToolListStores::ConstructL()
	{
	
	}

void CKeyToolListStores::DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam)
	{
	TInt storeCount = aKeyStore.KeyStoreCount();
	for (TInt j = 0; j < storeCount; j++)
		{
		MCTToken& token = (aKeyStore.KeyStore(j)).Token();
		TBuf<128> msg;
		const TDesC& tmp = token.Label();
		msg.Format(_L("[%d] Store Label: %S"), j, &tmp);	
		iController->DisplayErrorL(msg, aParam->iPageWise);
		const TDesC& tmp1 = token.Information(MCTToken::EVersion);
		const TDesC& tmp2 = token.Information(MCTToken::ESerialNo);
		const TDesC& tmp3 = token.Information(MCTToken::EManufacturer);

		msg.Format(_L("Version: %S \t Serial Number: %S \t Manufacturer: %S"), &tmp1, &tmp2, &tmp3);	
		iController->DisplayErrorL(msg, aParam->iPageWise);
		}
	CActiveScheduler::Stop();		
	}
	
void CKeyToolListStores::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}	
	}
	
void CKeyToolListStores::DoCancel()
	{
	
	}
