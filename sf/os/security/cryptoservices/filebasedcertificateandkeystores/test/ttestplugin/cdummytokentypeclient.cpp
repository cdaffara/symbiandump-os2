/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cdummytokentypeclient.h"
#include "cdummytokenclient.h"

_LIT(KTokenDesc, "Default Token");
	
CCTTokenType* CDummyTokenTypeClient::NewL()
	{
	//	Destroyed by call to CCTTokenType::Release (refcounted)
	CDummyTokenTypeClient* self = new (ELeave) CDummyTokenTypeClient();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	//	Caller only receives a valid CDummyTokenTypeClient instance if the server has connected
	return self;
	}

CDummyTokenTypeClient::CDummyTokenTypeClient()
	{
	}

CDummyTokenTypeClient::~CDummyTokenTypeClient()
	{
	}

void CDummyTokenTypeClient::ConstructL()
	{
	}
	
void CDummyTokenTypeClient::List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus)
	{
	HBufC* name = NULL;
	TRAPD(err, name = KTokenDesc().AllocL());
	
	if(KErrNone == err)
		{
		err = aTokens.Append(name);
		
		if(KErrNone != err)
			{
			delete name;
			}
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CDummyTokenTypeClient::CancelList()
	{
	// implementation not asynchronous
	}

void CDummyTokenTypeClient::OpenToken(const TDesC& /*aTokenInfo*/, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	// Token Information not used. TokenClient will be created for all tokeninfo
	aToken = NULL;
	TRAPD(err, aToken = CDummyTokenClient::NewL(this));

	//	Token opened, increment our reference count
	if(NULL != aToken)
		{
		IncReferenceCount();
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CDummyTokenTypeClient::OpenToken(TCTTokenHandle /*aHandle*/, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	aToken = NULL;
	TRAPD(err, aToken = CDummyTokenClient::NewL(this));

	//	Token opened, increment our reference count
	if(NULL != aToken)
		{
		IncReferenceCount();
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CDummyTokenTypeClient::CancelOpenToken()
	{
	// implementation not asynchronous
	}
