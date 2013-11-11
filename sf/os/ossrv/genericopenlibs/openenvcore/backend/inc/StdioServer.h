/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : stdioserver.h
* Part of     : stdio server
* To implement the server
* 
*
*/



 

#ifndef __STDIOSERVER_H__
#define __STDIOSERVER_H__

#include <f32file.h>
#include <c32comm.h>

#include "ShutDown.h"

/*
Messages supported by server.
*/
enum TRedirStream 
	{
	ERead,
	EWrite,
	EFlush,
	ECheckMedia,
	ENotifyActivity,
	ECancelNotify,
	EEcho
	};


/*
server name
*/
_LIT(KServerName,"StdioServer");


/*
A version must be specified when creating a session with the server
*/
const TUint KStdioServMajorVersionNumber=1;

const TUint KStdioServMinorVersionNumber=0;

const TUint KStdioServBuildVersionNumber=0;


//-----------------------------------------------------------------------
//Class name: 	CStdioServer
//Description: 	Implements the STDIO redirection server.
//				This class handles all redirection requests from client
//				process.
//-----------------------------------------------------------------------
 
class CStdioServer : public CServer2
	{
public:
	static CServer2* NewL();
	static RFs& FsSession();
	static RCommServ& CsSession();
	void AddSession();
	void RemoveSession();
	void SetStreamFactory();
	~CStdioServer();
	
private:
	CStdioServer(CActive::TPriority aPriority);
	CSession2* NewSessionL(const TVersion&,const RMessage2&)const;
	void ConstructL();
	static RFs iFs;
	static RCommServ iCs;
	TInt iSessionCount;
	CShutDown *iShutDown;
	};

#endif //__STDIOSERVER_H__




