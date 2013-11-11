// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __STRINGPOOLIMPLEMENTATION_H__
#define __STRINGPOOLIMPLEMENTATION_H__

#include <e32base.h>
#include <stringpool.h>
#include <stringtablesupport.h>

/**
This is an 8 bit value with every bit set except bit 5 (value
32). The ASCII codes of upper and lower case characters differ by
32.
@internalComponent 
*/
const TUint KCaseInsensitive = 223;

/**
@internalComponent 
*/
const TInt KHashModulo = 32;

/**
Bit 0 represents if it is a table (set) or not (unset).
Bit 1 represents case sensitivity (set) or not (unset)

For non-static table values (dynamic strings), the remaining bits 
are interpreted as a pointer to a CStringPoolNode (which is assumed 
to be word aligned). 

Bits 2-20 represent the table index.
Bits 21-31 represent the table uid (that is the address of the table).
*/

/**
Bit 1 is used in tokens to indicate case sensitivity. So mask it
out to get the node.
@internalComponent 
*/
const TUint KTokenToNode = 0xfffffffd;

/**
Determine if this id corresponds to a table.
@internalComponent
*/
#define IS_TABLE_ENTRY(tableId)(tableId & 0x01)

/**
Obtain the Table Index.
@internalComponent
*/
#define TABLE_INDEX(val)((val & 0xffffc)>>2)

/**
Obtain the Table UID.
@internalComponent
*/
#define TABLE_UID(val)(val>>20)

/**
This is used to mark the reference count of a node so that is will not be deleted when closed
@internalComponent
*/
const TUint KMarkedForNoDeleted = 0xffff;



class RStringTokenEither;

/**
@internalComponent
*/
struct TStringIdMap
	{
	TInt32 iSourceTableVal;
	TInt32 iTargetTableVal;
	};


/**
Internal node class
@internalComponent
*/
NONSHARABLE_CLASS(CStringPoolNode) : public CBase
	{
public:
	~CStringPoolNode();

 public:
	HBufC8* iDes;
	TUint16 iRefcount;
	TUint8  iHash;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CStringPoolImplementation) : public CBase
	{
public:
	CStringPoolImplementation();
	// Destructor
	~CStringPoolImplementation();

	// Constructs a string pool
	static CStringPoolImplementation* NewL();

	// USed for cleaning up when a OpenTableL leaves
	static void CleanupHashCS(TAny* aImplementation);
	static void CleanupHashCI(TAny* aImplementation);
	static void CleanupIdMap(TAny* aImplementation);
	static void CleanUpHash(RPointerArray <RStringTokenEither>* aHashCleanup, CArrayFixSeg<RStringTokenEither>* hash[KHashModulo]);

	// Adds a table to the pool. (Note this currently only works once)
	void AddTableL(const TStringTable& aTable);
	void AddCallBackL( MStringPoolCloseCallBack& aCallBack);


	// Find FirstVal given duplicate val
	TInt32 FindFirstValFromDuplicate(TInt32 aDuplicateVal) const;

	// Find table index Val given first val & table UID
	TInt FindTableIndexFromFirstVal(TInt32 aDuplicateVal, TInt aTableUid) const;


	TInt16 TableUid(const TStringTable& aTable) const;

	const TStringTable& TableRef(TInt32 aVal) const;

	// Looks up a particular index in the pre-loaded tables
	const TDesC8& TableLookup(TInt aIndex, TInt aTableUid) const;

	// Finds or creates a string. Increments the reference count if needed.
	RStringTokenEither OpenL( const TDesC8& aString, TBool aCaseInsensitive); 

	// Closes a string (decrements the reference count and deletes if 0
	void Close(RStringTokenEither aString);

	// Increments the reference count on a string
	void IncrementCount(RStringTokenEither aString);

	inline RStringPool Handle();

private:	

	// Check for any undeletable string and delete them now
	void DeleteUndeletableStrings(CArrayFixSeg<RStringTokenEither>* aArray[KHashModulo], TInt i);

private:

	// Finds a string in the pool.
	RStringTokenEither FindDes( const TDesC8& aString, TBool aCaseInsensitive);

	// Calculates a hash for a descriptor
	TUint Hash( const TDesC8& ) const;

	static TBool CompareCS(const TDesC8& s1, const TDesC8& s2);
	static TBool CompareCI(const TDesC8& s1, const TDesC8& s2);

 private:
	// The table.
	CArrayFixSeg<RStringTokenEither>* iCSHashTable[KHashModulo];
	CArrayFixSeg<RStringTokenEither>* iCIHashTable[KHashModulo];	
	RPointerArray<TStringTable> iTablePtrs;	// Stores array of tables, where the index is the table UID
	RArray <TStringIdMap> iStringMapList;
	RArray <TStringIdMap> iStringMapListReverse;

	// For rolling back when a leave occurs during CreateTableL
	RPointerArray <TStringIdMap> iRollbackMapList;	
	RPointerArray <RStringTokenEither> iRollbackHashListCS;
	RPointerArray <RStringTokenEither> iRollbackHashListCI;
	RPointerArray<MStringPoolCloseCallBack> iCallBacks;
	};

