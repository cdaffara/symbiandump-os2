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


#include "thwtokenclient.h"
#include "thwkeystoreclient.h"

_LIT(KTokenString, "Hardware plugin implementation");


MCTToken* CHardwareTokenClient::NewL(MCTTokenType* aTokenType)
	{
	CHardwareTokenClient* self = new (ELeave) CHardwareTokenClient(aTokenType);
	return static_cast<MCTToken*>(self);
	}

CHardwareTokenClient::CHardwareTokenClient(MCTTokenType* aTokenType)
			:	iTokenType(aTokenType),
				iRefCount(0)
	{
	}

MCTTokenType& CHardwareTokenClient::TokenType()
	{
	return *iTokenType;
	}

const TDesC& CHardwareTokenClient::Label()
	{
	return KTokenString();
	}

TCTTokenHandle CHardwareTokenClient::Handle()
	{
	return (TCTTokenHandle(iTokenType->Type(), 0));
	}

TInt& CHardwareTokenClient::ReferenceCount()
	{
	return iRefCount;
	}

void CHardwareTokenClient::DoGetInterface(TUid aRequiredInterface, MCTTokenInterface*& aReturnedInterface, TRequestStatus& aStatus)
	{
	// No longer calls server to get the interface - just creates a client object of the appropriate type
	TUid userUid = {0x101F7334};
	TUid managerUid = {0x101F7335};

	aReturnedInterface = NULL;
	TInt err = KErrGeneral;
	
	if( userUid == aRequiredInterface || managerUid == aRequiredInterface )
		{
		TRAP(err, aReturnedInterface = CHardwareKeyStoreClient::NewKeyStoreInterfaceL(*this));
		}
						
	if (NULL == aReturnedInterface)
		{
		Release();
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);		
	}

TBool CHardwareTokenClient::DoCancelGetInterface()
	{//	Not an asynchronous call for current file store, so nothing to do
	return EFalse;
	}

const TDesC& CHardwareTokenClient::Information(TTokenInformation /*aRequiredInformation*/)
	{
	return KNullDesC;
	}
