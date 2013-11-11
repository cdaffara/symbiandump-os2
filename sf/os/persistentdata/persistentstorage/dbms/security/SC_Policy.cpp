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
// CPolicyBase, CDbPolicy, CTblPolicy, CPolicyDomain classes
// 
//

#include "SC_Policy.h"

namespace DBSC
{

///////////////////////////////////////////////////////////////////////////////////////////
//CPolicyBase class

/**
*/
CPolicyBase::~CPolicyBase()
	{
	iPolicyCollection.Close();//Collection of R/W/S security policies
	}

#ifdef __DBDUMP__
/**
Dumps the content of a CPolicyBase instance to a text file.
@param aFile A reference to RFile object, which has to be used for the output.
*/
void CPolicyBase::Dump(RFile& aFile) const
	{
	DB_INVARIANT();

	_LIT8(KClassName, "Class: CPolicyBase. this=%X");
	_LIT8(KCount, "Security Policy, Count=%d");
	_LIT8(KCrLf, "\r\n");
	_LIT8(KPolicyType, "Policy type: ");
	_LIT8(KRead,   "Read, ");
	_LIT8(KWrite,  "Write, ");
	_LIT8(KSchema, "Schema, ");
	_LIT8(KPolicyData, "Policy data: ");
	_LIT8(KFmt, "%02X ");
	TBuf8<100> buf;

	buf.Format(KClassName, this);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);

	TInt cnt = iPolicyCollection.Count();
	buf.Format(KCount, TInt32(cnt));
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);

	for(TInt i=0;i<cnt;++i)
		{
		const TPolicy& policy = iPolicyCollection[i];
		(void)aFile.Write(KPolicyType);
		switch(policy.iType)
			{
			case EPTRead:
				(void)aFile.Write(KRead);
				break;
			case EPTWrite:
				(void)aFile.Write(KWrite);
				break;
			case EPTSchema:
				(void)aFile.Write(KSchema);
				break;
			default:
				__ASSERT(0);
				break;
			}
		(void)aFile.Write(KPolicyData);
		TPtrC8 packet = policy.iData.Package();
		TInt len = packet.Length();
		for(TInt j=0;j<len;++j)
			{
			buf.Format(KFmt, packet[j]);
			(void)aFile.Write(buf);
			}
		(void)aFile.Write(KCrLf);
		}
	}
#endif//__DBDUMP__

/**
Standard phase-two construction method for CPolicyBase instance.
@param aPolicyCollection A const reference to a collection of R/W/S policies, which has to
       be used to control the access to a database object, controlled by CPolicyBase
	   instance.
*/
void CPolicyBase::ConstructL(const CPolicyBase::RPolicyCollection& aPolicyCollection)
	{
	iPolicyCollection.Reset();
	TInt cnt = aPolicyCollection.Count();
	for(TInt i=0;i<cnt;++i)
		{
		__LEAVE_IF_ERROR(iPolicyCollection.Append(aPolicyCollection[i]));
		}
	DB_INVARIANT();
	}

/**
It is used in the production code.
If the object data is not in a consistent state, the method will leave 
with KErrGeneral error.
@leave KErrGeneral, if the object data is not in a consistent state
*/
void CPolicyBase::InvariantL() const
	{
	TUint32 mask = 0;
	for(TInt i=(iPolicyCollection.Count()-1);i>-1;--i)
		{
		TPolicy& policy = const_cast <TPolicy&> (iPolicyCollection[i]);
		if(policy.iType == EPTNone)
			{
			__LEAVE(KErrGeneral);
			}
		if(mask & policy.iType)	//This security policy is duplicated
			{
			__LEAVE(KErrGeneral);
			}
		TPtrC8 packet = policy.iData.Package();
		if(policy.iData.Set(packet) != KErrNone)
			{
			__LEAVE(KErrGeneral);
			}
		mask |= policy.iType;
		}
	}

