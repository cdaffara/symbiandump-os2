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
* CCertAppsSession class implementation
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __CCERAPPSSESSION_H__
#define __CCERAPPSSESSION_H__

#include "fsserver.h"

class RMessage2;
class CCertAppsConduit;

/**
 * A cert apps session.
 *
 * Handles client requests by passing them to the cert apps conduit.
 */
class CCertAppsSession : public CTokenServerSession
	{
public:
	/**
	 * Create a new session object.
	 * @param aConduit The cert apps conduit used to service user requests.
	 */
	static CCertAppsSession* NewL(CCertAppsConduit& aConduit);
	
private:
	CCertAppsSession(CCertAppsConduit& aConduit);
	virtual void DoServiceL(const RMessage2& aMessage);
private:
	CCertAppsConduit& iConduit;
	};

#endif
