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

#ifndef __LOGCHNGE_H__
#define __LOGCHNGE_H__

#include <logwrap.h>

// Classes referenced
class TLogConfig;
class CLogServRecentList;
class CLogServRecentListManager;
class MLogServDatabaseTransactionInterface;
class CLogDuplicate;
class CLogFilter;
class RLogEventDbTable;

 
/**
This class provides functions for updating properties of particular event.
The database record holding the event data will be updated.
 
@see MLogServDatabaseTransactionInterface
@see CLogServRecentList
@see CLogDuplicate
@see CLogEvent
@internalComponent
*/
class CLogChangeEvent : public CLogActive
	{
public:
	static CLogChangeEvent* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogChangeEvent();
	//
public:
	void StartL(const CLogEvent& aEvent, 
	            const CLogServRecentList* aRecentList, 
	            TRequestStatus& aStatus, 
	            const RMessage2& aMessage);
	//
private:
	CLogChangeEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void ConstructL();
	//
	void DoRunL();
	void DoComplete(TInt& aStatus);
	void ResetDuplicatesL();
	void UpdateDuplicateEventsL();
	void DoChangeEventL();
	void DoChangeL(RLogEventDbTable& aTbl, TLogStringId aDirectionId, TLogStringId aStatusId);
	//
	enum TLogChangeState
		{
		ELogNone = 0,
		ELogChangeEvent,
		ELogPurgeRecent
		};
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	CLogEvent* iEvent;
	TLogChangeState iState;
	const CLogServRecentList* iRecentList;
	TInt iOldRecentList;
	//
	CLogDuplicate* iDuplicate;
	CLogFilter* iDuplicateFilter;
	//
	const RMessage2* iMessage;
	};











//**********************************
// CLogChangeConfig
//**********************************

class CLogChangeConfig : public CLogActive
/**
@internalComponent
*/
	{
public:
	static CLogChangeConfig* NewL(MLogServDatabaseTransactionInterface& aDatabase, const CLogServRecentListManager& aRecentSetup, TInt aPriority);
	~CLogChangeConfig();
	//
public:
	void StartL(const TLogConfig& aConfig, TRequestStatus& aStatus);
	//
private:
	CLogChangeConfig(MLogServDatabaseTransactionInterface& aDatabase, const CLogServRecentListManager& aRecentSetup, TInt aPriority);
	//
	void DoRunL();
	void DoComplete(TInt& aStatus);
	//
	enum TLogChangeState
		{
		ELogNone,
		ELogChange,
		ELogPurgeRecent
		};
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	const TLogConfig* iConfig;
	const CLogServRecentListManager& iRecentSetup;
	TLogChangeState iState;
	TInt iRecent;
	
	};

#endif
