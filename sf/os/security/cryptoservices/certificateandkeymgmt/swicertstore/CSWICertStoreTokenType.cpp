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


#include "CSWICertStoreTokenType.h"
#include "CSWICertStoreToken.h"
#include <swicertstore.h>
#include <ct/tcttokenhandle.h>

// KSWICertStoreName is used for both the token type label and the token name
_LIT(KSWICertStoreName, "SWI cert store"); 

// CSWICertStoreTokenType implementation

CCTTokenType* CSWICertStoreTokenType::NewL()
	{
	return new (ELeave) CSWICertStoreTokenType();
	}

CCTTokenType* CSWICertStoreTokenType::NewL(RFs& aFs)
	{
	CSWICertStoreTokenType* self = new (ELeave) CSWICertStoreTokenType();
	CleanupStack::PushL(self);
	TUid tokenTypeUid = { KSWICertStoreTokenTypeUid };
	self->ConstructL(tokenTypeUid, KSWICertStoreName, aFs);
	CleanupStack::Pop(self);
	return self;
	}

CSWICertStoreTokenType::~CSWICertStoreTokenType()
	{
	}

void CSWICertStoreTokenType::List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus)
	{
	TInt error = KErrNone;
	HBufC* name = KSWICertStoreName().Alloc();
	if (name)
		{
		error = aTokens.Append(name);
		}
	else
		{
		error = KErrNoMemory;
		}
	if (error != KErrNone)
		{
		delete name;
		}

	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, error);
	}

void CSWICertStoreTokenType::CancelList()
	{
	}

void CSWICertStoreTokenType::OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	MCTToken* token = 0;
	TInt error = KErrNone;
	
	if (aTokenInfo != KSWICertStoreName)
		{
		error = KErrNotFound;
		}
	else
		{
		token = new CSWICertStoreToken(*this);
		if (!token)
			{
			error = KErrNoMemory;
			}
		}
	
	if (error == KErrNone)
		{
		IncReferenceCount();
		aToken = token;
		}

	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, error);
	}

void CSWICertStoreTokenType::OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	if ((aHandle.iTokenTypeUid.iUid != KSWICertStoreTokenTypeUid) ||
		(aHandle.iTokenId != ESWICertStore))
		{
		TRequestStatus* r = &aStatus;
		User::RequestComplete(r, KErrBadHandle);
		}
	else
		{
		OpenToken(KSWICertStoreName, aToken, aStatus);
		}
	}

void CSWICertStoreTokenType::CancelOpenToken()
	{
	}
