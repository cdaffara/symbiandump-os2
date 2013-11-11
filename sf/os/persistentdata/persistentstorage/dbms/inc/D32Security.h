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
//

#ifndef __D32SECURITY_H__
#define __D32SECURITY_H__

#include "D32Assert.h"

//Forward declarations
class RFs;

/**
DBSC namespace is a placeholder for security policy framework.
DBSC stands for [D]ata[B]ase [S]e[C]urity.
@internalComponent
*/
namespace DBSC
{

/**
KPolicyTypesCount specifies how many different policy type are maintained by the system.
@internalComponent
*/
const TInt KPolicyTypesCount = 3;

/**
Each secure shared database/table have a security policy associated with it.
There are three security policy types:"READ" - EPTRead - for any database/table read operation,
                                     "WRITE"- EPTWrite - for any database/table write operation.
                                     "SCHEMA"- EPTSchema - for any database admin operation.
To execute particular DBMS operation, the caller must have a set of Capabilities/SID/VID,
which must satisfy related R/W/S security policies of the database/table, on which the operation 
has to be performed.
@internalComponent
*/
typedef enum 
	{
	EPTNone		= 0, 
	EPTRead		= 1 << 0, 
	EPTWrite	= 1 << 1,
	EPTSchema	= 1 << 2,
	EPTLast	    = 1 << (KPolicyTypesCount - 1)
	} TPolicyType;

/**
Type of the controled by the security policy object: database or table
@internalComponent
*/
typedef enum
	{
	EPOTNone,
	EPOTDatabase,
	EPOTTable
	} TPolicyObjType;

/**
This enum represents possible type of the requested access when opening/creating a database
on the server side:
EATNonSecure - non-secure access to private/legacy/shared-non-secure database
EATSecure - secure access to shared-secure database
@internalComponent
*/
typedef enum 
	{
	EATNonSecure, 
	EATSecure
	} TAccessType;

/**
This structure packs together the uid from the database format string and
requested access type to the database.
@internalComponent
*/
struct TDbPolicyRequest
	{
	TUid		iUid;
	TAccessType	iAccessType;
	};

/**
MPolicy interface is used to check DBMS client capabilities against the security policy
managed by this interface.
The Check() method parameter, aPolicyType, specifies against which policy (R/W/S) caller
capabilities/SID/VID have to be asserted.
Do not put MPolicy interfaces in the CleanupStack! MPolicySpace instance will
take care about them.
Using MPolicy::Dump() method you can dump the content of the controled object
into a text file. Note that the dump works only if you have __DBDUMP__ macro defined.
@internalComponent
*/
class MPolicy
	{
public:
	virtual TBool Check(const RMessage2& aMessage, TPolicyType aPolicyType) const = 0; 
	virtual TInt Get(TPolicyType aPolicyType, TSecurityPolicy& aPolicy) const = 0;
	DECLARE_DB_DUMP(aFile)
	};

/**
MPolicySpace interface represents an interface to the security policiy space, which manages
all the security policies, presented in the system.
It can be used to retrieve MPolicy interface for particular database/table object or
getting the backup&restore security ID.
MPolicySpace interface manages static data structure, created during the DBMS startup.
The data in this structure will never be modified during the DBMS server life time.
DbPolicyL() and TblPolicyL() leave with KErrArgument error, if there is no policy for
the database/table object, represented in the method arguments.
@internalComponent
*/
class MPolicySpace
	{
public:
	virtual void Release() = 0;
	virtual const MPolicy* DbPolicyL(const TDbPolicyRequest& aDbPolicyRequest) const = 0;
	virtual const MPolicy* TblPolicyL(const TDbPolicyRequest& aDbPolicyRequest, const TDesC& aTblName) const = 0;
	virtual TSecureId BackupSIDL(TUid aDbUid) const = 0;
	};

/**
TPolicySpaceFactory is a factory class, used for creating an object, which implements 
MPolicySpace interface. 
Do not forget that MPolicySpace is a "M" interface, so if 
you want to push it in the Cleanup Stack, you should use CleanupReleasePushL() call, but not
CleanupStack::PushL().
@internalComponent
*/
class TPolicySpaceFactory
	{
public:
	static MPolicySpace* NewPolicySpaceL(RFs& aFs, const TDesC& aPrivatePath);
	};

} //end of - namespace DBSC

#endif//__D32SECURITY_H__
