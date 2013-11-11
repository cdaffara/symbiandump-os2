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
* Name        : ipcsesion.h
* Part of     : ipc server
* To implement the ipc session
* 
*
*/




#ifndef __IPCSESSION_H__
#define	__IPCSESSION_H__


#include "ipcserver.h" 	//CIpcServer
#include "ipcutils.h"





//-----------------------------------------------------------------------
//Class name: 	CIpcSession
//Description: 	Implement a session for the stdio redirection server.
//				This class contains the handle stream class for a 
//				particular media type. The type of media is read from the 
//				config.ini file.
//-----------------------------------------------------------------------

class CIpcSession : public CSession2
	{
public:
	static CSession2* NewL();
	void ServiceL(const RMessage2&);
	virtual void CreateL();
	CIpcServer& Server();
	virtual ~CIpcSession();
			
private:
	CIpcSession()
		{
		// nada
		}
		
	void ConstructL();
	};
	    	
#endif //__IPCSESSION_H__
