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

#ifndef __SQLSRVSECURITYMAP_H__
#define __SQLSRVSECURITYMAP_H__

#include "SqlMap.h"		//RSqlMap

//Forward declarations
class CSqlSecurityPolicy;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// {Secure database name : Security policy} map declarations
// "Secure database name" is UTF8 encoded, zero-terminated.
//
// "Secure database name" == <DRIVE:><NAME>[.EXT]
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
{const TUint8*, const CSqlSecurityPolicy*} pair destructor.
Used as a template argument (DESTRUCTOR) in RSqlMap class.

@see TSqlPair
@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlSecurityDestructor)
	{
	void Destroy(const TUint8*& aKey, const CSqlSecurityPolicy*& aData);
	};

/**
{const TUint8*, const CSqlSecurityPolicy*} pair reference counter.
Used as a template argument (REFCNTR) in RSqlMap class.

@see TSqlPair
@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlSecurityRefCounter)
	{
	inline TSqlSecurityRefCounter() :
		iRefCount(0)
		{
		(void)Increment();
		}
	inline TInt Increment()
		{
		return ++iRefCount;	
		}
	inline TInt Decrement()
		{
		return --iRefCount;	
		}
	
	TInt iRefCount;
	};

//Typedefs for {const TUint8*, const CSqlSecurityPolicy*} map.
typedef struct TSqlPair<const TUint8*, const CSqlSecurityPolicy*, TSqlSecurityRefCounter>  TSqlSecurityPair;
typedef class TLinearOrder<TSqlSecurityPair> TSqlSecurityLinearOrder;
typedef class RSqlMap<const TUint8*, const CSqlSecurityPolicy*, TSqlSecurityRefCounter, TSqlSecurityDestructor>  RSqlSecurityMap;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// {Attached database name : Secure database file name} map declarations
// "Attached database name" is UTF8 encoded, zero-terminated.
// "Secure database file name" is UTF8 encoded, zero-terminated. Format: <Drive:><FileName[.Ext]>
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
{const TUint8*, const TUint8*} pair destructor.
Used as a template argument (DESTRUCTOR) in RSqlMap class.

@see TSqlPair
@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlAttachDbDestructor)
	{
	void Destroy(const TUint8*& aKey, const TUint8*& aData);
	};

/**
{const TUint8*, const TUint8*} pair reference counter.
Used as a template argument (REFCNTR) in RSqlMap class.

@see TSqlPair
@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlAttachDbRefCounter)
	{
	inline TSqlAttachDbRefCounter()
		{
		(void)Increment();
		}
	inline TInt Increment()
		{
		return 1;	
		}
	inline TInt Decrement()
		{
		return 0;	
		}
	};

//Typedefs for {const TUint8*, const TUint8*} map.
typedef struct TSqlPair<const TUint8*, const TUint8*, TSqlAttachDbRefCounter>  TSqlAttachDbPair;
typedef class TLinearOrder<TSqlAttachDbPair> TSqlAttachDbLinearOrder;
typedef class RSqlMap<const TUint8*, const TUint8*, TSqlAttachDbRefCounter, TSqlAttachDbDestructor>  RSqlAttachDbMap;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// {Compact database name ("main" or attached db name) : Full database file name} map declarations
// "Compact database name" is UTF16 encoded.
// "Full database file name" is UTF16 encoded.
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
A reference counter for the TSqlCompactDbPair pair and RSqlCompactDbMap.

@see TSqlPair
@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlCompactDbRefCounter)
	{
	inline TSqlCompactDbRefCounter()
		{
		(void)Increment();
		}
	inline TInt Increment()
		{
		return 1;	
		}
	inline TInt Decrement()
		{
		return 0;	
		}
	};

/**
Destrucor for the TSqlCompactDbPair pair.

@see TSqlPair
@see TSqlMapIterator
@see RSqlMap

@internalComponent
*/
NONSHARABLE_STRUCT(TSqlCompactDbDestructor)
	{
	inline void Destroy(const HBufC*& aKey, const HBufC*& aData)
		{
		delete const_cast <HBufC*&> (aData);
		delete const_cast <HBufC*&> (aKey);
		}
	};

//Typedefs for the {const HBufC*, const HBufC*} map.
typedef struct TSqlPair<const HBufC*, const HBufC*, TSqlCompactDbRefCounter>  TSqlCompactDbPair;
typedef class TLinearOrder<TSqlCompactDbPair> TSqlCompactDbLinearOrder;
typedef class RSqlMap<const HBufC*, const HBufC*, TSqlCompactDbRefCounter, TSqlCompactDbDestructor>  RSqlCompactDbMap;
typedef class TSqlMapIterator<const HBufC*, const HBufC*, TSqlCompactDbRefCounter, TSqlCompactDbDestructor> TSqlCompactDbMapIterator;
typedef TSqlMapIterator<const TUint8*, const TUint8*, TSqlAttachDbRefCounter, TSqlAttachDbDestructor>  TSqlAttachDbMapIterator;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//__SQLSRVSECURITYMAP_H__
