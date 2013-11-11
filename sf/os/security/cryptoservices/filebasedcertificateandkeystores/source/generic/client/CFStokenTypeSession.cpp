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


#include "CFSTokenTypeSession.h"
#include "clientutils.h"
#include "CFStokenclient.h"
#include <ct.h>
#include "fstokencliserv.h"
#include "tokentypesenum.h"

CFSTokenTypeSession* CFSTokenTypeSession::NewL(TUid aUid)
	{
	// Destroyed by owner object (~CFSTokenTypeClient)
	CFSTokenTypeSession* me = new (ELeave) CFSTokenTypeSession(aUid);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);

	return (me);
	}

CFSTokenTypeSession::CFSTokenTypeSession(TUid aUid) :
	iUid(aUid)
	{
	}	

CFSTokenTypeSession::~CFSTokenTypeSession()
	{
	iClient.Close();
	}

void CFSTokenTypeSession::ConstructL()
	{
	iTokenId = (ETokenEnum) KErrNotFound;

	RTokenTypeUIDLookup tokenUids;
	for (TInt i = 0 ; i < ETotalTokensSupported ; ++i)
		{
		if (tokenUids[i] == iUid.iUid)
			{
			iTokenId = (ETokenEnum) i;
			break;
			}
		}

	User::LeaveIfError((TInt) iTokenId);
	User::LeaveIfError(iClient.Connect(iTokenId));
	}

void CFSTokenTypeSession::ListTokensL(RCPointerArray<HBufC>& aTokens)
	{
	RSupportedTokensArray tokenNames;
	HBufC* name = tokenNames[iTokenId]->AllocLC();
	User::LeaveIfError(aTokens.Append(name));
	CleanupStack::Pop(name);
	}

void CFSTokenTypeSession::OpenTokenL(const TDesC& aTokenInfo, MCTTokenType* aTokenType, MCTToken*& aToken)
	{
	RSupportedTokensArray tokenNames;
	if (*tokenNames[iTokenId] != aTokenInfo)
		{
		User::Leave(KErrArgument);
		}

	aToken = CFSTokenClient::NewL(iTokenId, aTokenType, iClient);
	}

void CFSTokenTypeSession::OpenTokenL(TCTTokenHandle aHandle, MCTTokenType* aTokenType, MCTToken*& aToken)
	{		
	if (aHandle.iTokenTypeUid != iUid ||
		aHandle.iTokenId != iTokenId)
		{
		User::Leave(KErrArgument);
		}

	aToken = CFSTokenClient::NewL(iTokenId, aTokenType, iClient);
	}
