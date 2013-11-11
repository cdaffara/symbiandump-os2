// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SQL Client side API header
// 
//

/**
 @file
 @publishedAll
 @released
*/
#ifndef __SQLDB_H__
#define __SQLDB_H__

#ifndef __S32STD_H__
#include <s32std.h>	//RReadStream, RWriteStream
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <sqlresourcetester.h>
#endif

//Forward declarations
class CSqlSecurityPolicy;
class RSqlDatabase;
class CSqlDatabaseImpl;
class RSqlStatement;
class CSqlStatementImpl;
class RSqlColumnReadStream;
class RSqlParamWriteStream;
class TSqlScalarFullSelectQuery;
class RSqlBlob;
class RSqlBlobReadStream;
class RSqlBlobWriteStream;
class TSqlResourceProfiler;

/**
Used to specify that the ROWID of the most recently inserted record
from the specified database connection should be used as the ROWID 
in a call to directly access a blob.

@see RSqlBlobReadStream
@see RSqlBlobWriteStream
@see TSqlBlob

@publishedAll
@released
*/
const TInt KSqlLastInsertedRowId = -1;

/**
A container for the security policies for a shared SQL database.

The container can contain:
- security policies that apply to the database.
- security policies that apply to individual database objects, i.e. database tables.

For the database, you use RSqlSecurityPolicy::SetDbPolicy() to apply a separate
security policy to:
- the database schema.
- read activity on the database.
- write activity on the database.

For database tables, you use RSqlSecurityPolicy::SetPolicy() to apply a separate
security policy to:
- write activity on each named database table.
- read activity on each named database table.

A client uses a RSqlSecurityPolicy object to create a secure database. It does this by:
- creating a RSqlSecurityPolicy object.
- setting all the appropriate security policies into it.
- passing the object as an argument to RSqlDatabase::Create().
- closing the RSqlSecurityPolicy object on return from RSqlDatabase::Create().

Once a secure shared database has been created with specific security policies,
these policies are made persistent and cannot be changed during the life of
that database.

Security policies are encapsulated by TSecurityPolicy objects.
The general usage pattern is to create the security policies container object
(RSqlSecurityPolicy) using a default security policy (TSecurityPolicy), and then
to assign more specific 'overriding' security policies.

The following code fragment shows how you do this:
   
@code
TSecurityPolicy defaultPolicy;
RSqlSecurityPolicy securityPolicy;
RSqlDatabase database;
TInt err;

// Create security policies container object using a default security policy.
securityPolicy.Create(defaultPolicy); 

// Set up policy to apply to database schema
// and assign it
TSecurityPolicy schemaPolicy;
...
err = securityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, schemaPolicy);

// Set up policy to apply to write activity on the database
// and assign it
TSecurityPolicy writePolicy;
...
err = securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, writePolicy);

// Set up policy to apply to write activity to the database table named "Table1"
// and assign it
TSecurityPolicy tablePolicy1;
...
err = securityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, _L("Table1"), RSqlSecurityPolicy::EWritePolicy, tablePolicy1);

// Set up policy to apply to read activity to the database table named "Table2"
TSecurityPolicy tablePolicy2;
err = securityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, _L("Table2"), RSqlSecurityPolicy::EReadPolicy, tablePolicy2);

// Create the database, passing the security policies
err = database.Create(KDatabaseName, securityPolicy);

// We can close the RSqlSecurityPolicy object.
securityPolicy.Close();
@endcode

Note that in this example code fragment, the client has not assigned specific
overriding policies for all possible cases; for example, no overriding policy
has been assigned to control read activity on the database, read activity
on "Table1", nor write activity on "Table2".
For these cases, the default security policy will apply.

A client can also retrieve a database's security policies by calling
RSqlDatabase::GetSecurityPolicy(); this returns a RSqlSecurityPolicy object
containing the security policies. Note that it is the client's responsibility
to close the RSqlSecurityPolicy object when the client no longer needs it. The
following code fragment suggests how you might do this:

@code
RSqlDatabase database;
RSqlSecurityPolicy securityPolicy;

// Retrieve the security policies; on return from the call to 
// GetSecurityPolicy(), the RSqlSecurityPolicy object passed 
// to this function will contain the security policies.
database.GetSecurityPolicy(securityPolicy);
...
// This is the security policy that applies to database schema
TSecurityPolicy schemaPolicy = securityPolicy.DbPolicy(RSqlSecurityPolicy::ESchemaPolicy);
...
// This is the security policy that applies to write activity to the database
// table named "Table1".
TSecurityPolicy writePolicy = securityPolicy.Policy(RSqlSecurityPolicy::ETable, _L("Table1"), RSqlSecurityPolicy::EWritePolicy);
...
// Close the RSqlSecurityPolicy object when no longer needed.
securityPolicy.Close();
@endcode

Note that in the cases where an 'overriding' security policy was not originally assigned,
then the security policy returned will simply be the default security policy.

Note: The database security policies are used to control the access to the objects (tables, indexes, triggers, views)
in the main database. The access to the temporary tables, indexes, etc. is not a subject of any restrictions, e.g.
a client with "read" database security policy only can create and use temporary tables, views, indexes, triggers.

@see TSecurityPolicy
@see RSqlDatabase
@see RSqlSecurityPolicy::SetDbPolicy()
@see RSqlSecurityPolicy::SetPolicy()

@publishedAll
@released
*/
class RSqlSecurityPolicy
	{
	friend class RSqlDatabase;
	
public:
	/**
	Defines a set of values that represents the database security policy types.
	Each database security policy type refers to a set of capabilities encapsulated in 
	a TSecurityPolicy object. The TSecurityPolicy object defines what capabilities the calling
	application must have in order to perform partiqular database operation.
	@see TSecurityPolicy
	*/
	enum TPolicyType 
		{
		/**
		Schema database security policy. An application with schema database security policy can 
		modify the database schema, write to database, read from database.
		*/
		ESchemaPolicy, 
		/**
		Read database security policy. An application with read database security policy can 
		read from database.
		*/
		EReadPolicy, 
		/**
		Write database security policy. An application with write database security policy can 
		write to database.
		*/
		EWritePolicy
		};
	/**
	Not currently supported.

	Defines a set of values that represents the database objects which can be protected by 
	database security policy types.
	*/
	enum TObjectType 
		{
		ETable
		};
	IMPORT_C RSqlSecurityPolicy();
	IMPORT_C TInt Create(const TSecurityPolicy& aDefaultPolicy);
	IMPORT_C void CreateL(const TSecurityPolicy& aDefaultPolicy);
	IMPORT_C void Close();
	IMPORT_C TInt SetDbPolicy(TPolicyType aPolicyType, const TSecurityPolicy& aPolicy);
	IMPORT_C TInt SetPolicy(TObjectType aObjectType, const TDesC& aObjectName, TPolicyType aPolicyType, const TSecurityPolicy& aPolicy);
	IMPORT_C TSecurityPolicy DefaultPolicy() const;
	IMPORT_C TSecurityPolicy DbPolicy(TPolicyType aPolicyType) const;
	IMPORT_C TSecurityPolicy Policy(TObjectType aObjectType, const TDesC& aObjectName, TPolicyType aPolicyType) const;
	
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
		
private:	
	void Set(CSqlSecurityPolicy& aImpl);
	CSqlSecurityPolicy& Impl() const;
					
private:
	CSqlSecurityPolicy* iImpl;
	};
	
