// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGMAIN_H__
#define __LOGMAIN_H__

#include <logwrap.h>
#include <logcli.h>

// Classes referenced
class CLogRecover;
class CLogCompact;
class MLogServDatabaseTransactionInterface;

/**
Performas maintenance tasks on the LogEng database, such as database compaction and database recovery. 

@see MLogServDatabaseTransactionInterface
@see CLogRecover
@see CLogCompact
@internalComponent
*/
class CLogMaintenance : public CLogActive
	{
public:
	static CLogMaintenance* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogMaintenance();
	//
	void Start(TBool aPurge, TRequestStatus& aStatus);
	//
private:
	CLogMaintenance(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void ConstructL();
	//
	void DoRunL();
	void DoComplete(TInt& aStatus);
	TBool DoNextL();
	void DoCancel();
	//
	void ClearLogL(const TTime& aDate, TRequestStatus& aStatus);

	//
	enum TLogState {	ELogRecover, ELogGetConfig, ELogPurgeMainAge, ELogCompact, ELogComplete };
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	TLogState iState;
	TLogConfig iConfig;
	TBool iPurge;
	CLogRecover* iRecover;
	CLogCompact* iCompact;
	};

#endif
