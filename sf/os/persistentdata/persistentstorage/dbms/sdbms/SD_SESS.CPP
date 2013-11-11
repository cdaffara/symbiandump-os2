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
// DBMS server-session and support classes
// 
//

#include <s32file.h>
#include "SD_STD.H"
#include "Sd_DbList.h"

using namespace DBSC;

/*
CDbsSession class is updated now to with a support for DBMS security policies.
What idea sits behind the update:
1) DBMS server has a set of server side CDbsSession objects.
2) Each CDbsSession object has a list of CDbsSession::TEntry items.
   Each CDbsSession::TEntry item holds a server side DBMS object, which is
   used to perform client requests.
3) CDbsSession::TEntry object could be:
	- EDbsDatabase - database object. It operates at database level.
		OpenDatabaseL is the method, which creates it.
	- EDbsIncremental - incremental object. It operates either at database or table level.
		NewIncrementalL is the method, which creates it.
	- EDbsCursor - cursor object. It operates at table level.
		NewCursorL is the method, which creates it.
	- EDbsConstraint - constraint object. It is used at table level and needs the same
		security policy as its table.
	- EDbsStream - stream object. It operates either at database or table level.
		NewStreamL are the methods, which create it.
	- EDbsObserver - observer object. It operates at database level.
4) When DBMS server is used in a security environment, database/table security policies 
	describe allowed W/R operations for the database/tables.
5) Each database/table has a security policy object assiciated with it. It may be accessed
	using MPolicy interface.
6) There is a MPolicySpace interface, which holds a collection of MPolicy interfaces
	for the database/tables. MPolicy interfaces can be queried using MPolicySpace methods.
7) All DBMS server side objects, described in (3), operate at database/table level. In a
	security environment, the client's access rights has to be checked, before performing
	particular database/table operation.
	It is known at the moment of DBMS object creation, on which database/table it will operate.
	The database/table cannot be changed during the DBMS object life time.
	At the moment of DBMS object creation, the related database/table interface is queried and
	stored in CDbsSession::TEntry::iPolicy data member. 
	It is an error if CDbsSession::TEntry item exists without a security policy.
8) Every time, when a client needs particular DBMS server functionality and CDbsSession::ServiceL
	has been called, the session object will find which DBMS server object has to be used and
	asserts client's access rights against its security policy.
	If the client has not enough access rights to perform desired DBMS operation, the DBMS
	session will leave with KErrPermissionDenied error code.
	======================================================================================
	In one sentence - every time, when an object from CDbsSession::TEntry list is about to 
	perform particular operation at database/table level, the associated database/table 
	security policy will be checked and the opration may be rejected.
*/

// Class CDbsSession::TEntry

//
// release the object
//
void CDbsSession::TEntry::Release()
	{
	TDbsType t=Type();
	__ASSERT(t!=EDbsFree);
	if (t==EDbsObserver)
		delete STATIC_CAST(CDbsObserver::HObserver*,iObject);
	else if (t==EDbsStream)
		delete STATIC_CAST(HDbsStream*,iObject);
	else
		CDbObject::Destroy(STATIC_CAST(CDbObject*,iObject));
	iType=TUint8(EDbsFree);
	iPolicy = NULL;
	}

#ifdef __DBDUMP__
//Using CDbsSession::TEntry::Dump() method you can dump the TEntry's content
//into a stream. Note that the dump works only if you have __DBDUMP__ macro defined.
void CDbsSession::TEntry::Dump(RFile& aFile) const
	{
	_LIT8(KClassName, "CDbsSession::TEntry. this=%X");
	_LIT8(KType, "Type=%S");
	_LIT8(KMagic, "Magic=%d");
	_LIT8(KCrLf, "\r\n");
	_LIT8(KEnd, "====");
	TPtrC8 KDbsType[EDbsMaxType + 1] = {_L8("Free"), _L8("Database"), _L8("Incremental"), _L8("Cursor"), _L8("Constraint"), _L8("Stream"), _L8("Observer")};
	TBuf8<40> buf;

	buf.Format(KClassName, this);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	buf.Format(KType, &KDbsType[iType]);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	buf.Format(KMagic, iMagic);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	__ASSERT(iPolicy);
	iPolicy->Dump(aFile);
	(void)aFile.Write(KEnd);
	(void)aFile.Write(KCrLf);
	}
#endif//__DBDUMP__

// Class CDbsSession

//
// Release all this clients resources
// Streams must be released before cursors, and everything before the connection
//
CDbsSession::~CDbsSession()
	{
	TEntry* const base=&iIx[0];
	if (base)
		{
		TInt type=EDbsMaxType;
		do	{
			for (TEntry* e=base+iSize;--e>=base;)
				{
				if (e->iType==type)
					e->Release();
				}
			} while (--type>EDbsFree);
		User::Free(base);
		}
	iDbPolicyRqColl.Close();
    delete iSessDriveSpace;
	Server().RemoveSession();
	}

