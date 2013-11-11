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
// DBMS client/server session class
// 
//

#include "SD_STD.H"

const TInt KTimesToRetryConnection = 2;


// Class RDbs

/** Returns the version of the DBMS server.

@return Version information. */
EXPORT_C TVersion RDbs::Version()
	{
	return TVersion(KDbmsMajorVersion,KDbmsMinorVersion,KDbmsBuildVersion);
	}

/** Makes a connection with the DBMS server. This function causes the server to 
start, if it is not already running.

This should be the first function called on an RDbs object after it is created.

Once a connection has been established, databases can be opened through the 
server.

Note that:

Threads can make any number of simultaneous connections to the DBMS server.

The session must stay open until all DBMS objects opened through the server 
have been closed.

The session is terminated by calling the Close() member function provided 
by the RSessionBase class.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
EXPORT_C TInt RDbs::Connect()
	{
	TInt retry = KTimesToRetryConnection;
	for (;;)
		{
		TInt r=DoConnect();
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=Dbs::Start();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

/** Marks the start point for checking the number of DBMS objects allocated in 
this session.

The function takes the current number of allocated DBMS objects as its benchmark 
number.

A call to this function is normally followed by a later call to ResourceCheck() 
which expects that the number of allocated DBMS objects to be the same as 
this benchmark number. */
EXPORT_C void RDbs::ResourceMark()
	{
	SessionMessage(EDbsResourceMark);
	}

/** Checks that the number of DBMS objects allocated in this session is the same 
as the benchmark number recorded by an earlier call to ResourceMark().

The function raises a CSession 2 panic if the current number of DBMS objects 
is not the same as that recorded by an earlier call to ResourceMark(). */
EXPORT_C void RDbs::ResourceCheck()
	{
	SessionMessage(EDbsResourceCheck);
	}

/** Returns the number of DBMS objects allocated in this session.

@return The number of DBMS allocated objects. */
EXPORT_C TInt RDbs::ResourceCount()
	{
	return SessionMessage(EDbsResourceCount);
	}

EXPORT_C void RDbs::SetHeapFailure(TInt aTAllocFail,TInt aRate)
	{
	SendReceive(DbsMessage(EDbsSetHeapFailure,KDbsSessionHandle),TIpcArgs(aTAllocFail,aRate));
	}

TInt RDbs::SessionMessage(TInt aFunction)
	{
	return SendReceive(DbsMessage(aFunction,KDbsSessionHandle));
	}

// Create the session
TInt RDbs::DoConnect()
	{
	return CreateSession(KDbsServerName,Version());
	}

/**
Reserves a prederfined amount of disk space on aDrive drive. 
At the moment the max possible amount, allowed by the file server, is reserved on aDrive drive.

Use this call to ensure that if your "delete records" transaction fails because of "out of
disk space" circumstances, you can get an access to the already reserved disk space and 
complete your transaction successfully the second time.

There is no strong, 100% guarantee, that the reserved disk space will always help the client
in "low memory" situations.

@param aDriveNo Drive number to reserve space on
@param aSpace This parameter is not used at the moment. The caller shall set it to 0.
@return KErrNoMemory Out of memory
        KErrArgument Invalid aDriveNo.
        KErrInUse The space has already been reserved
        RFs::ReserveDriveSpace() return value
@see RFs::ReserveDriveSpace()
*/
EXPORT_C TInt RDbs::ReserveDriveSpace(TInt aDriveNo, TInt /*aSpace*/)
	{
	return SendReceive(DbsMessage(EDbsReserveDriveSpace, KDbsSessionHandle), TIpcArgs(aDriveNo));
	}

/**
The method frees the reserved by the DBMS session disk space.

@param aDriveNo Drive number, which reserved space has to be freed.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no reserved disk space for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       the reserved disk space is granted but not released.
*/
EXPORT_C void RDbs::FreeReservedSpace(TInt aDriveNo)
    {
	(void)SendReceive(DbsMessage(EDbsFreeReservedSpace, KDbsSessionHandle), TIpcArgs(aDriveNo));
    }

/**
Grants access to a given area on a given drive for RDbs session.
Note this session must have reserved space on this particular drive in order to be 
granted access to the reserved area.

@param aDriveNo Drive number with a reserved disk space, an access to which is requested.
@return KErrArgument Invalid drive or there is no reserved disk space on aDriveNo.
        KErrInUse An access to the reserved space has already been given.
        RFs::GetReserveAccess() return values
@see RFs::GetReserveAccess()
*/
EXPORT_C TInt RDbs::GetReserveAccess(TInt aDriveNo)
	{
	return SendReceive(DbsMessage(EDbsReserveGetAccess, KDbsSessionHandle), TIpcArgs(aDriveNo));
	}

/**
Revokes access on a given drive for RDbs session.

@param aDriveNo Drive number with a reserved disk space, the access to which has to be released.
@return KErrNone.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no reserved disk space for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no granted access to the reserved disk space. 
*/
EXPORT_C TInt RDbs::ReleaseReserveAccess(TInt aDriveNo)
	{
	(void)SendReceive(DbsMessage(EDbsReserveReleaseAccess, KDbsSessionHandle), TIpcArgs(aDriveNo));
    return KErrNone;
	}

// Class RDbNamedDatabase


/**
Opens an existing shared secure or non-secure database.
Max allowed database name length (with the extension) is KDbMaxName symbols.

In this "client-server" mode the database can be shared with the other clients.

For opening a single, non-shareable connection to the database, see RDbNamedDatabase::Open(), which first
argument is a RFs reference.

@param aDbs A reference to DBMS session instance.
@param aDatabase The name of the file that hosts the database. If this is
                 a secure database, then the format of the name must be:
                 \<drive\>:\<database file name excluding the path\>.
                 If this is a non-secure database, then aDatabase has to be the full database file name.
@param aFormat Database format string. For shared secure databases the string format is: "SECURE[UID]", 
			   where UID is the database security policy UID. "SECURE" keyword is case insensitive.
			   For shared non-secure databases, the default parameter value (TPtrC()) can be used.
@return KErrNone if successful otherwise one of the system-wide error codes, including:
		KErrNotFound - the database is not found;
		KErrArgument - bad argument, including null/invaluid uids, the database name includes a path;
		KErrPermissionDenied - the caller has not enough rights to do the operation;

@capability Note For a secure shared database, the caller must satisfy the read,
            the write or the schema access policy for the database.

@see RDbNamedDatabase::Open(RFs& aFs, const TDesC& aSource, const TDesC& aFormat, TAccess aMode).

@publishedAll
@released
*/
EXPORT_C TInt RDbNamedDatabase::Open(RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat)
	{
	TRAPD(r,iDatabase=CDbsDatabase::NewL(aDbs,aDatabase,aFormat));
	return r;
	}

