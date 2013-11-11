// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ULogger Server header file
//



/**
 @file
 @internalTechnology
 @prototype
*/
#ifndef ULOGGERSESSION_H_
#define ULOGGERSESSION_H_

#include "uloggerserver.h"
namespace Ulogger {


/** Session object
This class is responsible for servicing the client requests sent. It forms the back-end server side of
each client connection to Ulogger server and implements the ServiceL() to service all client requests.
*/
class CULoggerSession : public CSession2
	{
	public:
	static CULoggerSession* NewL(RThread& aClient, CULoggerServer& aServer);
	static CULoggerSession* NewLC(RThread& aClient, CULoggerServer& aServer);
	~CULoggerSession();

	//from CSession
	void ServiceL(const RMessage2& aMessage);
private:
	CULoggerSession(RThread& aClient, CULoggerServer& aServer);
	void ConstructL();

	void SendIntegerL(const RMessage2& aMessage, TUint32 aInt);
	//void PanicClient(TInt aPanic) const;

	//data
	RMessage2 iMessage;
	CULoggerServer& iServer;
	RThread& iClient;
	};
}
#endif /*ULOGGERSESSION_H_*/
