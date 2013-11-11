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

#include <s32buf.h>
#include "SqlSecurityImpl.h"
#include "SqlUtil.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    CSqlSecurityPolicy implementation      ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Casts aPolicyType parameter to an integer value, which is used as policy index.

@param aPolicyType Security policy type: schema, write, read

@return Security policy type casted to an integer value, which is used as a poicy index
*/
inline TInt CSqlSecurityPolicy::PolicyType2Index(RSqlSecurityPolicy::TPolicyType aPolicyType)
	{
	return static_cast <TInt> (aPolicyType);
	}

/**
Returns a pointer to the data of the current flat buffer cell. The pointer type is CSqlSecurityPolicy::TPolicyItem.

@param aBegin Points to the beginning of flat buffer header
@param aCurrent Points to a cell in the flat buffer header, to which data a pointer will be cast and returned.

@return CSqlSecurityPolicy::TPolicyItem pointer, which can be used for R/W operations on the database security policy data.
*/
inline CSqlSecurityPolicy::TPolicyItem* CSqlSecurityPolicy::PolicyItemPtr(const RSqlBufFlat::TCell* aBegin, const RSqlBufFlat::TCell* aCurrent)
	{
	__ASSERT_DEBUG(aBegin != NULL && aCurrent != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aCurrent->iPos != 0, __SQLPANIC2(ESqlPanicBadArgument));
	const TUint8* begin = reinterpret_cast <const TUint8*> (aBegin);
	return reinterpret_cast <CSqlSecurityPolicy::TPolicyItem*> (const_cast <TUint8*> (begin) + aCurrent->iPos);
	}

/**
Searhes the flat buffer for an entry, which "object type" and "object name" attributes match aObjectType and aObjectName parameters.
If such entry exists, a pointer to the entry's data will be returned.

@param aObjectType Database object type
@param aObjectName Database object name

@return A pointer to the data of an entry which "object type" and "object name" attributes match aObjectType and aObjectName parameters.
		If no such entry exists, the function returns NULL.
*/
const CSqlSecurityPolicy::TPolicyItem* CSqlSecurityPolicy::FindPolicyItemPtr(RSqlSecurityPolicy::TObjectType aObjectType, 
																	   		 const TDesC& aObjectName) const
	{
	const RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	const RSqlBufFlat::TCell* end = begin + Count();
	const RSqlBufFlat::TCell* current = begin + CSqlSecurityPolicy::EDbPolicyIdx;//ignore default and database policiy types ("current" points before the first non-database policy)
	while(++current < end)
		{
		if(current->iPos > 0 && current->Type() == (TInt)aObjectType) //if present and the same type as aObjectType
			{
			const CSqlSecurityPolicy::TPolicyItem* item = CSqlSecurityPolicy::PolicyItemPtr(begin, current);
			__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
			__ASSERT_DEBUG(((current->Size() - sizeof(CSqlSecurityPolicy::TPolicyItem) - sizeof(TInt)) / sizeof(TUint16)) == item->NameSize(), __SQLPANIC(ESqlPanicInternalError));
			if(::CompareNoCase16(aObjectName, TPtrC(item->NamePtr(), item->NameSize())) == 0)
				{
				return item;	
				}
			}
		}
	return NULL;
	}
		
/**
Standard, phase-one CSqlSecurityPolicy factory method.

@param aDefaultPolicy Default security policy which will be used as a default replacement for all database and
                      database object security policies.

@return A pointer to the created CSqlSecurityPolicy instance.

@leave KErrNoMemory, an out of memory condition has occurred;
*/
CSqlSecurityPolicy* CSqlSecurityPolicy::NewL(const TSecurityPolicy& aDefaultPolicy)
	{
	CSqlSecurityPolicy* self = CSqlSecurityPolicy::NewLC(aDefaultPolicy);
	CleanupStack::Pop(self);
	return self;
	}

/**
Standard, phase-one CSqlSecurityPolicy factory method.

@param aDefaultPolicy Default security policy which will be used as a default replacement for all database and
                      database object security policies.

@return A pointer to the created CSqlSecurityPolicy instance.

@leave KErrNoMemory, an out of memory condition has occurred;
*/
CSqlSecurityPolicy* CSqlSecurityPolicy::NewLC(const TSecurityPolicy& aDefaultPolicy)
	{
	CSqlSecurityPolicy* self = new (ELeave) CSqlSecurityPolicy;
	CleanupStack::PushL(self);
	self->ConstructL(aDefaultPolicy);
	return self;
	}

/**
*/
CSqlSecurityPolicy::~CSqlSecurityPolicy()
	{
	iBufFlat.Close();
	}

