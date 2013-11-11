// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UD_STD.H"

// Class RDbDatabase

/**
Closes a database. Commits any pending transaction. Frees the allocated resources.
*/
EXPORT_C void RDbDatabase::Close()
	{
	CDbDatabase* db=iDatabase();
	if (db && InTransaction())
		Commit();	// attempt to commit
	iDatabase.Close();
	}

//
// Runs the incremental DDL object to completion.
//
LOCAL_C void CompleteDDLL(CDbIncremental* aIncremental,TInt& aStep)
	{
	__ASSERT((aIncremental==0)==(aStep==0));
	if (!aIncremental)
		return;
	aIncremental->PushL();
	do aIncremental->NextL(aStep);
		while (aStep!=0);
	CleanupStack::PopAndDestroy();	// aIncrmental
	}

//
// Runs the DML incremental object to completion.
//
LOCAL_C void CompleteDMLL(CDbIncremental* aIncremental,TInt& aRows)
	{
	__ASSERT((aIncremental==0)!=(aRows==0));
	if (!aIncremental)
		return;
	aIncremental->PushL();
	while (aIncremental->NextL(aRows))
		;
	CleanupStack::PopAndDestroy();	// aIncremental
	}

LOCAL_C TInt Property(const RDbHandle<CDbDatabase>& aDb,CDbDatabase::TProperty aProperty)
	{
	return aDb->Property(aProperty);
	}

LOCAL_C TInt Utility(RDbHandle<CDbDatabase>& aDb,CDbDatabase::TUtility aType)
	{
	TInt step;
	TRAPD(r,CompleteDDLL(aDb->UtilityL(aType,step),step));
	return r;
	}

/**
Reports the damage status of the database.
The function checks database indexes and returs true if some of them are broken.

@return True if the database is damaged, false otherwise.
*/
EXPORT_C TBool RDbDatabase::IsDamaged() const
	{
	return Property(iDatabase,CDbDatabase::EIsDamaged);
	}

/**
Synchronous database recovery.
Recover() will try to rebuild database indexes if they are broken.
If the database data is corrupted, it cannot be recovered.

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the write 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::Recover()
	{
	return Utility(iDatabase,CDbDatabase::ERecover);
	}

/**
Returns the currently available size information for the database. 
This comprises a size in bytes for the database objects and a percentage used value which indicates 
how much of that size is live data-the remainder may be available for compaction. 
Some types of database may not be able to report this information, e.g. a RDbStoreDatabase, 
and others may need to have UpdateStats() in order to provide valid data. 
In these cases, the values in the RDbDatabase::TSize structure will contain an error value to indicate this.

@return RDbDatabase::TSize object, containing the database size and the percentage used value.
*/
EXPORT_C RDbDatabase::TSize RDbDatabase::Size() const
	{
	TSize size;
	size.iSize=Property(iDatabase,CDbDatabase::ESize);
	size.iUsage=Property(iDatabase,CDbDatabase::EUsage);
	return size;
	}

/**
Update any calculated statistics for the database. 
Note that this can take an extended time to complete, 
so an incremental form is also provided - RDbIncremental::UpdateStats().

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the write 
            access policy for the database.
            
@see RDbIncremental::UpdateStats()
*/
EXPORT_C TInt RDbDatabase::UpdateStats()
	{
	return Utility(iDatabase,CDbDatabase::EStats);
	}

