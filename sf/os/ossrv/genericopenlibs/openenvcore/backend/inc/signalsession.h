/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : signalsession.h
* Part of     : Signal server
* 
*
*/



#ifndef SIGSESS_H
#define SIGSESS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "signalserver.h"

class CSignalSession : public CSession2
	{
public:
	virtual ~CSignalSession();
	static CSession2* NewL();
	void ServiceL(const RMessage2&);
	virtual void CreateL();
	CSignalServer& Server();
private:
	CSignalSession() {}
	void ConstructL();

	};

#endif // SIGSESS_H
