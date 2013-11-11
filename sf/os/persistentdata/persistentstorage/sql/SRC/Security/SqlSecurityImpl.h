// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSECURITYIMPL_H__
#define __SQLSECURITYIMPL_H__

#include <sqldb.h>
#include "SqlBufFlat.h"		//RSqlBufFlat

//Forward declarations
class TSqlSecurityPolicyIterator;

/**
CSqlSecurityPolicy class implements RSqlSecurityPolicy.

Current CSqlSecurityPolicy implementation uses a flat buffer (see RSqlBufFlat class) as a storage for the database 
objects security policies.
By default the flat buffer has threee entries:
 - entry 0 is a system entry and keeps the count of the database security entries;
 - entry 1 is reserved for the default database security policy. This is a nameless entry;
 - entry 2 is reserved for the database security policies: read, write and schema. This is a nameless entry;
The flat buffer may have more entries if database object security policies are added. In this case each flat buffer entry
has a 16-bit encoded database object name, kept at the end of the entry, after the database object security policy data and
database object name length.
Note: Current CSqlSecurityPolicy implementation can have no more than 13 database object security policy entries.
An access to the flat buffer can be obtained via CSqlSecurityPolicy::BufFlat() methods (const and non-const).
There are two ways to externalize (resp. internalize) CSqlSecurityPolicy data:
 - (1) using CSqlSecurityPolicy::ExternalizeL()/CSqlSecurityPolicy::InternalizeL();
 - (2) using CSqlSecurityPolicy::BufFlat() to get an access to the flat buffer and then externalize/internalize the buffer.
       CSqlSecurityPolicy class has only one data member which is the flat buffer;

TSqlSecurityPolicyIterator class gives a convinient way to access the security policies kept inside the flat buffer.
The given access is a read-only.

@see RSqlSecurityPolicy
@see RSqlBufFlat
@see TSqlSecurityPolicyIterator
@see RSqlSecurityPolicy::TPolicyType
@see RSqlSecurityPolicy::TObjectType
@see CSqlSecurityPolicy::NewL()
@see CSqlSecurityPolicy::NewLC()
@see CSqlSecurityPolicy::SetDefaultPolicy()
@see CSqlSecurityPolicy::SetDbPolicy()
@see CSqlSecurityPolicy::SetPolicy()
@see CSqlSecurityPolicy::DefaultPolicy()
@see CSqlSecurityPolicy::DbPolicy()
@see CSqlSecurityPolicy::Policy()
@see CSqlSecurityPolicy::ExternalizeL()
@see CSqlSecurityPolicy::InternalizeL()
@see CSqlSecurityPolicy::BufFlat()

@internalComponent
*/
NONSHARABLE_CLASS(CSqlSecurityPolicy) : public CBase
	{
	friend class TSqlSecurityPolicyIterator;
	struct TPolicyItem;

public:
	static CSqlSecurityPolicy* NewL(const TSecurityPolicy& aDefaultPolicy);
	static CSqlSecurityPolicy* NewLC(const TSecurityPolicy& aDefaultPolicy);
	virtual ~CSqlSecurityPolicy();
	void SetDefaultPolicy(const TSecurityPolicy& aPolicy);
	void SetDbPolicy(RSqlSecurityPolicy::TPolicyType aPolicyType, const TSecurityPolicy& aPolicy);
	TInt SetPolicy(RSqlSecurityPolicy::TObjectType aObjectType, const TDesC& aObjectName, RSqlSecurityPolicy::TPolicyType aPolicyType, const TSecurityPolicy& aPolicy);
	TSecurityPolicy DefaultPolicy() const;
	TSecurityPolicy DbPolicy(RSqlSecurityPolicy::TPolicyType aPolicyType) const;
	TSecurityPolicy Policy(RSqlSecurityPolicy::TObjectType aObjectType, const TDesC& aObjectName, RSqlSecurityPolicy::TPolicyType aPolicyType);
	TInt Count() const;
	const RSqlBufFlat&	BufFlat() const;
	RSqlBufFlat& BufFlat();

private:
	CSqlSecurityPolicy();
	void ConstructL(const TSecurityPolicy& aDefaultPolicy);
	inline static TInt PolicyType2Index(RSqlSecurityPolicy::TPolicyType aPolicyType);
	inline static TPolicyItem* PolicyItemPtr(const RSqlBufFlat::TCell* aBegin, const RSqlBufFlat::TCell* aCurrent);
	const TPolicyItem* FindPolicyItemPtr(RSqlSecurityPolicy::TObjectType aObjectType, const TDesC& aObjectName) const;
	void SetCount(TInt aCount);
	
private:
	enum {
		 EPolicyTypeCount	= 3,					//Schema, Read, Write policy types
		 EMaxCount 			= 16,					//iBufFlat max element count. The database cannot have more than EMaxCount security policies.
		 ECountIdx			= 0,					//The index of the policy counter
		 EDefaultPolicyIdx	= 1,					//The index of the default policy type in iBufFlat
		 EDbPolicyIdx		= 2						//The index of the database policy type in iBufFlat
		 };					
	
	//The database security policies are kept in objects of TPolicyItem type.
	//If particular TPolicyItem object represents the database security policies, then the memory layout is:
	//    [SchemaPolicy]
	//    [ReadPolicy]
	//    [WritePolicy]
	//If particular TPolicyItem object represents table security policies, then the memory layout is:
	//    [SchemaPolicy]
	//    [ReadPolicy]
	//    [WritePolicy]
	//    [The length of the table name - 32-bit integer]
	//    [The table name]
	//As you can see, TPolicyItem does not have "name length" and "name" data members. If that is needed, an appropriate block of memory will be allocated for the TPolicyItem object.
	//There SQL platform security interface currently supports only database security policies, but the implementation is capable of handling also table security policies.
	struct TPolicyItem
		{
		enum {EPolicyTypeCount	= 3};				//Schema, Read, Write policy types. The same as the declaration above.
		TSecurityPolicy iPolicy[EPolicyTypeCount];	//Database object policies
		//Database object name size will be after the end of TPolicyItem object
		inline TInt* NameSizePtr() 
			{
			//See the comments of the TPolicyItem structure above.
			//coverity[overrun-local]
			return reinterpret_cast <TInt*> (reinterpret_cast <TUint8*> (iPolicy) + sizeof(iPolicy));
			}
		inline TInt NameSize() const 
			{
			//See the comments of the TPolicyItem structure above.
			//coverity[overrun-local]
			return *reinterpret_cast <const TInt*> (reinterpret_cast <const TUint8*> (iPolicy) + sizeof(iPolicy));
			}
		//Database object name will be after the end of TPolicyItem object + sizeof(TInt)
		inline const TUint16* NamePtr() const 
			{
			//See the comments of the TPolicyItem structure above.
			//coverity[overrun-local]
			return reinterpret_cast <const TUint16*> 
				(reinterpret_cast <const TUint8*> (iPolicy) + sizeof(iPolicy) + sizeof(TInt));
			}
		inline TUint16* NamePtr() 
			{
			//See the comments of the TPolicyItem structure above.
			//coverity[overrun-local]
			return reinterpret_cast <TUint16*> (reinterpret_cast <TUint8*> (iPolicy) + sizeof(iPolicy) + sizeof(TInt));
			}
		inline static TInt CalcSize(TInt aNameLen) 
			{
			return sizeof(TPolicyItem) + sizeof(TInt) + aNameLen * sizeof(TUint16);
			}
		};	
	RSqlBufFlat			iBufFlat;
	};

