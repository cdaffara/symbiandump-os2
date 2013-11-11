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

#ifndef __LOGGET_H__
#define __LOGGET_H__

#include <logwrap.h>

// Classes referenced
class TLogConfig;
class CLogEvent;
class MLogServDatabaseTransactionInterface;

/**
Retrieves an event from the LogEng database. 
 
@see CLogEvent
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogGetEvent : public CLogActive
	{
public:
	static CLogGetEvent* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogGetEvent();
	void StartL(CLogEvent& aEvent, TRequestStatus& aStatus, const RMessage2& aMessage);
	
private:
	CLogGetEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void DoRunL();
	
private:
    MLogServDatabaseTransactionInterface& iDatabase;
    
	};

#endif
