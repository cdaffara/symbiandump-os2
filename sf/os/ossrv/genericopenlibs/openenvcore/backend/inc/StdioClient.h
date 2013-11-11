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
* Name        : stdioclient.h
* Part of     : stdio server
* stdio Redirection Server - client interface DLL
* 
*
*/



#ifndef __RSTDIO_SESSION__
#define __RSTDIO_SESSION__

#include<e32std.h> //RSessionBase


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

//-----------------------------------------------------------------------
//Class name: RStdioSession
//Description: It represents the seesion to Stdio Server. 
//-----------------------------------------------------------------------

class RStdioSession : public RSessionBase
	{
	public:
		TInt Connect();
		void Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength);
		void Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength);
		TInt CheckMedia(TDes8& aDes);
		void NotifyActivity(TInt aEvents, TRequestStatus& aRequest);
		void CancelNotify();
		int SetEcho(TBool aEcho);
	};	

#endif //__RSTDIO_SESSION__
