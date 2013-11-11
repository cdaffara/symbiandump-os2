// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef OMXILCORESERVER_H
#define OMXILCORESERVER_H

#include <e32std.h>
#include <e32base.h>
#include "omxilcoreclientserver.h"

class COmxILCore;
class COmxILCoreServerSession;


NONSHARABLE_CLASS( COmxILCoreServer ): public CServer2
	{
public:

	static COmxILCoreServer* NewL();
	static COmxILCoreServer* NewLC();
	~COmxILCoreServer();

public:
	// The thread function executed by the server
	static TInt ThreadFunction(TAny* aServerHandle);

	// Creates a new session with the server; the function
	// implements the pure virtutal function
	// defined in class CServer2
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

	void SetDeinitInProgress();

	void SessionClosed();

	void SessionOpened();

private :

	COmxILCoreServer();
	void ConstructL();

	// The thread function executed by the server
	static TInt ThreadFunctionL(TAny* aServerHandle);

	static TInt StopServer(TAny* aPtr);

	// Function to stop the IL Core server
	void DoStopServer();

private :

	COmxILCore* ipOmxILCore;
	CAsyncCallBack* ipStopCallback;
	TBool iDeinitInProgress;
	TUint iSessionCounter;

	};

#endif //OMXILCORESERVER_H
