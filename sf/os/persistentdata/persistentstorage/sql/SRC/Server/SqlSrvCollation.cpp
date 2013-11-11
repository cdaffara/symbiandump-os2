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
// SqlSrvStatementCollation.cpp
// 
//

#include "SqlSrvCollation.h"
#include "SqlAssert.h"
#include "SqlUtil.h"
#include "sqlite3.h"
#include "SqliteSymbian.h"		//sqlite3SymbianLastOsError()

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        Local const data   ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//4 collated comparisons + 1 folded comparison
const TInt KCollationMethodCount = 4 + 1;

//Names of the user defined collations (zero-terminated 16-bit strings)
const TUint16* KCollationMethodName[KCollationMethodCount] = 
	{
	(const TUint16*)L"CompareC0", 
	(const TUint16*)L"CompareC1", 
	(const TUint16*)L"CompareC2", 
	(const TUint16*)L"CompareC3", 
	(const TUint16*)L"CompareF"
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        Local functions      /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//User-defined collation function prototype, accepted by SQLITE
typedef TInt (*TCollationMethodPtr)(void* aUserDefinedData, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2);

/**
This function is used for collated/folded string comparisons,
{aByteSize1, aPtr1} - string 1, UTF16 encoded. aByteSize1 - length in bytes.
{aByteSize2, aPtr2} - string 2, UTF16 encoded. aByteSize2 - length in bytes.
aLevel - 0,1,2,3 - collation level; -1 - folded string comparison;
The function returns negative, zero or positive if the first string is less than, equal to, or 
greater than the second string.

@internalComponent
*/
static TInt Compare(TInt aLevel, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2)
	{
	TPtrC16 ptr1(static_cast <const TUint16*> (aPtr1), (TUint)aByteSize1 /sizeof(TUint16));
	TPtrC16 ptr2(static_cast <const TUint16*> (aPtr2), (TUint)aByteSize2 /sizeof(TUint16));
	
	return aLevel >= 0 ? ptr1.CompareC(ptr2, aLevel, NULL) : ptr1.CompareF(ptr2);
	}

/**
User defined function which will be used by SQLITE engine for collated string comparisons,
collation level 0.
{aByteSize1, aPtr1} - string 1, UTF16 encoded. aByteSize1 - length in bytes.
{aByteSize2, aPtr2} - string 2, UTF16 encoded. aByteSize2 - length in bytes.
The function returns negative, zero or positive if the first string is less than, equal to, or 
greater than the second string.

@internalComponent
*/
static TInt CompareC0(void*, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2)
	{
	return Compare(0, aByteSize1, aPtr1, aByteSize2, aPtr2);
	}

/**
User defined function which will be used by SQLITE engine for collated string comparisons,
collation level 1.
{aByteSize1, aPtr1} - string 1, UTF16 encoded. aByteSize1 - length in bytes.
{aByteSize2, aPtr2} - string 2, UTF16 encoded. aByteSize2 - length in bytes.
The function returns negative, zero or positive if the first string is less than, equal to, or 
greater than the second string.

@internalComponent
*/
static TInt CompareC1(void*, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2)
	{
	return Compare(1, aByteSize1, aPtr1, aByteSize2, aPtr2);
	}

/**
User defined function which will be used by SQLITE engine for collated string comparisons,
collation level 2.
{aByteSize1, aPtr1} - string 1, UTF16 encoded. aByteSize1 - length in bytes.
{aByteSize2, aPtr2} - string 2, UTF16 encoded. aByteSize2 - length in bytes.
The function returns negative, zero or positive if the first string is less than, equal to, or 
greater than the second string.

@internalComponent
*/
static TInt CompareC2(void*, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2)
	{
	return Compare(2, aByteSize1, aPtr1, aByteSize2, aPtr2);
	}

/**
User defined function which will be used by SQLITE engine for collated string comparisons,
collation level 3.
{aByteSize1, aPtr1} - string 1, UTF16 encoded. aByteSize1 - length in bytes.
{aByteSize2, aPtr2} - string 2, UTF16 encoded. aByteSize2 - length in bytes.
The function returns negative, zero or positive if the first string is less than, equal to, or 
greater than the second string.

@internalComponent
*/
static TInt CompareC3(void*, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2)
	{
	return Compare(3, aByteSize1, aPtr1, aByteSize2, aPtr2);
	}

/**
User defined function which will be used by SQLITE engine for folded string comparisons,
{aByteSize1, aPtr1} - string 1, UTF16 encoded. aByteSize1 - length in bytes.
{aByteSize2, aPtr2} - string 2, UTF16 encoded. aByteSize2 - length in bytes.
The function returns negative, zero or positive if the first string is less than, equal to, or 
greater than the second string.

@internalComponent
*/
static TInt CompareF(void*, TInt aByteSize1, const void* aPtr1, TInt aByteSize2, const void* aPtr2)
	{
	return Compare(-1, aByteSize1, aPtr1, aByteSize2, aPtr2);
	}

//KCollationMethodPtr array contains function pointers to all user-defined collation functions
const TCollationMethodPtr KCollationMethodPtr[KCollationMethodCount] = 
	{
	&CompareC0, &CompareC1, &CompareC2, &CompareC3, &CompareF
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        TSqlCollationUtil class definition      //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes TSqlCollationUtil data members.

@panic SqlDb 4 In _DEBUG mode. aDbHandle is NULL.
*/
TSqlCollationUtil::TSqlCollationUtil(sqlite3* aDbHandle) :
	iDbHandle(aDbHandle)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicBadArgument));
	}

