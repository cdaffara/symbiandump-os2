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


#include "CSWICertStoreToken.h"
#include "CSWICertStoreTokenType.h"
#include <swicertstore.h>

_LIT(KSWICertStoreTokenLabel, "SWI cert store");
_LIT(KSWICertStoreVersion, "1.00");
_LIT(KSWICertStoreSerialNo, "N/A");
_LIT(KSWICertStoreManufacturer, "Symbian Software Ltd.");

CSWICertStoreToken::CSWICertStoreToken(CCTTokenType& aTokenType) :
	iTokenType(aTokenType)
	{
	}

CSWICertStoreToken::~CSWICertStoreToken()
	{
	}

MCTTokenType& CSWICertStoreToken::TokenType()
	{
	return iTokenType;
	}

const TDesC& CSWICertStoreToken::Label()
	{
	return KSWICertStoreTokenLabel;
	}

TCTTokenHandle CSWICertStoreToken::Handle()
	{
	TUid tokenTypeUid = { KSWICertStoreTokenTypeUid };
	return TCTTokenHandle(tokenTypeUid, 0);
	}

TInt& CSWICertStoreToken::ReferenceCount()
	{
	return iRefCount;
	}

void CSWICertStoreToken::DoGetInterface(TUid aRequiredInterface,
										MCTTokenInterface*& aReturnedInterface, 
										TRequestStatus& aStatus)
	{
	TInt result = KErrNone;

	if (aRequiredInterface.iUid != KInterfaceCertStore)
		{
		result = KErrNotSupported;
		}
	else
		{
		if (!iCertStore)
			{
			TRAP(result, iCertStore = CSWICertStore::NewL(*this, iTokenType.Fs()));					
			}
		}
	
	if (result != KErrNone)
		{
		Release();
		}	
	else
		{
		++iInterfaceRefCount;
		aReturnedInterface = static_cast<MCTCertStore*>(iCertStore);
		}
	
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, result);
	}

TBool CSWICertStoreToken::DoCancelGetInterface()
	{
	return EFalse;
	}

const TDesC& CSWICertStoreToken::Information(TTokenInformation aRequiredInformation)
	{
	//there is no support for localisation here
	switch (aRequiredInformation)
		{
		case EVersion:
			return KSWICertStoreVersion;

		case ESerialNo:
			return KSWICertStoreSerialNo;

		case EManufacturer:
			return KSWICertStoreManufacturer;
			
		default:
			return KNullDesC;
		}
	}

TBool CSWICertStoreToken::ReleaseInterface()
	{
	// Decrement the interface's reference count and return whether it should be
	// deleted.  Called by CSWICertStore's DoRelease method.
	
	TBool canDelete = --iInterfaceRefCount == 0;
	if (canDelete)
		{
		iCertStore = NULL;
		}
	return canDelete;
	}
