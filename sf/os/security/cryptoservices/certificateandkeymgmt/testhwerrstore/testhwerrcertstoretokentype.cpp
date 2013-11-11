/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include "testhwerrcertstoretokentype.h"
#include <ecom/implementationproxy.h>


CCTTokenType* CTestHWErrStoreTokenType::NewL()
	{
	return new(ELeave) CTestHWErrStoreTokenType();
	}

CTestHWErrStoreTokenType::~CTestHWErrStoreTokenType()
	{
	}

void CTestHWErrStoreTokenType::List(RCPointerArray<HBufC>& /*aTokens*/, TRequestStatus &aStatus)
	{
	// Always return Error to test defect INC037687 - "Unified Certstore leaves if token type doesn't return KErrNone in List method" 
	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, KErrHardwareNotAvailable);
	}

void CTestHWErrStoreTokenType::CancelList()
	{
	}

void CTestHWErrStoreTokenType::OpenToken(const TDesC& /*aTokenInfo*/, 
										MCTToken*& /*aToken*/,
										TRequestStatus& aStatus)
	{
	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, KErrNotSupported);
	}

void CTestHWErrStoreTokenType::OpenToken(TCTTokenHandle /*aHandle*/, 
										MCTToken*& /*aToken*/, 
										TRequestStatus& aStatus)
	{
	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, KErrNotSupported);
	}

void CTestHWErrStoreTokenType::CancelOpenToken()
	{
	}


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101FE9EF,	CTestHWErrStoreTokenType::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


//
// testhwerrcertstore DLL entry point code
//
