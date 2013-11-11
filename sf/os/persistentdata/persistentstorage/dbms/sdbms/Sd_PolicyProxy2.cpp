// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CPolicyProxy class
// 
//

#include "SD_STD.H"
#include "Sd_PolicyProxy.h"
#include "D32SQL.H"

using namespace DBSC;

/**
TDbsFunction enum value is used as an index in KDbsFunc2SecurityPolicyMask array.
For each TDbsFunction enum value there is a set of security policies,
which have to be satisified by the caller capabilities (at least one of them) - 
before the related operation to be executed.
@internalComponent
*/
static const TUint8 KDbsFunc2SecurityPolicyMask[EDbsLast] = 
	{
/*EDbsResourceMark*/		EPTNone,
/*EDbsResourceCheck*/		EPTNone,
/*EDbsResourceCount*/		EPTNone,
/*EDbsSetHeapFailure*/		EPTNone,
/*EDbsOpenDatabase*/		EPTRead | EPTWrite | EPTSchema, //---The caller capabilities will be asserted later, when the db security uid is known
/*EDbsClose*/				EPTNone,						//---EDbsClose may be used to close every possible type of server side dbms object
/*EDbsDatabaseAuthenticate*/EPTNone,						//---Not supported for secure shared databases
/*EDbsDatabaseDestroy*/		EPTSchema,						//---Only admin can destroy the database
/*EDbsDatabaseBegin*/		EPTRead | EPTWrite | EPTSchema,	//---Begin transaction, which makes sense if the caller wants to change the database schema or to insert/delete/update database tables or to protect its "read" operations
/*EDbsDatabaseCommit*/		EPTRead | EPTWrite | EPTSchema,	//---Commit transaction, which makes sense if the caller wants to change the database schema or to insert/delete/update database tables or to protect its "read" operations
/*EDbsDatabaseRollback*/	EPTRead | EPTWrite | EPTSchema,	//---Rollback transaction, which makes sense if the caller wants to change the database schema or to insert/delete/update database tables or to protect its "read" operations
/*EDbsDatabaseProperty*/	EPTNone,
/*EDbsDatabaseCreateTable*/	EPTSchema,
/*EDbsDatabaseTables*/		EPTNone,
/*EDbsDatabaseColumns*/		EPTNone,
/*EDbsDatabaseIndexes*/		EPTNone,
/*EDbsDatabaseKeys*/		EPTNone,
/*EDbsDatabaseOpenObserver*/EPTNone,
/*EDbsDatabaseOpenUtility*/	EPTWrite, 						//---Compact/Recover operations
/*EDbsDatabaseOpenDropTable*/EPTSchema,
/*EDbsDatabaseOpenAlterTable*/EPTSchema,					
/*EDbsDatabaseOpenCreateIndex*/EPTSchema,
/*EDbsDatabaseOpenDropIndex*/EPTSchema,
/*EDbsDatabaseExecute*/		EPTWrite | EPTSchema,			//---either CREATE/DROP/ALTER database operations or INSERT/UPDATE/DELETE table operations. An additional caller capabilities check will be made after the parsing of the sql string.
/*EDbsDatabasePrepareView*/	EPTRead,						//---"SELECT" sql string
/*EDbsDatabaseOpenTable*/	EPTRead | EPTWrite,				//---If the caller neither have capabilities for read or write table security policies, then the the caller cannot open the table.
/*EDbsObserverNotify*/		EPTNone,
/*EDbsObserverCancel*/		EPTNone,
/*EDbsIncrementalNext*/		EPTNone,
/*EDbsCursorColumnTypes*/	EPTNone,
/*EDbsCursorReset*/			EPTNone,						//---Moves the cursor at the beginning of the table/dataset
/*EDbsCursorEvaluate*/		EPTNone,
/*EDbsCursorUnevaluated*/	EPTNone,
/*EDbsCursorSetIndex*/		EPTRead,
/*EDbsCursorSeek*/			EPTRead,
/*EDbsCursorAtBeginning*/	EPTNone,
/*EDbsCursorAtEnd*/			EPTNone,
/*EDbsCursorAtRow*/			EPTNone,
/*EDbsCursorCount*/			EPTRead | EPTWrite,
/*EDbsCursorGotoPos*/		EPTRead,
/*EDbsCursorBookmark*/		EPTRead,
/*EDbsCursorGotoBookmark*/	EPTRead,
/*EDbsCursorGet*/			EPTRead,
/*EDbsCursorInsert*/		EPTWrite,
/*EDbsCursorUpdate*/		EPTWrite,
/*EDbsCursorRetrieveRow*/	EPTNone,						//---Used by "Insert" operations sometime
/*EDbsCursorCancel*/		EPTNone,
/*EDbsCursorPut*/			EPTWrite,
/*EDbsCursorDelete*/		EPTWrite,
/*EDbsCursorColumns*/		EPTNone,
/*EDbsCursorColumnDef*/		EPTNone,
/*EDbsCursorSetNull*/		EPTWrite,
/*EDbsCursorColumnSize*/	EPTNone,
/*EDbsCursorColumnSource*/	EPTRead,						//---Used for large BLOB fields - read ops
/*EDbsCursorColumnSink*/	EPTWrite,						//---Used for large BLOB fields - write ops
/*EDbsCursorOpenConstraint*/EPTRead,
/*EDbsCursorMatch*/			EPTRead,
/*EDbsCursorFind*/			EPTRead,
/*EDbsStreamRead*/			EPTNone,
/*EDbsStreamWrite*/			EPTNone,
/*EDbsStreamSize*/			EPTNone,
/*EDbsStreamSynch*/			EPTNone,
/*EDbsCreateDatabase*/		EPTSchema,
/*EDbsDatabaseList*/		EPTNone,
/*EDbsCopyDatabase*/		EPTSchema,
/*EDbsDeleteDatabase*/		EPTSchema,
/*EDbsGetSecurityPolicy*/	EPTNone,
/*EDbsReserveDriveSpace*/	EPTNone,
/*EDbsFreeReservedSpace*/	EPTNone,
/*EDbsReserveGetAccess*/	EPTNone,
/*EDbsReserveReleaseAccess*/EPTNone,
/*EDbsGetBackupPath*/		EPTNone,			
/*EDbsGetBackupPaths*/		EPTNone
	};

