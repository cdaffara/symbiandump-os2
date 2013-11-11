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

#ifndef ACMSESSION_H
#define ACMSESSION_H

#include <e32base.h>

class MAcmController;

NONSHARABLE_CLASS(CAcmSession) : public CSession2
	{
public:
	static CAcmSession* NewL(MAcmController& aAcmController);
	~CAcmSession();

private:
	CAcmSession(MAcmController& aAcmController);
	void CreateFunctionsL(const RMessage2& aMessage);
		
private: // from CSession2
	/**
	Called when a message is received from the client.
	@param aMessage Message received from the client.
	*/
	void ServiceL(const RMessage2& aMessage);

private: // unowned
	MAcmController& iAcmController;
	};

#endif // ACMSESSION_H
