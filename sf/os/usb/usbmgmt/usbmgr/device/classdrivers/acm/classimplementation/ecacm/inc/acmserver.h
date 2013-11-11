/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

/**
 @file
 @internalComponent
*/

#ifndef ACMSERVER_H
#define ACMSERVER_H

#include <e32base.h>

class MAcmController;

/**
The ACM server class - this is created/owned by the ACM Port Factory.
Clients can use this server to create and destroy ACM functions.
*/
NONSHARABLE_CLASS(CAcmServer) : public CPolicyServer
	{
public:
	static CAcmServer* NewL(MAcmController& aAcmController);
	~CAcmServer();

private:
	CAcmServer(MAcmController& aAcmController);
	
private: // from CPolicyServer
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	
private: // unowned
	MAcmController& iAcmController;
	};

#endif // ACMSERVER_H