/**
Synchronous database compaction.
Compacts the database and returns when complete. 
Note that this can take an extended time to complete, so an incremental form is also provided.
There is a complementary interface to calculate and report database size and usage information, which
can be used by the clients to determine when it may be appropriate to compact the database. 

@see RDbIncremental::Compact()
@see RDbDatabase::UpdateStats()
@see RDbDatabase::Size()

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the write 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::Compact()
	{
	return Utility(iDatabase,CDbDatabase::ECompact);
	}


/**
Drops the tables and destroys the database.
This handle is closed on successful destruction.

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::Destroy()
	{
	CDbTableNames* t=NULL;
	TRAPD(r,t=TableNamesL());
	if (r!=KErrNone)
		return r;
	TInt ii=t->Count();
	do
		{
		if (--ii<0)
			{
			r=iDatabase->Destroy();
			if (r==KErrNone)
				iDatabase.Close();
			break;
			}
		r=DropTable((*t)[ii]);
		} while (r==KErrNone);
	delete t;
	return r;
	}

/**
Begins a transaction.

DBMS server only supports one 'granularity' of transaction lock: the whole database. 
Beginning a transaction locks the database, and this can fail if another client has already got a lock which 
excludes this client.
If the same client has already locked the database it will be panic'd.
The function is guaranteed to return KErrNone for client-side access. 

DBMS transactions do not provide any form of isolation between the clients: 
while one client is updating a table within a transaction, other clients will be able to see the changes as 
they are made. As a result, if a client retrieves two separate rows  from a database there is no automatic 
guarantee that the data being retrieved has not been changed between the reads - this can lead to 
an 'inconsistent read'. A client can prevent an update while retrieving related rows by enclosing the individual 
reads within a transaction. Such a transaction will not modify the database and only operates as a read-lock: 
releasing such a lock using Commit() or Rollback() will not affect the database in any way.

How RDbDatabase::Begin() works:
 - on a shared database Begin() will attempt to get a shared read-lock on the database, and will fail with 
   KErrLocked if anyone has an exclusive write-lock. Other clients with read-locks will not cause this operation 
   to fail.
 - any operation which will modify the database attempts to gain an exclusive write-lock on the database, 
   and will fail with KErrLocked if anyone else has any lock on the database. If the current client already has 
   a read-lock as a result of calling Begin(), then it will be upgraded to an exclusive write-lock.
 - Commit() or Rollback() after a read-lock has been acquired (but not a write-lock) will release that client's 
   lock. The database will only be considered to be unlocked when all such locks are removed by all clients, 
   when it will report a EUnlock event to any notifiers.
 - Commit() or Rollback() after a write-lock has been acquired will release the lock, and report the ECommit or 
   ERollback event to any notifiers.
 - automatic transactions will be used as at present if updates are made outside of explicit transactions, 
   and such updates will also be able to fail with KErrLocked if an exclusive lock cannot be acquired.
   
Allowing read-locks to be shared enables greater concurrency at the same time as providing some safe guard 
against inconsistent reads. It does, however, lead to the possibility of deadlock: two clients wanting to update 
the database can reach deadlock if they both Begin() a transaction before either of them starts an update, 
then one client's read-lock will prevent the other from upgrading to a write lock and vice versa. The only way out
of this is to code the clients in such a way as to back out of such a deadlock situation, rather than retry 
forever without releasing the locks. 

A client will be able to change the database schema while other clients are using the database, 
as long as the other clients have no locks on it. As described above, other clients may find that their
rowsets are then invalidated asynchronously as a result of this.

@return KErrNone The operation has completed successfully;
        KErrLocked, the database is locked by another client;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy either the read, write
			or the schema access policy for the database.
*/
EXPORT_C TInt RDbDatabase::Begin()
	{
	return iDatabase->Begin();
	}

/**
Commits the current transaction.

@return KErrNone The operation has completed successfully;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy either the read, write
			or the schema access policy for the database.
*/
EXPORT_C TInt RDbDatabase::Commit()
	{
	return iDatabase->Commit();
	}

/**
Rollbacks the current transaction.

@capability Note For a secure shared database, the caller must satisfy either the read, write
			or the schema access policy for the database.
*/
EXPORT_C void RDbDatabase::Rollback()
	{
	iDatabase->Rollback();
	}

/**
@return True if the database is in a transaction, false otherwise.
*/
EXPORT_C TBool RDbDatabase::InTransaction() const
	{
	return Property(iDatabase,CDbDatabase::EInTransaction);
	}