/**
Installs user defined collations.

At the moment 5 user defined collations with the following names are installed:
- CompareC0 - 16-bit strings collated comaprison at level 0;
- CompareC1 - 16-bit strings collated comaprison at level 1;
- CompareC2 - 16-bit strings collated comaprison at level 2;
- CompareC3 - 16-bit strings collated comaprison at level 3;
- CompareF  - 16-bit strings folded comaprison;

These user defined collations can be used in the following cases:

- as column constraint in "CREATE TABLE" SQL statements. For example:
@code
  CREATE TABLE A(Col1 TEXT COLLATE CompareC1)
@endcode
In this case every time when Col1 values have to be compared, the SQL server will use CompareC1 collation.

- as column constraint in "CREATE INDEX" SQL statements. For example:
@code
  CREATE INDEX I ON A(Col1 COLLATE CompareC2)
@endcode
In this case SQL server will use CompareC2 collation to compare Col1 values when using the index.

- In "ORDER BY" clause of "SELECT" SQL statements. For example:
@code
  SELECT * FROM A ORDER BY Col1 COLLATE CompareC3
@endcode
In this case SQL server will use CompareC3 collation to compare Col1 values when building the result set.

@leave The function may leave with some database specific errors categorised as ESqlDbError.

@panic SqlDb 2 In _DEBUG mode. iDbHandle is NULL.
*/
void TSqlCollationUtil::InstallCollationsL()
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	(void)sqlite3SymbianLastOsError();//clear last OS error
	//Register user defined collations
	for(TInt i=0;i<KCollationMethodCount;++i)
		{
		TInt err = sqlite3_create_collation16(iDbHandle, reinterpret_cast <const char*> (KCollationMethodName[i]), 
											  SQLITE_UTF16 | SQLITE_UTF16_ALIGNED, 0, KCollationMethodPtr[i]);
		__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
		}
	}

/**
@return User-defined collation methods count.

@see TSqlCollationUtil::CollationName() 
*/
TInt TSqlCollationUtil::CollationCount() const
	{
	return KCollationMethodCount;
	}

/**
@return The name of the collation method, identified by aIndex.

@panic SqlDb 4 In _DEBUG mode. aIndex >= CollationMethodCount().

@see TSqlCollationUtil::CollationsCount() 
*/
TPtrC TSqlCollationUtil::CollationName(TInt aIndex) const
	{
	__ASSERT_DEBUG((TUint)aIndex < KCollationMethodCount, __SQLPANIC(ESqlPanicBadArgument));
	return TPtrC(KCollationMethodName[aIndex]);
	}
