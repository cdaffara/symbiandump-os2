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
* CSWICertStoreTokenType class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTORETOKENTYPE_H__
#define __CSWICERTSTORETOKENTYPE_H__

#include <ct/ccttokentype.h>

/**
 * SWI Cert store token type.
 */
NONSHARABLE_CLASS(CSWICertStoreTokenType) : public CCTTokenType
	{
public:
	static CCTTokenType* NewL();
	static CCTTokenType* NewL(RFs& aFs);
	virtual ~CSWICertStoreTokenType();

	// Implementation of MCTTokenType
	virtual void List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, TRequestStatus& aStatus);
	virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, TRequestStatus& aStatus);
	virtual void CancelOpenToken();

public:
	enum TTokenId
		{
		ESWICertStore
		};
	};

#endif