/**
A handle to a SQL database.

A RSqlDatabase object is, in effect, a handle to the SQL database. A client can:
- create a SQL database by calling RSqlDatabase::Create().
- open an existing SQL database by calling RSqlDatabase::Open().
- close a SQL database by calling RSqlDatabase::Close().
- copy a SQL database by calling RSqlDatabase::Copy().
- delete a SQL database by calling RSqlDatabase::Delete().
- attach a SQL database to current database connection by calling RSqlDatabase::Attach().
- detach a SQL database from current database connection by calling RSqlDatabase::Detach().

The RSqlDatabase handles are not thread-safe.

A client can create either a non-secure database or a secure database,
depending on the variant of RSqlDatabase::Create() that is used.
- a non-secure database is created if the RSqlDatabase::Create(const TDesC&) variant is used.
- a secure database is created if the RSqlDatabase::Create(const TDesC&, const RSqlSecurityPolicy&)
variant is used. In this case, a container containing a collection of security
policies needs to be set up first and passed to this Create() function.
See references to RSqlSecurityPolicy for more information on security policies.

A client can also specify how it wants a transaction to interact with
other transactions that may be running concurrently. The various ways in which
transactions can interact (i.e. how one transaction can affect another) are
referred to as "transaction isolation levels", and are defined by the values
of the TIsolationLevel enum. A client specifies this by calling RSqlDatabase::SetIsolationLevel().

Each of the various flavours of Open and Create allows the optional provision of a
configuration string. It is acceptable for this string to be missing.
In the case where the string is missing, the config in the SqlServer.sql file
will be used. If that does not exist then the MMH macro definitions will be used.

The config string is in the format PARAM=VALUE; PARAM=VALUE;...

Allowed parameters are:
	cache_size=nnnn
	page_size=nnnn
	encoding=UTF8|UTF16

Badly formed config strings are reported as KErrArgument

The string may not exceed 255 characters.

Please note that a database can only be accessed within the thread where it has been created. It is then not possible
to create a database from thread1 and access it from thread2.

A client calls RSqlDatabase::Exec() to execute SQL statements.
@see RSqlDatabase::Create()
@see RSqlDatabase::Open()
@see RSqlDatabase::Close()
@see RSqlDatabase::Copy()
@see RSqlDatabase::Delete()
@see RSqlDatabase::Attach()
@see RSqlDatabase::Detach()
@see RSqlDatabase::SetIsolationLevel()
@see RSqlDatabase::Exec()
@see TIsolationLevel
@see RSqlSecurityPolicy

@publishedAll
@released
*/
class RSqlDatabase
	{
	friend class RSqlStatement;
	friend class TSqlScalarFullSelectQuery;
	friend class RSqlBlob;
	friend class RSqlBlobReadStream;
	friend class RSqlBlobWriteStream;
	friend class TSqlResourceProfiler;
	
public:
	/**
	Defines a set of values that represents the transaction isolation level.
	
	A transaction isolation level defines the way in which a transaction
	interacts with other transactions that may be in progress concurrently.
	
	A client sets the transaction isolation level by calling SetIsolationLevel()
	
	@see RSqlDatabase::SetIsolationLevel()
	*/
	enum TIsolationLevel 
		{
		/**
		A transaction can read uncommitted data, i.e. data that is being changed
		by another transaction, which is still in progress.
		
		This means that
		- a 'database read' transaction will not block 'database write' transactions
		being performed by different database connections on the same shared database.
		- a 'database read' transaction will not be blocked by 'database write'
		transactions performed by the same database connection.
		- concurrent 'database write' transactions are prevented.
		
		This transaction isolation level can be set at any time during
		the lifetime of the database.
		
		@see TIsolationLevel
		@see RSqlDatabase::SetIsolationLevel()
		*/
		EReadUncommitted, 

		/**
		Not currently supported.
		
		A transaction cannot read uncommitted data. "Dirty reads" are prevented.
		
		"Dirty read" is a data inconsistency type which can be described with the following example:
		- Transaction A updates TableA.Column1 value from 1 to 2;
		- Transaction B reads TableA.Column1 value;
		- Transaction A rolls back and restores the original value of TableA.Column1 (1);
		- Transaction B ends showing that TableA.Column1 value is 2, even though, logically and transactionally, 
		  this data never really even existed in the database because Transaction A never committed that change 
		  to the database;
		
		@see TIsolationLevel
		@see RSqlDatabase::SetIsolationLevel()
		*/
		EReadCommitted, 
		
		/**
		Not currently supported.
		
		A transaction cannot change data that is being read by a different transaction. 
		"Dirty reads" and "non-repeatable reads" are prevented.

		"Non-repeatable reads" is a data inconsistency type which can be described with the following example:
		- Transaction A reads TableA.Column1 value which is 1;
		- Transaction B updates TableA.Column1 value from 1 to 2;
		- Transaction B commits the chages;
		- Transaction A reads TableA.Column1 value again. Transaction A has inconsistent data because TableA.Column1 
		  value now is 2 instead of 1, all within the scope of the same Transaction A;
		
		@see TIsolationLevel
		@see RSqlDatabase::SetIsolationLevel()
		*/
		ERepeatableRead, 
		
		/**
		Any number of 'database read' transactions can be performed concurrently
		by different database connections on the same shared database.
		
		Only one 'database write' transaction can be performed at any one time. If a
		'database write' transaction is in progress, then any attempt to start
		another 'database read' or 'database write' transaction will be blocked
		until the first 'database write' transaction has completed.
		
		This is the default isolation level, if no isolation level is
		explicitly set.
		
		"Dirty reads", "non-repeatable" reads and "phantom reads" are prevented.
		
		"Phantom reads" is a data inconsistency type which can be described with the following example:
		- Transaction A reads all rows that have Column1 = 1;
		- Transaction B inserts a new row which has Column1 = 1;
		- Transaction B commits;
		- Transaction A updates all rows that have Column1 = 1. This will also update the row that 
		  Transaction B inserted, because Transaction A must read the data again in order to update it.
		- Transaction A commits;
		
		@see TIsolationLevel
		@see RSqlDatabase::SetIsolationLevel()
		*/
		ESerializable
		};
	/**
	This structure is used for retrieving the database size and database free space.
	@see RSqlDatabase::Size(TSize&)
	*/
	struct TSize
		{
		/** The database size in bytes*/
		TInt64	iSize;
		/** The database free space in bytes*/
		TInt64	iFree;
		};

	/** If this value is used as an argument of RSqlDatabase::Compact() (aSize argument), then all free space will be removed */
	enum {EMaxCompaction = -1};
		
	IMPORT_C RSqlDatabase();
	
	IMPORT_C TInt Create(const TDesC& aDbFileName, const TDesC8* aConfig=NULL);
	IMPORT_C TInt Create(const TDesC& aDbFileName,
			const RSqlSecurityPolicy& aSecurityPolicy, const TDesC8* aConfig=NULL);
	IMPORT_C TInt Open(const TDesC& aDbFileName, const TDesC8* aConfig=NULL);
	IMPORT_C void CreateL(const TDesC& aDbFileName, const TDesC8* aConfig=NULL);
	IMPORT_C void CreateL(const TDesC& aDbFileName,
			const RSqlSecurityPolicy& aSecurityPolicy, const TDesC8* aConfig=NULL);
	IMPORT_C void OpenL(const TDesC& aDbFileName, const TDesC8* aConfig=NULL);
	
	IMPORT_C void Close();
	
	IMPORT_C TInt Attach(const TDesC& aDbFileName, const TDesC& aDbName);
	IMPORT_C TInt Detach(const TDesC& aDbName);
	
	IMPORT_C static TInt Copy(const TDesC& aSrcDbFileName, const TDesC& aDestDbFileName);
	IMPORT_C static TInt Delete(const TDesC& aDbFileName);
	
	IMPORT_C TInt GetSecurityPolicy(RSqlSecurityPolicy& aSecurityPolicy) const;
	IMPORT_C void GetSecurityPolicyL(RSqlSecurityPolicy& aSecurityPolicy) const;
	
	IMPORT_C TInt SetIsolationLevel(TIsolationLevel aIsolationLevel);
	
	IMPORT_C TInt Exec(const TDesC& aSqlStmt);
	IMPORT_C TInt Exec(const TDesC8& aSqlStmt);
	
	IMPORT_C void Exec(const TDesC& aSqlStmt, TRequestStatus& aStatus);
	IMPORT_C void Exec(const TDesC8& aSqlStmt, TRequestStatus& aStatus);
	
	IMPORT_C TPtrC LastErrorMessage() const;
	IMPORT_C TInt64 LastInsertedRowId() const; 
	
	IMPORT_C TBool InTransaction() const;
	IMPORT_C TInt Size() const;
	IMPORT_C TInt Size(TSize& aSize, const TDesC& aDbName = KNullDesC) const;

	IMPORT_C TInt Compact(TInt64 aSize, const TDesC& aDbName = KNullDesC);
	IMPORT_C void Compact(TInt64 aSize, TRequestStatus& aStatus, const TDesC& aDbName = KNullDesC);
	
	IMPORT_C TInt ReserveDriveSpace(TInt aSize);
	IMPORT_C void FreeReservedSpace();
	IMPORT_C TInt GetReserveAccess();
	IMPORT_C void ReleaseReserveAccess();
	
private:
	CSqlDatabaseImpl& Impl() const;

private:
	CSqlDatabaseImpl* iImpl;
	};

