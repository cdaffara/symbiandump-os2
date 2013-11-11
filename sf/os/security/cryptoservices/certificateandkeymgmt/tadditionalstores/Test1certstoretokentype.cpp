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

#include "Test1certstoretokentype.h"
#include "test1certstoretoken.h"

_LIT(KName1, "Test store 1");
_LIT(KName2, "Test store 2");

CCTTokenType* CTest1CertStoreTokenType::NewL()
	{
	return new(ELeave) CTest1CertStoreTokenType();
	}

CTest1CertStoreTokenType::~CTest1CertStoreTokenType()
	{
	}

void CTest1CertStoreTokenType::List(RCPointerArray<HBufC>& aTokens,
									TRequestStatus &aStatus)
	{
	TInt error = KErrNone;
	HBufC* name = KName1().Alloc();
	if (name)
		{
		error = aTokens.Append(name);
		if (error != KErrNone)
			{
			delete name;
			name = NULL;
			}
		}
	else
		{
		error = KErrNoMemory;
		}


	if (error == KErrNone)
		{
		name = KName2().Alloc();
		if (name)
			{
			error = aTokens.Append(name);
			if (error != KErrNone)
				{
				delete name;
				name = NULL;
				}
			}
		else
			{
			error = KErrNoMemory;			
			}
		}
	
	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, error);
	}

void CTest1CertStoreTokenType::CancelList()
	{
	}

void CTest1CertStoreTokenType::OpenToken(const TDesC& aTokenInfo, 
										 MCTToken*& aToken,
										 TRequestStatus& aStatus)
	{
	MCTToken* token = 0;
	if (aTokenInfo == KName1)
		{
		token = CTest1CertStoreToken::New(ETest1CertStore, *this);
		}
	else if (aTokenInfo == KName2)
		{
		token = CTest1CertStoreToken::New(ETest2CertStore, *this);
		}

	TInt error = KErrNone;
	if (!token)
		{
		error = KErrNoMemory;
		}
	else
		{
		IncReferenceCount();
		aToken = token;
		}

	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, error);
	}

void CTest1CertStoreTokenType::OpenToken(TCTTokenHandle /*aHandle*/, 
										 MCTToken*& /*aToken*/, 
										 TRequestStatus& /*aStatus*/)
	{
	//Not supported
	}

void CTest1CertStoreTokenType::CancelOpenToken()
	{
	}