/**
Creates a table on the database.

@param aName Table name.
@param aColSet A set of column definitions which describe the table structure.
@param aPrimaryKey Primary key definition. If it is NULL, no primary key will be created for the new table.

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrAlreadyExists, a table with that name already exists;
        KErrArgument, empty column set, duplicated column name, invalid column length;
        KErrBadName, invalid table name, invalid column name (containing spaces for example);
        KErrNotSupported, unknown column type, unknown column attributes;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::CreateTable(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey)
	{
	TRAPD(r,iDatabase->CreateTableL(aName,aColSet,aPrimaryKey));
	return r;
	}

/**
Drops a table synchronously.

@param aName Table name.

@return KErrNone The operation has completed successfully;
        KErrNotFound, there is no table with the supplied name;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::DropTable(const TDesC& aName)
	{
	TInt step;
	TRAPD(r,CompleteDDLL(iDatabase->DropTableL(aName,step),step));
	return r;
	}

/**
Alters a table synchronously.

@param aName Table name.
@param aColSet A new set of column definitions which describe the table structure.

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrArgument, empty column set, duplicated column name, invalid column length;
        KErrNotFound, there is no table with the supplied name;
        KErrNotSupported, unknown column type, unknown column attributes;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::AlterTable(const TDesC& aName,const CDbColSet& aColSet)
	{
	TInt step;
	TRAPD(r,CompleteDDLL(iDatabase->AlterTableL(aName,aColSet,step),step));
	return r;
	}

/**
Creates an index synchronously.

@param aName Index name.
@param aTableName Table name.
@param aKey Index definition

@return KErrNone The operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrBadName, invalid index name (containing spaces for example);
        KErrAlreadyExists, an index with that name already exists;
        KErrNotFound, there is no table with that name;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::CreateIndex(const TDesC& aName,const TDesC& aTableName,const CDbKey& aKey)
	{
	TInt step;
	TRAPD(r,CompleteDDLL(iDatabase->CreateIndexL(aName,aTableName,aKey,step),step));
	return r;
	}

/**
Drops an index synchronously.

@param aName Index name.
@param aTableName Table name.

@return KErrNone The operation has completed successfully;
        KErrNotFound, there is no table or index with that name;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbDatabase::DropIndex(const TDesC& aName,const TDesC& aTableName)
	{
	TInt step;
	TRAPD(r,CompleteDDLL(iDatabase->DropIndexL(aName,aTableName,step),step));
	return r;
	}

/**
Executes a SQL statement on the database, and returns when it is complete. 
The aComp parameter is used in the execution of some SQL statements:
 - in CREATE INDEX statements it specifies the comparison operation used for text columns in the index key;
 - in UPDATE and DELETE statements it specifies the comparison operation used to evaluate the WHERE clause;
Other statements ignore the value of aComp.
A negative return value indicates an error. A successful DDL operation always returns KErrNone (zero), 
a successful DML operation returns the number of rows that were inserted, updated or deleted by the operation.

@param aSql A string of 16-bit wide characters containing one SQL statement.
@param aComparison Tells the DBMS how to compare text and long text columns.

@return Zero or positive value, the number of rows that were inserted, updated or deleted by the operation;
		KErrLocked, the database is locked by another client;
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that other system-wide error codes may also be returned.

@capability Note For a secure shared database, the caller must satisfy:
            - the schema access policy for the database, if the SQL statement is 
			  CREATE/DROP/ALTER; 
            - the write access policy for the table in the SQL, if the SQL statement is 
			  INSERT/UPDATE/DELETE; 
*/
EXPORT_C TInt RDbDatabase::Execute(const TDesC& aSql,TDbTextComparison aComparison)
	{
	TInt ret;
	TRAP(ret, \
		CDbIncremental* inc=iDatabase->ExecuteL(aSql,aComparison,ret); \
		if (inc) \
			ret ? CompleteDDLL(inc,ret) : CompleteDMLL(inc,ret); \
		)
	return ret;
	}

