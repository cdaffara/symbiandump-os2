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
* Name        : stdiosesion.h
* Part of     : stdio server
* To implement the stdio session
* 
*
*/




#ifndef __STDIOSESSION_H__
#define	__STDIOSESSION_H__


#include "inidata.h" 		//CIniData
#include "StdioServer.h" 	//CStdioServer
#include "streambase.h"		//CStreamBase2
#include "ReadRequestHandler.h"


//ini file path and name
_LIT(KFileName, "\\system\\data\\config.ini");

class CStreamFactoryBase;


//-----------------------------------------------------------------------
//Class name: 	CStdioSession
//Description: 	Implement a session for the stdio redirection server.
//				This class contains the handle stream class for a 
//				particular media type. The type of media is read from the 
//				config.ini file.
//-----------------------------------------------------------------------

class CStdioSession : public CSession2
	{
public:
	static CSession2* NewL();
	void ServiceL(const RMessage2&);
	virtual void CreateL();
	~CStdioSession();
			
private:
	CStdioSession()
		{
		// nada
		}
		
	void ReadIniL(CIniData* aIniData,TPtrC& aMedia,
						CStreamBase*& aStream,TBool aIsWrite);
	void ConstructL();	
	CStdioServer& Server();
	TInt WriteStream(const RMessage2& aMessage);
	CStreamBase* iStreamRead;
	CStreamBase* iStreamWrite;
	CIniData* iIniData;
	CReadRequestHandler* iReadRequestHandler;
	TBuf8<256> iErrInBuf;
	TBuf8<256> iErrOutBuf;
	TBool iIsInputConsole;
	TBool iIsInMediaFile;
	};
	
    	
#endif //__STDIOSESSION_H__
