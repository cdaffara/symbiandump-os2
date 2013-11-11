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

#ifndef __LOGDUP_H__
#define __LOGDUP_H__

#include <logwrap.h>

// Classes referenced
class CLogFilter;
class CLogFilterList;
class MLogServDatabaseTransactionInterface;

/**
Updates the records of duplicated events in the recent list.

@see CLogFilterList
@see CLogEvent
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogDuplicate : public CLogActive
	{
public:
	static CLogDuplicate* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogDuplicate();
	TBool StartL(TLogId aId, TLogRecentList aRecentList, const CLogFilter& aFilter, TRequestStatus& aStatus);

private:
	CLogDuplicate(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void ConstructL();
	//
	void DoRunL();
	void DoComplete(TInt& aStatus);
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	CLogFilterList* iFilterList;
	TLogId iId;
	TLogRecentList iRecentList;
	};

#endif