/**
Lists the tables on the database.

@return A pointer to a CDbTableNames container with table names. The caller is responsible for destroying
        the returned CDbTableNames instance.

@leave KErrNoMemory, an out of memory condition has occurred;
                     Note that the function may leave with other system-wide error codes.
*/
EXPORT_C CDbTableNames* RDbDatabase::TableNamesL() const
	{
	CDbTableNames* names=CDbTableNames::NewLC();
	iDatabase->TablesL(*names);
	CleanupStack::Pop();
	return names;
	}

/**
Returns the table definition.

@param aName Table name.

@return A pointer to a CDbColSet container with column definitions . The caller is responsible for destroying
        the returned CDbColSet instance.

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrNotFound, no table with that name exists;
                     Note that the function may leave with other system-wide error codes.
*/
EXPORT_C CDbColSet* RDbDatabase::ColSetL(const TDesC& aName) const
	{
	CDbColSet* set=CDbColSet::NewLC();
	iDatabase->ColumnsL(*set,aName);
	CleanupStack::Pop();
	return set;
	}

/**
Lists the indexes on a table.

@param aTable Table name.

@return A pointer to a CDbIndexNames container with column definitions . The caller is responsible for destroying
        the returned CDbIndexNames instance.

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrNotFound, no table with that name exists;
                     Note that the function may leave with other system-wide error codes.
*/
EXPORT_C CDbIndexNames* RDbDatabase::IndexNamesL(const TDesC& aTable) const
	{
	CDbIndexNames* names=CDbIndexNames::NewLC();
	iDatabase->IndexesL(*names,aTable);
	CleanupStack::Pop();
	return names;
	}

/**
Returns the index key.

@param aName Index name.
@param aTable Table name.

@return A pointer to a CDbKey object containing the index definition. The caller is responsible for destroying
        the returned CDbKey instance.

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrNotFound, no index or table with that name exists;
                     Note that the function may leave with other system-wide error codes.
*/
EXPORT_C CDbKey* RDbDatabase::KeyL(const TDesC& aName,const TDesC& aTable) const
	{
	CDbKey* key=CDbKey::NewLC();
	iDatabase->KeysL(*key,aName,aTable);
	CleanupStack::Pop();
	return key;
	}

// class CDbDatabase

CDbNotifier* CDbDatabase::NotifierL()
	{
	return AttachContext(this,OpenNotifierL());
	}

CDbIncremental* CDbDatabase::UtilityL(CDbDatabase::TUtility aType,TInt& aStep)
	{
	return AttachContext(this,OpenUtilityL(aType,aStep));
	}

CDbIncremental* CDbDatabase::DropTableL(const TDesC& aTable,TInt& aStep)
	{
	return AttachContext(this,OpenDropTableL(aTable,aStep));
	}

CDbIncremental* CDbDatabase::AlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep)
	{
	return AttachContext(this,OpenAlterTableL(aTable,aNewDef,aStep));
	}

CDbIncremental* CDbDatabase::CreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep)
	{
	return AttachContext(this,OpenCreateIndexL(aName,aTable,aKey,aStep));
	}

CDbIncremental* CDbDatabase::DropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep)
	{
	return AttachContext(this,OpenDropIndexL(aName,aTable,aStep));
	}

CDbIncremental* CDbDatabase::ExecuteL(const TDesC& aSql,TDbTextComparison aComparison,TInt& aInit)
	{
	return AttachContext(this,OpenExecuteL(aSql,aComparison,aInit));
	}

CDbCursor* CDbDatabase::ViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess anAccess)
	{
	return AttachContext(this,PrepareViewL(aQuery,aWindow,anAccess));
	}

CDbCursor* CDbDatabase::TableL(const TDesC &aName,RDbRowSet::TAccess anAccess)
	{
	return AttachContext(this,OpenTableL(aName,anAccess));
	}

//
// Reserved for future development
//
EXPORT_C void CDbDatabase::Reserved_1()
	{
	}

//
// Reserved for future development
//
EXPORT_C void CDbDatabase::Reserved_2()
	{
	}
