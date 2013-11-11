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
* Name        : signalserver.h
* Part of     : Signal server
* 
*
*/



#ifndef SIGNALSERVER_H
#define SIGNALSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "signalshutdown.h"
#include "rpipe.h"

/*
server name
*/
_LIT(KServerName,"SignalServer");
/*
A version must be specified when creating a session with the server
*/
const TUint KSignalServMajorVersionNumber=1;
const TUint KSignalServMinorVersionNumber=0;
const TUint KSignalServBuildVersionNumber=0;

// CLASS DECLARATION
class CSignalServer : public CServer2
	{
public:
	virtual ~CSignalServer();
	static CServer2* NewL();
	void AddSession();
	void RemoveSession();
	void ServiceL(const RMessage2&);

private:
	CSignalServer(CActive::TPriority aPriority);
	CSession2* NewSessionL(const TVersion&,const RMessage2&) const;
	void ConstructL();
	TInt CheckForCapability(const RMessage2&);
	TInt iSessionCount;
	CSigShutDown *iShutDown;
	struct TOeProcessHdl
		{
		TInt 	mPid;
		RPipe	mPipe;

		static TBool MatchByPid(const TOeProcessHdl& aProc1, const TOeProcessHdl& aProc2)
			{
			if (aProc1.mPid == aProc2.mPid)
				{
				return ETrue;
				}
			return EFalse;
			}
		};
	RArray<TOeProcessHdl>	iProcessHdlList;
	};

#endif // SIGNALSERVER_H
