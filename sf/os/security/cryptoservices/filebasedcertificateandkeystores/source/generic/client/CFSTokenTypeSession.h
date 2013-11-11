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
* Token type session
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSTOKENTYPESESSION_H__
#define __CFSTOKENTYPESESSION_H__

#include "clientsession.h"
#include <ct/ccttokentype.h>

/**
 * Token type session.
 *
 * This doesn't do much communication with the server as the most of the
 * ctframework stuff is hard coded into the client.
 *
 * It is owned and created by the CFSTokenTypeClient so is not released until
 * all references to the tokentype object are relinquished.  It creates
 * RFileStoreClientSession, with which it communicates with the filetokens
 * server.
 */

NONSHARABLE_CLASS(CFSTokenTypeSession) : public CBase
{
public:
	virtual ~CFSTokenTypeSession();
public:
	static CFSTokenTypeSession* NewL(TUid aUid);
public:
	void ListTokensL(RCPointerArray<HBufC>& aTokens);
	void OpenTokenL(const TDesC& aTokenInfo, MCTTokenType* aTokenType, MCTToken*& aToken);
	void OpenTokenL(TCTTokenHandle aHandle, MCTTokenType* aTokenType, MCTToken*& aToken);
private:
	CFSTokenTypeSession(TUid aUid);
	void ConstructL();
private:
	RFileStoreClientSession iClient;
	TUid iUid;
	ETokenEnum iTokenId;
	};

#endif	//	__CFSTOKENTYPESESSION_H__
