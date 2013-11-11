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

#ifndef __LOGDEL_H__
#define __LOGDEL_H__

#include <logwrap.h>

// Classes referenced
class MLogServDatabaseTransactionInterface;

/**
Deletes an event from the LogEng database.

@see CLogEvent
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogDeleteEvent : public CLogActive
	{
public:
	static CLogDeleteEvent* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogDeleteEvent();
	//
	void StartL(TLogId aId, TRequestStatus& aStatus, const RMessage2& aMessage);
	
	//
private:
	CLogDeleteEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	//
	void DoRunL();
	void DoComplete(TInt& aStatus);
	void DoCancel();
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	TLogId iId;
	RMessage2* iMessage;
	};

#endif
