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
#include <f32file.h>
#include <e32uid.h>

// Class CDbContext

inline void CDbContext::Open()
	{
    ++iRef;
    }

//
// Attach an object to this context
//
void CDbContext::Attach(CDbObject* aObject)
	{
	if (aObject && aObject->iContext!=this)
		{
		__ASSERT(!aObject->iContext);
		aObject->iContext=this;
		Open();
		}
	}

//
// self destruct when we are no longer referenced
//
void CDbContext::Close()
	{
	__ASSERT(iRef>0);
	if (--iRef==0)
		Idle();
	}

//
// default implementation is to delete ourself
//
void CDbContext::Idle()
	{
	delete this;
	}

// Class CDbObject

//
// Attach aObject to the same context as it's factory
//
CDbObject* CDbObject::Attach(CDbObject* aObject)
	{
	CDbContext* context=iContext;
	if (context)
		context->Attach(aObject);
	return aObject;
	}

//
// Library safe destruction of a implementation object
// This ensures that all objects from a Driver are deleted BEFORE the library is unloaded
//
void CDbObject::Destroy(CDbObject* aObject)
	{
	if (aObject)
		{
		CDbContext* context=aObject->iContext;
		delete aObject;
		if (context)
			context->Close();
		}
	}

//
// Library safe cleanup-stack function
//
void CDbObject::PushL()
	{
	CleanupStack::PushL(TCleanupItem(TCleanupOperation(Destroy),this));
	}


// Class CDbSource

//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
// Encapsulate the 2-phase construction for client access
//
CDbDatabase* CDbSource::OpenL()
	{
	return AuthenticateL();
	}
	
// Class RDbNamedDatabase

//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
LOCAL_C CDbDatabase* OpenDatabaseL(TDbFormat::TOpen aMode, RFs& aFs, const TDesC& aSource,
                                   const TDesC& /*aFormat*/)
	{
	CDbSource* source=KBuiltinDriver.iFormats[0].OpenL(aFs,aSource,aMode);
	CleanupStack::PushL(source);
	CDbDatabase* db=source->OpenL();
	CleanupStack::PopAndDestroy();		// source (not needed)
	return db;
	}

//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
//The method may be used in the other cpp files too
CDbDatabase* CreateDatabaseL(TDbFormat::TCreate aMode, RFs& aFs, const TDesC& aSource,
                             const TDesC& aFormat)
	{
	const TDbFormat& fmt=KBuiltinDriver.iFormats[0];
	
	TUid policyId = KNullUid;
	TPtrC uidName;
	
	::ExtractUidAndName(aFormat, policyId, uidName);

	CDbDatabase* db=fmt.CreateL(aFs,aSource,aMode,
	  TUidType(TUid::Uid(fmt.iUid[0]),TUid::Uid(fmt.iUid[1]),policyId));
		
	return db;
	}


/**
Creates a new non-secure database.

In this "single client" mode, the database cannot be shared with the other clients.
The database server is not involved in the operations with the database, the client side 
database library (edbms.dll) will be used.
If the database has to be shared, the following example shows how this may be accomplished:
@code
RFs fs;
TInt err = fs.Connect();
<process the error>
_LIT(KDatabaseName, _L("C:\\A.DB"));
RDbNamedDatabase db;
err = db.Create(fs, KDatabaseName);		//Step 1 - create the database using the RFs object
<process the error>
db.Close();								//Step 2 - close the database
RDbs dbs;
err = dbs.Connect();
<process the error>
err = db.Open(dbs, KDatabaseName);		//Step 3 - reopen the database using the RDbs object
<process the error>
...
@endcode

Max allowed database name length (with the extension) is KDbMaxName symbols.

For creating a new secure shared database, see RDbNamedDatabase::Create(), which first argument 
is a RDbs reference.

@param aFs Handle to a file server session.
@param aSource Database file name.
@param aFormat Database format string. It can be omitted in which case the default parameter
			   value (TPtrC()) will be used.
@return KErrNone if successful otherwise one of the system-wide error codes, including:
		KErrAlreadyExists - the database already exists;
		KErrArgument - bad argument, including null/invaluid uids, the database name includes a null;

@see RDbNamedDatabase::Create(RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat)

@publishedAll
@released
*/
EXPORT_C TInt RDbNamedDatabase::Create(RFs& aFs, const TDesC& aSource, const TDesC& aFormat)
	{
	TRAPD(r,iDatabase=CreateDatabaseL(TDbFormat::ECreate,aFs,aSource,aFormat));
	return r;
	}


