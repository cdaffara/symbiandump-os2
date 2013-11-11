// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGSERVSHUTDOWNTIMER_H__
#define __LOGSERVSHUTDOWNTIMER_H__

#include <e32base.h>

/**
LogEng server shutdown timer.
If the connection (session) count to the LogEng server becomes 0 and the server is transient
then the timer will be scheduled to run after KLogShutdownDelay microseconds (set to be 2 seconds).
If during that period of time no connetcion to the server is established then the server will be
shut down.

@see CLogServServer
@internalComponent
*/
class CLogServShutdownTimer : public CTimer
	{
public:
	static CLogServShutdownTimer* NewL(TInt aPriority);
    void Start();

private:
	CLogServShutdownTimer(TInt aPriority);
	void ConstructL();
    // From CActive
	void RunL();

private: // Internal constants
	enum { KLogShutdownDelay = 2000000 };
	};

#endif
