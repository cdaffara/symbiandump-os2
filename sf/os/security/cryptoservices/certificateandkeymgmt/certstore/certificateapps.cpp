/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CFSTokenTypeClient.h"
#include <certificateapps.h>
#include <ct/mcttoken.h>
#include <mctcertapps.h>
#include "mctcertappinterface.h"


////////////////////////////////////////////////////////////////
//	CCertificateAppInfoManager
////////////////////////////////////////////////////////////////

EXPORT_C CCertificateAppInfoManager* CCertificateAppInfoManager::NewLC()
	{
	CCertificateAppInfoManager* self = new(ELeave) CCertificateAppInfoManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CCertificateAppInfoManager* CCertificateAppInfoManager::NewL()
	{
	CCertificateAppInfoManager* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// deprecated
EXPORT_C CCertificateAppInfoManager* CCertificateAppInfoManager::NewLC(RFs& /*aFs*/,
																	   TBool /*aOpenedForWrite*/)
	{
	return NewLC();
	}

// deprecated
EXPORT_C CCertificateAppInfoManager* CCertificateAppInfoManager::NewL(RFs& /*aFs*/,
																	  TBool /*aOpenedForWrite*/)
	{
	return NewL();
	}

EXPORT_C CCertificateAppInfoManager::~CCertificateAppInfoManager()
	{
	iClients.Close();
	if (iCertAppsIf)
		{
		iCertAppsIf->Release();
		}
	}

CCertificateAppInfoManager::CCertificateAppInfoManager()
	{
	}

void CCertificateAppInfoManager::ConstructL()
	{
	// This method is the second phase of the construction process.
	//
	// It will open the cert apps token type, get the token, and 
	// then get the token interface.
	//
	// This class is not an active object but we can safely
	// wait for requests to complete because the filetokens
	// server completes requests immediately
	MCTTokenType* tokenType = CFSTokenTypeClient::NewL(TUid::Uid(KTokenTypeCertApps));
	CleanupReleasePushL(*tokenType);

	// Now extract all the tokens this token type contains
	RCPointerArray<HBufC> tokenArray;
	CleanupClosePushL(tokenArray);

	TRequestStatus stat;
	tokenType->List(tokenArray, stat);
	User::WaitForRequest(stat);

	// make sure we have at least one token, otherwise leave
	User::LeaveIfError(stat.Int());
	__ASSERT_DEBUG(tokenArray.Count(), User::Panic(_L("CCertificateAppInfoManager"), 1));

	MCTToken* token = NULL;

	// We assume the 1st token is the one we want
	tokenType->OpenToken(*tokenArray[0], token, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	CleanupReleasePushL(*token);

	// Now try and get the appropriate token interface
	MCTTokenInterface* tokenIf = NULL;
	token->GetInterface(TUid::Uid(KInterfaceCertApps), tokenIf, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	__ASSERT_DEBUG(tokenIf, User::Panic(_L("CCertificateAppInfoManager"), 1));

	// now upcast to a certapps interface. This should be a fairly safe cast
	// since we specifically requested for this interface
	iCertAppsIf = static_cast<MCTCertApps*>(tokenIf);

	// Now we can release the token and the token type and destroy the token
	// array
	token->Release();
	tokenType->Release();
	tokenArray.Close();

	// Pop the stuff from the cleanup stack - could have done a
	// PopAndDestroy instead of Release()/Close() but thought I'd be 
	// more explicit
	CleanupStack::Pop(3);

	// Populate the applications array
	iCertAppsIf->ApplicationsL(iClients);
	}

EXPORT_C void CCertificateAppInfoManager::AddL(const TCertificateAppInfo& aClient)
	{
	// We have to update our cached applications array, but must keep this in
	// sync with the server in the face of leaves and OOM
	User::LeaveIfError(iClients.Append(aClient));
	TRAPD(err, iCertAppsIf->AddL(aClient));
	if (err != KErrNone)
		{
		iClients.Remove(iClients.Count() - 1);
		User::Leave(err);
		}
	}

EXPORT_C void CCertificateAppInfoManager::RemoveL(const TUid& aUid)
	{
	// We have to update our cached applications array, but must keep this in
	// sync with the server in the face of leaves and OOM
	iCertAppsIf->RemoveL(aUid);
	
	// Count backwards so we don't have to worry about the size changing
	for (TInt i = iClients.Count() - 1 ; i >= 0 ; --i)
		{
		if (iClients[i].Id() == aUid)
			{
			iClients.Remove(i);
			}
		}
	}

EXPORT_C const TCertificateAppInfo& CCertificateAppInfoManager::ApplicationL(const TUid& aUid, TInt& aIndex) const
	{
	aIndex = KErrNotFound;
	
	for (TInt i = 0 ; i < iClients.Count() ; ++i)
		{
		if (iClients[i].Id() == aUid)
			{
			aIndex = i;
			break;
			}
		}
	
	User::LeaveIfError(aIndex);
	return iClients[aIndex];
	}

EXPORT_C const RArray<TCertificateAppInfo>& CCertificateAppInfoManager::Applications() const
	{
	return iClients;
	}
