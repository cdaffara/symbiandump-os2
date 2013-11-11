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

#ifndef __LOGSERVVIEW_H__
#define __LOGSERVVIEW_H__

#include <logcli.h>
#include "LOGQUERY.H"
#include "LogCliServShared.h"
#include "LogServBackupObserver.h"
#include "LogServDatabaseChangeObserver.h"
#include "LogServDatabaseTransactionInterface.h"

// Classes referenced
class CLogPackage;
class CLogServViewLockObserver;
class CLogServViewChangeManager;
class MLogServDatabaseTransactionInterface;
class MLogServBackupInterface;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////   CLogServViewBase /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Base class for the view that the LogEng server can create.
Implements MLogServBackupObserver and MLogServDatabaseChangeObserver interfaces. 

@see MLogServBackupObserver 
@see MLogServDatabaseChangeObserver 
@internalComponent
*/
class CLogServViewBase : public CBase, public MLogServBackupObserver, public MLogServDatabaseChangeObserver
	{
public:
	~CLogServViewBase();
    void RequestChangeNotifications(const RMessage2& aMessage);
    void RequestChangeNotificationsCancel();
    void RequestLockStatusChanges(const RMessage2& aMessage);
    void RequestLockStatusChangesCancel();
    void RequestChangesL(const RMessage2& aMessage);
    void SetupL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType);
    virtual TPtrC GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType)=0;
    virtual void RemoveL(const RMessage2& aMessage);
    virtual void ClearDuplicatesL(const RMessage2& aMessage);
    virtual void SetFlagsL(const RMessage2& aMessage);
    inline TLogViewId ViewId() const;
    TInt Count() const;
    TLogId At(TInt aIndex) const;
    inline TBool ViewIsReady() const;

protected:
	CLogServViewBase(MLogServDatabaseTransactionInterface& aDatabase, 
	                 MLogServBackupInterface& aBackupInterface, 
	                 CLogPackage& aPackage, 
	                 TLogViewType aType, 
	                 TLogViewId aViewId, 
	                 const RMessage2& aMessage);
	                 
	void ConstructL();
    static void DestroyList(TAny *aPtr);

private: // FROM MLogServDatabaseChangeObserver
	void DCOHandleChangeEventsL(const CLogChangeDefinition& aChanges);
    // FROM MLogServBackupObserver
	void BOHandleEventL(TLogServBackupEvent aEvent);

	void InitializeColumnsL(RDbRowSet& aRowSet);
	inline TInt ClientSideCursorPosition() const;
	//
	void ResetViewContentsL(RDbRowSet& aRowSet);
	void PrepareViewContentsL(const TDesC& aSQL);
	TInt RebuildViewContentsIfNecessary();
	void DoHandleChangeEventsL(const CLogChangeDefinition& aChanges);
	void RebuildViewL();
	TBool IsAllowed(TEventOp aEventOp, TUint8 aEventTypeIndex);

protected:
	MLogServDatabaseTransactionInterface& iDatabase;
	CLogPackage& iPackage;

private:
	MLogServBackupInterface& iBackupInterface;
	TLogViewType iType;//The type of view that this object represents
	TLogViewId iViewId;//The id of this view
	RArray<TLogId> iViewContents;//An array which contains the log ids of all the items in this view
	TBool          iViewContentsReady;
	HBufC* iSql;//The SQL filter/condition which is used to build this view
	TBool iRebuildViewContents;//A flag to indicate that we need to rebuild the view because handling a change notification failed (e.g due to OOM).
	CLogServViewLockObserver* iLockChangeObserver;//Manages lock change events
	CLogServViewChangeManager* iChangeManager;//Manages change events for this view
	const RMessage2& iMessage;//Passed in the constructor by the LogServOperationFactory - used to check security
	/**
	Data structure for storage of cached security information
	@internalComponent
	*/
	struct SStandardTypeSecurity 
		{
		TUid eventType;
		TBool readAccess;
		TBool writeAccess;
		};
	RArray<SStandardTypeSecurity> iStandardTypeSecurityCache;
    static TDbColNo   iIdColNo;
    static TDbColNo   iTypeColNo;
    static TDbColNo   iFlagColNo[KLogFlagsCount];

	};

/**
Returns the id of this view
*/
inline TLogViewId CLogServViewBase::ViewId() const
	{
	return iViewId;
	}

