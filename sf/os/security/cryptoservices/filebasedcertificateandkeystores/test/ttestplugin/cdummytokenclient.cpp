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


#include "cdummytokenclient.h"
#include "cdummykeystoreclient.h"

_LIT(KTokenString, "Dummy Token");


MCTToken* CDummyTokenClient::NewL(MCTTokenType* aTokenType)
	{
	CDummyTokenClient* self = new (ELeave) CDummyTokenClient(aTokenType);
	return static_cast<MCTToken*>(self);
	}

CDummyTokenClient::CDummyTokenClient(MCTTokenType* aTokenType)
			:	iTokenType(aTokenType),
				iRefCount(0)
	{
	}

MCTTokenType& CDummyTokenClient::TokenType()
	{
	return *iTokenType;
	}

const TDesC& CDummyTokenClient::Label()
	{
	return KTokenString();
	}

TCTTokenHandle CDummyTokenClient::Handle()
	{
	return (TCTTokenHandle(iTokenType->Type(), 0));
	}

TInt& CDummyTokenClient::ReferenceCount()
	{
	return iRefCount;
	}

void CDummyTokenClient::DoGetInterface(TUid aRequiredInterface, MCTTokenInterface*& aReturnedInterface, TRequestStatus& aStatus)
	{
	// No longer calls server to get the interface - just creates a client object of the appropriate type
	TUid userUid = {0x101F7334};
	#ifndef KEYSTORE_INTERFACE
	TUid managerUid = {0x101F7335};
	#endif // KEYSTORE_INTERFACE

	//	Complete the TRequestStatus here since not asynchronous
	TRequestStatus* status = &aStatus;
	aReturnedInterface = NULL;
	TInt err = KErrGeneral;
	
	if(userUid == aRequiredInterface
	#ifndef KEYSTORE_INTERFACE
		|| managerUid == aRequiredInterface
	#endif //KEYSTORE_INTERFACE
	  )
		{
		TRAP(err, aReturnedInterface = CDummyKeyStoreClient::NewKeyStoreInterfaceL(*this));
		}
						
	if (NULL == aReturnedInterface)
		{
		Release();
		}
	
	User::RequestComplete(status, err);		
	}

TBool CDummyTokenClient::DoCancelGetInterface()
	{//	Not an asynchronous call for current file store, so nothing to do
	return EFalse;
	}

const TDesC& CDummyTokenClient::Information(TTokenInformation /*aRequiredInformation*/)
	{
	return KNullDesC;
	}