/**
TSqlScalarFullSelectQuery interface is used for executing SELECT sql queries, which 
return a single row consisting of a single column value.

Examples.

CASE 1 - retrieving records count of a table:
@code
RSqlDatabase db;
//initialize db object....
.......
TSqlScalarFullSelectQuery fullSelectQuery(db);
TInt recCnt = fullSelectQuery.SelectIntL(_L("SELECT COUNT(*) FROM PersonTbl"));
@endcode

CASE 2 - retrieving specific column value using a condition in the SELECT statement:
@code
RSqlDatabase db;
//initialize db object....
.......
TSqlScalarFullSelectQuery fullSelectQuery(db);
TInt personId = fullSelectQuery.SelectIntL(_L("SELECT ID FROM PersonTbl WHERE Name = 'John'"));
@endcode

CASE 3 - retrieving a text column value, the receiving buffer is not big enough:
@code
RSqlDatabase db;
//initialize db object....
.......
TSqlScalarFullSelectQuery fullSelectQuery(db);
HBufC* buf = HBufC::NewLC(20);
TPtr name = buf->Des();
TInt rc = fullSelectQuery.SelectTextL(_L("SELECT Name FROM PersonTbl WHERE Id = 1"), name);
TEST(rc >= 0); //the function may return only non-negative values
if(rc > 0)
	{
	buf = buf->ReAllocL(rc);
	CleanupStack::Pop();	
	CleanupStack::PushL(buf);
	name.Set(buf->Des());
	rc = fullSelectQuery.SelectTextL(_L("SELECT Name FROM PersonTbl WHERE Id = 1"), name);
	TEST(rc == 0);
	}
CleanupStack::PopAndDestroy();//buf
@endcode

@see RSqlDatabase

@publishedAll
@released
*/
class TSqlScalarFullSelectQuery
	{
public:
	IMPORT_C TSqlScalarFullSelectQuery();
	IMPORT_C TSqlScalarFullSelectQuery(RSqlDatabase& aDatabase);
	IMPORT_C void SetDatabase(RSqlDatabase& aDatabase);

	IMPORT_C TInt SelectIntL(const TDesC& aSqlStmt);
	IMPORT_C TInt64 SelectInt64L(const TDesC& aSqlStmt);
	IMPORT_C TReal SelectRealL(const TDesC& aSqlStmt);
	IMPORT_C TInt SelectTextL(const TDesC& aSqlStmt, TDes& aDest);
	IMPORT_C TInt SelectBinaryL(const TDesC& aSqlStmt, TDes8& aDest);

	IMPORT_C TInt SelectIntL(const TDesC8& aSqlStmt);
	IMPORT_C TInt64 SelectInt64L(const TDesC8& aSqlStmt);
	IMPORT_C TReal SelectRealL(const TDesC8& aSqlStmt);
	IMPORT_C TInt SelectTextL(const TDesC8& aSqlStmt, TDes& aDest);
	IMPORT_C TInt SelectBinaryL(const TDesC8& aSqlStmt, TDes8& aDest);
	
private:
	inline CSqlDatabaseImpl& Impl() const;
	
private:	
	CSqlDatabaseImpl* iDatabaseImpl;
	};

