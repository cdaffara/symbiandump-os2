// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms debug utility (File and Serial logger) server side header
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __COMSDBGSTUB_H__
#define __COMSDBGSTUB_H__


#include <e32base.h>

_LIT(KFLoggerServerName,"!Comsdbg server");	///< Identifier to symbian OS server system.


/**
 * CFileLoggerServer - maintain server state
 *
 * This class is responsible for maintaining the server state. It provides
 * control of the second thread, the ini file change notifier, the current
 * list of valid logs and other ini file settings, and
 * the log queue.
 */
class CFileLoggerServer : public CServer2
	{
public:
	static CFileLoggerServer* NewL();
	~CFileLoggerServer();
// CServer
	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
protected:
	virtual TInt RunError(TInt aError);
private:
	CFileLoggerServer();
	void ConstructL();
	};	

class MIniFlushModeAndLogValidQuery;
class MIniLoggingMediaQuery;

/**
 * CFileLogSession -  
 *
 * This class is responsible for servicing the client requests sent. It forms the back-end server side of
 * each client connection to flogger server and implements the ServiceL() to service all client requests.
 */
class CFileLogSession : public CSession2
	{
public:
	static CFileLogSession* NewL();
	~CFileLogSession();
	// CSession
	virtual void ServiceL(const RMessage2& aMessage);
	//
private:
	CFileLogSession();
	};




/**
FLogger - flogger server process startup class 
This class contains functions to start the flogger server.
@internalComponent 
*/ 
class FLogger
	{
public:
	class TSignal;
public:
	IMPORT_C static TInt Start();
	IMPORT_C static TInt Run();
private:
	static TInt Init();
	};



#endif // __COMSDBGSTUB_H__

