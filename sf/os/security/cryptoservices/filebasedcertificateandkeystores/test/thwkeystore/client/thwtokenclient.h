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
* This class inherits from class MCTToken and implements all the pure 
* virtual methods exposed by MCTToken. This provides the 
* MCTTokenInterface and also provides reference counting.
*
*/


/**
 * @file
 * @internalComponent
 * @released
 */
#ifndef CHARDWARETOKENCLIENT_H
#define	CHARDWARETOKENCLIENT_H

#include <ct.h>

NONSHARABLE_CLASS(CHardwareTokenClient) : public CBase, public MCTToken
{
public:
	static MCTToken* NewL(MCTTokenType* aTokenType);

public:	//	From MCTToken
	virtual MCTTokenType& TokenType();
	virtual const TDesC& Label();
	virtual TCTTokenHandle Handle();

protected:	//	From MCTToken
	virtual TInt& ReferenceCount();
	virtual void DoGetInterface(TUid aRequiredInterface, MCTTokenInterface*& aReturnedInterface, TRequestStatus& aStatus);
	virtual TBool DoCancelGetInterface();
	virtual const TDesC& Information(TTokenInformation aRequiredInformation);

private:
	CHardwareTokenClient(MCTTokenType* aTokenType);

private:
	MCTTokenType* iTokenType;
	TInt iRefCount;
};

#endif // CHARDWARETOKENCLIENT_H
