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
 @file Server.h
*/

#if (!defined __TESTEXECUTE_PIPS_LOG_SERVER_H__)
#define __TESTEXECUTE_PIPS_LOG_SERVER_H__
#include <e32base.h>
#include <f32file.h>
#include <test/testexecutepipslog.h>

class CLogSession ; 
class CLogServer : public CServer2
	{
public:
	static CLogServer* NewL();
	~CLogServer();
	void ControlComplete(CLogSession& aControl);
	
	inline RPointerArray<CLogSession>& LogControl();

	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	void SessionClosed();
private:
	CLogServer();
	void ConstructL();
private:
	RPointerArray<CLogSession> iControl;
	};

class CLogSession : public CSession2
	{
public:
	inline const CLogServer& LogServer() const;
	CLogSession();
	~CLogSession();
	
	void ServiceL(const RMessage2& aMessage);
	
	inline void AddSession();
	inline void RemoveSession();
	inline TInt SessionCount() const;

private:
	int iFileDes ; 
	TPtr8* iPipeName;
	TInt iSessionCount;
	};

#include "server.inl"

#endif
