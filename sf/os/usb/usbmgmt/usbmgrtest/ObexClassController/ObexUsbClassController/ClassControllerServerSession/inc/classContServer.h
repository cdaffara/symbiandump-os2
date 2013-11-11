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

#ifndef CLASSCONTSERVER_H
#define CLASSCONTSERVER_H

#include <e32base.h>


_LIT(KServerPanicCategory, "ServServer");
_LIT(KClientPanicCategory, "Client");

_LIT_SECURE_ID(KUsbSvrSecureId,0x101fe1db);

enum TServServerPanic
	{
	EServServerPanicIllegal = 0
	};

enum TClientPanic
	{
	EClientPanicBadMessage = 0
	};

class CServerTimer;

/**
 * CClassContServer is a CServer2 derived class, it resides on the server side
 * of the Client and Server configuration and implements a new Session to deal with requests 
 * from the client side.
 *
 */

NONSHARABLE_CLASS(CClassContServer) : public CServer2
	{
	public:
		static CClassContServer* NewLC();
		~CClassContServer();
		void SessionRemoved();
		void StartShutdownTimer();
		void CancelShutdownTimer();
		static TInt ShutdownTimerFired(TAny*);

	
	private:
		CClassContServer();
		CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
		void ConstructL();

	private:
		CSession2* iSession;
		CPeriodic* iShutdownTimer;
		static const TUint KShutdownDelay = 5 * 1000 * 1000;
		static const TUint KShutdownInterval = 0;
	};




#endif // CLASSCONTSERVER_H