/**
An enumeration whose values represent the supported database column types.


@see RSqlStatement::ColumnType()

@publishedAll
@released
*/
enum TSqlColumnType 
	{
	/**
	Null column value.
	*/
	ESqlNull,
	
 	/**
 	32-bit integer column value.
 	*/  
	ESqlInt, 
	
 	/**
 	64-bit integer column value.
 	*/
	ESqlInt64, 
	
	/**
	64-bit floating point column value.
	*/
	ESqlReal, 
	
	/**
	Unicode text, a sequence of 16-bit character codes.
	*/
	ESqlText, 
	
	/**
	Binary data, a sequence of bytes.
	*/
	ESqlBinary
	};

/**
Represents an SQL statement.

An object of this type can be used to execute all types of SQL statements; this
includes SQL statements with parameters.

If a SELECT statament is passed to RSqlStatement::Prepare(), then the returned record set 
is forward only, non-updateable.

There are a number of ways that this object is used; here are some examples.

CASE 1 - the execution of a SQL statement, which does not return record set:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = stmt.Prepare(database, _L("INSERT INTO Tbl1(Fld1) VALUES(:Val)"));
TInt paramIndex = stmt.ParameterIndex(_L(":Val"));
for(TInt i=1;i<=10;++i)
	{
	err = stmt.BindInt(paramIndex, i);
	err = stmt.Exec();
	err = stmt.Reset();
	}
stmt.Close();
@endcode

The following pseudo code shows the general pattern:

@code
<RSqlStatement::Prepare()>
[begin:]
<RSqlStatement::Bind<param_type>()>
<RSqlStatement::Exec()>
[<RSqlStatement::Reset()>]
[<RSqlStatement::Bind<param_type>()>]
[<Goto :begin>]
@endcode

CASE 2 - the execution of a SQL statement, which returns a record set:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = stmt.Prepare(database, _L("SELECT Fld1 FROM Tbl1 WHERE Fld1 > :Val"));
TInt paramIndex = stmt.ParameterIndex(_L(":Val"));
err = stmt.BindInt(paramIndex, 5);
TInt columnIndex = stmt.ColumnIndex(_L("Fld1"));
while((err = stmt.Next()) == KSqlAtRow)
	{
	TInt val = stmt.ColumnInt(columnIndex);
	RDebug::Print(_L("val=%d\n"), val);
	}
if(err == KSqlAtEnd)
	<OK - no more records>;
else
	<process the error>;
stmt.Close();
@endcode

The following pseudo code shows the general pattern:

@code
<RSqlStatement::Prepare()>
[begin:]
<while (RSqlStatement::Next() == KSqlAtRow)>
	<do something with the records>
if(err == KSqlAtEnd)
	<OK - no more records>;
else
	<process the error>;
[<RSqlStatement::Reset()>]
[<RSqlStatement::Bind<param_type>()>]
[<Goto begin>]
@endcode

CASE 3.1 - SELECT statements: large column data processing, where the data is
copied into a buffer supplied by the client:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = stmt.Prepare(database, _L("SELECT BinaryField FROM Tbl1"));
TInt columnIndex = stmt.ColumnIndex(_L("BinaryField"));
while((err = stmt.Next()) == KSqlAtRow)
	{
	TInt size = stmt. ColumnSize(columnIndex);
	HBufC8* buf = HBufC8::NewL(size);
	err = stmt.ColumnBinary(columnIndex, buf->Ptr());
	<do something with the data>;
	delete buf;
	}
if(err == KSqlAtEnd)
	<OK - no more records>;
else
	<process the error>;
stmt.Close();
@endcode

CASE 3.2 - SELECT statements: large column data processing, where the data is
accessed by the client without copying:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = stmt.Prepare(database, _L("SELECT BinaryField FROM Tbl1"));
TInt columnIndex = stmt.ColumnIndex(_L("BinaryField"));
while((err = stmt.Next()) == KSqlAtRow)
	{
	TPtrC8 data = stmt.ColumnBinaryL(columnIndex);
	<do something with the data>;
	}
if(err == KSqlAtEnd)
	<OK - no more records>;
else
	<process the error>;
stmt.Close();
@endcode

CASE 3.3 - SELECT statements, large column data processing (the data is accessed by 
the client without copying), leaving-safe processing:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = stmt.Prepare(database, _L("SELECT BinaryField FROM Tbl1"));
TInt columnIndex = stmt.ColumnIndex(_L("BinaryField"));
while((err = stmt.Next()) == KSqlAtRow)
	{
	TPtrC8 data;
	TInt err = stmt.ColumnBinary(columnIndex, data);
	if(err == KErrNone)
		{
		<do something with the data>;
		}
	}
if(err == KSqlAtEnd)
	<OK - no more records>;
else
	<process the error>;
stmt.Close();
@endcode

CASE 3.4 - SELECT statements: large column data processing, where the data is
accessed by the client using a stream:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = stmt.Prepare(database, _L("SELECT BinaryField FROM Tbl1"));
TInt columnIndex = stmt.ColumnIndex(_L("BinaryField"));
while((err = stmt.Next()) == KSqlAtRow)
	{
	RSqlColumnReadStream stream;
	err = stream.ColumnBinary(stmt, columnIndex);
	<do something with the data in the stream>;
	stream.Close();
	}
if(err == KSqlAtEnd)
	<OK - no more records>;
else
	<process the error>;
stmt.Close();
@endcode

CASE 4 - the execution of a SQL statement with parameter(s), some of which may
be large text or binary values:

@code
RSqlDatabase database;
.........
RSqlStatement stmt;
TInt err = 
	stmt.Prepare(database, _L("UPDATE Tbl1 SET LargeTextField = :LargeTextVal WHERE IdxField = :KeyVal"));
TInt paramIndex1 = stmt.ParameterIndex(_L(":LargeTextVal"));
TInt paramIndex2 = stmt.ParameterIndex(_L(":KeyVal"));
for(TInt i=1;i<=10;++i)
	{
	RSqlParamWriteStream stream;
	err = stream.BindText(stmt, paramIndex1);
	<insert large text data into the stream>;
	stream.Close();
	err = stmt.BindInt(paramIndex2, i);
	err = stmt.Exec();
	stmt.Reset();
	}
stmt.Close();
@endcode

The following table shows what is returned when the caller uses a specific
column data retrieving function on a specific column type.

@code
--------------------------------------------------------------------------------
Column type | ColumnInt() ColumnInt64() ColumnReal() ColumnText() ColumnBinary()
--------------------------------------------------------------------------------
Null........|.0...........0.............0.0..........KNullDesC....KNullDesC8
Int.........|.Int.........Int64.........Real.........KNullDesC....KNullDesC8 
Int64.......|.clamp.......Int64.........Real.........KNullDesC....KNullDesC8 
Real........|.round.......round.........Real.........KNullDesC....KNullDesC8 
Text........|.0...........0.............0.0..........Text.........KNullDesC8   
Binary......|.0...........0.............0.0..........KNullDesC....Binary
--------------------------------------------------------------------------------
@endcode
Note the following definitions:
- "clamp": return KMinTInt or KMaxTInt if the value is outside the range that can be 
represented by the type returned by the accessor function.
- "round": the floating point value will be rounded up to the nearest integer.
If the result is outside the range that can be represented by the type returned
by the accessor function, then it will be clamped.

Note that when handling blob and text data over 2Mb in size it is recommended that the 
RSqlBlobReadStream and RSqlBlobWriteStream classes or the TSqlBlob class is used instead. 
These classes provide a more RAM-efficient way of reading and writing large amounts of 
blob or text data from a database.

@see KMinTInt
@see KMaxTInt
@see KNullDesC
@see KNullDesC8
@see RSqlBlobReadStream
@see RSqlBlobWriteStream
@see TSqlBlob

@publishedAll
@released
*/
class RSqlStatement
	{
	friend class RSqlColumnReadStream;
	friend class RSqlParamWriteStream;

public:
	IMPORT_C RSqlStatement();
	IMPORT_C TInt Prepare(RSqlDatabase& aDatabase, const TDesC& aSqlStmt);
	IMPORT_C TInt Prepare(RSqlDatabase& aDatabase, const TDesC8& aSqlStmt);
	IMPORT_C void PrepareL(RSqlDatabase& aDatabase, const TDesC& aSqlStmt);
	IMPORT_C void PrepareL(RSqlDatabase& aDatabase, const TDesC8& aSqlStmt);
	IMPORT_C void Close();
	IMPORT_C TBool AtRow() const;
	IMPORT_C TInt Reset();
	IMPORT_C TInt Exec();
	IMPORT_C void Exec(TRequestStatus& aStatus);
	IMPORT_C TInt Next();
	
	IMPORT_C TInt ParameterIndex(const TDesC& aParameterName) const;
	IMPORT_C TInt ColumnCount() const;
	IMPORT_C TInt ColumnIndex(const TDesC& aColumnName) const;
	IMPORT_C TSqlColumnType ColumnType(TInt aColumnIndex) const;
	IMPORT_C TInt DeclaredColumnType(TInt aColumnIndex, TSqlColumnType& aColumnType) const;
	IMPORT_C TInt ColumnSize(TInt aColumnIndex) const;
	
	IMPORT_C TInt BindNull(TInt aParameterIndex);
	IMPORT_C TInt BindInt(TInt aParameterIndex, TInt aParameterValue);
	IMPORT_C TInt BindInt64(TInt aParameterIndex, TInt64 aParameterValue);
	IMPORT_C TInt BindReal(TInt aParameterIndex, TReal aParameterValue);
	IMPORT_C TInt BindText(TInt aParameterIndex, const TDesC& aParameterText);
	IMPORT_C TInt BindBinary(TInt aParameterIndex, const TDesC8& aParameterData);
	IMPORT_C TInt BindZeroBlob(TInt aParameterIndex, TInt aBlobSize);
	
	IMPORT_C TBool IsNull(TInt aColumnIndex) const;
	IMPORT_C TInt ColumnInt(TInt aColumnIndex) const;
	IMPORT_C TInt64 ColumnInt64(TInt aColumnIndex) const;
	IMPORT_C TReal ColumnReal(TInt aColumnIndex) const;
	
	IMPORT_C TPtrC ColumnTextL(TInt aColumnIndex) const;
	IMPORT_C TInt ColumnText(TInt aColumnIndex, TPtrC& aPtr) const;
	IMPORT_C TInt ColumnText(TInt aColumnIndex, TDes& aDest) const;
	
	IMPORT_C TPtrC8 ColumnBinaryL(TInt aColumnIndex) const;
	IMPORT_C TInt ColumnBinary(TInt aColumnIndex, TPtrC8& aPtr) const;
	IMPORT_C TInt ColumnBinary(TInt aColumnIndex, TDes8& aDest) const;
	
	IMPORT_C TInt ColumnName(TInt aColumnIndex, TPtrC& aNameDest);
	IMPORT_C TInt ParameterName(TInt aParameterIndex, TPtrC& aNameDest);
	IMPORT_C TInt ParamName(TInt aParameterIndex, TPtrC& aNameDest);
private:
	CSqlStatementImpl& Impl() const;
	
private:
	CSqlStatementImpl* 	iImpl;
	
	};

