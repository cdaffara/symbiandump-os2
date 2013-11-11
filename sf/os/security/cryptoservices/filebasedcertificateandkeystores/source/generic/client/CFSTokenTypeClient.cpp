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


#include "CFSTokenTypeClient.h"
#include "clientutils.h"
#include "CFSTokenTypeSession.h"
#include <ct.h>

EXPORT_C CCTTokenType* CFSTokenTypeClient::NewL(TUid aUid)
	{
	//	Destroyed by call to CCTTokenType::Release (refcounted)
	CFSTokenTypeClient* me = new (ELeave) CFSTokenTypeClient();
	CleanupStack::PushL(me);
	me->ConstructL(aUid);
	CleanupStack::Pop(me);

	//	Caller only receives a valid CFSTokenTypeClient instance if the server has connected
	return (me);
	}

CFSTokenTypeClient::CFSTokenTypeClient()
	{
	}

CFSTokenTypeClient::~CFSTokenTypeClient()
	{
	delete iClientSession;
	}

void CFSTokenTypeClient::ConstructL(TUid aUid)
	{
	iClientSession = CFSTokenTypeSession::NewL(aUid);
	}

void CFSTokenTypeClient::List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iClientSession, FSTokenPanic(EClientSession));
	TRAPD(err, iClientSession->ListTokensL(aTokens));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSTokenTypeClient::CancelList()
	{
	// implementation not asynchronous
	}

void CFSTokenTypeClient::OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iClientSession, FSTokenPanic(EClientSession));

	TRAPD(err, iClientSession->OpenTokenL(aTokenInfo, this, aToken));
	if (err == KErrNone)
		{
		//	Token opened, increment our reference count
		IncReferenceCount();
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSTokenTypeClient::OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iClientSession, FSTokenPanic(EClientSession));

	TRAPD(err, iClientSession->OpenTokenL(aHandle, this, aToken));
	if (err == KErrNone)
		{
		//	Token opened, increment our reference count
		IncReferenceCount();
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSTokenTypeClient::CancelOpenToken()
	{
	// implementation not asynchronous
	}
