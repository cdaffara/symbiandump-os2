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
* This class inherits from class CCTTokenType which in turn inherits 
* from the class MCTTokenType. This class implements all the pure 
* virtual functions exposed by class MCTTokenType. This defines a new 
* crypto token type.
*
*/


/**
 * @file
 * @internalComponent
 * @released
 */
#ifndef CHARDWARETOKENTYPECLIENT_H
#define CHARDWARETOKENTYPECLIENT_H

#include <ct/ccttokentype.h>

NONSHARABLE_CLASS(CHardwareTokenTypeClient) :  public CCTTokenType
{
public:
	/** Creates a representation of the CCTTokenType
	 *	Called by ECom or directly accessible.
	 */
	static CCTTokenType* NewL();

public:
	virtual ~CHardwareTokenTypeClient();

public:	//	From MCTTokenType
	virtual void List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, TRequestStatus& aStatus);
	virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, TRequestStatus& aStatus);
	virtual void CancelOpenToken();

private:
	void OpenToken(MCTTokenType* aTokenType);

private:
	CHardwareTokenTypeClient();
	void ConstructL();
};

#endif	//	CHARDWARETOKENTYPECLIENT_H