/**
This function returns bit-field mask value, containing security policies types (R/W/S)
Each security policy contains a set of Capabilities/SID/VID.
In order particular database operation to be executed, the caller Capabilities/SID/VID have
to be checked against security policy Capabilities/SID/VID.
Don't forget to map new DBMS functions here!
@param aFunction DBMS server function code
@return An integer mask with a set of security policy types. The caller has to satisfy at least 
        one of of them.
*/
static TUint DbsFunction2PolicyMask(TDbsFunction aFunction)
	{
	__ASSERT(aFunction < EDbsLast);
	return KDbsFunc2SecurityPolicyMask[aFunction];
	}

/**
Extracts DBMS server function code from aMessage argument.
@param aMessage DBMS server message
@return DBMS server function code
@internalComponent
*/
static TDbsFunction Message2Function(const RMessage2& aMessage)
	{
	TDbsFunction func = ::DbsFunction(aMessage.Function());
	return static_cast <TDbsFunction> (func & ~KDbsObjectReturn);
	}

/**
*/
inline CPolicyProxy::CPolicyProxy(RFs& aFs):iFs(aFs)
	{
	}

/**
Standard phase-one factory method for CPolicyProxy instances.
@param aFs A file server session instance
@param aPrivatePath DBMS server private data path
@return A pointer to the created CPolicyProxy instance.
@leave KErrNoMemory
*/
CPolicyProxy* CPolicyProxy::NewL(RFs& aFs,const TDesC& aPrivatePath)
	{
	CPolicyProxy* self = new (ELeave) CPolicyProxy(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aPrivatePath);
	CleanupStack::Pop(self);
	return self;
	}

/**
*/
CPolicyProxy::~CPolicyProxy()
	{
	if(iPolicySpace) //iPolicySpace can be NULL in OOM tests
		{
		iPolicySpace->Release();
		}
	}

/**
Returns requested database security policy interface, which cannot be NULL.
@param aDbPolicyRequest Request params: request type (secure/non-secure) and domain UID
@return A const pointer to the related UID security policy object.
*/
const MPolicy* CPolicyProxy::DbPolicyL(const TDbPolicyRequest& aDbPolicyRequest)
	{
	return iPolicySpace->DbPolicyL(aDbPolicyRequest);
	}

/**
Returns requested table security policy interface, which cannot be NULL.
@param aDbPolicyRequest Request params: request type (secure/non-secure) and domain UID
@param aTblName Database table name
@return A const pointer to the related table security policy object.
*/
const MPolicy* CPolicyProxy::TblPolicyL(const TDbPolicyRequest& aDbPolicyRequest, 
										const TDesC& aTblName)
	{
	return iPolicySpace->TblPolicyL(aDbPolicyRequest, aTblName);
	}