/**
@return A const reference to the flat buffer
*/
const RSqlBufFlat& CSqlSecurityPolicy::BufFlat() const
	{
	return iBufFlat;
	}

/**
@return A reference to the flat buffer
*/
RSqlBufFlat& CSqlSecurityPolicy::BufFlat()
	{
	return iBufFlat;
	}

/**
@return Database security policy entry count. This number is at least 2, because every CSqlSecurityPolicy object has by default
        one default security policy entry and one database security policy entry.
*/
TInt CSqlSecurityPolicy::Count() const
	{
	const RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	return *reinterpret_cast <const TInt*> (reinterpret_cast <const TUint8*> (begin) + (begin + CSqlSecurityPolicy::ECountIdx)->iPos);
	}

/**
Sets the number of database security entries.
*/
void CSqlSecurityPolicy::SetCount(TInt aCount)
	{
	__ASSERT_DEBUG(aCount >= 0, __SQLPANIC(ESqlPanicBadArgument));
	RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	*reinterpret_cast <TInt*> (reinterpret_cast <TUint8*> (begin) + (begin + CSqlSecurityPolicy::ECountIdx)->iPos) = aCount;
	}

/**
Sets the default policy.
*/
void CSqlSecurityPolicy::SetDefaultPolicy(const TSecurityPolicy& aPolicy)
	{
	RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	CSqlSecurityPolicy::TPolicyItem* item = CSqlSecurityPolicy::PolicyItemPtr(begin, begin + CSqlSecurityPolicy::EDefaultPolicyIdx);
	__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
	item->iPolicy[0] = aPolicy;
	}
	
/**
@param aPolicyType Database security policy type: RSqlSecurityPolicy::ESchemaPolicy, 
				   RSqlSecurityPolicy::EReadPolicy, RSqlSecurityPolicy::EWritePolicy.
@param aPolicy Security policy data used for setting the related database security policy.

@panic SqlDb 4 In _DEBUG mode. Invalid policy type.
*/
void CSqlSecurityPolicy::SetDbPolicy(RSqlSecurityPolicy::TPolicyType aPolicyType, const TSecurityPolicy& aPolicy)
	{
	const TInt KPolicyIndex = CSqlSecurityPolicy::PolicyType2Index(aPolicyType);
	__ASSERT_DEBUG((TUint)KPolicyIndex < EPolicyTypeCount, __SQLPANIC(ESqlPanicBadArgument));
	RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	CSqlSecurityPolicy::TPolicyItem* item = CSqlSecurityPolicy::PolicyItemPtr(begin, begin + CSqlSecurityPolicy::EDbPolicyIdx);
	__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
	//KPolicyIndex value is tested at the beginning of the function
	//coverity[overrun-local]
	item->iPolicy[KPolicyIndex] = aPolicy;
	}

/**
If there is no entry in the container for the object with aObjectName name, a new entry for this object will be 
created and all object security policies will be initialized with the default security policy. aPolicyType object 
policy will be reinitialized with aPolicy argument after that.

If an entry for aObjectName object already exists, its aPolicyType security policy will be reinitialized with aPolicy
argument.

@param aObjectType Database object type. At the moment there is only one database object type allowed for use with
					SetPolicy() - RSqlSecurityPolicy::ETable.
@param aObjectName Database object name. It cannot be a null descriptor.
@param aPolicyType Database object security policy type: RSqlSecurityPolicy::EReadPolicy, 
				   RSqlSecurityPolicy::EWritePolicy.
@param aPolicy Security policy data used for setting the related database object security policy type.

@return KErrNone, operation completed successfully;
		KErrNoMemory, an out of memory condition has occurred;
		KErrNotSupported, the count of the security policies is too big.

@panic SqlDb 4 In _DEBUG mode. Invalid policy type.
@panic SqlDb 4 In _DEBUG mode. Invalid database object name (Null descriptor).
*/
TInt CSqlSecurityPolicy::SetPolicy(RSqlSecurityPolicy::TObjectType aObjectType, const TDesC& aObjectName,
							   	   RSqlSecurityPolicy::TPolicyType aPolicyType, const TSecurityPolicy& aPolicy)
	{
	const TInt KPolicyIndex = CSqlSecurityPolicy::PolicyType2Index(aPolicyType);
	__ASSERT_DEBUG((TUint)KPolicyIndex < EPolicyTypeCount, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aObjectName.Length() > 0, __SQLPANIC(ESqlPanicBadArgument));
	CSqlSecurityPolicy::TPolicyItem* item = const_cast <CSqlSecurityPolicy::TPolicyItem*> (FindPolicyItemPtr(aObjectType, aObjectName));
	if(item)
		{//There is a field in the flat buffer for {aObjectType, aObjectName}. Set the policy.
		//KPolicyIndex value is tested at the beginning of the function
		//coverity[overrun-local]
		item->iPolicy[KPolicyIndex] = aPolicy;
		return KErrNone;
		}
	//No field in the flat buffer for {aObjectType, aObjectName}.
	TInt idx = Count();
	if(idx >= iBufFlat.Count())
		{
		return KErrNotSupported;
		}
	//Create and fill a new CSqlSecurityPolicy::TPolicyItem object.
	const TInt KPolicyDataLen = TPolicyItem::CalcSize(aObjectName.Length());
	TUint8* buf = new TUint8[KPolicyDataLen];
	if(!buf)
		{
		return KErrNoMemory;	
		}
	item = reinterpret_cast <CSqlSecurityPolicy::TPolicyItem*> (buf);
	//coverity[DEADCODE]
	//The ASSERT might be useful in catching future defect in this function
	__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
	TSecurityPolicy defaultPolicy = DefaultPolicy();
	for(TInt i=0;i<CSqlSecurityPolicy::EPolicyTypeCount;++i)
		{
		item->iPolicy[i] = defaultPolicy;
		}
	item->iPolicy[KPolicyIndex] = aPolicy;
	//Set the object name length and the object name.
	*item->NameSizePtr() = aObjectName.Length();
	TPtr name(item->NamePtr(), item->NameSize());
	name.Copy(aObjectName);
	//Copy the item in iBufFlat and release the allocated memory.
	TInt err = iBufFlat.SetField(idx, (TInt)aObjectType, item, KPolicyDataLen);
	delete [] buf;
	if(err == KErrNone)
		{
		SetCount(idx + 1);
		}
	return err;
	}

