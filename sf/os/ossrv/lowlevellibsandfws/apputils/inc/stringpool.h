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

#ifndef __STRINGPOOL_H__
#define __STRINGPOOL_H__ 

#include <e32base.h>


struct TStringTable;
class RStringF;
class RString;
class RStringToken;
class RStringTokenF;
class MStringPoolCloseCallBack;
class RStringBase;
class CStringPoolImplementation;


class MStringPoolCloseCallBack
/** 
Abstract callback interface that alerts implementors to when a string pool 
closes.
@publishedAll
@released
*/
	{
	//Implement this function to perform some operation when the string pool is being closed
	public:
	/** Called when the string pool is about to close. */
		virtual void StringPoolClosing()=0;
	};

/** The string pool handle object.  A string pool can have several
	distinct handles associated with it, each associated with a
	different pre-loaded table. The difference between them is that
	creating a string from an enum will interpret the enum as an
	offset into the relevant pre-loaded table */
class RStringPool
/** 
String pool handle.

A string pool can have several distinct handles associated with it, each associated 
with a different pre-loaded table. Creating a string from an enum value interprets 
the value as an offset into the relevant pre-loaded table. 
@publishedAll
@released
*/
	{
 public:
	inline RStringPool();

	IMPORT_C void OpenL();

	IMPORT_C void OpenL(const TStringTable& aTable);

	IMPORT_C void OpenL(const TStringTable& aTable,MStringPoolCloseCallBack& aCallBack);

	IMPORT_C void Close();

	IMPORT_C RStringF OpenFStringL(const TDesC8& aString) const;

	IMPORT_C RString OpenStringL(const TDesC8& aString) const;

	IMPORT_C RString String(RStringToken aString) const;

	IMPORT_C RString String(TInt aIndex,const TStringTable& aTable) const;	

	IMPORT_C RStringF StringF(RStringTokenF aString) const;

	IMPORT_C RStringF StringF(TInt aIndex,const TStringTable& aTable) const;	

 private:
	friend class RStringBase;
	friend class RString;
	friend class RStringF;
	friend class CStringPoolImplementation;

	CStringPoolImplementation* iImplementation;
	};

/** A compact (4 byte) representation of a string in the string pool. This
	class must be turned into a RStringBase (or one of its derived classes) before you can do anything
	useful with it. It is only intended to be used when storing strings 
	in situations where space matters; normaly use RStringBase.
	@see RStringBase
*/
class RStringTokenBase
/** 
Base class for the RStringToken and RStringTokenF string representations.
A compact (4 byte) representation of a string in the string pool. This
class must be turned into a RStringBase (or one of its derived classes) before you can do anything
useful with it. It is only intended to be used when storing strings 
in situations where space matters; normaly use RStringBase.
@see RStringBase
@publishedAll
@released
*/
	{
 public:
	inline RStringTokenBase();

	inline RStringTokenBase operator=(RStringBase aVal);

	inline TBool IsNull() const;

 protected:
	friend class RStringPool;
	friend class RStringBase;
	friend class RStringTokenEither;
	friend class CStringPoolImplementation;

	TUint32 iVal;
	};


class RStringToken : public RStringTokenBase
/** 
A compact (4 byte) representation of a string in the string pool. 

This class must be turned into a RString before you can do anything useful 
with it. It is only intended to be used when storing strings in situations 
where space matters. You should normally use RString. 
@publishedAll
@released
*/
	{
 public:
	inline TBool operator==(RStringToken aVal) const;

	inline TBool operator!=(RStringToken aVal) const;

	inline RStringToken operator=(RString aVal);

	friend class RStringPool;
	friend class RString;
	friend class CStringPoolImplementation;
	};

class RStringTokenF  : public RStringTokenBase
/** 
A compact (4 byte) representation of a RStringF string in the string pool. 

This class must be turned into a RStringF before you can do anything useful 
with it. It is only intended to be used when storing strings in situations 
where space matters. You should normally use RStringF. 
@publishedAll
@released
*/
	{
 public:
	inline TBool operator==(RStringTokenF aVal) const;

	inline TBool operator!=(RStringTokenF aVal) const;

	inline RStringTokenF operator=(RStringF aVal);

	friend class RStringPool;
	friend class RStringF;
	friend class CStringPoolImplementation;
	};

class RStringBase
/** 
Base class for classes that represent a string in a string pool. 

There are sub-classes for folded and non-folded strings. Use this class when 
you want to receive a string, but have no intention of comparing it with anything. 


@see RStringPool 
@publishedAll
@released
*/
	{
 public:
	inline RStringBase();

	IMPORT_C void Close();

	IMPORT_C RStringBase Copy();

	IMPORT_C const TDesC8& DesC() const;

	inline operator RStringTokenBase() const;

	IMPORT_C TInt Index(const TStringTable& aTable) const;

	IMPORT_C const TStringTable*  OriginalTableRef() const;

	inline RStringPool Pool() const;

 protected:
	friend class RStringPool;
	friend class RStringTokenBase;

	RStringPool iPool;
	TUint32 iVal;
	};

class RString : public RStringBase
/** 
A string that is stored in a string pool, with case-sensitive comparisons. 

To initialise values of this class, you need to use the operator() functions 
on the RStringPool. 

This class performs comparisons in a non-folded (case sensitive) manner. 

@see RStringPool
@publishedAll
@released
*/
	{
 public:
	inline RString Copy();

	inline operator RStringToken() const;

	inline TBool operator==(const RString& aVal) const;

	inline TBool operator!=(const RString& aVal) const;

	friend class RStringPool;
	friend class RStringToken;
	};

class RStringF : public RStringBase
/** 
A string that is stored in a string pool, with case-insensitive comparisons.

To initialise values of this class, you need to use the operator() functions 
on the RStringPool. 

Comparisons with this class use folded (case insensitive) comparisons. 
@publishedAll
@released
*/
	{
 public:
	inline RStringF Copy();

	inline operator RStringTokenF() const;

	inline TBool operator==(const RStringF& aVal) const;

	inline TBool operator!=(const RStringF& aVal) const;

	friend class RStringPool;
	friend class RStringTokenF;
	};

/**
A pointer to a string table.
@publishedAll
@released
*/

struct TStringTable
	{
	// The number of elements in the table
	/** The number of elements in the table. */
	TUint iCount;
	// A pointer to an array of pointers to the strings
	/** A pointer to an array of pointers to the strings. */
	const void* const * iTable;	

	// ETrue if the table should be considered case-sensitive.
	/** Flag that is ETrue if the table should be considered case-sensitive, otherwise 
	EFalse. */
	TBool iCaseSensitive;
	};

// Include the inline functions
#include <stringpool.inl>

#endif // __STRINGPOOL_H__