/**
The read stream interface.

The class is used for reading the content of a column containing either 
binary data or text data.

The class derives from RReadStream, which means that all RReadStream public
member functions and predefined stream operators \>\> can be used to deal
with column data.

If the blob or text data is over 2Mb in size then it is recommended that the 
RSqlBlobReadStream or TSqlBlob class is used instead. These classes provide 
a more RAM-efficient way of reading large amounts of blob or text data from
a database.

The following two cases are typical:

CASE 1 - processing large binary column data.

@code
RSqlDatabase db;
<open/create "db" object>;
RSqlStatement stmt;
<prepare "stmt" object>;
TInt rc = stmt.Next();
if(rc == KSqlAtRow)
	{
	RSqlColumnReadStream colStream;
	CleanupClosePushL(colStream);
	User::LeaveIfError(colStream.ColumnBinary(stmt, <column_number>));
	TInt size = stmt.ColumnSize(<column_number>);
	//read the column data in a buffer ("buf" variable).
	//(or the column data can be retrieved in a smaller portions)
	colStream.ReadL(buf, size);
	//Close the stream
	CleanupStack::PopAndDestroy(&colStream);
	}
else
	{
	...
	}
@endcode

CASE 2 - processing large text column data.

@code
RSqlDatabase db;
<open/create "db" object>;
RSqlStatement stmt;
<prepare "stmt" object>;
TInt rc = stmt.Next();
if(rc == KSqlAtRow)
	{
	RSqlColumnReadStream colStream;
	CleanupClosePushL(colStream);
	User::LeaveIfError(colStream.ColumnText(stmt, <column_number>));
	TInt size = stmt.ColumnSize(<column_number>);
	//read the column data in a buffer ("buf" variable).
	//(or the column data can be retrieved in a smaller portions)
	colStream.ReadL(buf, size);
	//Close the stream
	CleanupStack::PopAndDestroy(&colStream);
	}
else
	{
	...
	}
@endcode

@see RSqlBlobReadStream
@see TSqlBlob

@publishedAll
@released
*/
class RSqlColumnReadStream : public RReadStream
	{
public:	
	IMPORT_C TInt ColumnText(RSqlStatement& aStmt, TInt aColumnIndex);
	IMPORT_C TInt ColumnBinary(RSqlStatement& aStmt, TInt aColumnIndex);
	IMPORT_C void ColumnTextL(RSqlStatement& aStmt, TInt aColumnIndex);
	IMPORT_C void ColumnBinaryL(RSqlStatement& aStmt, TInt aColumnIndex);

	};