/** 
An internal version of the string token class. This class can hold
either folding or non-folding versions, and it is up to the user
to get it right.
@internalComponent
*/
class RStringTokenEither : public RStringTokenBase
	{
 public:
	inline RStringTokenEither();

	inline RStringTokenEither(TUint32 aVal);

	/** Comparison operator
		@param aVal The string to compare. */
	inline TBool operator==(RStringTokenEither aVal) const;

	/** Comparison operator
		@param aVal The string to compare. */
	inline TBool operator!=(RStringTokenEither aVal) const;

	/** Assignment operator; makes a string token from a string.
		@param aVal The string to copy */
	inline RStringTokenEither operator=(RStringBase aVal);

	friend class RStringPool;
	friend class RStringEither;
	friend class CStringPoolImplementation;
	};

class RStringEither : public RStringBase
/**
@internalComponent
*/
	{
	public:
	RStringEither(CStringPoolImplementation* aPool, RStringTokenEither aVal);
	};

class StringUtils
/**
@internalComponent
*/
	{
 public:
	static inline TBool IsTableEntry(TInt aVal);
	static inline TInt TableIndex(TInt aVal);
	static inline TInt16 TableUid(TInt aVal);
	static inline CStringPoolNode* NodePtr(TInt aVal);
	static TInt ValFromIndex(TInt aIndex, TUint16 aTableId);
	static TInt ValFromIndexF(TInt aIndex, TUint16 aTableId);
	static TInt ValFromIndex(TInt aIndex, TUint16 aTableId, TBool aCaseSensitive);
	
	static void LogIt(TRefByValue<const TDesC8> aFmt, ...);
	static void LogIt1(TRefByValue<const TDesC8> aFmt);
	};

#define _LOGGING

#if defined (_DEBUG) && defined (_LOGGING)

/**
HTTP Logging macros
@internalComponent
*/
#define __LOG(C)			StringUtils::LogIt1(C);
#define __LOG1(C, X)		StringUtils::LogIt(C, X);
#define __LOG2(C, X, Y)		StringUtils::LogIt(C, X, Y);
#define __LOG3(C, X, Y, Z)	StringUtils::LogIt(C, X, Y, Z);

#else

/**
NULL macros
@internalComponent
*/
#define __LOG(C)			
#define __LOG1(C, X)		
#define __LOG2(C, X, Y)		
#define __LOG3(C, X, Y, Z)	
#endif // !_DEBUG


inline TBool StringUtils::IsTableEntry(TInt aVal)
	{
	return IS_TABLE_ENTRY(aVal);
	}

inline TInt StringUtils::TableIndex(TInt aVal)
	{
	return TABLE_INDEX(aVal);
	}

inline TInt16 StringUtils::TableUid(TInt aVal)
	{
	return (TInt16) TABLE_UID(aVal);
	}

inline CStringPoolNode* StringUtils::NodePtr(TInt aVal)
	{
	return reinterpret_cast<CStringPoolNode*>(aVal & KTokenToNode);
	}

inline RStringEither::RStringEither(CStringPoolImplementation* aPool, 
							 RStringTokenEither aVal)
	{
	iPool = aPool->Handle();
	iVal = aVal.iVal;
	}

inline RStringPool CStringPoolImplementation::Handle()
	{
	RStringPool p;
	p.iImplementation = this;
	return p;
	}

inline RStringTokenEither::RStringTokenEither()
	{
	}

inline RStringTokenEither::RStringTokenEither(TUint32 aVal)
	{
	iVal = aVal;
	}

inline TBool RStringTokenEither::operator==(RStringTokenEither aVal) const
	{
	return iVal == aVal.iVal;
	}

inline TBool RStringTokenEither::operator!=(RStringTokenEither aVal) const
	{
	return iVal != aVal.iVal;
	}

inline RStringTokenEither RStringTokenEither::operator=(RStringBase aVal)
	{
	RStringTokenBase b = aVal;
	iVal = b.iVal;
	return *this;
	}


#endif // __STRINGPOOLIMPLEMENTATION_H__
