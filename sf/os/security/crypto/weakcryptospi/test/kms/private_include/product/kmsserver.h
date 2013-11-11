/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Values and data types used by KMS server-side implementation.
*
*/




/**
 @internalComponent
 @file
*/

#ifndef KMSSERVER_H
#define KMSSERVER_H

#include <e32base.h>

#include "kmsservercommon.h"


// forward declaration so can contain pointer in server object
class CKmsShutdown;
class CKeyMgmtServer : public CServer2
/**
	Symbian OS server object creates sessions for KMS clients.
 */
	{
public:
	static CKeyMgmtServer* NewLC();
	virtual ~CKeyMgmtServer();
	
	// implement CServer2
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	
	void IncrementSessionCount();
	void DecrementSessionCount();
	
private:
	CKeyMgmtServer();
	void ConstructL();

private:
	/**
		If there have been no open sessions for this period of time,
		then the server is stopped.
	 */
	static const TInt KShutdownPeriodUs = 2 * 1000 * 1000;
	TBool iLoadedLdd;		///< Whether the ldd was successfully loaded.
	
	/**
		The number of currently open sessions.  When the last open session
		is closed then the shutdown timer is started.
	 */
	TInt iSessionCount;
	
	/**
		Shuts down the server after an inactivity period when no sessions
		have been open.
	 */
	CKmsShutdown* iShutdown;
	};

class CKeyMgmtSession : public CSession2
/**
	Symbian OS server session object handles requests from KMS clients
	which are sent through the RKmsSession class.
 */
	{
public:
	static CKeyMgmtSession* NewL();
	virtual ~CKeyMgmtSession();
	
	// implement CSession2
	virtual void ServiceL(const RMessage2& aMessage);
	// override CSession2
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);

private:
	void ConstructL();
	
	void GenerateKeyL(const RMessage2& aMessage, RProcess aClientProc);
	void StoreKeyL(const RMessage2& aMessage, RProcess aClientProc);
	void WriteHandleToClientL(const RMessage2& aMessage, RProcess aClientProc, TKeyHandle aHandle);
	
	void DeleteKeyL(const RMessage2& aMessage, RProcess aClientProc);

	void AddUsageL(const RMessage2& aMessage, RProcess aClientProc);
	void DeleteUsageL(const RMessage2& aMessage, RProcess aClientProc);
	
private:
	/**
		This channel provides access to the hardware key store via a device driver.
		It is opened when the session is created, and closed when the session is
		destroyed.
	 */
	RKmsChannel iChannel;
	};

class CKmsShutdown : public CTimer
/**
	Stops the server after no sessions have been connected for
	a specified period.
 */
	{
public:
	static CKmsShutdown* NewL(TInt iDelayUs);
	
	void Restart();
	
private:
	CKmsShutdown(TInt aPeriodUs);
	
	// implement CTimer
	virtual void RunL();
	
private:
	/** Timer delay in microseconds. */
	TInt iDelayUs;
	};

#endif	// #ifndef KMSSERVER_H


