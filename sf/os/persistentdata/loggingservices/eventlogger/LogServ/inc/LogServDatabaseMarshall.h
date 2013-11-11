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

#ifndef __LOGSERVDATABASEMARSHALL__
#define __LOGSERVDATABASEMARSHALL__

#include <d32dbms.h>
#include <logcli.h>
#include "LogServBackupObserver.h"
#include "LogServDatabaseTransactionInterface.h"
#include <centralrepository.h>

// Classes referenced
class MLogServBackupInterface;
class CLogServResourceInterpreter;
class CLogServSecurity;

/**
Implements MLogServBackupObserver and MLogServDatabaseTransactionInterface interfaces.
Owns and provides access to strings, config and types caches.  

@see CLogServCacheConfig
@see CLogServCacheStrings
@see CLogServCacheTypes
@see MLogServBackupObserver
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogServDatabaseMarshall : public CBase, public MLogServBackupObserver, public MLogServDatabaseTransactionInterface
	{
public:
	static CLogServDatabaseMarshall* NewL(RFs& aFsSession, CLogServResourceInterpreter& aResourceInterface, MLogServBackupInterface& aBackupInterface);
	~CLogServDatabaseMarshall();
    inline const TDesC& DatabaseName() const;
    inline void SetChangeInterface(MLogServDatabaseChangeInterface& aChangeInterface);

private:
	CLogServDatabaseMarshall(RFs& aFsSession, CLogServResourceInterpreter& aResourceInterface, MLogServBackupInterface& aBackupInterface);
	void ConstructL();

// From MLogServDatabaseTransactionInterface
	TInt DTIBegin();
	TInt DTICommitAndEnd();
	void DTIRollBack();
	TInt DTIExecuteSql(const TDesC& aStatement, TDbTextComparison aComparison = EDbCompareNormal);
	TBool DTIInTransaction() const;
	TBool DTIDatabaseIsDamaged() const;
	CLogServResourceInterpreter& DTIResourceInterface() const;
	MLogServDatabaseChangeInterface& DTIChangeInterface() const;
	CLogServCacheConfig& DTICacheConfig() const;
	CLogServCacheStrings& DTICacheStrings() const;
	CLogServCacheTypes& DTICacheTypes() const;
	RDbDatabase& DTIDatabase();
	TBool DTIIsAllowed(TEventOp aEventOp, const RMessage2& aMessage, TUid aEventType, const char* aDiagnostic = 0) const;
	const RArray<TUid>& DTIUidsOfStandardTypes();

// From MLogServBackupObserver
	void BOHandleEventL(TLogServBackupEvent aEvent);

// Internal methods
	void DatabaseLocateL();
	void DatabaseOpenL();
	void DatabaseCreateL(const TDesC& aFileName);
	//
	void CreateTablesL();
	void CreateTypesL(TBool aReadOnly = EFalse);
	void CreateIndiciesL();
	void CreateConfigurationL();
	//	
	void ReadRepositoryFileConfigurationL(TLogConfig& aConfig, CRepository& repository) const;
	void ReadResourceFileConfigurationL(TLogConfig& aConfig) const;
	//
	void MakeColumnAutoIncremetingL(const TDesC& aTable, const TDesC& aColumn);
	//
	inline void RestoreStandardTypesL();

    TInt DoDbOpen();
    TBool DbTableCntCheckL();
    TInt AlterDbIfOldFmtL(); 
    TInt AlterEventTblIfOldFmt(CDbColSet& aEventTblColSet); 
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
    TInt AlterEventTblIfNoSimIdL(CDbColSet& aEventTblColSet);
#endif    
    void DbDelete();
    TInt DbCreate();
    void CreateCachesL();
	
private:
	RFs& iFsSession;//File server interface handle
	CLogServResourceInterpreter& iResourceInterface;//Used to access resources
	MLogServDatabaseChangeInterface* iChangeInterface;//Used to access change interface
	HBufC* iDatabaseName;//So we know what to open after a backup
	MLogServBackupInterface& iBackupInterface;//Notifies us when a backup takes place
	RDbNamedDatabase iDatabase;//The actual log database
	CLogServCacheConfig* iCacheConfig;//Config table cache
	CLogServCacheStrings* iCacheStrings;//String table cache
	CLogServCacheTypes* iCacheTypes;//Type table cache
 	CLogEventType* iEventType;//Used to get event type TUid from the cache before calling the security manager
 	CLogServSecurity* iSecurity;//Platform security implementation
 	RArray<TUid> iStandardTypeUids;//Needed here for use by views during backups. When a backup is in progress, access to CacheTypes isn't allowed.

	};

inline const TDesC& CLogServDatabaseMarshall::DatabaseName() const
	{
	return *iDatabaseName;
	}

inline void CLogServDatabaseMarshall::SetChangeInterface(MLogServDatabaseChangeInterface& aChangeInterface)
	{
	// Bit ugly, but anyway...
	iChangeInterface = &aChangeInterface;
	}
	
inline void CLogServDatabaseMarshall::RestoreStandardTypesL()
	{
	// Force LogServCacheTypes to read the TUids of all built-in types.
	// The ETrue argument will stop LogServCacheTypes from writing the results
	// into the database - i.e. a read-only operation on the cache types.
	CreateTypesL(ETrue);
	}

#endif
