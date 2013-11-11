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
* CCertAppsSession.h
*
*/


#include "CCertAppsSession.h"
#include "CCertAppsConduit.h"

CCertAppsSession* CCertAppsSession::NewL(CCertAppsConduit& aConduit)
	{
	return new (ELeave) CCertAppsSession(aConduit);
	}
	
CCertAppsSession::CCertAppsSession(CCertAppsConduit& aConduit) :
	iConduit(aConduit)
	{
	}

void CCertAppsSession::DoServiceL(const RMessage2& aMessage)
	{
	iConduit.ServiceCertAppsRequestL(aMessage);
	}
