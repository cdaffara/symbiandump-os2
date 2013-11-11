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
// CPolicyBase, CDbPolicy, CTblPolicy, CPolicyDomain, 
// TPolicyDomainBuilder, TPolicyDomainReader classes.
// MPolicyDomainPersister, MPolicyDomainLoader interfaces
// 
//

#ifndef __SC_POLICY_H__
#define __SC_POLICY_H__

#include <e32base.h>		//CBase
#include "D32Security.h"

namespace DBSC
{

//Forward declarations
class CPolicyDomain;

/**
CPolicyBase class implements MPolicy interface.
It describes an object that manages a set of TSecurityPolicy objects.
CPolicyBase::iPOType data member gives an information access to what kind of 
database object (database or table) is controlled by the set of security policies.
CPolicyBase::Check() can be used to check caller access rights against specified policy type.
@see MPolicy
@internalComponent
*/
NONSHARABLE_CLASS(CPolicyBase) : public CBase, public MPolicy
	{
public:
	struct TPolicy
		{
		TPolicyType		iType;
		TSecurityPolicy	iData;
		};
	typedef RArray<TPolicy> RPolicyCollection;
	typedef enum {EPCNotFound, EPCPassed, EPCNotPassed} TPolicyCheckResult;

public:
	virtual ~CPolicyBase();
	virtual TBool Check(const RMessage2& aMessage, TPolicyType aPolicyType) const = 0; 
	virtual TInt Get(TPolicyType aPolicyType, TSecurityPolicy& aPolicy) const;
	virtual void InvariantL() const;
	DECLARE_DB_INVARIANT()
	inline const RPolicyCollection& PolicyCollection() const;

protected:
	inline CPolicyBase();
	void ConstructL(const RPolicyCollection& aPolicyCollection);
	const TSecurityPolicy* Policy(TPolicyType aPolicyType) const;
	TPolicyCheckResult DoCheck(const RMessage2& aMessage, TPolicyType aPolicyType) const; 
	DECLARE_DB_DUMP2(aFile)

private:
	RPolicyCollection	iPolicyCollection;

	};

/**
CDbPolicy class describes an object that manages the access to all databases, which
have the same format UID.
@see CPolicyBase
@see MPolicy
@internalComponent
*/
NONSHARABLE_CLASS(CDbPolicy) : public CPolicyBase
	{
public:
	static CDbPolicy* NewLC(const RPolicyCollection& aPolicyCollection);
	inline static CDbPolicy* NewL(const RPolicyCollection& aPolicyCollection);
	virtual ~CDbPolicy();
	virtual TBool Check(const RMessage2& aMessage, TPolicyType aPolicyType) const; 
	virtual void InvariantL() const;
	DECLARE_DB_DUMP2(aFile)

private:
	inline CDbPolicy();

	};

/**
CTblPolicy class describes an object that manages the access to all tables, which
have particular format UID and particular name.
@see CPolicyBase
@see MPolicy
@internalComponent
*/
NONSHARABLE_CLASS(CTblPolicy) : public CPolicyBase
	{
public:
	static CTblPolicy* NewLC(const TDesC& aTblName, 
							 const RPolicyCollection& aPolicyCollection,
							 const CDbPolicy* aDbPolicy);
	inline static CTblPolicy* NewL(const TDesC& aTblName, 
								   const RPolicyCollection& aPolicyCollection,
								   const CDbPolicy* aDbPolicy);
	virtual ~CTblPolicy();
	virtual TBool Check(const RMessage2& aMessage, TPolicyType aPolicyType) const; 
	virtual TInt Get(TPolicyType aPolicyType, TSecurityPolicy& aPolicy) const;
	virtual void InvariantL() const;
	inline const TDesC& TableName() const;
	DECLARE_DB_DUMP2(aFile)

private:
	inline CTblPolicy(const CDbPolicy* aDbPolicy);
	void ConstructL(const TDesC& aTblName, const RPolicyCollection& aPolicyCollection);

private:
	HBufC*	iTblName;
	const CDbPolicy* iDbPolicy;

	};

/**
TPolicyDomainBuilder class describes an object that can be used during the initialization
to initialize CPolicyDomain objects. It is used by MPolicyDomainLoader interface.
@internalComponent
*/
class TPolicyDomainBuilder
	{
public:
	inline TPolicyDomainBuilder(CPolicyDomain& aPolicyDomain);
	void SetDbPolicyL(CDbPolicy* aDbPolicy);
	inline void AddTblPolicyL(CTblPolicy* aTblPolicy);
	inline void SetBackupSID(TSecureId& aSecureId);
private:
	CPolicyDomain&	iPolicyDomain;
	};

/**
TPolicyDomainBuilder class describes an object that can be used to explore the content of
CPolicyDomain objects. It is used by MPolicyDomainPersister interface.
@internalComponent
*/
class TPolicyDomainReader
	{
public:
	inline TPolicyDomainReader(const CPolicyDomain& aPolicyDomain);
	inline TUid Uid() const;
	inline const CDbPolicy& DbPolicy() const;
	inline void ResetTblPos() const;
	inline TInt TblPolicyCount() const;
	inline const CTblPolicy* NextTblPolicy() const;
	inline TSecureId BackupSID() const;
private:
	const CPolicyDomain&	iPolicyDomain;
	mutable TInt			iIndex;
	};

/**
MPolicyDomainPersister interface has to be implemented by DBSC clients, who want to store
the information from CPolicyDomain objects (set of security policies) somewhere - text files,
streams, ... It uses TPolicyDomainReader class to traverse CPolicyDomain collection of
security policies.
@see TPolicyDomainReader
@internalComponent
*/
class MPolicyDomainPersister
	{
public:
	virtual void RunL(const TPolicyDomainReader& aPolicyDomainReader) = 0;
	};

/**
MPolicyDomainPersister interface has to be implemented by DBSC clients, who want to load
set of security policies to CPolicyDomain objects from somewhere - text files,
streams, ... It uses TPolicyDomainBuilder class to add to CPolicyDomain collection new
security policies.
@see TPolicyDomainBuilder
@internalComponent
*/
class MPolicyDomainLoader
	{
public:
	virtual void RunL(TPolicyDomainBuilder& aPolicyDomainBuilder) = 0;
	};

/**
CPolicyDomain object describes a set of all security policies related to particular format UID.
It describes only a collection of security policies and offers some functionality for
retrieving particular database/table MPolicy interfaces.
The responsibility for adding new items to secirity policy collection is delegated to 
TPolicyDomainBuilder class.
The responsibility for traversing secirity policy collection is delegated to 
TPolicyDomainReader class.
Both, TPolicyDomainBuilder and TPolicyDomainReader classes are not used directly by 
the CPolicyDomain class implementation. CPolicyDomain instances are created using 
MPolicyDomainLoader interface and externalized using MPolicyDomainPersister interface and
they (the interfaces) use TPolicyDomainBuilder and TPolicyDomainReader respectively.
The delegation of responsibilities for creation/traversing of CPolicyDomain security policy collection
was done because CPolicyDomain class is a part of shared sources used in DBMS server implementation
and DbSpConv tool implementation.
DBMS server creates CPolicyDomain security policy collection from a binary policy files using
TPDStreamLoader class (which implements MPolicyDomainLoader) and TPolicyDomainBuilder class
to insert created CDbPolicy and CTblPolicy instances into CPolicyDomain collection.
The rest of the possibilities: creating CPolicyDomain collection from a text policy file,
exporting CPolicyDomain collection to a binary policy file, 
exporting CPolicyDomain collection to a text policy file, are used by the DbSpConv tool.
So, CPolicyDomain class uses interfaces for its loading/storing, but their 
implementations are part of separate exe-s - no waste of production code.
@see MPolicy
@see CDbPolicy
@see CTblPolicy
@see TPolicyDomainBuilder
@see TPolicyDomainReader
@see MPolicyDomainLoader
@see MPolicyDomainPersister
@internalComponent
*/
NONSHARABLE_CLASS(CPolicyDomain) : public CBase
	{
	friend class TPolicyDomainBuilder;
	friend class TPolicyDomainReader;
public:
	static CPolicyDomain* NewLC(TUid aUid, MPolicyDomainLoader& aPDLoader);
	inline static CPolicyDomain* NewL(TUid aUid, MPolicyDomainLoader& aPDLoader);
	virtual ~CPolicyDomain();
	void ExternalizeL(MPolicyDomainPersister& aPDPersister) const;

	inline TUid Uid() const;
	const MPolicy* DbPolicy() const;
	const MPolicy* TblPolicy(const TDesC& aTblName) const;
	inline TSecureId BackupSID() const;

	virtual void InvariantL() const;
	DECLARE_DB_DUMP3(aFile)

private:
	inline CPolicyDomain(TUid aUid);
	void InternalizeL(MPolicyDomainLoader& aPDLoader);
	void Destroy();
	DECLARE_DB_INVARIANT2()

private:
	TUid						iUid;
	CDbPolicy*					iDbPolicy;
	RPointerArray<CTblPolicy>	iTPCollection;
	TSecureId 					iBackupSID;

	};

} //end of - namespace DBSC

#include "SC_Policy.inl"

#endif//__SC_POLICY_H__