/**
@return The default security policy.
*/
TSecurityPolicy CSqlSecurityPolicy::DefaultPolicy() const
	{
	const RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	const CSqlSecurityPolicy::TPolicyItem* item = CSqlSecurityPolicy::PolicyItemPtr(begin, begin + CSqlSecurityPolicy::EDefaultPolicyIdx);
	__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
	return item->iPolicy[0];
	}
	
/**
@param aPolicyType Database security policy type: RSqlSecurityPolicy::ESchemaPolicy, 
				   RSqlSecurityPolicy::EReadPolicy, RSqlSecurityPolicy::EWritePolicy.

Note: By default all database security policies will be initialized with the default security policy.

@return The requested database security policy.
				   
@panic SqlDb 4 In _DEBUG mode. Invalid policy type.
*/
TSecurityPolicy CSqlSecurityPolicy::DbPolicy(RSqlSecurityPolicy::TPolicyType aPolicyType) const
	{
	const TInt KPolicyIndex = CSqlSecurityPolicy::PolicyType2Index(aPolicyType);
	__ASSERT_DEBUG((TUint)KPolicyIndex < EPolicyTypeCount, __SQLPANIC(ESqlPanicBadArgument));
	const RSqlBufFlat::TCell* begin = iBufFlat.Header();
	__ASSERT_DEBUG(begin != NULL, __SQLPANIC(ESqlPanicInternalError));
	const CSqlSecurityPolicy::TPolicyItem* item = CSqlSecurityPolicy::PolicyItemPtr(begin, begin + CSqlSecurityPolicy::EDbPolicyIdx);
	__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
	//KPolicyIndex value is tested at the beginning of the function
	//coverity[overrun-local]
	return item->iPolicy[KPolicyIndex];
	}
	
/**
Searches the container for an entry belonging to an object with aObjectName name and aObjectType type. If such entry
exists the method returns aPolicyType object security policy.

If there is no entry for the object with aObjectName name, the default security policy will be returned.

@param aObjectType Database object type. At the moment there is only one database object type allowed for use with
					Policy() - RSqlSecurityPolicy::ETable.
@param aObjectName Database object name. It cannot be a null descriptor.
@param aPolicyType Database object security policy type: RSqlSecurityPolicy::ERead, RSqlSecurityPolicy::EWrite.

Note: By default all database object security policies will be initialized with the default security policy. 
	  
@return The requested security policy

@panic SqlDb 4 In _DEBUG mode. Invalid policy type.
@panic SqlDb 4 In _DEBUG mode. Invalid onject name (Null descriptor).
*/
TSecurityPolicy CSqlSecurityPolicy::Policy(RSqlSecurityPolicy::TObjectType aObjectType, 
										   const TDesC& aObjectName, RSqlSecurityPolicy::TPolicyType aPolicyType)
	{
	const TInt KPolicyIndex = CSqlSecurityPolicy::PolicyType2Index(aPolicyType);
	__ASSERT_DEBUG((TUint)KPolicyIndex < EPolicyTypeCount, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aObjectName.Length() > 0, __SQLPANIC(ESqlPanicBadArgument));
	const CSqlSecurityPolicy::TPolicyItem* item = FindPolicyItemPtr(aObjectType, aObjectName);
	//KPolicyIndex value is tested at the beginning of the function
	//coverity[overrun-local]
	return item ? item->iPolicy[KPolicyIndex] : DefaultPolicy();
	}