/**
This method implements pure virtual MPolicy::Get().
It searches object's policy collection for a policy of type aPolicyType
and initializes aPolicy parameter with the found policy.
@param aPolicyType Type of the requested security policy: read/write/schema
@param aPolicy Outout parameter, which will be initialized with the found security policy data.
@return System-wide error code, including KErrNotFound if the requested policy was not found.
*/
TInt CPolicyBase::Get(TPolicyType aPolicyType, TSecurityPolicy& aPolicy) const
	{
	DB_INVARIANT();
	TInt err = KErrNotFound;
	const TSecurityPolicy* securityPolicy = Policy(aPolicyType);
	if(securityPolicy)
		{
		err = aPolicy.Set(securityPolicy->Package());
		}
	return err;
	}

#ifdef __DBINVARIANT__
/**
Asserts the internal state of CPolicyBase instance.
It can be used for pre- or post- condition checks in CPolicyBase methods implementations.
*/
void CPolicyBase::Invariant() const
	{
	TRAPD(err, InvariantL());
	DB_INVARIANT_ASSERT(err == KErrNone);
	}
#endif//__DBINVARIANT__

/**
The method traverses the policies collection and searches for a policy of aPolicyType type.
If such a policy exists, a const pointer to it will be returned, otherwise - NULL.
@param aPolicyType Policy type - R/W/S
@return A const pointer to the found policy or NULL if not found.
*/
const TSecurityPolicy* CPolicyBase::Policy(TPolicyType aPolicyType) const
	{
	__ASSERT(aPolicyType != EPTNone);
	const TSecurityPolicy* policy = NULL;
	for(TInt i=(iPolicyCollection.Count()-1);i>-1;--i)
		{
		if(iPolicyCollection[i].iType == aPolicyType)
			{
			policy = &iPolicyCollection[i].iData;
			break;
			}
		}
	return policy;
	}

/**
Asserts caller capabilities/SID/VID.
@param aMessage An object whith caller capabilities/SID/VID, which has to be checked.
@param aPolicyType Policy type - R/W/S. 
@return EPCNotFound - the policy cannot be found
        EPCPassed - policy check passed
        EPCNotPassed - policy check not passed
*/
CPolicyBase::TPolicyCheckResult CPolicyBase::DoCheck(const RMessage2& aMessage, TPolicyType aPolicyType) const
	{
	const TSecurityPolicy* securityPolicy = Policy(aPolicyType);

	if(!securityPolicy)
		{
		return EPCNotFound;
		}

	return securityPolicy->CheckPolicy(aMessage) ? EPCPassed : EPCNotPassed;
	}

///////////////////////////////////////////////////////////////////////////////////////////
//CDbPolicy class

/**
*/
CDbPolicy::~CDbPolicy()
	{
	}

/**
Asserts caller capabilities/SID/VID.
@param aMessage An object whith caller capabilities/SID/VID, which has to be checked.
@param aPolicyType Policy type - R/W/S. 
@return ETrue The caller capabilities/SID/VID satisfy the specified security policy.
        EFalse The check not passed.
@panic EDBSCPolicyNotFound, if there is no such policy 
*/
TBool CDbPolicy::Check(const RMessage2& aMessage, TPolicyType aPolicyType) const
	{
	__ASSERT(aPolicyType != EPTNone);
	DB_INVARIANT();
	TPolicyCheckResult res = DoCheck(aMessage, aPolicyType);
	__ASSERT(res != EPCNotFound);
	return  res == EPCPassed ? ETrue : EFalse;
	}

/**
Standard phase-one factory method for CDbPolicy instance.
@param aPolicyCollection A const reference to a collection of R/W/S policies, which has to
       be used to control the access to the database, controlled by CDbPolicy instance.
@return A pointer to just created CDbPolicy instance.
@leave System-wide error codes, including KErrNoMemory.
*/
CDbPolicy* CDbPolicy::NewLC(const CPolicyBase::RPolicyCollection& aPolicyCollection)
	{
	CDbPolicy* self = new (ELeave) CDbPolicy;
	CleanupStack::PushL(self);
	self->ConstructL(aPolicyCollection);
	return self;
	}

