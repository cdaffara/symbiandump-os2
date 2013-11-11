/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TEST1CERTSTORETOKENTYPE_H__
#define __TEST1CERTSTORETOKENTYPE_H__

#include <ct.h>

class CTest1CertStoreTokenType : public CCTTokenType
	{
public:
	static CCTTokenType* NewL();
	virtual ~CTest1CertStoreTokenType();

public:	// from MCTTokenType
	virtual void List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, 
		TRequestStatus& aStatus);
	virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, 
						   TRequestStatus& aStatus);
	virtual void CancelOpenToken();

public:
	enum TTokenId
		{
		ETest1CertStore, 
		ETest2CertStore
		};
	};

#endif

