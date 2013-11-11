// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <f32file.h>
#include <f32file64.h>
#include <s32strm.h>
#include <s32mem.h>
#include "IPCBuf.h"				//HIpcBuf
#include "SqlDbSession.h"		//RSqlDbSession
#include "SqlSrvStartup.h"		//StartSqlServer()
#include "SqlResourceTest.h"	//TSqlResourceTestData
#include "SqlSecurityImpl.h"	//CSqlSecurityPolicy
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlDbSessionTraces.h"
#endif
#include "SqlTraceDef.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////             TSqlFhCmdFunctor             ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
TSqlFhCmdFunctor derived classes are used by RSqlDbSession for sending to SQL server localy created/opened 
file handles.

@see RSqlDbSession
@see RSqlDbSession::CreateAndSendFileHandle()

@internalComponent
*/
NONSHARABLE_CLASS(TSqlFhCmdFunctor)
	{
public: 
	TSqlFhCmdFunctor(RSqlDbSession& aDbSession, const TDesC& aDbFileName, const TDesC8* aConfig=NULL) :
		iDbSession(aDbSession),
		iDbFileName(aDbFileName),
		iConfig(aConfig) // ownership not transferred
		{
		}
	virtual TInt operator()(RFile64& aFile, TBool aCreated, TBool aReadOnly) = 0;
	
public:	
	RSqlDbSession&	iDbSession;
	const TDesC& 	iDbFileName;
	const TDesC8*	iConfig;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////              TSqlFhOpenCmdFunctor         //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Implements the sending of ESqlSrvDbOpenFromHandle command to the SQL server.

@internalComponent
*/
NONSHARABLE_CLASS(TSqlFhOpenCmdFunctor) : public TSqlFhCmdFunctor
	{
public:	
	TSqlFhOpenCmdFunctor(RSqlDbSession& aDbSession, const TDesC& aDbFileName,const TDesC8* aConfig=NULL) :
		TSqlFhCmdFunctor(aDbSession, aDbFileName, aConfig)
		{
		}
	virtual TInt operator()(RFile64& aDbFile, TBool aCreated, TBool aReadOnly);
	
	};

/**
Sends a command to the SQL server to use the supplied file session and file handles for adopting,
when opening/creating a database.

Usage of the IPC call arguments:
 - Arg 0: [out]  The 32 bits of the argument are used as follow:
 @code
 MSB                                                                                 LSB
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 Ro Cr  C  C  C  C  C  C  C  C  C  C  C  C  C  C  F  F  F  F  F  F F F F F F F F F F F 
 @endcode
 Where:
 @code
  - "Ro" - read-only flag, true if the file is read-only;
  - "Cr" - create/open flag, true if the file was created, false if the file was opened;
  - "C"  - config string length;
  - "F"  - database file name length;
 @endcode
 - Arg 1: [out]  database file name + configuration string (if there is a configuration string)
 - Arg 2: [out]  file session handle
 - Arg 3: [out]  database file handle
*/		
TInt TSqlFhOpenCmdFunctor::operator()(RFile64& aDbFile, TBool aCreated, TBool aReadOnly)
	{
	const TInt KConfigStrLen = iConfig ? iConfig->Length() : 0;
	if(KConfigStrLen > KSqlSrvMaxConfigStrLen)
		{
		return KErrArgument;
		}
	TIpcArgs ipcArgs;
	HBufC* arg1Buf = HBufC::New(iDbFileName.Length() + KConfigStrLen);
	if(!arg1Buf)
		{
		return KErrNoMemory;	
		}
	TPtr arg1 = arg1Buf->Des();
	arg1.Copy(iDbFileName);
	if(iConfig)
		{
		TBuf<KSqlSrvMaxConfigStrLen> cfgBuf;
		cfgBuf.Copy(*iConfig);
		arg1.Append(cfgBuf);
		}
	TUint32 arg0 = iDbFileName.Length() | (KConfigStrLen << 16);
	if(aReadOnly)
		{
		arg0 |= 0x80000000;	
		}
	if(aCreated)
		{
		arg0 |= 0x40000000;	
		}
	ipcArgs.Set(0, arg0);
	ipcArgs.Set(1, &arg1);
	TInt err = aDbFile.TransferToServer(ipcArgs, 2, 3);
	if(err == KErrNone)
		{
		err = iDbSession.SendReceive(ESqlSrvDbOpenFromHandle, ipcArgs);
		}
	delete arg1Buf;	
	return err;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////              TSqlFhAttachCmdFunctor         ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Implements the sending of ESqlSrvDbAttachFromHandle command to the SQL server.

@internalComponent
*/
NONSHARABLE_CLASS(TSqlFhAttachCmdFunctor) : public TSqlFhCmdFunctor
	{
public:	
	TSqlFhAttachCmdFunctor(RSqlDbSession& aDbSession, const TDesC& aDbFileName,
		const TDesC& aDbName) :
		TSqlFhCmdFunctor(aDbSession, aDbFileName),
		iDbName(aDbName)
		{
		}
	virtual TInt operator()(RFile64& aDbFile, TBool aCreated, TBool aReadOnly);
	
private:
	void SerializeToStreamL(RWriteStream& aStream);
		
private:
	const TDesC&	iDbName;
	
	};
	
/**
Sends a command to the SQL server to use the supplied file session and file handles for adopting,
when attaching a database.

Usage of the IPC call arguments:
 - Arg 0: [out]  The 32 bits of the argument are used as follow:
 @code
 MSB                                                                                 LSB
 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 Ro  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F  F F F F F F F F F F F
 @endcode
 Where:
 @code
  - "Ro" - read-only flag, true if the file is read-only;
  - "F"  - database file name length;
 @endcode
 - Arg 1: [out]  db names buffer
 - Arg 2: [out]  file session handle
 - Arg 3: [out]  database file handle
*/		
TInt TSqlFhAttachCmdFunctor::operator()(RFile64& aDbFile, TBool /*aCreated*/, TBool aReadOnly)
	{
	const TInt KMaxBufLen = iDbFileName.Length() * sizeof(TText) + iDbName.Length() * sizeof(TText) + 32;//"32" should be enough for all additional data like name length, alignment, etc.
	HBufC8* buf = HBufC8::New(KMaxBufLen);
	if(!buf)
		{
		return KErrNoMemory;	
		}
	TPtr8 bufPtr = buf->Des();
	RDesWriteStream out(bufPtr);
	TRAPD(err, SerializeToStreamL(out));
	__ASSERT_DEBUG(err == KErrNone, __SQLPANIC(ESqlPanicInternalError));//"Write to descriptor" streaming operatons can't fail
   TUint32 arg0 = (TUint32)bufPtr.Length() | (aReadOnly ? 0x80000000 : 0);
    TIpcArgs ipcArgs(arg0, &bufPtr);
    err = aDbFile.TransferToServer(ipcArgs, 2, 3);
    if(err == KErrNone)
        {
        err = iDbSession.SendReceive(ESqlSrvDbAttachFromHandle, ipcArgs);
        }
	delete buf;
	return err;
	}
	
/**
Serializes TSqlFhAttachCmdFunctor object content to a stream (aStream parameter).
*/
void TSqlFhAttachCmdFunctor::SerializeToStreamL(RWriteStream& aStream)
	{
	aStream << iDbFileName;
	aStream << iDbName;
	aStream.CommitL();
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Database type.
enum TDbType 
	{
	EDbTypeUnknown,			//The database resides outside the client's private data cage 
	EDbTypeClientPrivate	//The database resides in the client's private data cage
	};

/**
Returns the database type.

@param aDbFileName Database file name, including the path. If it is a request for creating/opening
			secure database, then the name format is <drive>:<[SID]database_file_name>.

@return EDbTypeClientPrivate If the database resides in the client's private data cage;
		EDbTypeUnknown This may be a database in the SQL server private data cage or somewhere else;
		If the error code is less than 0, then the function cannot determine the database type
		(the function was unable to connect the local file session instance).

@internalComponent
*/
static TInt GetDbType(const TDesC& aDbFileName)
	{
	RFs fs;
	TInt err = fs.Connect();
	if(err == KErrNone)
		{
		TFileName clientPrivatePath;
		err = fs.PrivatePath(clientPrivatePath);
		fs.Close();
		if(err == KErrNone)
			{
			TInt pos = aDbFileName.FindF(clientPrivatePath);
			return (TUint)pos <= (TUint)KMaxDriveName ? EDbTypeClientPrivate : EDbTypeUnknown;
			}
		}
	return err;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////                RSqlDbSession           /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Establishes a connection with the SQL server.

The method initializes RSqlDbSession object establishing a connection with the SQL server.
If the connection has established successfully, the method sends a message to the server to
create or open (depending on aFunction parameter) a database file with aDbFileName name.

@param aFunction Specifies which operation has to be performed:
  ESqlSrvDbCreate       - Create a shared non-secure or private secure database;
  ESqlSrvDbCreateSecure - Create a shared secure database;
  ESqlSrvDbOpen         - Open a shared non-secure, shared secure or private secure database;

@param aDbFileName Database file name, including the path. If it is a request for creating/opening
			secure database, then the name format is <drive>:<[SID]database_file_name>.
@param aSecurityPolicyData Security policy data. Non-empty descriptor only for secure databases
@param aConfig the configuration string "PARAM=VALUE;...."

@return KErrNone, the operation completed successfully;
		KErrNoMemory, an out of memory condition has occured;
		KErrBadName, bad database file name: zero length, directory name;
		KErrAlreadyExists, database file already exists;
		KErrNotReady, the drive does not exist or is not ready;
		KErrInUse, the file has been opened already;
		KErrNotFound, file not found;
		KErrGeneral, no or invalid security policies (if the database to be opened is a secure database with bad security data);
		KErrNotSupported, incompatible sql security version (if the database to be opened is a secure database with bad security data);
		KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.
        KErrArgument invalid config string or config string length exceeds KSqlSrvMaxConfigStrLen.

Usage of the IPC call arguments:
 - Arg 0: [out]  database file name length
 - Arg 1: [out]  database file name
 - Arg 2: [out]  security policies buffer length if aFunction is ESqlSrvDbCreateSecure
 - Arg 3: [out]  security policies buffer if aFunction is ESqlSrvDbCreateSecure
*/
TInt RSqlDbSession::Connect(TSqlSrvFunction aFunction, const TDesC& aDbFileName, const TDesC8& aSecurityPolicyData, const TDesC8* aConfig)
	{
    SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, RSQLDBSESSION_CONNECT_ENTRY, "Entry;0x%X;RSqlDbSession::Connect;aDbFileName=%S", (TUint)this, __SQLPRNSTR(aDbFileName)));
#ifdef SYSLIBS_TEST
    const TInt KDefaultMsgBufLen = 4;
#else	
	const TInt KDefaultMsgBufLen = 128;
#endif	
	iLastErrorMessage = HBufC::New(KDefaultMsgBufLen);
	if(!iLastErrorMessage)
		{
		return KErrNoMemory;	
		}
	TInt err = DoCreateSession();
	if(err == KErrNone)
		{
		err = InitResourceTestData();
		if(err == KErrNone)
			{
			err = ::GetDbType(aDbFileName);
			if(err >= 0)	
				{
				if(err == EDbTypeClientPrivate)
					{//The database is in the client's private data cage. Set err = KErrPermissionDenied.
					err = KErrPermissionDenied;
					}
				else
					{//Unknown database type. Try to connect.
					err = DoConnect(aFunction, aDbFileName, aSecurityPolicyData, aConfig);
					}
				if(err == KErrPermissionDenied && aFunction != ESqlSrvDbCreateSecure)
					{
					//What do we have here now? - the operation is create/open non-secure, the error is KErrPermissionDenied.
					//So, the dll will try now to create/open the database in the application's private data cage.
					//What if the used database file name was "C:MyDb.db"!? - the dll will try to create/open 
					//"C:MyDb.db" in the application's data cage and there
					//is a possibility that this operation my return a result, like KErrNotFound, which will hide the original
					//error code (KErrPermissionDenied).
					//The dll shall not try to create/open file in the application's data cage, if the format of the
					//database file name is secure.
					TParsePtrC parse(aDbFileName);//this call may panic if aDbFileName cannot be parsed, but it will panic the client
					if(parse.PathPresent())
						{
						//The caller or the SQL server has no enough rights to create or open the database with aDbFileName name.
						//One of the reasons may be that the database file is in the caller's private data cage where the SQL
						//server cannot create/open the database file.
						//The SQL dll will try to create/open the database file  on the client side and pass the file handle to the
						//SQL server.
						TSqlFhOpenCmdFunctor fhOpenCmdSender(*this, aDbFileName, aConfig);
						err = CreateAndSendFileHandle(fhOpenCmdSender,
							aFunction == ESqlSrvDbCreate ? RSqlDbSession::EDbfCreate : RSqlDbSession::EDbfOpen);
						}
					}	
				}
			}
		}
	if(err != KErrNone)
		{
		Close();	
		}
    SQL_TRACE_SESSION(OstTraceExt3(TRACE_INTERNALS, RSQLDBSESSION_CONNECT_EXIT, "Exit;0x%X;RSqlDbSession::Connect;err=%d;handle=0x%X", (TUint)this, err, (TUint)Handle()));
	return err;
	}

/**
Sends a command to the server to attach an existing database to the current connection.

The database can be:
@code
 - shared secure database;
 - shared non-secure database;
 - private secure database;
@endcode

@param aDbFileName Database file name.
@param aDbName Logical database name. 

@return KErrNone, the operation completed successfully;
		KErrNoMemory, an out of memory condition has occured;
		KErrBadName, bad database file name: zero length, directory name;
		KErrNotReady, the drive does not exist or is not ready;
		KErrInUse, the file has been opened already;
		KErrNotFound, file not found;
		KErrGeneral, no or invalid security policies (if the database to be opened is a secure database with bad security data);
		KErrNotSupported, incompatible sql security version (if the database to be opened is a secure database with bad security data);
		KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.

Usage of the IPC call arguments: 
Arg 0: [out]	Database file name length.
Arg 1: [out]	Database file name.
Arg 2: [out]	Logical database name length.
Arg 3: [out]	Logical database name.
*/
TInt RSqlDbSession::Attach(const TDesC& aDbFileName, const TDesC& aDbName)
	{
	TInt err = ::GetDbType(aDbFileName);
	if(err >= 0)	
		{
		if(err == EDbTypeClientPrivate)
			{//The database is in the client's private data cage. Set err = KErrPermissionDenied.
			err = KErrPermissionDenied;
			}
		else
			{//Unknown database type. Try to attach.
			err = SendReceive(ESqlSrvDbAttach, TIpcArgs(aDbFileName.Length(), &aDbFileName, aDbName.Length(), &aDbName));
			}
		if(err == KErrPermissionDenied)
			{
			//What do we have here now? - the operation is "attach", the error is KErrPermissionDenied.
			//Which means, it is (possibly) a secure database in the server private data cage and the caller 
			//failed to pass the security checks.
			//So, the dll will try now to open the database assuming it is in the application's private data cage
			//and pass the file and session handles to the server which will try to attach the database using the handles.
			//What if the used database file name was "C:MyDb.db"!? (which means that the client's intention was
			//to attach a public shared secure database)
			//The dll will try to open "C:MyDb.db" in the application's data cage and there
			//is a possibility that this operation may return a result, like KErrNotFound, which will
			//hide the original error code (KErrPermissionDenied).
			//The dll shall not try to attach a database from the application's data cage, if the format of the
			//database file name is secure.
			TParsePtrC parse(aDbFileName);//this call may panic if aDbFileName cannot be parsed, but it will panic the client
			if(parse.PathPresent())
				{
				TSqlFhAttachCmdFunctor fhAttachCmdSender(*this, aDbFileName, aDbName);
				err = CreateAndSendFileHandle(fhAttachCmdSender, RSqlDbSession::EDbfOpen);
				}
			}
		}
	return err;
	}
	
/**
Copies a database.

The method establishes a temporary connection with the SQL server and issues a request for copying
aSrcDbFileName database file to aDestDbFileName file. After the server completes the request 
CopyDatabase() closes the connection.

@param aSrcDbFileName Source database file name.
@param aDestDbFileName Destination database file name.

@return KErrNone, the operation completed successfully;
		KErrNoMemory, an out of memory condition has occured;
		KErrBadName, bad database file name: zero length, directory name;
		KErrAlreadyExists, target database file already exists;
		KErrNotReady, the drive does not exist or is not ready;
		KErrInUse, the file has been opened already;
		KErrNotFound, file not found;
        KErrPermissionDenied, the SID of the calling application does not match the SID of source or destination database.
                      Note that other system-wide error codes may also be returned.

Usage of the IPC call arguments: 
Arg 0: [out]  source database file name length
Arg 1: [out]  source database file name
Arg 2: [out]  destination database file name length
Arg 3: [out]  destination database file name
*/
TInt RSqlDbSession::CopyDatabase(const TDesC& aSrcDbFileName, const TDesC& aDestDbFileName)
	{
	RSqlDbSession sess;
	TInt err = sess.DoCreateSession();
	if(err == KErrNone)
		{
		err = sess.InitResourceTestData();
		if(err == KErrNone)
			{
			err = sess.SendReceive(ESqlSrvDbCopy, TIpcArgs(aSrcDbFileName.Length(), &aSrcDbFileName, aDestDbFileName.Length(), &aDestDbFileName));
			}
		sess.Close();
		}
	return err;
	}

/**
Deletes a database.

The database can be:
@code
 - shared secure database;
 - shared non-secure database;
 - private secure database;
@endcode

The method establishes a temporary connection with the SQL server and issues a "delete" request regarding
aDbFileName database file. After the server completes the request DeleteDatabase() closes the connection.

@param aDbFileName Database file name. It must include the path if it is not a secure database.

@return KErrNone, the operation completed successfully;
		KErrNoMemory, an out of memory condition has occured;
		KErrBadName, bad database file name: zero length, directory name;
		KErrNotReady, the drive does not exist or is not ready;
		KErrInUse, the file has been opened already;
		KErrNotFound, file not found;
		KErrAccessDenied, access to the database file is denied (it might be a read-only file);
        KErrPermissionDenied, the SID of the calling application does not match the SID of the database.
                      Note that other system-wide error codes may also be returned.
                      
Usage of the IPC call arguments: 
Arg 0: [out]  database file name length
Arg 1: [out]  database file name
*/
TInt RSqlDbSession::DeleteDatabase(const TDesC& aDbFileName)
	{
	RSqlDbSession sess;
	TInt err = sess.DoCreateSession();
	if(err == KErrNone)
		{
		err = sess.InitResourceTestData();
		if(err == KErrNone)
			{
			err = ::GetDbType(aDbFileName);
			if(err >= 0)	
				{
				if(err == EDbTypeClientPrivate)
					{//The database is in the client's private data cage. Set err = KErrPermissionDenied.
					err = KErrPermissionDenied;
					}
				else
					{//Unknown database type. Try to delete.
					err = sess.SendReceive(ESqlSrvDbDelete, TIpcArgs(aDbFileName.Length(), &aDbFileName));
					}
				if(err == KErrPermissionDenied)
					{
					//What do we have here now? - the operation is "delete db", the error is KErrPermissionDenied.
					//So, the dll will try now to delete a database with the same name from the application's private data cage.
					//What if the used database file name was "C:MyDb.db" and there is a such file in server's private
					//data cage? - the dll will try to delete a file with name "C:MyDb.db" from the application's data cage 
					//and there is a possibility that this operation my pass or may return strange result, like KErrNotFound.
					//Bith cases are not what the user would expect.
					//The dll shall not try to delete a file from the application's data cage, if the format of the
					//database file name is secure.
					TParsePtrC parse(aDbFileName);//this call may panic if aDbFileName cannot be parsed, but it will panic the client
					if(parse.PathPresent())
						{
						//The caller or the SQL server has no enough rights to delete the database with aDbFileName name.
						//One of the reasons may be that the database file is in the caller's private data cage where the SQL
						//server cannot delete the database file.
						//The SQL dll will try to delete the database file on the client side.
						RFs fs;
						err = fs.Connect();
						if(err == KErrNone)
							{
							err = fs.Delete(aDbFileName);
							fs.Close();
							}
						}
					}
				}
			}
		sess.Close();
		}
	return err;
	}

/**
Retrieves a reference to the textual description of the error returned by the
most recent call to any of the functions:
- RSqlDatabase::Exec()
- RSqlStatement::Exec()
- RSqlStatement::Next()
- RSqlStatement::Reset()

Note that the function can only return a reference to text for
database-specific type errors, i.e. those errors that are categorised as of
type ESqlDbError.

If an error occurs during the retrieval of the last error message, the function silently ignores the error
and returns a NULL descriptor.

@return A non-modifiable pointer descriptor representing the most recent error
        message. Note that message may be NULL, i.e. the descriptor may have
        zero length.

Usage of the IPC call arguments:
Arg 0: [out]	max length of the message buffer
Arg 1: [in/out]	buffer for the last error message
*/
TPtrC RSqlDbSession::LastErrorMessage()
	{
	TPtr msg(iLastErrorMessage->Des());
	msg.Zero();
	TInt size = 0;
	while((size = SendReceive(ESqlSrvLastErrorMsg, TIpcArgs(msg.MaxLength(), &msg))) > KSqlClientBufOverflowCode)
		{
		HBufC* newMsgBuf = iLastErrorMessage->ReAlloc(size - KSqlClientBufOverflowCode);
		if(!newMsgBuf)
			{
			break;	
			}
		iLastErrorMessage = newMsgBuf;	
		msg.Set(iLastErrorMessage->Des());
		}
	return msg;
	}

/**
Returns the ROWID of the most recent successful INSERT into the database 
from this database connection.

@return >0, the ROWID of the most recent successful INSERT into the database
			from this database connection;
		0, 	if no successful INSERTs have ever occurred from this database connection
		<0, if one of the system-wide error codes is returned

Usage of the IPC call arguments:
Arg 0: [in/out]	the receiving buffer for the last inserted ROWID
*/	
TInt64 RSqlDbSession::LastInsertedRowId()
	{
	TInt64 res;
	TPtr8 ptr(reinterpret_cast <TUint8*> (&res), sizeof(res));
	TInt err = SendReceive(ESqlSrvDbLastInsertedRowId, TIpcArgs(&ptr));
	return err == KErrNone ? res : err;
	}

/**
Retrieves the database security policies.

@return A pointer to the created and internalized CSqlSecurityPolicy instance.

@return KErrNone, the operation has completed successfully;
		KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with some database specific errors categorised as 
                      ESqlDbError or other system-wide error codes.

Usage of the IPC call arguments:
Arg 0: [out]	security policy buffer size
Arg 1: [in/out]	buffer for the database security policies
*/
TInt RSqlDbSession::GetSecurityPolicy(RSqlBufFlat& aSecurityPolicyBuf)
	{
	TPtr8 ptr(aSecurityPolicyBuf.BufPtr());
	TInt rc = KErrNone;
	while((rc = SendReceive(ESqlSrvDbGetSecurityPolicy, TIpcArgs(ptr.MaxLength(), &ptr))) > KSqlClientBufOverflowCode)
		{
		rc = aSecurityPolicyBuf.ReAlloc(rc - KSqlClientBufOverflowCode);
		if(rc != KErrNone)
			{
			break;
			}
		ptr.Set(aSecurityPolicyBuf.BufPtr());
		}
	return rc;
	}
	
/**
Closes the database and releases the connection with the database server.
*/
void RSqlDbSession::Close()
	{
    SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, RSQLDBSESSION_CLOSE, "0x%X;RSqlDbSession::Close;handle=0x%X", (TUint)this, (TUint)Handle()));
	if(Handle())
		{
		(void)SendReceive(ESqlSrvDbClose);
		}
	TSqlResourceTestData::Release();
	delete iLastErrorMessage;
	iLastErrorMessage = NULL;
	RSessionBase::Close();
	}

