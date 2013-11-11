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
*  Name        : ipcserver.h
*  Part of     : stdio server
* To implement the ipc server
* 
*
*/

   

 

#ifndef __IPCSERVER_H__
#define __IPCSERVER_H__

#include "shutdown.h"


/*
server name
*/
_LIT(KServerName,"LibCIpcServer");

class MRequestHandler;

/*
A version must be specified when creating a session with the server
*/
const TUint KStdioServMajorVersionNumber=1;

const TUint KStdioServMinorVersionNumber=0;

const TUint KStdioServBuildVersionNumber=0;


//-----------------------------------------------------------------------
//Class name: 	CIpcServer
//Description: 	Implements the Posix IPC Server.
//				This class handles all IPC requests from client
//				process.
//-----------------------------------------------------------------------
 
class CIpcServer : public CServer2
	{
public:
	static CServer2* NewL();
	void AddSession();
	void RemoveSession();
	void ServiceL(const RMessage2&);
	inline MRequestHandler* GetSemHandler()
		{
		return iSemQHandler;
		}
	virtual ~CIpcServer();
	void RemoveFileSockPathBySession(CSession2 *aSession);
	
private:
	CIpcServer(CActive::TPriority aPriority);
	CSession2* NewSessionL(const TVersion&,const RMessage2&)const;
	void ConstructL();
	TInt iSessionCount;
	CShutDown *iShutDown;
	MRequestHandler *iMsgQHandler;
	MRequestHandler *iSharedMemHandler;
	MRequestHandler *iSemQHandler;
	MRequestHandler *iFileSockAddrManager;
	};

#endif //__IPCSERVER_H__