/**
The write stream interface.

The class is used to set binary data or text data into a parameter. 
This is a also known as binding a parameter.

The class derives from RWriteStream, which means that all RWriteStream public
member functions and predefined stream operators \<\< can be used to deal with
the parameter data.

If the blob or text data is over 2Mb in size then it is recommended that the 
RSqlBlobWriteStream or TSqlBlob class is used instead. These classes provide 
a more RAM-efficient way of writing large amounts of blob or text data to
a database.

The following two cases are typical:

CASE 1 - binding a large binary parameter.

@code
RSqlDatabase db;
<open/create "db" object>;
RSqlStatement stmt;
<prepare "stmt" object>;//The SQL statement references large binary parameter
RSqlParamWriteStream paramStream;
CleanupClosePushL(paramStream);
User::LeaveIfError(paramStream.BindBinary(stmt, <parameter_number>));
//Write out the parameter data
paramStream.WriteL(..);
paramStream << <data>;
...
//Commit the stream
paramStream.CommitL();
//Continue with the statement processing issuing Next() or Exec().
TInt rc = stmt.Next();//rc = stmt.Exec()
//Close the stream
CleanupStack::PopAndDestroy(&paramStream);
@endcode

CASE 2 - binding a large text parameter.

@code
RSqlDatabase db;
<open/create "db" object>;
RSqlStatement stmt;
<prepare "stmt" object>;//The SQL statement references large text parameter
RSqlParamWriteStream paramStream;
CleanupClosePushL(paramStream);
User::LeaveIfError(paramStream.BindText(stmt, <parameter_number>));
//Write out the parameter data
paramStream.WriteL(..);
paramStream << <data>;
...
//Commit the stream
paramStream.CommitL();
//Continue with the statement processing issuing Next() or Exec().
TInt rc = stmt.Next();//rc = stmt.Exec()
//Close the stream
CleanupStack::PopAndDestroy(&paramStream);
@endcode

@see RSqlBlobWriteStream
@see TSqlBlob

@publishedAll
@released
*/
class RSqlParamWriteStream : public RWriteStream
	{
public:	
	IMPORT_C TInt BindText(RSqlStatement& aStmt, TInt aParameterIndex);
	IMPORT_C TInt BindBinary(RSqlStatement& aStmt, TInt aParameterIndex);
	IMPORT_C void BindTextL(RSqlStatement& aStmt, TInt aParameterIndex);
	IMPORT_C void BindBinaryL(RSqlStatement& aStmt, TInt aParameterIndex);

	};

/**
A direct handle to a blob, used for reading the content of the blob via a streaming interface.

The target blob is identified using the relevant database connection, table name, 
column name and ROWID of the record to which the blob belongs (also the attached
database name if the blob is contained in an attached database).

A blob in this context refers to the content of a BLOB or TEXT column, 
and a read handle can be opened on both types of column.
For TEXT columns it is important to note that no conversions are performed on 
data retrieved using this class - the data is returned as a stream of bytes.

The class derives from RReadStream and provides all of its streaming methods.
The SizeL() method can be used to check the total size of the blob, in bytes.

It is strongly recommended to use this class for reading the content of large blobs 
because it significantly reduces the amount of RAM that is used when compared to using the 
RSqlColumnReadStream, RSqlStatement::ColumnBinary(L) or RSqlStatement::ColumnText(L) APIs.

Specifically, it is recommended to use this class for blobs over 2Mb in size.
Indeed, in some circumstances where very large blobs are in use it may be impossible
to read the blob content using the legacy APIs (due to the server's finite RAM capacity), 
and this class may provide the only way to access the data.

The following code illustrates typical use cases of this class:

CASE 1 - reading large blob data from the last inserted record.

@code
RSqlDatabase db;
CleanupClosePushL(db);
<open/create "db" object>;
RSqlBlobReadStream rdStrm;
CleanupClosePushL(rdStrm);
rdStrm.OpenL(db, <table_name>, <column_name>);
HBufC8* buffer = HBufC8::NewLC(KBlockSize);
TPtr8 bufPtr(buffer->Des());
TInt size = rdStrm.SizeL();
while(size)
	{
	TInt bytesToRead = (size >= KBlockSize) ? KBlockSize : size ;
	rdStrm.ReadL(bufPtr, bytesToRead); // read the next block of data		
	<do something with the block of data>
	size =- bytesToRead;
	}
CleanupStack::PopAndDestroy(3); // buffer, rdStrm, db
@endcode

CASE 2 - reading large blob data from a selection of records.

@code
RSqlDatabase db;
CleanupClosePushL(db);
<open/create "db" object>;
RSqlStatement stmt;
CleanupClosePushL(stmt);
<prepare "stmt" object to SELECT the ROWIDs of a collection of blob objects>;
TInt rc = 0;
while((rc = stmt.Next()) == KSqlAtRow)
	{
	TInt64 rowid = stmt.ColumnInt64(0);	
	RSqlBlobReadStream rdStrm;
	CleanupClosePushL(rdStrm);
	rdStrm.OpenL(db, <table_name>, <column_name>, rowid);
	
	HBufC8* buffer = HBufC8::NewLC(KBlockSize);
	TPtr8 bufPtr(buffer->Des());
	TInt size = rdStrm.SizeL();
	while(size)
		{
		TInt bytesToRead = (size >= KBlockSize) ? KBlockSize : size ;
		rdStrm.ReadL(bufPtr, bytesToRead); // read the next block of data		
		<do something with the block of data>
		size =- bytesToRead;
		}
	CleanupStack::PopAndDestroy(2); // buffer, rdStrm
	}
CleanupStack::PopAndDestroy(2); // stmt, db
@endcode

@see RSqlBlobWriteStream
@see RSqlDatabase::LastInsertedRowId()

@publishedAll
@released
*/
class RSqlBlobReadStream : public RReadStream
	{
public:						
	IMPORT_C void OpenL(RSqlDatabase& aDb, const TDesC& aTableName, const TDesC& aColumnName, 
						TInt64 aRowId = KSqlLastInsertedRowId, const TDesC& aDbName = KNullDesC);
	IMPORT_C TInt SizeL();
	};

