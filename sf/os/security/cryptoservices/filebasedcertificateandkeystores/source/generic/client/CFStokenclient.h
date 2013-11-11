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
* Filetokens client side representation of MCTToken
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSTOKENCLIENT_H__
#define	__CFSTOKENCLIENT_H__

#include "clientsession.h"
#include "tokentypesenum.h"
#include <ct.h>

//!	Filetokens client side representation of MCTToken, provides client-side reference counting
//!	which calls server with ECloseToken request when appropriate
//! Created by CFSTokenTypeSession as part of OpenToken call
//! Uses RFileStoreClientSession to communicate with filetokens server

NONSHARABLE_CLASS(CFSTokenClient) : public CBase, public MCTToken
{
	friend class CFSCertStoreClient;	//	For token reference counting	
public:
	static MCTToken* NewL(ETokenEnum aTokenTypeVal, MCTTokenType* aTokenType, RFileStoreClientSession& aClient);
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
	CFSTokenClient(ETokenEnum aTokenTypeVal, MCTTokenType* aTokenType, RFileStoreClientSession& aClient);
private:
	ETokenEnum iTokenEnum;
	MCTTokenType* iTokenType;
	TInt iRefCount;
	RFileStoreClientSession& iClient;
};

#endif
