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
* Test implementation of a session count server tests the base functionality
* will work for sessions, subsessions, and asynchronous requests.
*
*/


/**
 @internalComponent
 @test
 @file
*/

#ifndef SCSTESTSERVER_H
#define SCSTESTSERVER_H

#include <scs/scsserver.h>
#include "scstestcommon.h"

// forward declarations
class CScsTestSession;
class CScsTestSubsession;

class CTrebleRequest : public CAsyncRequest
/**
	Trebles a client-side TInt.  The request is completed asynchronously,
	some time after it has been queued.
 */
	{
public:
	static CTrebleRequest* NewL(CScsTestSession* aSession, CScsTestSubsession* aSubsession, const RMessage2& aMessage);
	virtual ~CTrebleRequest();
	
	// implement CActive
	virtual void DoCancel();
	// implement CActive, override CAsyncRequest
	virtual void RunL();
	
private:
	CTrebleRequest(CScsTestSession* aSession, CScsTestSubsession* aSubsession, const RMessage2& aMessage);
	void ConstructL();
	
private:
	RTimer iTimer;	///< Provides the delay which makes this request asynchronous.
	};

class CScsTestSession;
class CScsTestSubsession : public CScsSubsession
/**
	This subsession class is defined to ensure that requests are
	routed to, and asynchronous requests associated with, the correct
	object.
 */
	{
public:
	static CScsTestSubsession* NewL(CScsTestSession &aSession, const RMessage2& aMessage);
	virtual ~CScsTestSubsession();
	
	// implement CScsSubsession
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);

private:
	CScsTestSubsession(CScsTestSession &aSession);

private:
	TInt iValue;			///< Value on which this subsession is curried.
	};

class CScsTestServer;
class CScsTestSession : public CScsSession
/**
	Tests the underlying session count session implementation by
	handling requests, asynchronous requests, and creating subsessions.
 */
	{
public:
	static CScsTestSession* NewL(CScsTestServer &aServer);
	virtual ~CScsTestSession();

	// implement CScsSession
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	// override CScsSession
	virtual CScsSubsession* DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage);
	
private:
	CScsTestSession(CScsTestServer &aServer);
	};

class CScsTestServer : public CScsServer
/**
	This test implementation of the session count server provides
	the functionality to create sessions and subsessions, and to
	queue asynchronous requests on each of them.
 */
	{
public:
	static CScsTestServer* NewLC();
	virtual ~CScsTestServer();

	// implement CScsServer
	virtual CScsSession* DoNewSessionL(const RMessage2& aMessage);

private:
	CScsTestServer();
	void ConstructL(TInt aShutdownPeriodUs);
	};

#endif	// #ifndef SCSTESTSERVER_H