/**
A direct handle to a blob, used for writing the content of the blob via a streaming interface.

The target blob is identified using the relevant database connection, table name, 
column name and ROWID of the record to which the blob belongs (also the attached
database name if the blob is contained in an attached database).

A blob in this context refers to the content of a BLOB or TEXT column, 
and a write handle can be opened on both types of column, except if the
column is indexed, in which case the open call will fail with KSqlErrGeneral.
For TEXT columns it is important to note that no conversions are performed on data 
that is stored using this class - the data is simply stored as a stream of bytes.

The class derives from RWriteStream and provides all of its streaming methods.
The SizeL() method can be used to check the total size of the blob, in bytes.
Note that this class cannot be used to increase the size of a blob, only to modify 
the existing contents of a blob. An attempt to write beyond the end of a blob will
fail with KErrEof.

It is strongly recommended to use this class for writing the content of large blobs 
because it significantly reduces the amount of RAM that is used when compared to using 
the RSqlParamWriteStream, RSqlStatement::BindBinary or RSqlStatement::BindText APIs.

Specifically, it is recommended to use this class for blobs over 2Mb in size.
Indeed, in some circumstances where very large blobs are required it may be impossible
to create a blob or update its content using the legacy APIs (due to the server's finite 
RAM capacity), and this class may provide the only way to achieve this.

Using this class in combination with zeroblobs it is possible to create and manipulate 
blobs that are gigabytes in size. A zeroblob acts as a place-holder for a blob whose 
content is later written using this class and one can be created using an INSERT 
statement that either contains the SQLite 'zeroblob()' function or on which 
RSqlStatement::BindZeroBlob() has been executed.
Note that a zeroblob should be created in a column after which there are no columns 
that contain anything other than zeroblobs or NULLs, otherwise the zeroblob must be 
allocated in full in RAM.

When creating a zeroblob it is recommended, where possible, to create the zeroblob and
then write the blob content within the same transaction. Otherwise the zeroblob will 
have to be journalled before being written to.

It is also strongly recommended to execute calls to WriteL() within a transaction. 
If a leave occurs during a call to WriteL() then the current state of the blob object is
undefined and a ROLLBACK should be executed to return the blob object to its previous state.
Note that in order for a ROLLBACK to execute successfully all open RSqlBlobReadStream 
and RSqlBlobWriteStream handles and all open RSqlStatement objects must be closed 
before the ROLLBACK is executed.

The following code illustrates typical use cases of this class:

CASE 1 - creating a 5Mb blob.

@code
RSqlDatabase db;
CleanupClosePushL(db);
<open/create "db" object>;
CleanupStack::PushL(TCleanupItem(&DoRollback, &db)); // rollback function
TInt err = db.Exec(_L("BEGIN"));
<check err>
err = db.Exec(_L("INSERT INTO table1 VALUES(35, zeroblob(5242880))"));
<check err>
RSqlBlobWriteStream wrStrm;
CleanupClosePushL(wrStrm);
wrStrm.OpenL(db, <table_name>, <column_name>);
TInt size = wrStrm.SizeL();
while(size)
	{
	TInt bytesToWrite = (size >= KBlockSize) ? KBlockSize : size ;
	<fill a buffer 'buf' with this amount of the blob data>
	wrStrm.WriteL(buf); // write the next block of data		
	size =- bytesToWrite;
	}
CleanupStack::PopAndDestroy(&wrStrm);
CleanupStack::Pop(); // TCleanupItem
err = db.Exec(_L("COMMIT")); // blob data committed to disk
<check err>
CleanupStack::PopAndDestroy(&db);
@endcode

CASE 2 - updating a large blob in the last inserted record.

@code
RSqlDatabase db;
CleanupClosePushL(db);
<open/create "db" object>;
CleanupStack::PushL(TCleanupItem(&DoRollback, &db)); // rollback function
TInt err = db.Exec(_L("BEGIN"));
<check err>
RSqlBlobWriteStream wrStrm;
CleanupClosePushL(wrStrm);
wrStrm.OpenL(db, <table_name>, <column_name>);
<fill a buffer 'buf' with the changed blob data>
wrStrm.WriteL(buf); // update the blob
CleanupStack::PopAndDestroy(&wrStrm);
CleanupStack::Pop(); // TCleanupItem
err = db.Exec(_L("COMMIT")); // blob data committed to disk
<check err>
CleanupStack::PopAndDestroy(&db);
@endcode

@see RSqlBlobReadStream
@see RSqlDatabase::LastInsertedRowId()
@see RSqlStatement::BindZeroBlob()

@publishedAll
@released
*/
class RSqlBlobWriteStream : public RWriteStream
	{
public:
	IMPORT_C void OpenL(RSqlDatabase& aDb, const TDesC& aTableName, const TDesC& aColumnName, 
						TInt64 aRowId = KSqlLastInsertedRowId, const TDesC& aDbName = KNullDesC);
	IMPORT_C TInt SizeL();
	};

/**
Utility class that provides methods for reading and writing the entire content of 
a blob in a single call.

The target blob is identified using the relevant database connection, table name, 
column name and ROWID of the record to which the blob belongs (also the attached
database name if the blob is contained in an attached database).

The behaviour of the RSqlBlobReadStream class and the recommendations for using
it exist for the Get() and GetLC() methods of this class. Similarly, the behaviour 
of the RSqlBlobWriteStream class and the recommendations for using it exist for the 
SetL() method of this class.

In particular, it is strongly recommended to use this class or the RSqlBlobReadStream
and RSqlBlobWriteStream classes for reading and writing the content of large blobs 
because it significantly reduces the amount of RAM that is used when compared to using 
the legacy streaming and RSqlStatement APIs.

Specifically, it is recommended to use this class for blobs over 2Mb in size.
Indeed, in some circumstances where very large blobs are in use it may be impossible
to read or write to a blob using the legacy APIs (due to the server's finite 
RAM capacity), and this class or the RSqlBlobReadStream and RSqlBlobWriteStream classes 
may provide the only way to achieve this.

It is strongly recommended to execute calls to the SetL() method within a transaction. 
If a leave occurs during a call to SetL() then the current state of the blob object is 
undefined and a ROLLBACK should be executed to return the blob object to its previous state.
Note that in order for a ROLLBACK to execute successfully all open RSqlBlobReadStream 
and RSqlBlobWriteStream handles and all open RSqlStatement objects must be closed 
before the ROLLBACK is executed.

When using SetL() to update the content of a zeroblob it is recommended, where possible, 
to create the zeroblob and then call SetL() within the same transaction. 
Otherwise the zeroblob will have to be journalled before being written to.

The following code illustrates typical use cases of this class:

CASE 1 - retrieving the entire content of a large blob.

@code
RSqlDatabase db;
CleanupClosePushL(db);
<open/create "db" object>;
HBufC8* wholeBlob = TSqlBlob::GetLC(db, <table_name>, <column_name>, <rowid>);
<do something with the blob data>
CleanupStack::PopAndDestroy(2); // wholeBlob, db
@endcode


CASE 2 - creating a 4Mb blob.

@code
RSqlDatabase db;
CleanupClosePushL(db);
<open/create "db" object>;
CleanupStack::PushL(TCleanupItem(&DoRollback, &db)); // rollback function
TInt err = db.Exec(_L("BEGIN"));
<check err>
err = db.Exec(_L("INSERT INTO table1 VALUES(99, zeroblob(4194304))"));
<check err>
<fill a buffer 'buf' with 4Mb of blob data>
TSqlBlob::SetL(db, <table_name>, <column_name>, buf);
CleanupStack::Pop(); // TCleanupItem
err = db.Exec(_L("COMMIT")); // blob data committed to disk
<check err>
CleanupStack::PopAndDestroy(&db);
@endcode

@see RSqlBlobReadStream
@see RSqlBlobWriteStream
@see RSqlDatabase::LastInsertedRowId()
@see RSqlStatement::BindZeroBlob()

@publishedAll
@released
*/
class TSqlBlob
	{
public:					  		  	  
	IMPORT_C static HBufC8* GetLC(RSqlDatabase& aDb, 	
					     		  const TDesC& aTableName, 
					     		  const TDesC& aColumnName, 	
					     		  TInt64 aRowId = KSqlLastInsertedRowId,
					     		  const TDesC& aDbName = KNullDesC);
								  		  	  
	IMPORT_C static TInt Get(RSqlDatabase& aDb, 	
					 		 const TDesC& aTableName, 
					 		 const TDesC& aColumnName, 	
					 		 TDes8& aBuffer,
					 		 TInt64 aRowId = KSqlLastInsertedRowId,
					 		 const TDesC& aDbName = KNullDesC);			 		 

	IMPORT_C static void SetL(RSqlDatabase& aDb, 	
					  		  const TDesC& aTableName, 
					  		  const TDesC& aColumnName,
					  		  const TDesC8& aData,	
					  		  TInt64 aRowId = KSqlLastInsertedRowId,
					  		  const TDesC& aDbName = KNullDesC);				  
	};