/**
The method establishes a connection with the SQL server.

@return KErrNone, The connection was established successfully;
		KErrAlreadyExists, the connection already exists.
                 The function may also return some other system-wide error codes.
*/
TInt RSqlDbSession::DoCreateSession()
	{
	const TInt KTimesToRetryConnection = 2;
	TInt retry = KTimesToRetryConnection;
	for(;;)
		{
		TInt err = CreateSession(KSqlSrvName, ::SqlSrvVersion());
		if(err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if(--retry == 0)
			{
			return err;
			}
		err = ::StartSqlServer();
		if(err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

/**
The method gets called immediatelly after the establishing client-server connection but before the 
create/open database message. 
If a request was made by the client (using TSqlResourceTester methods) to test the server under out of memory
conditions, InitResourceTestData() will send this request to the server, putting the just created connection
in a "out of memory" test mode.
*/
TInt RSqlDbSession::InitResourceTestData()
	{
	TInt err = KErrNone;
#ifdef _DEBUG	
	TSqlResourceTestData* data = TSqlResourceTestData::Instance();
	if(data)
		{
		data->Init(*this);
		}
	else
		{
		err = KErrNoMemory;	
		}
#endif	
	return err;	
	}

/**
Creates/opens database file locally and sends the file handle to the SQL server.

This function is used only when the SQL server fails to create/open/attach the requested database file with
KErrPermissionDenied error. One of the reasons for that error may be that the database file is in the 
calling application's private data cage. In this case an attempt is made to create/open the database file locally
and if the operation completes successfully, then the file handle is passed to the SQL server.

@param aFhCmdSender A reference to a functor instance which is used for sending the file handles to the SQL server. 
@param aDbfAction It is set by the caller to RSqlDbSession::EDbfCreate if the database file has to be created or
				  to RSqlDbSession::EDbfOpen if the database file already exists and has to be opened

@return KErrNone, the operation completed successfully;
		KErrNoMemory, an out of memory condition has occured;
		KErrBadName, bad database file name: zero length, directory name, missing drive in the file name;
		KErrAlreadyExists, database file already exists;
		KErrNotReady, the drive does not exist or is not ready;
		KErrInUse, the file has been opened already;
		KErrNotFound, file not found;
		KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.
*/
TInt RSqlDbSession::CreateAndSendFileHandle(TSqlFhCmdFunctor& aFhCmdSender, RSqlDbSession::TDbfAction aDbfAction)
	{
	//Create a file session
	RFs fs;
	TInt err = fs.Connect();
	if(err != KErrNone)
		{
		return err;	
		}
	//Share the file session
	err = fs.ShareProtected();
	if(err != KErrNone)
		{
		fs.Close();	
		return err;	
		}
	//Create/open the database file locally
	TBool readOnly = EFalse;
	TBool fileCreated = EFalse;
	RFile64 dbFile;
	if(aDbfAction == RSqlDbSession::EDbfCreate)
		{//Create the database file in R/W mode
		err = dbFile.Create(fs, aFhCmdSender.iDbFileName, EFileShareAny | EFileWrite);
		if(err == KErrNone)
			{
			fileCreated = ETrue;	
			}
		}
	else //aDbfAction == RSqlDbSession::EDbfOpen
		{//Open the database file in shared R/W mode
		err = dbFile.Open(fs, aFhCmdSender.iDbFileName, EFileShareAny | EFileWrite);
		if(err != KErrNone)
			{//If the the database open operation in R/W mode has failed - try to open the database in shared read-only mode.
			readOnly = ETrue;
			err = dbFile.Open(fs, aFhCmdSender.iDbFileName, EFileShareReadersOnly);
			}
		}
	if(err == KErrNone)
		{//Successful create/open database file operation. Send the database file and session handles to the server.
		err = aFhCmdSender(dbFile, fileCreated, readOnly);
		dbFile.Close();
		}
	if(err != KErrNone && fileCreated)
		{
		dbFile.Close();
		TInt err2 = fs.Delete(aFhCmdSender.iDbFileName);
		if(err2 != KErrNone)
			{//Sometimes it is not possible the file to be deleted, the reported error is - KErrInUse. 			
			fs.Close();
			User::After(100000);
			err2 = fs.Connect();
			if(err2 == KErrNone)
				{
				(void)fs.Delete(aFhCmdSender.iDbFileName);
				}
			}
		}
	fs.Close();	
	return err;
	}

/**
Prepares the "security & configuration" string and sends the command to the SQL server.

@param aFunction Specifies which operation has to be performed:
  ESqlSrvDbCreate       - Create a shared non-secure or private secure database;
  ESqlSrvDbCreateSecure - Create a shared secure database;
  ESqlSrvDbOpen         - Open a shared non-secure, shared secure or private secure database;

@param aDbFileName Database file name, including the path. If it is a request for creating/opening
			secure database, then the name format is <drive>:<[SID]database_file_name>.
@param aSecurityPolicyData Security policy data. Non-empty descriptor only for secure databases
@param aConfig the configuration string "PARAM=VALUE;...."

@return KErrNone, the operation completed successfully;
		KErrNoMemory, an out of memory condition has occured;
        KErrArgument invalid config string or config string length exceeds KSqlSrvMaxConfigStrLen.
                     Other system-wide error codes may also be returned.
		
*/
TInt RSqlDbSession::DoConnect(TSqlSrvFunction aFunction, const TDesC& aDbFileName, const TDesC8& aSecurityPolicyData, const TDesC8* aConfig)
	{
	const TInt KConfigStrLen = aConfig ? aConfig->Length() : 0;
	if(KConfigStrLen > KSqlSrvMaxConfigStrLen)
		{
		return KErrArgument;
		}
	const TInt KSecPolicyLen = aFunction == ESqlSrvDbCreateSecure ? aSecurityPolicyData.Length() : 0;
	TIpcArgs ipcArgs(aDbFileName.Length(), &aDbFileName);
	// pack the length xxxxyyyy where xxxx is the policy length
	// and yyyy is the config length.
	ipcArgs.Set(2, (TUint)(KSecPolicyLen << 16) | (TUint)KConfigStrLen);
	const TInt KTotalLen = KConfigStrLen + KSecPolicyLen;
	ipcArgs.Set(3, 0);
	HBufC8* arg3Buf = NULL;
	if(KTotalLen > 0)
		{
		arg3Buf = HBufC8::New(KTotalLen);
		if(!arg3Buf)
			{
			return KErrNoMemory;
			}
		TPtr8 arg3Ptr = arg3Buf->Des();
		if(KSecPolicyLen > 0)
			{
			arg3Ptr.Copy(aSecurityPolicyData);
			}
		if(KConfigStrLen > 0)
			{
			//coverity[DEADCODE]
			//The ASSERT might be useful in catching future defect in this function
			__ASSERT_DEBUG(aConfig != NULL, __SQLPANIC(ESqlPanicInternalError));
			arg3Ptr.Append(*aConfig);
			}
		ipcArgs.Set(3, &arg3Ptr);
		}
	TInt err = SendReceive(aFunction, ipcArgs);
	delete arg3Buf;
	return err;
	}