#ifdef __DBDUMP__
/**
Dumps the content of a CDbPolicy instance to a text file.
@param aFile A reference to RFile object, which has to be used for the output.
*/
void CDbPolicy::Dump(RFile& aFile) const
	{
	DB_INVARIANT();

	_LIT8(KClassName, "Class: CDbPolicy. this=%X");
	_LIT8(KCrLf, "\r\n");
	_LIT8(KObjType, "Object: Database");
	_LIT8(KEnd, "==========================");
	TBuf8<40> buf;

	buf.Format(KClassName, this);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	(void)aFile.Write(KObjType);
	(void)aFile.Write(KCrLf);
	CPolicyBase::Dump(aFile);
	(void)aFile.Write(KEnd);
	(void)aFile.Write(KCrLf);
	}
#endif//__DBDUMP__

/**
It is used in the production code.
If the object data is not in a consistent state, the method will leave 
with KErrGeneral error.
@leave KErrGeneral, if the object data is not in a consistent state
*/
void CDbPolicy::InvariantL() const
	{
	for(TInt c=0;c<KPolicyTypesCount;++c)
		{
		TPolicyType t = static_cast <TPolicyType> (1 << c);
		if(Policy(t) == NULL)
			{
			__LEAVE(KErrGeneral);
			}
		}
	CPolicyBase::InvariantL();
	}

///////////////////////////////////////////////////////////////////////////////////////////
//CTblPolicy class

/**
*/
CTblPolicy::~CTblPolicy()
	{
	delete iTblName;
	}

/**
Asserts caller capabilities/SID/VID.
@param aMessage An object whith caller capabilities/SID/VID, which has to be checked.
@param aPolicyType Policy type - R/W/S. 
@return ETrue The caller capabilities/SID/VID satisfy the specified security policy.
        EFalse The check not passed.
@panic EDBSCPolicyNotFound, if there is no such policy 
*/
TBool CTblPolicy::Check(const RMessage2& aMessage, TPolicyType aPolicyType) const
	{
	__ASSERT(aPolicyType != EPTNone);
	__ASSERT(iDbPolicy);
	DB_INVARIANT();
	TPolicyCheckResult res = EPCNotPassed;
	//1. Check database security policy
	if(iDbPolicy->Check(aMessage, aPolicyType))
		{
	//2. Check table security policy
		res = DoCheck(aMessage, aPolicyType);
		}
	//If there is no table security policy of the requested type - no problem, the database
	//security policy of that type has been checked already and the check passed.
	return res == EPCNotPassed ? EFalse : ETrue;
	}

/**
This method implements pure virtual MPolicy::Get().
It searches object's policy collection for a policy of type aPolicyType
and initializes aPolicy parameter with the found policy.
@param aPolicyType Type of the requested security policy: read/write
@param aPolicy Outout parameter, which will be initialized with the found security policy data.
@return System-wide error codes, including KErrNotSupported, if the request is for a schema policy.
*/
TInt CTblPolicy::Get(TPolicyType aPolicyType, TSecurityPolicy& aPolicy) const
	{
	if(aPolicyType == EPTSchema)
		{
		return KErrNotSupported;
		}
	DB_INVARIANT();
	TInt err = CPolicyBase::Get(aPolicyType, aPolicy);
	if(err == KErrNotFound)
		{
		err = iDbPolicy->Get(aPolicyType, aPolicy);
		}
	__ASSERT(err != KErrNotFound);
	return err;
	}

/**
Standard phase-one factory method for CTblPolicy instance.
@param aTblName Name of the controlled by this instance database table.
@param aPolicyCollection A const reference to a collection of R/W/S policies, which has to
       be used to control the access to the table, controlled by CTblPolicy instance.
@param aDbPolicy The related for the table database policy.
       CTblPolicy instance does not take the ownership on aDbPolicy pointer!        
@return A pointer to just created CTblPolicy instance.
@leave System-wide error codes, including KErrNoMemory.
*/
CTblPolicy* CTblPolicy::NewLC(const TDesC& aTblName, 
							  const CPolicyBase::RPolicyCollection& aPolicyCollection,
							  const CDbPolicy* aDbPolicy)
	{
	CTblPolicy* self = new (ELeave) CTblPolicy(aDbPolicy);
	CleanupStack::PushL(self);
	self->ConstructL(aTblName, aPolicyCollection);
	return self;
	}

