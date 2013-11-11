/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "thwtokentypeclient.h"
#include "thwtokenclient.h"

_LIT(KTokenDesc, "Default Token");
	
CCTTokenType* CHardwareTokenTypeClient::NewL()
	{
	//	Destroyed by call to CCTTokenType::Release (refcounted)
	CHardwareTokenTypeClient* self = new (ELeave) CHardwareTokenTypeClient();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

CHardwareTokenTypeClient::CHardwareTokenTypeClient()
	{
	}

CHardwareTokenTypeClient::~CHardwareTokenTypeClient()
	{
	}

void CHardwareTokenTypeClient::ConstructL()
	{
	}
	
void CHardwareTokenTypeClient::List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus)
	{
	HBufC* name = NULL;
	TRAPD(err, name = KTokenDesc().AllocL());
	
	if(KErrNone == err)
		{
		err = aTokens.Append(name);
		}

	if(KErrNone != err)
		{
		delete name;
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CHardwareTokenTypeClient::CancelList()
	{
	// implementation not asynchronous
	}

void CHardwareTokenTypeClient::OpenToken(const TDesC& /*aTokenInfo*/, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	// Token Information not used. TokenClient will be created for all tokeninfo
	aToken = NULL;
	TRAPD(err, aToken = CHardwareTokenClient::NewL(this));

	//	Token opened, increment our reference count
	if(err == KErrNone)
		{
		IncReferenceCount();
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CHardwareTokenTypeClient::OpenToken(TCTTokenHandle /*aHandle*/, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	aToken = NULL;
	TRAPD(err, aToken = CHardwareTokenClient::NewL(this));

	//	Token opened, increment our reference count
	if(NULL != aToken)
		{
		IncReferenceCount();
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CHardwareTokenTypeClient::CancelOpenToken()
	{
	// implementation not asynchronous
	}