/**
Defines a set of categories for the values returned by the SQL API.

A call to an SQL API may complete with a non-zero return code indicating that some
unexpected behaviour has occurred. This can be categorised in a number of ways,
for example, as a Symbian OS error, or as a database error etc. 

Callers to the SQL API may not want to be concerned with the detailed meaning of
a specific return code value, and may find it sufficient just to know the category
of the error.

The category associated with a specific return code can be found by passing the 
return code value to the function SqlRetCodeClass().

@publishedAll
@released
*/
enum TSqlRetCodeClass 
	{
	/**
	Indicates that a return code is just for information.
	
	This category corresponds to the SQL API return codes: KSqlAtRow and KSqlAtEnd. 
	
	@see SqlRetCodeClass()
	@see TSqlRetCodeClass
	@see KSqlAtRow 
	@see KSqlAtEnd
	*/
	ESqlInformation, 
	
	/**
	Indicates that a return code represents a database-specific error.
	
	This category corresponds to SQL API return codes in the range KSqlErrGeneral to KSqlErrStmtExpired.
	
	@see SqlRetCodeClass()
	@see TSqlRetCodeClass
	@see KSqlErrGeneral
	@see KSqlErrStmtExpired
	*/
	ESqlDbError,
	
	/**
	Indicates that a return code represents a Symbian OS error.
	
	This category corresponds to SQL API return codes in the range KErrPermissionDenied to KErrNone,
	
	@see SqlRetCodeClass()
	@see TSqlRetCodeClass
	@see KErrPermissionDenied
	@see KErrNone
	*/
	ESqlOsError 
	};

/**
An information type return code from a call to RSqlStatement::Next().

It means that the RSqlStatement object points to a valid row, and that
the user can access the column data using the appropriate RSqlStatement
member functions.

@see RSqlStatement::Next()
@see RSqlStatement
@see ESqlInformation
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlAtRow = 1;

/**
An information type return code from a call to RSqlStatement::Next().

It means that the RSqlStatement object does not point to a valid row,
and that column data accessors cannot be used.

@see RSqlStatement::Next()
@see RSqlStatement
@see ESqlInformation
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlAtEnd = 2;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates a general SQL error or a missing database.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrGeneral		= -311;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates an internal logic error in the SQL database engine, and specifically
that an internal consistency check within the SQL database engine has failed.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrInternal		= -312;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that access permission has been denied.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrPermission	= -313;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates an internal logic error in the SQL database engine, and specifically
that a callback routine requested an abort.

@publishedAll
@released
*/
const TInt KSqlErrAbort			= -314;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that the database file is locked.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrBusy			= -315;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that a table in the database is locked.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrLocked		= -316;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates an attempt to write to a database that is read-only.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrReadOnly		= -318;

/**
SQL database-specific error type. Operation terminated.

@publishedAll
@released
*/
const TInt KSqlErrInterrupt		= -319;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that a disk I/O error has occurred.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrIO			= -320;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that the database disk image is malformed.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrCorrupt		= -321;

/**
SQL database-specific error type. Table or record not found.

@publishedAll
@released
*/
const TInt KSqlErrNotFound		= -322;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that an insertion operation has failed because an autoincrement column used up 
all awailable rowids.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrFull			= -323;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates a failure to open the database file.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrCantOpen		= -324;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates a database lock protocol error.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrProtocol		= -325;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that the database is empty.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrEmpty			= -326;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that a prepared SQL statement is no longer valid 
and cannot be executed.

The most common reason for this return code is that the database schema was modified after
the SQL statement was prepared. The SQL statement must be prepared again
using the RSqlStatement::Prepare() member functions.

Another possible reason for this return code is a detached database.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrSchema		= -327;

/**
SQL database-specific error type. Too much data for one row.

@publishedAll
@released
*/
const TInt KSqlErrTooBig		= -328;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates an abort due to constraint violation.

"Constraint violation" means violation of one or more column constraints ("NOT NULL", "PRIMARY KEY",
"UNIQUE", "CHECK", "DEFAULT", "COLLATE" SQL keywords) or table constraints ("PRIMARY KEY", "UNIQUE", 
"CHECK" SQL keywords).

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrConstraint	= -329;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates a data type mismatch.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrMismatch		= -330;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates an internal logic error in the SQL database engine.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrMisuse		= -331;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that a parameter index value is out of range.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrRange			= -335;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that the file that has been opened is not a database file.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrNotDb			= -336;

/**
An SQL database-specific error type return code from a call to the SQL API.

It indicates that an SQL statement has expired, and needs to be prepared again.

@see RSqlStatement
@see ESqlDbError
@see TSqlRetCodeClass

@publishedAll
@released
*/
const TInt KSqlErrStmtExpired	= -360;

IMPORT_C TSqlRetCodeClass SqlRetCodeClass(TInt aSqlRetCode);

#endif //__SQLDB_H__