/**
This method is used to get the SQL related MPolicy interface and the related security 
policy type.
If aUPRequest.iRqAccess is EATNonSecure, then the default security policy will be returned.
Currently the DBMS can process the following SQL strings:
1)DDL  - CREATE/DROP/ALTER SQL statements - EPTSchema database access level.
2)DML  - INSERT/UPDATE/DELETE SQL statements. Only one table can be put after the "FROM"
         SQL keyword. EPTWrite table access level.
3)QUERY- SELECT SQL statements. Only one table can be out after the "FROM" SQL keyword.
         EPTRead table access level.
@param aDbPolicyRequest A const reference to an object packing security policy uid and the request type:
                  secure/non-secure.
@param aSql SQL string
@param aPolicyType An output parameter, referencing the location, where the policy type will be stored.
@return A const pointer to the related policy interface. It cannot be NULL, and must not be deleted.
@leave One of the system-wide error codes.
*/
const MPolicy* CPolicyProxy::SqlPolicyL(const TDbPolicyRequest& aDbPolicyRequest, const TDesC& aSql, 
										TPolicyType& aPolicyType)
	{
	const MPolicy* policy = NULL;
	aPolicyType = EPTNone;
	//Get table name and sql type.
	TSqlParser2 sqlParser;
	sqlParser.ParseL(aSql);
	Sql::TStatementType sqlType = sqlParser.StatementType();
	//Reinitialize aSqlSecurityPolicyData, which is sql type dependent.
	switch(sqlType)
		{
		case Sql::EDDL:
			//Database EPTSchema access level
			policy = DbPolicyL(aDbPolicyRequest);
			aPolicyType = EPTSchema;
			break;
		case Sql::EDML:
		default:
			{//Table access level - EPTRead or EPTWrite.
			const TDesC& tblName = sqlParser.TableName();
			__ASSERT(tblName.Length() > 0);
			TBuf<KDbMaxName> tblNameBuf;
			tblNameBuf.Copy(tblName);
			policy = TblPolicyL(aDbPolicyRequest, tblNameBuf);
			aPolicyType = sqlType == Sql::EDML ? EPTWrite : EPTRead;
			}
			break;
		}
	__ASSERT(policy);
	return policy;
	}

/**
Returns backup&restore SID for the databases, the access to which is controlled by the
security policy, identified by aDbUid parameter.
@param aDbUid Domain UID
@return Backup&restore SID for the supplied domain UID
@leave KErrArgument if there is no security policy domain for the supplied UID.
*/
TSecureId CPolicyProxy::BackupSIDL(TUid aDbUid) const
	{
	return iPolicySpace->BackupSIDL(aDbUid);
	}
	
/**
Asserts caller capabilities/SID/VID, packed in aMessage parameter against the security policy
managed by aPolicy parameter. The caller has to satisfy at least one of the related to 
the message security policies.
@param aMessage An object whith caller capabilities/SID/VID, which has to be checked.
@param aPolicy A const reference to the security policy object. 
@leave KErrPermissionDenied The caller has no enough rights for the requested DBMS operation
*/
void CPolicyProxy::CheckL(const RMessage2& aMessage, const MPolicy& aPolicy) const
	{
	TDbsFunction func = ::Message2Function(aMessage);
	TUint mask = ::DbsFunction2PolicyMask(func);
	if(mask != EPTNone)
		{
		for(TInt c=0;c<KPolicyTypesCount;++c)
			{
			TPolicyType policyType = static_cast <TPolicyType> (1 << c);
			if(policyType & mask)
				{
				if(aPolicy.Check(aMessage, policyType))
					{
					return;
					}
				}
			}
		__LEAVE(KErrPermissionDenied);
		}
	}

/**
Asserts caller capabilities/SID/VID, packed in aMessage parameter against the security policy
managed by aPolicy parameter. 
@param aPolicyType The policy type, against which the check has to be done.
@param aMessage An object whith caller capabilities/SID/VID, which has to be checked.
@param aPolicy A const reference to the security policy object. 
@leave KErrPermissionDenied The caller has no enough rights for the requested DBMS operation
*/
void CPolicyProxy::CheckL(TPolicyType aPolicyType, const RMessage2& aMessage, const MPolicy& aPolicy) const
	{
	if(aPolicyType != EPTNone)
		{
		if(!aPolicy.Check(aMessage, aPolicyType))
			{
			__LEAVE(KErrPermissionDenied);
			}
		}
	}

/**
Standard phase-two construction method for CPolicyProxy instances.
@param aPrivatePath DBMS server private data path
@leave KErrNoMemory
*/
void CPolicyProxy::ConstructL(const TDesC& aPrivatePath)
	{
	iPolicySpace = TPolicySpaceFactory::NewPolicySpaceL(iFs, aPrivatePath);
	}