/**
Overrides virtual CSession2::Create2().
Creates iSessDriveSpace instance.
*/
void CDbsSession::CreateL()
    {
    iSessDriveSpace = CDbsSessDriveSpace::NewL(Server().DriveSpaceCol());
    }

//
// provide for CSession
// If this leaves, we complete message through ServiceError(...)
//
void CDbsSession::ServiceL(const RMessage2& aMessage)
	{
	//set received message in server so it can be used in cleanup when server panics.
	CPolicyProxy& policyProxy = Server().PolicyProxy();
	TDbsFunction f=DbsFunction(aMessage.Function());
	if (f&KDbsObjectReturn)		// may return a derived object
		AllocL();	// ensure index space
	TInt r=0;//object handle
	TInt h=DbsHandle(aMessage.Function());
	if (h==KDbsSessionHandle)
		{	// session based requests
		switch (f&~KDbsObjectReturn)
			{
		case EDbsResourceMark:
			ResourceCountMarkStart();
			break;
		case EDbsResourceCheck:
			ResourceCountMarkEnd(aMessage);
			break;
		case EDbsResourceCount:
			r=CountResources();
			break;
		case EDbsSetHeapFailure:
			User::__DbgSetAllocFail(RHeap::EUser,RHeap::TAllocFail(aMessage.Int0()),aMessage.Int1());
			break;
		case EDbsOpenDatabase:
			r=OpenDatabaseL(aMessage);
			break;
		case EDbsReserveDriveSpace:
			ReserveDriveSpaceL(static_cast <TDriveNumber> (aMessage.Int0()));
			break;
        case EDbsFreeReservedSpace:
	        FreeReservedSpace(static_cast <TDriveNumber> (aMessage.Int0()));
			break;
		case EDbsReserveGetAccess:
			GetReserveAccessL(static_cast <TDriveNumber> (aMessage.Int0()));
			break;
		case EDbsReserveReleaseAccess:
			ReleaseReserveAccess(static_cast <TDriveNumber> (aMessage.Int0()));
			break;
		default:
			r=ExtServiceL(aMessage, static_cast <TDbsFunction> (f&~KDbsObjectReturn));
			break;
			}
		}
	else
		{	// object based requests
		TEntry& e=Object(h);
		__ASSERT(e.iPolicy);
		policyProxy.CheckL(aMessage, *e.iPolicy);
		TDbsFunction dbmsFuncNo = static_cast <TDbsFunction> (f & ~KDbsObjectReturn);
		switch (dbmsFuncNo)
			{
	// the common close function
		case EDbsClose:
			//If this is a database object, remove the related pair (handle, uid) from iDbPolicyRqColl collection.
			if(e.Type() == EDbsDatabase)
				{
				iDbPolicyRqColl.Remove(h);
				}
			Free(e);
			break;
	// database functions
		case EDbsDatabaseAuthenticate:
			{
            __LEAVE(KErrNotSupported);             
			}
			break;
		case EDbsDatabaseDestroy:
			r=e.Database().Destroy();
			break;
		case EDbsDatabaseBegin:
			r=e.Database().Begin();
			break;
		case EDbsDatabaseCommit:
			r=e.Database().Commit();
			break;
		case EDbsDatabaseRollback:
			e.Database().Rollback();
			break;
		case EDbsDatabaseProperty:
			r=e.Database().Property(CDbDatabase::TProperty(aMessage.Int0()));
			break;
		case EDbsDatabaseTables:
			{//The new stream object will have the same security policy as the database
			 //Read only stream
			CDbTableNames* names=CDbTableNames::NewLC();
			e.Database().TablesL(*names);
			r=NewStreamL(names,Externalizer(names),aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseColumns:
			{//The new stream object will have the same security policy as the database
			 //Read only stream
			CDbColSet* set=CDbColSet::NewLC();
			e.Database().ColumnsL(*set,ReadName0L(0,aMessage));
			r=NewStreamL(set,Externalizer(set),aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseIndexes:
			{//The new stream object will have the same security policy as the database
			 //Read only stream
			CDbIndexNames* names=CDbIndexNames::NewLC();
			e.Database().IndexesL(*names,ReadName0L(0,aMessage));
			r=NewStreamL(names,Externalizer(names),aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseKeys:
			{//The new stream object will have the same security policy as the database
			 //Read only stream
			CDbKey* key=CDbKey::NewLC();
			e.Database().KeysL(*key,ReadName0L(0,aMessage),ReadName1L(1,aMessage));
			r=NewStreamL(key,Externalizer(key),aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseCreateTable:
			{
			const TDesC& name=ReadName0L(0,aMessage);
			CDbColSet* set=ColSetLC(1,aMessage);
			CDbKey* primary=0;
			if (aMessage.Ptr2())
				primary=KeyLC(2,aMessage);

			e.Database().CreateTableL(name,*set,primary);
			if (primary)
				CleanupStack::PopAndDestroy();	// primary
			CleanupStack::PopAndDestroy();	// set
			}
			break;
		case EDbsDatabaseOpenObserver:
			//The new observer object will have the same security policy as the database
			r=Add(CDbsConnection::Source(e.Database()).ObserverL(),e.iPolicy);
			break;
		case EDbsDatabaseOpenUtility:
			{//The new incremental object will have the same security policy as the database
			TInt step;
			r=NewIncrementalL(e.Database().UtilityL(CDbDatabase::TUtility(aMessage.Int0()),step),step,aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseOpenDropTable:
			{//The new incremental object will have the same security policy as the database
			TInt step;
			const TDesC& name=ReadName0L(0,aMessage);
			r=NewIncrementalL(e.Database().DropTableL(name,step),step,aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseOpenAlterTable:
			{//The new incremental object will have the same security policy as the database
			TInt step;
			const TDesC& name=ReadName0L(0,aMessage);
			CDbColSet* set=ColSetLC(1,aMessage);
			r=NewIncrementalL(e.Database().AlterTableL(name,*set,step),step,aMessage,e.iPolicy);
			CleanupStack::PopAndDestroy();	// set
			}
			break;
		case EDbsDatabaseOpenCreateIndex:
			{//The new incremental object will have the same security policy as the database
			TInt step;
			const TDesC& name=ReadName0L(0,aMessage);
			const TDesC& table=ReadName1L(1,aMessage);
			CDbKey* key=KeyLC(2,aMessage);
			r=NewIncrementalL(e.Database().CreateIndexL(name,table,*key,step),step,aMessage,e.iPolicy);
			CleanupStack::PopAndDestroy();	// key
			}
			break;
		case EDbsDatabaseOpenDropIndex:
			{//The new incremental object will have the same security policy as the database
			TInt step;
			const TDesC& name=ReadName0L(0,aMessage);
			const TDesC& table=ReadName1L(1,aMessage);
			r=NewIncrementalL(e.Database().DropIndexL(name,table,step),step,aMessage,e.iPolicy);
			}
			break;
		case EDbsDatabaseExecute:
			{//The new incremental object will have the same security policy as the database or table - sql dependent.
			//In order EDbsDatabaseExecute request to be performed, the DBMS session has already
			//checked the client capabilities at the database level.
			//Now, the sql string will be parsed and the client capabilities will be checked
			//again at more specific database or table level.
			TInt init;
			HBufC* sql=ReadHBufLC(0,aMessage);
			const TDbPolicyRequest& rq = iDbPolicyRqColl[h];
			TPolicyType policyType = EPTNone;
			const MPolicy* policy = policyProxy.SqlPolicyL(rq, *sql, policyType);
			policyProxy.CheckL(policyType, aMessage, *policy);			
			r=NewIncrementalL(e.Database().ExecuteL(*sql,TDbTextComparison(aMessage.Int1()),init),init,aMessage,policy);
			CleanupStack::PopAndDestroy(sql);
			}
			break;
		case EDbsDatabasePrepareView:
			{//The new cursor object will have the same security policy as the table in the sql string.
			//In order EDbsDatabasePrepareView request to be performed, the DBMS session has already
			//checked the client capabilities at the database level.
			//Now, the sql string will be parsed and the client capabilities will be checked
			//again at table level.
			HBufC* sql=ReadHBufLC(0,aMessage);
			const TDbPolicyRequest& rq = iDbPolicyRqColl[h];
			TPolicyType policyType = EPTNone;
			const MPolicy* policy = policyProxy.SqlPolicyL(rq, *sql, policyType);
			policyProxy.CheckL(policyType, aMessage, *policy);			
			TPckgBuf<TDbWindow> window;
			aMessage.ReadL(2,window);
			TInt mode=aMessage.Int1();
			CDbCursor* cursor=e.Database().ViewL(TDbQuery(*sql,TDbTextComparison(mode&0xff)),window(),RDbRowSet::TAccess(mode>>8));
			r=NewCursorL(cursor,aMessage,policy);
			CleanupStack::PopAndDestroy(sql);
			}
			break;
		case EDbsDatabaseOpenTable:
			{//The new cursor object will have the same security policy as the table
			const TDesC& name=ReadName0L(0,aMessage);
			const TDbPolicyRequest& rq = iDbPolicyRqColl[h];
			const MPolicy* policy = policyProxy.TblPolicyL(rq, name);
			//Check caller capabilities against the table policy
			RDbRowSet::TAccess accessType = static_cast <RDbRowSet::TAccess> (aMessage.Int1());
			TPolicyType policyType = accessType == RDbRowSet::EReadOnly ? EPTRead : EPTWrite;
			policyProxy.CheckL(policyType, aMessage, *policy);
			//Create cursor
			r=NewCursorL(e.Database().TableL(name,accessType),aMessage,policy);
			}
			break;
	// Incremental functions
		case EDbsIncrementalNext:
			{
			TPckgBuf<TInt> step=aMessage.Int0();
			r=e.Incremental().NextL(step());
			aMessage.WriteL(1,step);
			}
			break;
	// Observer functions
		case EDbsObserverNotify:
			e.Observer().Notify(aMessage);
			return;		// deferred completion of the message!
		case EDbsObserverCancel:
			e.Observer().Cancel();
			break;
	// cursor functions
		case EDbsCursorColumnTypes:
			{
			TInt count=e.Cursor().ColumnCount();
			TUint8* types=(TUint8*)User::AllocLC(count);
			for (TInt ii=count;--ii>=0;)
				types[ii]=TUint8(e.Cursor().ColumnType(ii+1));
			aMessage.WriteL(3,TPtrC8(types,count));
			CleanupStack::PopAndDestroy();
			}
			break;
		case EDbsCursorReset:
			e.Cursor().Reset();
			break;
		case EDbsCursorEvaluate:
			r=e.Cursor().EvaluateL()?1:0;
			break;
		case EDbsCursorUnevaluated:
			r=e.Cursor().Unevaluated()?1:0;
			break;
		case EDbsCursorSetIndex:
			{
			const TDesC* name=0;
			if (aMessage.Ptr0())
				name=&ReadName0L(0,aMessage);
			e.Cursor().SetIndexL(name);
			}
			break;
		case EDbsCursorSeek:
			{
			TDbLookupKey* key=LookupKeyLC(0,aMessage.Int1(),aMessage);
			r=e.Cursor().SeekL(*key,RDbTable::TComparison(aMessage.Int2()))?1:0;
			CleanupStack::PopAndDestroy();		// key;
			}
			break;
		case EDbsCursorAtBeginning:
			r=e.Cursor().AtBeginning()?1:0;
			break;
		case EDbsCursorAtEnd:
			r=e.Cursor().AtEnd()?1:0;
			break;
		case EDbsCursorAtRow:
			r=e.Cursor().AtRow()?1:0;
			break;
		case EDbsCursorCount:
			r=e.Cursor().CountL(RDbRowSet::TAccuracy(aMessage.Int0()))+1;
			break;
		case EDbsCursorGotoPos:
			if (e.Cursor().GotoL(RDbRowSet::TPosition(aMessage.Int0())))
				{
				e.Cursor().GetL();
				r=RetrieveRowL(e.Cursor(),aMessage);
				}
			break;
		case EDbsCursorBookmark:
			{
			TPckgBuf<TDbBookmark::TMark> mark;
			e.Cursor().Bookmark(mark());
			aMessage.WriteL(3,mark);
			}
			break;
		case EDbsCursorGotoBookmark:
			{
			TPckgBuf<TDbBookmark::TMark> mark;
			aMessage.ReadL(0,mark);
			e.Cursor().GotoL(mark());
			}
			break;
		case EDbsCursorGet:
			e.Cursor().GetL();
			r=RetrieveRowL(e.Cursor(),aMessage);
			break;
		case EDbsCursorInsert:
			e.Cursor().InsertL(CDbCursor::TInsert(aMessage.Int0()));
			r=RetrieveRowL(e.Cursor(),aMessage);
			break;
		case EDbsCursorUpdate:
			e.Cursor().UpdateL();
			r=RetrieveRowL(e.Cursor(),aMessage);
			break;
		case EDbsCursorRetrieveRow:	// pass the row buffer back to the client
			r=RetrieveRowL(e.Cursor(),aMessage);
			break;
		case EDbsCursorCancel:
			e.Cursor().Cancel();
			break;
		case EDbsCursorPut:
			PutRowL(e.Cursor(),aMessage);
			e.Cursor().PutL();
			break;
		case EDbsCursorDelete:
			e.Cursor().DeleteL();
			break;
		case EDbsCursorColumns:
			{// reduce memory usage by extracting and stream columns individually
			 //Read only stream
			RWriteStream strm(HBufBuf::NewLC());
			TInt count=e.Cursor().ColumnCount();
			strm.WriteInt32L(count);
			TDbCol col;
			for (TInt ii=0;++ii<=count;)
				{
				e.Cursor().ColumnDef(col,ii);
				strm<<col;
				}
			strm.CommitL();
			TInt ext=strm.Sink()->SizeL();
			CleanupStack::Pop();			// stream
			r=NewStreamL(strm.Sink(),aMessage,e.iPolicy,ext);
			}
			break;
		case EDbsCursorColumnDef:
			{
			TPckgBuf<TDbCol> col;
			e.Cursor().ColumnDef(col(),aMessage.Int0());
			aMessage.WriteL(3,col);
			}
			break;
		case EDbsCursorSetNull:
			__ASSERT(TDbCol::IsLong(e.Cursor().ColumnType(aMessage.Int0())));
			e.Cursor().SetNullL(aMessage.Int0());
			break;
		case EDbsCursorColumnSize:
			__ASSERT(TDbCol::IsLong(e.Cursor().ColumnType(aMessage.Int0())));
			r=e.Cursor().ColumnSize(aMessage.Int0());
			break;
		case EDbsCursorColumnSource:
			__ASSERT(TDbCol::IsLong(e.Cursor().ColumnType(aMessage.Int0())));
			{//The new stream object will have the same security policy as the table
			 //Read only stream, used for large BLOB fields.
			TDbColNo col=aMessage.Int0();
			CDbCursor& cursor=e.Cursor();
			r=NewStreamL(cursor.ColumnSourceL(col),aMessage,e.iPolicy,cursor.ColumnSize(col));
			}
			break;
		case EDbsCursorColumnSink:
			//The new stream object will have the same security policy as the table
			//Write stream, used for large BLOB fields.
			r=NewStreamL(e.Cursor().ColumnSinkL(aMessage.Int0()),aMessage,e.iPolicy);
			break;
		case EDbsCursorOpenConstraint:
			{//The new constraint object will have the same security policy as the table
			HBufC* sql=ReadHBufLC(0,aMessage);
			CDbRowConstraint* ct=e.Cursor().ConstraintL(TDbQuery(*sql,TDbTextComparison(aMessage.Int1())));
			CleanupStack::PopAndDestroy();	//sql
			r=Add(ct, e.iPolicy);
			}
			break;
		case EDbsCursorMatch:
			r=e.Cursor().MatchL(Object(aMessage.Int0()).Constraint())?1:0;
			break;
		case EDbsCursorFind:
			{
			HBufC* sql=ReadHBufLC(0,aMessage);
			TDbQuery q(*sql,TDbTextComparison(aMessage.Int1()));
			r=e.Cursor().FindL(RDbRowSet::TDirection(aMessage.Int2()),q)-KErrNotFound;
			CleanupStack::PopAndDestroy();	//sql
			}
			break;
			// stream functions
		case EDbsStreamRead:
			{
			r=e.Stream().ReadL(aMessage);
			}
			break;
		case EDbsStreamWrite:
			{
			e.Stream().WriteL(aMessage);
			}
			break;
		case EDbsStreamSize:
			{
			r=e.Stream().SizeL();
			}
			break;
		case EDbsStreamSynch:
			{
			e.Stream().SynchL();
			}
			break;
	// we don't know about any other functions
		default:
			r=KErrNotSupported;
			break;
			}
		}

	// don't complete message if we have paniced client, just reset variable;
	if (!aMessage.IsNull())
		aMessage.Complete(r);
	}


// Handle an error from CDbsSession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
void CDbsSession::ServiceError(const RMessage2& aMessage,TInt aError)
 	{
#ifdef __DBDUMP__
	Dump();
#endif//__DBDUMP__
 	if (aError==KErrBadDescriptor)
 		{
 		// this completes message as well so we have to handle it in
 		// ServiceL
 		_LIT(KCategory,"DBMS-server");
 		Server().Panic(KCategory,EDbsBadDescriptor);
 		}
 	CSession2::ServiceError(aMessage,aError);
   	}

// Read aPtr as a name
const TDesC& CDbsSession::ReadName0L(TInt aIndex,const RMessage2& aMessage)
	{
	TDes& name=Server().Name0();
	aMessage.ReadL(aIndex,name);
	return name;
	}

// Read Ptr1 as a name
const TDesC& CDbsSession::ReadName1L(TInt aIndex,const RMessage2& aMessage)
	{
	TDes& name=Server().Name1();
	aMessage.ReadL(aIndex,name);
	return name;
	}

// Reads aPtr as a variable length 8-bit descriptor
HBufC8* CDbsSession::ReadHBuf8LC(TInt aIndex,const RMessage2& aMessage)
	{
	TInt len= aMessage.GetDesLengthL(aIndex);
	HBufC8* buf=HBufC8::NewLC(len);
	TPtr8 read(buf->Des());
	aMessage.ReadL(aIndex,read);
	return buf;
	}

// Reads aPtr as a variable length build-width descriptor
HBufC* CDbsSession::ReadHBufLC(TInt aIndex,const RMessage2& aMessage)
	{
	TInt len= aMessage.GetDesLengthL(aIndex);
	HBufC* buf=HBufC::NewLC(len);
	TPtr read(buf->Des());
	aMessage.ReadL(aIndex,read);
	return buf;
	}

// Pulls a column set from the client
CDbColSet* CDbsSession::ColSetLC(TInt aIndex,const RMessage2& aMessage)
	{
	CDbColSet* set=CDbColSet::NewLC();
	HBufC8* data=ReadHBuf8LC(aIndex,aMessage);
	RDesReadStream strm(*data);
	strm>>*set;
	CleanupStack::PopAndDestroy();	// data
	return set;
	}

// Pulls a key from the client
CDbKey* CDbsSession::KeyLC(TInt aIndex,const RMessage2& aMessage)
	{
	CDbKey* key=CDbKey::NewLC();
	HBufC8* data=ReadHBuf8LC(aIndex,aMessage);
	RDesReadStream strm(*data);
	strm>>*key;
	CleanupStack::PopAndDestroy();	// data
	return key;
	}

// Reconstructs a TDbLookupKey from the message parameter
TDbLookupKey* CDbsSession::LookupKeyLC(TInt aIndex,TInt aSize,const RMessage2& aMessage)
	{
	TDbLookupKey* key=(TDbLookupKey*)User::AllocLC(aSize);
	TPtr8 pckg((TUint8*)key,aSize);
	aMessage.ReadL(aIndex,pckg); 
	TDbLookupKey::SColumn* iter=CONST_CAST(TDbLookupKey::SColumn*,key->First());
	const TDbLookupKey::SColumn* end=iter+key->Count();
	if ((TUint8*)end-(TUint8*)key<aSize)
		{	// there is some text data following...
		do
			{
			switch (iter->iType)
				{
			default:
				break;
			case EDbColText8:
				iter->iDes8.iPtr=(TUint8*)key+TInt(iter->iDes8.iPtr);
				break;
			case EDbColText16:
				iter->iDes16.iPtr=(TUint16*)((TUint8*)key+TInt(iter->iDes8.iPtr));
				break;
				};
			} while (++iter<end);
		}
	return key;
	}

// Reads the row from the client buffer into the server-side buffer
void CDbsSession::ReadRowL(RDbRow& aRow,TInt aSize,const RMessage2& aMessage)
	{
	if (aSize)
		{
		aRow.GrowL(aSize);
		TPtr8 buf((TUint8*)aRow.First(),aSize);
		aMessage.ReadL(0,buf);
		}
	aRow.SetSize(aSize);
	}

// Writes a row from the client into the cursor
// Any blobs which are modified must not be transferred into the cursor row buffer
void CDbsSession::PutRowL(CDbCursor& aCursor,const RMessage2& aMessage)
	{
	RDbRow* pRow=aCursor.RowBuffer();
	TInt size= aMessage.Int1();
	const TBool omitBlobs=aMessage.Int2();

	if (!omitBlobs && pRow)
		{	// write straight into cursor row
		ReadRowL(*pRow,size,aMessage);
		return;
		}
	RDbRow row;
	row.PushL();
	ReadRowL(row,size,aMessage);
	TInt max=aCursor.ColumnCount();
	for (TInt ii=0;++ii<=max;)
		{
		if (omitBlobs && TDbCol::IsLong(aCursor.ColumnType(ii)))
			;
		else
			aCursor.Column(ii).SetL(row.ColCell(ii));
		}
	CleanupStack::PopAndDestroy();		// row
	}

// Writes the row buffer to the client row buffer, if big enough
// If the client buffer is too small it will attempt a second retrieval
TInt CDbsSession::WriteRowL(const RDbRow& aRow,const RMessage2& aMessage)
	{
	TInt size=aRow.Size();
	if (size>0 && size<=aMessage.Int2())
		aMessage.WriteL(3,TPtrC8((const TUint8*)aRow.First(),size));
	return size+1;		// return 0 reserved for Goto
	}

// Returns a row to the client
TInt CDbsSession::RetrieveRowL(CDbCursor& aCursor,const RMessage2& aMessage)
	{
	RDbRow* pRow=aCursor.RowBuffer();
	if (pRow)	// direct row transfer
		return WriteRowL(*pRow,aMessage);
	// need to retrieve columns independantly
	RDbRow row;
	row.PushL();
	TInt max=aCursor.ColumnCount();
	for (TInt ii=0;++ii<=max;)
		TDbColumn(row,ii).SetL(aCursor.ColumnC(ii));
	TInt size=WriteRowL(row,aMessage);
	CleanupStack::PopAndDestroy();	// row
	return size;
	}

//This method creates new EDbsCursor type object.
//It is used at table level so it needs table object security policies.
//The related table MPolicy interface will be 
//put together with the EDbsCursor object in TEntry list.
//
//Complete the cursor construction
TInt CDbsSession::NewCursorL(CDbCursor* aCursor,const RMessage2& aMessage, const MPolicy* aTblSecurityPolicy)
	{
	aCursor->PushL();		// it has a context: use safe cleanup
	TPckgBuf<TDbsColumns> cols;
	TInt ii=cols().iCount=aCursor->ColumnCount();
	if (ii<=cols().EMax)
		{
		while (--ii>=0)
			cols().iData[ii]=TUint8(aCursor->ColumnType(ii+1));
		}
	aMessage.WriteL(3,cols);
	CleanupStack::Pop(aCursor);
	return Add(aCursor, aTblSecurityPolicy);
	}

//This method creates new EDbsIncremental type object.
//It is used either at database or  table level so it needs database or table object
//security policies. The related database/table MPolicy interface will be
//put together with the EDbsIncremental object in TEntry list.
//
//Complete a message which returns an incremental object by writing the step back
//When there is no work to do, aIncremental may be 0: then return a null handle
TInt CDbsSession::NewIncrementalL(CDbIncremental* aIncremental,
								  TInt& aInit,const RMessage2& aMessage,
								  const MPolicy* aPolicy)
	{
	aIncremental->PushL();		// it has a context: use safe cleanup
	aMessage.WriteL(3,TPckgC<TInt>(aInit));
	CleanupStack::Pop();
	return aIncremental ? Add(aIncremental, aPolicy) : 0;
	}

//This method creates new EDbsStream type object.
//It is used either at database or table level so it needs either 
//database or table object security policies.
//The related database/table MPolicy interface will be
//put together with the EDbsStream object in TEntry list.
//
//Complete a stream based message return
TInt CDbsSession::NewStreamL(MStreamBuf* aHost,const RMessage2& aMessage,
							 const MPolicy* aPolicy,TInt aExtent)
	{
	aHost->PushL();
	TPckgBuf<TDbsStreamBuf> buf;
	if (aExtent>0)	// read the first buffer-full
		aHost->ReadL(buf().iData,Min(aExtent,KDbsStreamBufSize));
	TInt h=0;
	if (aExtent<0 || aExtent>KDbsStreamBufSize)
		{	// create the stream object
		HDbsStream* stream = new(ELeave) HDbsStream(aHost,KDbsStreamBufSize);
		h=Add(stream, aPolicy);
		CleanupStack::Pop();		// aHost
		}
	else	// no more data to send
		CleanupStack::PopAndDestroy();		// aHost
	if (aExtent>=0)
		{
		buf().iExt=aExtent;
		aMessage.WriteL(3,buf);
		}
	return h;
	}

//This method creates new EDbsStream type object.
//It is used at database level so it needs the database object security policies.
//The related database MPolicy interface will be 
//put together with the EDbsStream object in TEntry list.
//
//Generic object passing code
//aPtr should be on the cleanup stack, aExter can externalize it
TInt CDbsSession::NewStreamL(TAny* aPtr,TExternalizeFunction aExter,
							 const RMessage2& aMessage, const MPolicy* aDbSecurityPolicy)
	{
	RWriteStream strm(HBufBuf::NewLC());
	aExter(aPtr,strm);
	strm.CommitL();
	TInt ext=strm.Sink()->SizeL();
	CleanupStack::Pop();			// host
	CleanupStack::PopAndDestroy();	// aPtr
	TInt res = NewStreamL(strm.Sink(),aMessage,aDbSecurityPolicy,ext);
	return res;
	}

//allocates a free entry if required
void CDbsSession::AllocL()
	{
	__ASSERT(TUint(iFree)<=TUint(iSize));
	if (iFree==iSize)
		{
		TInt size=iSize+EIndexGranularity;
		if (size>KDbsIndexLimit)	// maximum number of objects reached
			__LEAVE(KErrNoMemory);
		TEntry* ix=iIx=(TEntry*)User::ReAllocL(iIx,size*sizeof(TEntry));
		iSize=size;
		for (TInt ii=iFree;ii<size;)
			{
			TEntry& e=ix[ii];
			e.iNext=++ii;
			e.iType=TUint8(EDbsFree);
			e.iPolicy = NULL;
			}
		}
	}

// Sets the free entry and return a handle to it
TInt CDbsSession::DoAdd(TAny* aObject,TDbsType aType, const MPolicy* aPolicy)
	{
	__ASSERT(TUint(iFree)<TUint(iSize));
	__ASSERT(aType!=EDbsFree);
	TInt ix=iFree;
	TEntry& e=iIx[ix];
	__ASSERT(e.Type()==EDbsFree);
	iFree=e.iNext;
	__ASSERT(TUint(iFree)<=TUint(iSize));
	e.iObject=aObject;
	e.iType=TUint8(aType);
	e.iPolicy = aPolicy;
	TInt magic=(e.iMagic+1)&KDbsMagicMask;
	e.iMagic=TUint8(magic);
	return DbsMakeHandle(ix,magic,aType);
	}

// releases the object and the entry to the free pool
void CDbsSession::Free(TEntry& aEntry)
	{
	__ASSERT(TUint(iFree)<=TUint(iSize));
	aEntry.Release();
	aEntry.iNext=iFree;
	iFree=&aEntry-&iIx[0];
	__ASSERT(TUint(iFree)<TUint(iSize));
	}

CDbsSession::TEntry& CDbsSession::Object(TInt aHandle)
	{
	TEntry* e=0;
	TInt ix=DbsObjectIndex(aHandle);
	if (TUint(ix)<TUint(iSize))
		{
		e=&iIx[ix];
		if (DbsMakeHandle(ix,e->iMagic,e->Type())!=aHandle)
			e=0;
		}
	__ASSERT_ALWAYS(e && e->iPolicy,::Panic(EDbsBadHandle));
	return *e;
	}

// Reports how many objects are allocated by the client
TInt CDbsSession::CountResources()
	{
	TInt alloc=0;
	const TEntry* const base=iIx;
	if (base)
		{
		for (const TEntry* e=base+iSize;--e>=base;)
			{
			if (e->Type()!=EDbsFree)
				++alloc;
			}
		}
	return alloc;
	}

#ifdef __DBDUMP__
//Using CDbsSession::Dump() method you can dump the session content
//into a stream. Note that the dump works only if you have __DBDUMP__ macro defined.
void CDbsSession::Dump()
	{
	RFile dumpFile;
	RFs fileSess;
	TInt err = fileSess.Connect();
	if(err == KErrNone)
		{
		_LIT(KDumpFileName, "_:\\PUBLIC\\DBMS\\DUMP%X.TXT");
		TFileName dumpFileName(KDumpFileName);
		dumpFileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
		
		TBuf<40> buf;
		buf.Format(dumpFileName, this);
		err = fileSess.MkDirAll(buf);
		if(err == KErrNone || err == KErrAlreadyExists)
			{
			err = dumpFile.Replace(fileSess, buf, EFileWrite);
			if(err == KErrNone)
				{
				TEntry* const base = iIx;
				if(base)
					{
					for(TEntry* e=base+iSize;--e>=base;)
						{
						if(e->Type() != EDbsFree)
							{
							e->Dump(dumpFile);
							}
						}
					}//end of - if(base)
				}//end of - if(err == KErrNone)
			}
		}//end of - if(err == KErrNone)
	dumpFile.Close();
	fileSess.Close();
	RDebug::Print(_L("CDbsSession::Dump() error=%d\n"), err);
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	}
#endif//__DBDUMP__

/**
Reserves a prederfined amount of disk space on aDrive drive. 
At the moment the max possible amount, allowed by the file server, is reserved on aDrive drive.

Use this call to ensure that if your "delete records" transaction fails because of "out of
disk space" circumstances, you can get an access to the already reserved disk space and
complete your transaction successfully the second time.

There is no strong, 100% guarantee, that the reserved disk space will always help the client
in "low memory" situations.

This method processes EDbsReserveDriveSpace message.

@param aDrive Drive number to reserve space on
@leave KErrArgument Invalid aDrive value.
@leave CDbsSessDriveSpace::ReserveL() leaving error codes
@see CDbsSessDriveSpace::ReserveL()
*/
void CDbsSession::ReserveDriveSpaceL(TDriveNumber aDrive)
	{
	if(aDrive < EDriveA || aDrive > EDriveZ)
        {
        __LEAVE(KErrArgument);
        }
    iSessDriveSpace->ReserveL(aDrive);
	}

/**
The method frees the reserved by the DBMS session disk space.

This method processes EDbsFreeReservedSpace message.

@param aDrive Drive number, which reserved space has to be freed.
@see CDbsSessDriveSpace::Free()
@panic Client side panic "DBMS-server 10" in debug mode, if the drive number is invalid.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no reserved disk space for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       the reserved disk space is granted but not released.
*/
void CDbsSession::FreeReservedSpace(TDriveNumber aDrive)
    {
    TBool valid = aDrive >= EDriveA && aDrive <= EDriveZ;
    __ASSERT_DEBUG(valid, ::Panic(EDbsInvalidDrive));
    if(valid)
        {
        iSessDriveSpace->Free(aDrive);
        }
    }

/**
Grants access to a given area on a given drive for CDbsSession session.
Note this session must have reserved space on this particular drive in order to be 
granted access to the reserved area.

This method processes EDbsReserveGetAccess message.
  
@param aDrive Drive number with a reserved disk space, an access to which is requested.
@leave KErrArgument Invalid drive.
@leave CDbsSessDriveSpace::GrantAccessL() leaving error codes
@see CDbsSessDriveSpace::GrantAccessL()
*/
void CDbsSession::GetReserveAccessL(TDriveNumber aDrive)
	{
	if(aDrive < EDriveA || aDrive > EDriveZ)
        {
        __LEAVE(KErrArgument);
        }
    iSessDriveSpace->GrantAccessL(aDrive);
	}

/**
Revokes access on a given drive for CDbsSession session.

This method processes EDbsReserveReleaseAccess message.
  
@param aDrive Drive number with a reserved disk space, the access to which has to be released.
@panic Client side panic "DBMS-server 10" in debug mode, if the drive number is invalid.
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no reserved disk space for aDrive. 
@panic In debug mode there will be a panic with the line number as an error code if 
       there is no granted access to the reserved disk space. 
*/
void CDbsSession::ReleaseReserveAccess(TDriveNumber aDrive)
	{
    TBool valid = aDrive >= EDriveA && aDrive <= EDriveZ;
    __ASSERT_DEBUG(valid, ::Panic(EDbsInvalidDrive));
    if(valid)
        {
        iSessDriveSpace->ReleaseAccess(aDrive);
        }
	}
