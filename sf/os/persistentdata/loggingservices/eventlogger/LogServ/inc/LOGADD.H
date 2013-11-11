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

#ifndef __LOGADD_H__
#define __LOGADD_H__

#include <logcli.h>
#include <logcntmodel.h>

// Forward declarations
class CLogDuplicate;
class CLogServRecentList;
class MLogServDatabaseTransactionInterface;
class RLogEventDbTable;
class CRepository;

/**
CLogAddEvent class controls the operations with LogEng database - adding/removing events.
It uses the a plug-in to the contacts implementation when it needs to add
a contact name to the LogEng record being prepared. 
There is no need to monitor contacts database events such as backup&restore events, because the
contacts implementation environment will handle them for itself.
@internalComponent
*/
class CLogAddEvent : public CLogActive
	{
public:
	static CLogAddEvent* NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	~CLogAddEvent();

public:
	void StartL(CLogEvent& aEvent, const CLogServRecentList* aRecentList, TRequestStatus& aStatus, const RMessage2& aMessage);

private:
	CLogAddEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void ConstructL();
	TBool PerformContactMatchL();

	void DoRunL();
	
	void DoAddEventL(RLogEventDbTable& aTbl, TLogTypeId aTypeId, TLogStringId aDirectionId, TLogStringId aStatusId);
	void DoComplete(TInt& aStatus);
	void DoCancel();
	
	void OpenContactsL();
	void CloseContactsPlugin();

	enum TLogState
		{
		ELogNone,
		ELogSetContactAndRemoteParty,
		ELogAddEvent,
		ELogPurgeRecent
		};

    void SetEventContact();
    void SetRemoteParty();
    void GetConfigL();
    TLogTypeId SetDescriptionL();
    TBool DetectDuplicateEventsL();
	
private:
	MLogServDatabaseTransactionInterface& iDatabase;

	TBool iEventAdded;
	CLogEvent* iEvent;
	TLogState iState;
	const CLogServRecentList* iRecentList;
	CLogFilter* iDuplicateFilter;
	TLogConfig iConfig;

	CLogDuplicate* iDuplicate;
	CLogCntModel* iContactPlugin;
	
	TInt iContactMatchCount;
	//iContactNameFormat gets its value from logserv resource file and determines the 
	//order in the logs of {given_name,family_name} strings pair.
	TLogContactNameFormat iContactNameFormat;
	};

#endif