/**
CSqlSecurityPolicy container read-only iterator.

Usage:

@code
//Valid CSqlSecurityPolicy instance.
CSqlSecurityPolicy* securityPolicy;
...
TSqlSecurityPolicyIterator it(*securityPolicy);

//The following variables will be initialized on each iteration
RSqlSecurityPolicy::TObjectType objectType;
TPtrC objectName;
RSqlSecurityPolicy::TPolicyType policyType;
TSecurityPolicy policy;

while(it.Next(objectType, objectName, policyType, policy))
	{
	//"objectType" is initialized with the object type (see RSqlSecurityPolicy::TObjectType)
	//"objectName" is initialized with the object name (database table name)
	//"policyType" is initialized with the policy type (see RSqlSecurityPolicy::TPolicyType)
	//"policy" is initialized with the security policy data (see TSecurityPolicy)
	<do something with the policy>;
	}
	
@endcode

@see CSqlSecurityPolicy
@see RSqlSecurityPolicy::TObjectType
@see RSqlSecurityPolicy::TPolicyType
@see TSecurityPolicy

@internalComponent
*/
NONSHARABLE_CLASS(TSqlSecurityPolicyIterator)
	{
public:
	TSqlSecurityPolicyIterator(const CSqlSecurityPolicy& aSqlSecurityPolicy);
	TBool Next(RSqlSecurityPolicy::TObjectType& aObjectType, TPtrC& aObjectName, RSqlSecurityPolicy::TPolicyType& aPolicyType, TSecurityPolicy& aPolicy);

private:
	const RSqlBufFlat::TCell* 	iBegin;
	const RSqlBufFlat::TCell* 	iCurrent;
	const RSqlBufFlat::TCell* 	iEnd;
	TInt 						iCurPolicyIdx;
	
	};

#endif//__SQLSECURITYIMPL_H__
