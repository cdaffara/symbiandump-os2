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
* CCertStoreSession class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CCERSTORESESSION_H__
#define __CCERSTORESESSION_H__

#include "fsserver.h"

class RMessage2;
class CCertStoreConduit;

/**
 * A cert store session.
 *
 * Handles client requests by passing them to the cert store conduit.
 */
class CCertStoreSession : public CTokenServerSession
	{
public:
	/**
	 * Create a new session object.
	 * @param aConduit The cert store conduit used to service user requests.
	 */
	static CCertStoreSession* NewL(CCertStoreConduit& aConduit);
	
private:
	CCertStoreSession(CCertStoreConduit& aConduit);
	virtual void DoServiceL(const RMessage2& aMessage);
private:
	CCertStoreConduit& iConduit;
	};

#endif
