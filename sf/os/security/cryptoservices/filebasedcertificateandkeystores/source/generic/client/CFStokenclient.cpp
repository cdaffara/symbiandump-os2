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


#include "CFStokenclient.h"
#include "clientutils.h"
#include "clientfactories.h"

// Information strings returned by MCTToken::Information()
_LIT(KVersion, "1.00");
_LIT(KSerialNo, "0");
_LIT(KManufacturer, "Nokia Corporation and/or its subsidiary(-ies).");

MCTToken* CFSTokenClient::NewL(ETokenEnum aTokenTypeVal, MCTTokenType* aTokenType, RFileStoreClientSession& aClient)
{
	__ASSERT_DEBUG(aTokenType, FSTokenPanic(EBadArgument));

//	Destroyed by MCTToken::Release() (refcounted)
	CFSTokenClient* me = new (ELeave) CFSTokenClient(aTokenTypeVal, aTokenType, aClient);
	return (static_cast<MCTToken*>(me));
}

CFSTokenClient::CFSTokenClient(ETokenEnum aTokenTypeVal, MCTTokenType* aTokenType, RFileStoreClientSession& aClient)
:	iTokenEnum(aTokenTypeVal),
	iTokenType(aTokenType),
	iRefCount(0),
	iClient(aClient)
{
    ASSERT(iTokenEnum < ETotalTokensSupported);
}

MCTTokenType& CFSTokenClient::TokenType()
{
	__ASSERT_DEBUG(iTokenType, FSTokenPanic(ENotInitialised));
	return (*iTokenType);
}

const TDesC& CFSTokenClient::Label()
{
    RSupportedTokensArray supportedTokens;
	const TDesC* token = supportedTokens[iTokenEnum];
	return (*token);
}

TCTTokenHandle CFSTokenClient::Handle()
{
	__ASSERT_DEBUG(iTokenType, FSTokenPanic(ENotInitialised));
	return (TCTTokenHandle(iTokenType->Type(), iTokenEnum));
}

TInt& CFSTokenClient::ReferenceCount()
{
	return (iRefCount);
}

void CFSTokenClient::DoGetInterface(TUid aRequiredInterface, MCTTokenInterface*& aReturnedInterface, TRequestStatus& aStatus)
	{
	// No longer calls server to get the interface - just creates a client object of the appropriate type
	
	aReturnedInterface = NULL;
	TRAPD(result, aReturnedInterface = CClientInterfaceFactory::ClientInterfaceL(aRequiredInterface.iUid, *this, iClient));
	if (result != KErrNone)
		{
		Release();
		}
	
	//	Complete the TRequestStatus here since not asynchronous
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, result);		
	}

TBool CFSTokenClient::DoCancelGetInterface()
{//	Not an asynchronous call for current file store, so nothing to do
	return (EFalse);
}

const TDesC& CFSTokenClient::Information(TTokenInformation aRequiredInformation)
	{
	switch (aRequiredInformation)
		{
		case EVersion:
			return KVersion;

		case ESerialNo:
			return KSerialNo;

		case EManufacturer:
			return KManufacturer;
		
		default:
			FSTokenPanic(EBadArgument);
		}
	
	return KNullDesC;
	}
