// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "StringPoolImplementation.h"



EXPORT_C void RStringBase::Close()
/** Closes a string. 

You must close every string you open. In other words, every call to RStringBase::Copy(), 
RStringPool::OpenStringL() and RStringPool::OpenFStringL() must be matched 
by a close.

Strings created through RStringPool::String() or RStringPool::StringF() with 
an integer argument need not be closed (but closing is harmless). 

Strings created through RStringPool::String() or RStringPool::StringF() with 
a StringToken or StringTokenF argument must not be closed, as they're just 
changing the external representation. */
	{
	if (iVal)
		//harmless for empty strings
		iPool.iImplementation->Close(RStringTokenEither(iVal));
	// invalidate the string if it's a dinamic one	otherwise do nothing
	if(!StringUtils::IsTableEntry(this->iVal))
		iVal=NULL;
	}


EXPORT_C RStringBase RStringBase::Copy()
/** Copies a string.

Both the original and the copy string must be separately closed.

@return The string base. */
	{
	if (iVal)
		iPool.iImplementation->IncrementCount(RStringTokenEither(iVal));
	return *this;
	}


/** Returns the content of the string
 */
EXPORT_C const TDesC8& RStringBase::DesC() const
/** Gets the content of the string.

@return Descriptor containing the content of the string. */
	{
	// check for empty string
	if (iVal == 0)
		return KNullDesC8();
	if (StringUtils::IsTableEntry(iVal))
		{
		TInt index = StringUtils::TableIndex(iVal);
		TInt tableUid= StringUtils::TableUid(iVal);
		return iPool.iImplementation->TableLookup(index, tableUid);
		}
	else
		return *StringUtils::NodePtr(iVal)->iDes;
	}


EXPORT_C TInt RStringBase::Index(const TStringTable& aTable) const
/** Gets the enumeration value corresponding to this string.

@param	aTable String table to look in
@return	The string's enumeration value, or -1 if there is no such value 
@panic	EStringTableNotFound If the table supplied is not found. This panic is raised in debug builds only, in release mode the behaviour is undefined*/
	{
	//return an error for empty strings(i.e. with iVal=0 )
	if(!this->iVal)
	   return KErrNotFound;
	TInt tableId=iPool.iImplementation->TableUid(aTable);
	__ASSERT_DEBUG(tableId!=KErrNotFound,StringPoolPanic::Panic(StringPoolPanic::EStringTableNotFound));
	// First check the iVal is part of the same table
	if (StringUtils::IsTableEntry(iVal))
	   {		
	   if (tableId==StringUtils::TableUid(iVal))
		   return StringUtils::TableIndex(iVal);
	   }		
	// Then check if the iVal is in the reverse duplicate list
	TInt index=iPool.iImplementation->FindTableIndexFromFirstVal(iVal, tableId);
	return index;
	}


EXPORT_C const TStringTable* RStringBase::OriginalTableRef() const
/** Gets the table (if any) that first added the current string to the pool.

Note there multiple tables can contain the same string.

@return The table or NULL if the string was created dynamically (not from 
a table) */
	{
	if(StringUtils::IsTableEntry(iVal))
		return &iPool.iImplementation->TableRef(iVal);
	else 
		return NULL;
	}
	
/** Implementation of RStringPool class*/

EXPORT_C void RStringPool::OpenL()
/** Creates an initialised string pool with no pre-loaded string tables.

@leave KErrNoMemory Not enough memory to open the pool */
	{
	iImplementation = new (ELeave) CStringPoolImplementation;
	}

EXPORT_C void RStringPool::OpenL(const TStringTable& aTable,MStringPoolCloseCallBack& aCallBack)
/** Creates an initialised string pool with a pre-loaded string table, and a string-pool-closing 
callback.

@param aTable The pre-loaded string table.
@param aCallBack Callback interface that is called when the string pool closes
@leave KErrNoMemory Not enough memory to open the pool */
	{
	if (!iImplementation)
		{
		iImplementation = new (ELeave) CStringPoolImplementation;
		CleanupClosePushL(*this);
		iImplementation->AddTableL(aTable);
		iImplementation->AddCallBackL(aCallBack);
		CleanupStack::Pop(); //this
		}
	else
		{
		iImplementation->AddTableL(aTable);
		iImplementation->AddCallBackL(aCallBack);
		}
	}

EXPORT_C void RStringPool::OpenL(const TStringTable& aTable)
/** Creates an initialised string pool with a pre-loaded string table.

@param aTable The pre-loaded string table.
@leave KErrNoMemory Not enough memory to open the pool */
	{
	if (!iImplementation)
		{
		iImplementation = new (ELeave) CStringPoolImplementation;
		CleanupClosePushL(*this);
		iImplementation->AddTableL(aTable);
		CleanupStack::Pop(); //this
		}
	else
		{
		iImplementation->AddTableL(aTable);
		}
	}