/**
Creates a new non-secure database. 
If a database with the same file name exists, it will be replased.

In this "single client" mode, the database cannot be shared with the other clients.
The database server is not involved in the operations with the database, the client side 
database library (edbms.dll) will be used.
If the database has to be shared, the following example shows how this may be accomplished:
@code
RFs fs;
TInt err = fs.Connect();
<process the error>
_LIT(KDatabaseName, _L("C:\\A.DB"));
RDbNamedDatabase db;
err = db.Replace(fs, KDatabaseName);	//Step 1 - create the database using the RFs object
<process the error>
db.Close();								//Step 2 - close the database
RDbs dbs;
err = dbs.Connect();
<process the error>
err = db.Open(dbs, KDatabaseName);		//Step 3 - reopen the database using the RDbs object
<process the error>
...
@endcode

Max allowed database name length (with the extension) is KDbMaxName symbols.

For creating a new secure shared database, see RDbNamedDatabase::Create(), which first argument 
is a RDbs reference.

@param aFs Handle to a file server session.
@param aSource Database name. The name format is: <drive>:<path><name>.<ext>
@param aFormat Database format string. It can be omitted in which case the default parameter
			   value (TPtrC()) will be used.
@return KErrNone if successful otherwise one of the system-wide error codes, including:
		KErrArgument - bad argument, including null/invaluid uids, the database name includes a null;

@see RDbNamedDatabase::Create(RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat)

@publishedAll
@released
*/
EXPORT_C TInt RDbNamedDatabase::Replace(RFs& aFs, const TDesC& aSource, const TDesC& aFormat)
	{
	TRAPD(r,iDatabase=CreateDatabaseL(TDbFormat::EReplace,aFs,aSource,aFormat));
	return r;
	}

/**
Opens an existing non-secure database.

In this "single client" mode, the database cannot be shared with the other clients.
The database server is not involved in the operations with the database, the client side 
database library (edbms.dll) will be used.

For opening a new secure shared database, see RDbNamedDatabase::Open(), which first argument 
is a RDbs reference.

@param aFs Handle to a file server session.
@param aSource Database name. The name format is: <drive>:<path><name>.<ext>
@param aFormat Database format string. It can be omitted in which case the default parameter
			   value (TPtrC()) will be used.
@param aMode The mode in which the database is to be accessed. The mode is 
defined by the TAccess type.
@return KErrNone if successful otherwise one of the system-wide error codes, including:
		KErrNotFound - the database is not found;
		KErrPathNotFound - the path of database is not found 
		KErrNotSupported - the format is not supported.
		KErrArgument - bad argument,including null/invaluid uids,the database name is null;
		KErrPermissionDenied - the caller has not enough rights to do the operation;

@see RDbNamedDatabase::Open(RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat)

@publishedAll
@released
*/
EXPORT_C TInt RDbNamedDatabase::Open(RFs& aFs, const TDesC& aSource, const TDesC& aFormat,
                                     TAccess aMode)
	{
	TRAPD(r,iDatabase=OpenDatabaseL(TDbFormat::TOpen(aMode),aFs,aSource,aFormat));
	return r;
	}
	

CDbNotifier* CDbSource::NotifierL()
	{
	return AttachContext(this,OpenNotifierL());
	}