#ifdef __DBDUMP__
/**
Dumps the content of a CTblPolicy instance to a text file.
@param aFile A reference to RFile object, which has to be used for the output.
*/
void CTblPolicy::Dump(RFile& aFile) const
	{
	DB_INVARIANT();

	_LIT8(KClassName, "Class: CTblPolicy. this=%X");
	_LIT8(KDbPolicyPtr, "Db policy ptr=%X");
	_LIT8(KCrLf, "\r\n");
	_LIT8(KName, "Table name: ");
	_LIT8(KObjType, "Object: Table");
	_LIT8(KEnd, "==========================");
	TBuf8<100> buf;

	buf.Format(KClassName, this);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	(void)aFile.Write(KObjType);
	(void)aFile.Write(KCrLf);
	buf.Format(KDbPolicyPtr, iDbPolicy);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	buf.Copy(KName);
	buf.Append(*iTblName);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	CPolicyBase::Dump(aFile);
	(void)aFile.Write(KEnd);
	(void)aFile.Write(KCrLf);
	}
#endif//__DBDUMP__

/**
It is used in the production code.
If the object data is not in a consistent state, the method will leave 
with KErrGeneral error.
@leave KErrGeneral, if the object data is not in a consistent state
*/
void CTblPolicy::InvariantL() const
	{
	if(iDbPolicy == NULL)
		{
		__LEAVE(KErrGeneral);
		}
	if(iTblName == NULL || iTblName->Length() == 0)
		{
		__LEAVE(KErrGeneral);
		}
	if(Policy(EPTSchema) != NULL)
		{
		__LEAVE(KErrGeneral);
		}
	CPolicyBase::InvariantL();
	}

/**
Standard phase-two construction method for CTblPolicy instance.
@param aTblName Name of the controlled by this instance database table.
@param aPolicyCollection A const reference to a collection of R/W/S policies, which has to
       be used to control the access to the table object, controlled by CTblPolicy
	   instance.
*/
void CTblPolicy::ConstructL(const TDesC& aTblName, const CPolicyBase::RPolicyCollection& aPolicyCollection)
	{
	iTblName = HBufC::NewL(aTblName.Length());
	*iTblName = aTblName;
	CPolicyBase::ConstructL(aPolicyCollection);
	DB_INVARIANT();
	}

///////////////////////////////////////////////////////////////////////////////////////////
//CPolicyDomain class

/**
Standard phase-one factory method for CPolicyDomain instance.
@param aUid UID of the controlled by this instance security policy domain.
@param aPDLoader A reference to an implementation of MPolicyDomainLoader interface,
       which is used to load and add security policies to the controlled collection.
@return A pointer to just created CPolicyDomain instance.
@leave System-wide error codes, including KErrNoMemory.
*/
CPolicyDomain* CPolicyDomain::NewLC(TUid aUid, MPolicyDomainLoader& aPDLoader)
	{
	CPolicyDomain* self = new (ELeave) CPolicyDomain(aUid);
	CleanupStack::PushL(self);
	self->InternalizeL(aPDLoader);
	return self;
	}

/**
*/
CPolicyDomain::~CPolicyDomain()
	{
	Destroy();
	}

/**
The method returns the database policy interface.
@return A const pointer to the database policy interface in CPolicyDomain.
*/
const MPolicy* CPolicyDomain::DbPolicy() const
	{
	DB_INVARIANT();
	return iDbPolicy;
	}

/**
The method returns a table policy interface, identified by aTblName parameter.
@param aTblName Name of the table, which policy interface has to be retrieved.
@return A const pointer to the table policy interface, which is identified by aTblName parameter.
*/
const MPolicy* CPolicyDomain::TblPolicy(const TDesC& aTblName) const
	{
	__ASSERT(aTblName.Length() > 0);
	DB_INVARIANT();
	const MPolicy* policy = NULL;
	TInt cnt = iTPCollection.Count();
	for(TInt i=0;i<cnt;++i)
		{
		CTblPolicy* tblPolicy = iTPCollection[i];
		__ASSERT(tblPolicy);
		if(aTblName.CompareF(tblPolicy->TableName()) == 0)
			{
			policy = tblPolicy;
			break;
			}
		}
	if(!policy)
		{
		policy = iDbPolicy;
		}
	__ASSERT(policy);
	return policy;
	}