/**
Is the view ready?
*/
inline TBool CLogServViewBase::ViewIsReady() const
	{
	return (iViewContentsReady && iSql);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////   CLogServViewLockObserver  ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Implements MLogServBackupObserver interface.
An instance of this class is data member of CLogServViewBase.
It is used every time when the client side requests notification regarding the view status.
The view status changes to "locked" during the backup/restore operations.

@see CLogServViewBase
@see MLogServBackupObserver
@internalComponent
*/
class CLogServViewLockObserver : public CBase, public MLogServBackupObserver
	{
public:
	static CLogServViewLockObserver* NewL(MLogServBackupInterface& aBackupInterface);
	~CLogServViewLockObserver();
    void RequestLockStatusChanges(const RMessage2& aMessage);
    void RequestLockStatusChangesCancel();

private:
	CLogServViewLockObserver(MLogServBackupInterface& aBackupInterface);
	void ConstructL();
    // FROM MLogServBackupObserver
	void BOHandleEventL(TLogServBackupEvent aEvent);

	inline TBool HaveLockStatusChangePointer() const;
	void CompleteLockStatusChangeMessage(TInt aCompletionCode);

private:
	MLogServBackupInterface& iBackupInterface;
	RArray<TLogViewLockStatus> iLockEvents;//Holds state changes when the client isn't able to receive them immediately. 
	RMessagePtr2 iLockStatusChangeMessage;//Completed when the view is locked/unlocked

	};

inline TBool CLogServViewLockObserver::HaveLockStatusChangePointer() const
	{
	return (iLockStatusChangeMessage != RMessagePtr2());
	} 

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////   CLogServViewEvent  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
CLogServViewBase derived class.

@see CLogServViewBase 
@internalComponent
*/
class CLogServViewEvent : public CLogServViewBase
	{
public:
	static CLogServViewEvent* NewL(MLogServDatabaseTransactionInterface& aDatabase, 
	                               MLogServBackupInterface& aBackupInterface, 
	                               CLogPackage& aPackage, 
	                               TLogViewId aViewId, 
	                               const RMessage2& aMessage);
	                               
	TPtrC GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType);
	//
private:
	CLogServViewEvent(MLogServDatabaseTransactionInterface& aDatabase, 
	                  MLogServBackupInterface& aBackupInterface, 
	                  CLogPackage& aPackage, 
	                  TLogViewId aViewId, 
	                  const RMessage2& aMessage);
	};


///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////   CLogServViewRecent  //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
CLogServViewBase derived class.

@see CLogServViewBase 
@internalComponent
*/
class CLogServViewRecent : public CLogServViewBase
	{
public:
	static CLogServViewRecent* NewL(MLogServDatabaseTransactionInterface& aDatabase, 
	                                MLogServBackupInterface& aBackupInterface, 
	                                CLogPackage& aPackage, 
	                                TLogViewId aViewId, 
	                                const RMessage2& aMessage);
	                                
	TPtrC GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType);
	void RemoveL(const RMessage2& aMessage);
	void ClearDuplicatesL(const RMessage2& aMessage);
	//

	//
private:
	CLogServViewRecent(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage);
    void InitializeColumns2L(RDbRowSet& aRowSet); 
	//
private:
	TLogRecentList iRecentList;

    static TDbColNo   iIdColNo;
    static TDbColNo   iRecentColNo;
    static TDbColNo   iDuplicateColNo;
	
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////   CLogServViewDuplicate  ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
CLogServViewBase derived class.

@see CLogServViewBase 
@internalComponent
*/
class CLogServViewDuplicate : public CLogServViewBase
	{
public:
	static CLogServViewDuplicate* NewL(MLogServDatabaseTransactionInterface& aDatabase, 
	                                   MLogServBackupInterface& aBackupInterface,
	                                   CLogPackage& aPackage, 
	                                   TLogViewId aViewId, 
	                                   const RMessage2& aMessage);
	                                   
	TPtrC GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType);
	void RemoveL(const RMessage2& aMessage);
	//
private:
	CLogServViewDuplicate(MLogServDatabaseTransactionInterface& aDatabase, 
	                      MLogServBackupInterface& aBackupInterface, 
	                      CLogPackage& aPackage, 
	                      TLogViewId aViewId, 
	                      const RMessage2& aMessage);
	//
private:
	TLogId iSourceId;
	};

#endif
