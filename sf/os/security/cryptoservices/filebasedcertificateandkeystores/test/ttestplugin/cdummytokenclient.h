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


#ifndef __CDUMMYTOKENCLIENT_H__
#define	__CDUMMYTOKENCLIENT_H__

#include <ct.h>

//!	Filetokens client side representation of MCTToken, provides client-side reference counting
//!	which calls server with ECloseToken request when appropriate
//! Created by CDummyTokenTypeSession as part of OpenToken call
//! Uses RFileStoreClientSession to communicate with filetokens server

NONSHARABLE_CLASS(CDummyTokenClient) : public CBase, public MCTToken
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
	CDummyTokenClient(MCTTokenType* aTokenType);

private:
	MCTTokenType* iTokenType;
	TInt iRefCount;
};

#endif // __CDUMMYTOKENCLIENT_H__