/**
Externalizes the security policy collection using MPolicyDomainPersister interface as an
persister.
@param aPDPersister A reference to an MPolicyDomainPersister implementation, which will 
       persist the controlled collection of security policies.
*/
void CPolicyDomain::ExternalizeL(MPolicyDomainPersister& aPDPersister) const
	{
	DB_INVARIANT();
	TPolicyDomainReader reader(*this);
	aPDPersister.RunL(reader);
	}

#ifdef __DBDUMP__
/**
Dumps the content of a CPolicyDomain instance to a text file.
@param aFile A reference to RFile object, which has to be used for the output.
*/
void CPolicyDomain::Dump(RFile& aFile) const
	{
	DB_INVARIANT();

	_LIT8(KClassName, "Class: CPolicyDomain. this=%X");
	_LIT8(KUidFmt, "UID=%X");
	_LIT8(KCrLf, "\r\n");
	_LIT8(KEnd, "==========================");
	_LIT8(KBackupSIDFmt, "BackupSID=%X");
	TBuf8<40> buf;

	buf.Format(KClassName, this);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	buf.Format(KUidFmt, iUid.iUid);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	(void)aFile.Write(KEnd);
	(void)aFile.Write(KCrLf);
	iDbPolicy->Dump(aFile);
	TInt cnt = iTPCollection.Count();
	for(TInt i=0;i<cnt;++i)
		{
		__ASSERT(iTPCollection[i]);
		iTPCollection[i]->Dump(aFile);
		}
	(void)aFile.Write(KEnd);
	buf.Format(KBackupSIDFmt, iBackupSID.iUid);
	(void)aFile.Write(buf);
	(void)aFile.Write(KCrLf);
	}
#endif//__DBDUMP__

/**
It is used in the production code.
If the object data is not in a consistent state, the method will leave 
with KErrGeneral error.
@leave KErrGeneral, if the object data is not in a consistent state
*/
void CPolicyDomain::InvariantL() const
	{
	if(iUid == KNullUid)
		{
		__LEAVE(KErrGeneral);
		}
	if(iDbPolicy == NULL)
		{
		__LEAVE(KErrGeneral);
		}
	iDbPolicy->InvariantL();

	TInt cnt = iTPCollection.Count();
	TInt i;
	for(i=0;i<cnt;++i)
		{
		if(iTPCollection[i] == NULL)
			{
			__LEAVE(KErrGeneral);
			}
		iTPCollection[i]->InvariantL();
		}
	//Check that each represented table has unique name
	for(i=0;i<(cnt-1);++i)
		{
		for(TInt j=(i+1);j<cnt;++j)
			{
			if(iTPCollection[i]->TableName() == iTPCollection[j]->TableName())
				{
				__LEAVE(KErrGeneral);
				}
			}
		}
	}

#ifdef __DBINVARIANT__
/**
Asserts the internal state of CPolicyDomain instance.
It can be used for pre- or post- condition checks in CPolicyDomain methods implementations.
*/
void CPolicyDomain::Invariant() const
	{
	TRAPD(err, InvariantL());
	DB_INVARIANT_ASSERT(err == KErrNone);
	}
#endif//__DBINVARIANT__

/**
Creates the collection of security policies using MPolicyDomainLoader interface as a security
policy loader.
@param aPDLoader A reference to MPolicyDomainLoader implementation, which is used to load
       and add security policies to the controlled collection.
@leave System-wide error code including KErrGeneral if the data is not consistent
*/
void CPolicyDomain::InternalizeL(MPolicyDomainLoader& aPDLoader)
	{
	TPolicyDomainBuilder builder(*this);
	aPDLoader.RunL(builder);
#ifdef __DBINVARIANT__
	Invariant();
#else
	InvariantL();
#endif
	}

/**
The method destroys the controlled by CPolicyDomain collection of security policies.
*/
void CPolicyDomain::Destroy()
	{
	TInt cnt = iTPCollection.Count();
	for(TInt i=0;i<cnt;++i)
		{
		__ASSERT(iTPCollection[i]);
		delete iTPCollection[i];
		}
	iTPCollection.Close();
	delete iDbPolicy;
	iDbPolicy = NULL;
	}

} //end of - namespace DBSC

