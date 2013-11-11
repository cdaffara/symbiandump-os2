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
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __FSSERVER_H__
#define __FSSERVER_H__

#define FILECERTSTORE_SERVER

#include <e32base.h>
#include "fstokencliserv.h"
#include "tokentypesenum.h"

class CFSKeyStoreServer;
class CFSCertStoreServer;
class CFSCertAppsServer;

/**
 * Implements shutdown of the server.  When the last client disconnects, this
 * class is activated, and when the timer expires, causes the server to
 * close.
 */
class CShutdown : public CTimer
	{
	enum {KServerShutdownDelay=0x200000};	//	approx 2s
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};


/** Filetokens server class, manages sessions. */
class CTokenServer : public CServer2
	{
public:
	static CServer2* NewLC();
public:
	~CTokenServer();
public:	
	void AddSession();
	void DropSession();
	CFSKeyStoreServer& KeyStoreServerL() const;
	CFSCertStoreServer& CertStoreServerL() const;
	CFSCertAppsServer& CertAppsServerL() const;
private:
	// For CServer2
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	CTokenServer();
	void ConstructL();
private:
	TInt iSessionCount;
	CShutdown iShutdown;

	// Ah, I knew there was a good use for that mutable keyword...
	mutable CFSKeyStoreServer* iKeyStoreServer;
	mutable CFSCertStoreServer* iCertStoreServer;
	mutable CFSCertAppsServer* iCertAppsServer;
	};								

/**
 * Base class for session objects.
 */
class CTokenServerSession : public CSession2
	{
public:
	CTokenServerSession();
	virtual ~CTokenServerSession();
public:
	inline CTokenServer& Server();
protected:
	virtual void DoServiceL(const RMessage2& aMessage) = 0;
private:
	virtual void CreateL();
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);
	virtual void ServiceL(const RMessage2& aMessage);
private:
	};

#endif	//	__FSSERVER_H__
