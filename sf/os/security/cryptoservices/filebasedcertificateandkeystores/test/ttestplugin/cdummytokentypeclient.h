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


#ifndef __CDUMMYTOKENTYPECLIENT_H__
#define __CDUMMYTOKENTYPECLIENT_H__

#include <ct/ccttokentype.h>

//! Filetokens client side implementation of CCTTokenType, provides ListTokens and
//!	OpenToken functionality
//!	Only accessed by ECom.

NONSHARABLE_CLASS(CDummyTokenTypeClient) :  public CCTTokenType
{
public:
	/** Creates a client side representation of the CCTTokenType
	*	Called by ECom or directly accessible.
	*/
	static CCTTokenType* NewL();

public:
	virtual ~CDummyTokenTypeClient();

public:	//	From MCTTokenType
	virtual void List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, TRequestStatus& aStatus);
	virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, TRequestStatus& aStatus);
	virtual void CancelOpenToken();

private:
	void OpenToken(MCTTokenType* aTokenType);

private:
	CDummyTokenTypeClient();
	void ConstructL();
};

#endif	//	__CDUMMYTOKENTYPECLIENT_H__