EXPORT_C void RStringPool::Close()
/** Closes the string pool table.

This invalidates all other handles to the table. */
	{
	delete iImplementation;
	iImplementation = NULL;	 
	}

EXPORT_C RStringF RStringPool::OpenFStringL(const TDesC8& aString) const
/** Creates an RStringF using the current string pool.

The string is opened as case-insensitive. 

@param aString The value of the string.
@leave KErrNoMemory Not enough memory to open the string
@return Initialised RStringF object */
	{
	RStringTokenEither newString = iImplementation->OpenL(aString, ETrue);
	RStringF r;
	r.iPool = *this;
	r.iVal = newString.iVal;
	return r;
	}

EXPORT_C RString RStringPool::OpenStringL(const TDesC8& aString) const
/** Creates an RString using the current string pool.

The string is opened as case-sensitive. 

@param aString The value of the string.
@leave KErrNoMemory Not enough memory to open the string
@return Initialised RString object */
	{
	RStringTokenEither newString = iImplementation->OpenL(aString, EFalse);
	RString r;
	r.iPool = *this;
	r.iVal = newString.iVal;
	return r;
	}

EXPORT_C RString RStringPool::String(RStringToken aString) const
/** Creates an RString from the supplied RStringToken. 

@param aString The string token
@return Initialised RString object */
	{
	RString r;
	r.iPool = *this;
	r.iVal = aString.iVal;
	return r;
	}

EXPORT_C RString RStringPool::String(TInt aIndex,const TStringTable& aTable) const
/** Gets a case-sensitive string specified by a string table enumeration value.

aIndex is interpreted as an offset into the handle's pre-loaded string table. 

@param	aIndex The string table enumeration value
@param	aTable The string table from which to read the string
@return	Initialised RString object
@panic	EStringTableNotFound If the table supplied is not found. This panic is raised in debug builds only, in release mode the behaviour is undefined*/
	{	
	__ASSERT_DEBUG(aTable.iCaseSensitive==1,StringPoolPanic::Panic(StringPoolPanic::ECreatingStringWithWrongCase));
	if(aIndex <(TInt)aTable.iCount)
		{//the index is in valid range the index 
		RString r;
		r.iPool = *this;
		TInt16 tableUid = iImplementation->TableUid(aTable);
		__ASSERT_DEBUG(tableUid!=KErrNotFound,StringPoolPanic::Panic(StringPoolPanic::EStringTableNotFound));
		r.iVal = StringUtils::ValFromIndex(aIndex, tableUid);
		TInt originalVal;	
		if (KErrNotFound!=(originalVal=iImplementation->FindFirstValFromDuplicate(r.iVal)))
			{
			r.iVal=originalVal;
			}
		return r;
		}
	else // the index is out of range 
	return RString();
	}

EXPORT_C RStringF RStringPool::StringF(RStringTokenF aString) const
/** Creates a RStringF from the supplied RStringToken. 

@param aString The value of the string 
@return Initialised RStringF object */
	{
	RStringF r;
	r.iPool = *this;
	r.iVal = aString.iVal;
	return r;
	}

EXPORT_C RStringF RStringPool::StringF(TInt aIndex,const TStringTable& aTable) const
/** Gets a case-insensitive string specified by a string table enumeration value.

Creates an RStringF from a string table enumeration value.

aIndex is interpreted as an offset into the handle's pre-loaded string table. 

@param	aIndex The string table enumeration value
@param	aTable The string table from which to read the string
@return	Initialised RStringF object
@panic	EStringTableNotFound If the table supplied is not found. This panic is raised in debug builds only, in release mode the behaviour is undefined*/
	{
	__ASSERT_DEBUG(aTable.iCaseSensitive==0,StringPoolPanic::Panic(StringPoolPanic::ECreatingStringWithWrongCase));
	if(aIndex <(TInt)aTable.iCount)
		{//the index is in valid range the index 
		RStringF r;
		r.iPool = *this;
		TInt16 tableUid = iImplementation->TableUid(aTable);
		__ASSERT_DEBUG(tableUid!=KErrNotFound,StringPoolPanic::Panic(StringPoolPanic::EStringTableNotFound));
		r.iVal = StringUtils::ValFromIndexF(aIndex, tableUid);
		TInt originalVal;	
		if (KErrNotFound!=(originalVal=iImplementation->FindFirstValFromDuplicate(r.iVal)))
			{
			r.iVal=originalVal;
			}
		return r;
		}
	else // the index is out of range so return an empty string
	return RStringF();
	}