/**
*/
CSqlSecurityPolicy::CSqlSecurityPolicy()
	{
	}

/**
Standard, phase-two CSqlSecurityPolicy construction method.

Note: By default all database security policies will be initialized with the default security policy.

@param aDefaultPolicy Security policy data used for setting the default database security policy and
                      database security policies.

@leave KErrNoMemory, an out of memory condition has occurred;
*/
void CSqlSecurityPolicy::ConstructL(const TSecurityPolicy& aDefaultPolicy)
	{
	//Create the policy flat buffer.
	__SQLLEAVE_IF_ERROR(iBufFlat.SetCount(CSqlSecurityPolicy::EMaxCount));
	//Reserve places for the default policy and database policies.
	CSqlSecurityPolicy::TPolicyItem item;
	for(TInt i=0;i<CSqlSecurityPolicy::EPolicyTypeCount;++i)
		{
		item.iPolicy[i]	= aDefaultPolicy;
		}
	TInt count = CSqlSecurityPolicy::EDbPolicyIdx + 1;//count + default policy + database policy;
	__SQLLEAVE_IF_ERROR(iBufFlat.SetField(CSqlSecurityPolicy::ECountIdx, 0, &count, sizeof(count)));
	__SQLLEAVE_IF_ERROR(iBufFlat.SetField(CSqlSecurityPolicy::EDefaultPolicyIdx, 0, &item, sizeof(item) - sizeof(TSecurityPolicy) * (CSqlSecurityPolicy::EPolicyTypeCount - 1)));
	__SQLLEAVE_IF_ERROR(iBufFlat.SetField(CSqlSecurityPolicy::EDbPolicyIdx, 0, &item, sizeof(item)));
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////           TSqlSecurityPolicyIterator  implementation        //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@param aSecuritySettings Const reference to CSqlSecurityPolicy object on which content the iterator will work.
*/
TSqlSecurityPolicyIterator::TSqlSecurityPolicyIterator(const CSqlSecurityPolicy& aSqlSecurityPolicy) :
	iBegin(aSqlSecurityPolicy.iBufFlat.Header()),
	iCurrent(iBegin + CSqlSecurityPolicy::EDbPolicyIdx),//ignore the default and database policies
	iEnd(iBegin + aSqlSecurityPolicy.Count()),
	iCurPolicyIdx(CSqlSecurityPolicy::EPolicyTypeCount)
	{
	__ASSERT_DEBUG(iBegin != NULL, __SQLPANIC(ESqlPanicInternalError));
	}
	
/**
Moves the iterator one step forward and gives a read-only access to the next database object security policy.

@param aObjectType Output parameter. Next database object type, if the iteration was successful.
@param aObjectName Output parameter. Set to point to the next database object name, if the iteration was successful.
@param aPolicyType Output parameter. Next database object security policy type, if the iteration was successful.
@param aPolicy     Output parameter. Next database object security policy, if the iteration was successful.

@return Non-zero - successful iteration step, zero otherwise (no more security policies in the CSqlSecurityPolicy container).
*/
TBool TSqlSecurityPolicyIterator::Next(RSqlSecurityPolicy::TObjectType& aObjectType, TPtrC& aObjectName, 
									   RSqlSecurityPolicy::TPolicyType& aPolicyType, TSecurityPolicy& aPolicy)
	{
	if(++iCurPolicyIdx >= CSqlSecurityPolicy::EPolicyTypeCount)
		{
		iCurPolicyIdx = static_cast <TInt> (RSqlSecurityPolicy::EReadPolicy);
		if(++iCurrent >= iEnd)
			{
			return EFalse;	
			}
		}
	const CSqlSecurityPolicy::TPolicyItem* item = CSqlSecurityPolicy::PolicyItemPtr(iBegin, iCurrent);
	__ASSERT_DEBUG(item != NULL, __SQLPANIC(ESqlPanicInternalError));
	aObjectType = static_cast <RSqlSecurityPolicy::TObjectType> (iCurrent->Type());
	aPolicyType = static_cast <RSqlSecurityPolicy::TPolicyType> (iCurPolicyIdx);
	aPolicy = item->iPolicy[iCurPolicyIdx];
	aObjectName.Set(item->NamePtr(), item->NameSize());
	return ETrue;		
	}
