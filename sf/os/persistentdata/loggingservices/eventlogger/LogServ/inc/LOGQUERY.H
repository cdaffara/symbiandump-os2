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

#ifndef __LOGQUERY_H__
#define __LOGQUERY_H__

#include <d32dbms.h>
#include <logwraplimits.h>

// Classes referenced
class MLogServDatabaseTransactionInterface;

/**
RDbTable derived class which offers an OpenLC() method.
As part of the "open table" call, the method will try to recover the database if the database is damaged.

@internalComponent
*/
class RLogDbTable : public RDbTable
	{
public:
	void OpenLC(RDbDatabase& aDb, const TDesC& aTblName, RDbRowSet::TAccess aAccess = RDbRowSet::EUpdatable);
	};

/**
RLogDbTable derived class.
Opens "Event" table.
If the table is opened for the first time, the OpenLC() method will retrieve the table column numbers. 

@see RLogDbTable 
@internalComponent
*/
class RLogEventDbTable : public RLogDbTable
    {
public:
    void OpenLC(RDbDatabase& aDb, RDbRowSet::TAccess aAccess = RDbRowSet::EUpdatable);
       
public:
    static TDbColNo iIdColNo;
    static TDbColNo iTypeColNo;
    static TDbColNo iRemotePartyColNo;
    static TDbColNo iDirectionColNo;
    static TDbColNo iTimeColNo;
    static TDbColNo iDurationTypeColNo;
    static TDbColNo iDurationColNo;
    static TDbColNo iStatusColNo;
    static TDbColNo iSubjectColNo;
    static TDbColNo iNumberColNo;
    static TDbColNo iContactColNo;
    static TDbColNo iLinkColNo;
    static TDbColNo iDataColNo;
    static TDbColNo iFlagColNo[KLogFlagsCount];
    static TDbColNo iRecentColNo;
    static TDbColNo iDuplicateColNo;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
    static TDbColNo iSimIdColNo;
#endif
    
private:
    void InitializeColumnsL();
    };

/**
RLogDbTable derived class.
Opens "Config" table.
If the table is opened for the first time, the OpenLC() method will retrieve the table column numbers. 

@see RLogDbTable 
@internalComponent
*/
class RLogConfigDbTable : public RLogDbTable
    {
public:
    void OpenLC(RDbDatabase& aDb, RDbRowSet::TAccess aAccess = RDbRowSet::EUpdatable);
       
public:
    static TDbColNo iSizeColNo;
    static TDbColNo iRecentColNo;
    static TDbColNo iAgeColNo;
    
private:
    void InitializeColumnsL();
    };

/**
RDbView derived class.

As part of the "prepare view" call, the method will try to recover the database if the database is damaged.
The PrepareLC() method will call also view's EvaluateAll() method, if the preparation was successful.

@internalComponent
*/
class RLogDbView : public RDbView
	{
public:
	void PrepareLC(RDbDatabase& aDb, const TDesC& aQuery, RDbRowSet::TAccess aAccess = RDbRowSet::EUpdatable);
	};

void LogGetRecentEventsLC(MLogServDatabaseTransactionInterface& aDb, TLogRecentList aRecentListId, 
                          TLogRecentSize aMaxRecentLogSize, RArray<TLogId>& aEventIds);
void LogPurgeRecentEventsL(MLogServDatabaseTransactionInterface& aDb, const RArray<TLogId>& aEventIds);
void LogPurgeMainL(MLogServDatabaseTransactionInterface& aDb, RLogEventDbTable& aTbl, 
                   TLogSize aMaxLogSize, TInt aCountPlus);
void LogResetDuplicatesL(MLogServDatabaseTransactionInterface& aDb, TLogId aEventId);

#endif
