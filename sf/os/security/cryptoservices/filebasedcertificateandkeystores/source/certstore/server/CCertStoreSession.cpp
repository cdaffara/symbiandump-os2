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
* CCertStoreSession.h
*
*/


#include "CCertStoreSession.h"
#include "CCertStoreConduit.h"

CCertStoreSession* CCertStoreSession::NewL(CCertStoreConduit& aConduit)
	{
	return new (ELeave) CCertStoreSession(aConduit);
	}
	
CCertStoreSession::CCertStoreSession(CCertStoreConduit& aConduit) :
	iConduit(aConduit)
	{
	}

void CCertStoreSession::DoServiceL(const RMessage2& aMessage)
	{
	iConduit.ServiceCertStoreRequestL(aMessage);
	}
