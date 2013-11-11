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



#include <e32base.h> 
#include <in_sock.h>
#include <commdbconnpref.h>
#include <commdb.h>

//micro seconds to wait before attempting to connect
//to allow NTRAS connection to be initialised (2 sec)
#define KDelay 2000000

class CActiveConnection : public CActive
	{
public:
	enum TActiveConnectionMode { EModeIAP, EModeSnap };
	 // Construction
	static CActiveConnection* NewL(const TDesC& ipAddress, TInt port);
	 // Destruction
	~CActiveConnection();
	void Start(TActiveConnectionMode aConnMode = EModeIAP, TInt aConnIndex = 0);
	void DoCancel();
	void CloseSocket();

private:
	CActiveConnection();
	void ConstructL(const TDesC& ipAddress, TInt port);
	void RunL();
	void Connect(TActiveConnectionMode aConnMode = EModeIAP, TInt aConnIndex = 0);

private:
	RSocketServ iSocketServer;
	RSocket iSocket; 
	TInetAddr iDstAddr;
	};
