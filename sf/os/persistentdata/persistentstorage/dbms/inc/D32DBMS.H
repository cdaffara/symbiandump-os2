// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__D32DBMS_H__)
#define __D32DBMS_H__

#ifndef __S32STD_H__
#include <s32std.h>
#endif
#ifndef __F32FILE_H__
#include <f32file.h>
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <d32dbmsconstants.h>
#endif

// other classes referenced
class RFs;
class CDbObject;
class CDbRowConstraint;
class CDbCursor;
class CDbDatabase;
class CDbIncremental;
class CDbNotifier;
class TDbColumn;
class TDbColumnC;

// classes defined
class TDbCol;
class CDbColSet;
class TDbColSetIter;
class TDbKeyCol;
class CDbKey;
class TDbBookmark;
class TDbQuery;
class RDbHandleBase;
template<class T> class RDbHandle;
class RDbRowConstraint;
class RDbRowSet;
class RDbColReadStream;
class RDbColWriteStream;
class TDbWindow;
class RDbView;
template<class T> class TUnion;
class TDbLookupKey;
class TDbSeekKey;
template<TInt S> class TDbSeekMultiKey;
class RDbTable;
class CDbNames;
class RDbDatabase;
class RDbIncremental;
class RDbUpdate;
class RDbNotifier;
class RDbs;
class RDbNamedDatabase;
class RDbStoreDatabase;

/**
The maximum length for a DBMS name: 64 characters.
@publishedAll
@released
*/
const TInt KDbMaxName=0x40;

/**
The maximum length for a DBMS column name: 64 characters.
@publishedAll
@released
*/
const TInt KDbMaxColName=KDbMaxName;

/** 
Represents a writable DBMS name. It maps to a modifiable buffer descriptor 
with maximum size KDbMaxName.

Notes:

A DBMS name must begin with an alphabetic character, after which any alphabetic, 
numeric or the _ (underscore) character may be used. DBMS names are also limited 
to 64 characters in length.

Table names must be unique within a database, and columns and indexes must 
have unique names within the table to which they belong. For the purposes 
of uniqueness and identification, the names are folded before comparison, 
so two columns named column_one and Column_ONE are considered to have the 
same name.

@see TBuf 
@publishedAll
@released
*/
typedef TBuf<KDbMaxName> TDbName;

/** Represents a non-writeable DBMS name. It maps to a non modifiable buffer descriptor 
with maximum size KDbMaxName.

Notes:

A DBMS name must begin with an alphabetic character, after which any alphabetic, 
numeric or the _ (underscore) character may be used. DBMS names are also limited 
to 64 characters in length.

Table names must be unique within a database, and columns and indexes must 
have unique names within the table to which they belong. For the purposes 
of uniqueness and identification, the names are folded before comparison, 
so two columns named column_one and Column_ONE are considered to have the 
same name.

@see TBufC 
@publishedAll
@released
*/
typedef TBufC<KDbMaxName> TDbNameC;

/** Represents a writable DBMS column name. It maps to a modifiable buffer descriptor 
with maximum size KDbMaxColName.

Notes:

A DBMS name must begin with an alphabetic character, after which any alphabetic, 
numeric or the _ (underscore) character may be used. DBMS names are also limited 
to 64 characters in length.

Table names must be unique within a database, and columns and indexes must 
have unique names within the table to which they belong. For the purposes 
of uniqueness and identification, the names are folded before comparison, 
so two columns named column_one and Column_ONE are considered to have the 
same name. 
@publishedAll
@released
*/
typedef TBuf<KDbMaxColName> TDbColName;

/** Represents a non-writable DBMS column name. It maps to a non-modifiable buffer 
descriptor with maximum size KDbMaxColName.

Notes:

A DBMS name must begin with an alphabetic character, after which any alphabetic, 
numeric or the _ (underscore) character may be used. DBMS names are also limited 
to 64 characters in length.

Table names must be unique within a database, and columns and indexes must 
have unique names within the table to which they belong. For the purposes 
of uniqueness and identification, the names are folded before comparison, 
so two columns named column_one and Column_ONE are considered to have the 
same name.

@see TBufC 
@publishedAll
@released
*/
typedef TBufC<KDbMaxColName> TDbColNameC;

/** Specifies when DBMS objects require a column ordinal to identify a column in 
a column set or in a rowset.

Note that KDbNullColNo is a special value of TDbColNo used to indicate that 
no such column exists. 
@publishedAll
@released
*/
typedef TInt TDbColNo;

/** 
Indicates that a specified column does not exist. 
@publishedAll
@released
*/
const TDbColNo KDbNullColNo=0;

/** 
A value returned by RDbRowSet::Count() when the number of rows cannot be determined.

@see RDbRowSet 
@publishedAll
@released
*/
const TInt KDbUndefinedCount=-1;

/** 
The length of a column is undefined. 
@publishedAll
@released
*/
const TInt KDbUndefinedLength=-1;

/** 
The default text column length. This is equal to 50. 
@publishedAll
@released
*/
const TInt KDbDefaultTextColLength=50;

/** 
Represents every supported column type. 
@publishedAll
@released
*/
enum TDbColType
	{
	/** The column stores a single bit.
	
	Representation range: 0 to 1
	
	Storage: 1 bit
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColBit,

	/** The column stores an 8-bit signed integer.
	
	Representation range: -2^7 to 2^7 - 1
	
	Storage: 1 byte
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColInt8,

	/** The column stores an 8-bit unsigned integer.
	
	Representation range: 0 to 2^8-1
	
	Storage: 1 byte
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColUint8,

	/** The column stores a 16-bit signed integer.
	
	Representation range: -2^15 to 2^15 - 1
	
	Storage: 2 bytes
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColInt16,

	/** The column stores a 16-bit unsigned integer.
	
	Representation range: 0 to 2^16-1
	
	Storage: 2 bytes
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColUint16,

	/** The column stores a 32-bit signed integer.
	
	Representation range: -2^31 to 2^31 - 1
	
	Storage: 4 bytes
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColInt32,

	/** The column stores a 32-bit unsigned integer.
	
	Representation range: 0 to 2^32-1
	
	Storage: 4 bytes
	
	Auto-increment: Yes
	
	Index key: Yes */
	EDbColUint32,

	/** The column stores a 64-bit signed integer.
	
	Representation range: -2^63 to 2^63 - 1
	
	Storage: 8 bytes
	
	Auto-increment: No
	
	Index key: Yes */
	EDbColInt64,

	/** The column stores a 32-bit floating point value.
	
	Representation range: 1.4 X 10^-45 to 3.40282 X 10^38
	
	Storage: 4 bytes
	
	Auto-increment: No
	
	Index key: Yes */
	EDbColReal32,

	/** The column stores a 64-bit floating point value.
	
	Representation range: 2.2 X 10^-308 to 1.79769 X 10^308
	
	Storage: 8 bytes
	
	Auto-increment: No
	
	Index key: Yes */
	EDbColReal64,

	/** The column stores a date/time value.
	
	Representation range: 1/1/0 to 31/12/9999
	
	Storage: 8 bytes
	
	Auto-increment: No
	
	Index key: Yes */
	EDbColDateTime,

	/** The column stores a (short) variable length of non-Unicode text data.
	
	Representation range: 0 to 256 characters
	
	Storage: 0 to 256 bytes
	
	Auto-increment: No
	
	Index key: Yes */
	EDbColText8,

	/** The column stores a (short) variable length of Unicode text data.
	
	Representation range: 0 to 256 characters
	
	Storage: 0 to 512 bytes
	
	Auto-increment: No
	
	Index key: Yes */
	EDbColText16,

	/** The column stores a (short) variable length of untyped data.
	
	Representation range: 0 to 255 bytes
	
	Storage: 0 to 256 bytes
	
	Auto-increment: No
	
	Index key: No */
	EDbColBinary,

	/** The column stores a potentially large amount of non-Unicode text data.
	
	Representation range: 0 to 2^31 characters
	
	Storage: 0 to 2 Gbytes
	
	Auto-increment: No
	
	Index key: Truncated */
	EDbColLongText8,

	/** The column stores a potentially large amount of Unicode text data.
	
	Representation range: 0 to 2^30 characters
	
	Storage: 0 to 2 Gbytes
	
	Auto-increment: No
	
	Index key: Truncated */
	EDbColLongText16,

	/** The column stores a potentially large amount of untyped data.
	
	Representation range: 0 to 2^31 bytes
	
	Storage: 0 to 2 Gbytes
	
	Auto-increment: No
	
	Index key: No */
	EDbColLongBinary,

	/** This is equivalent to EDbColText8 if the build is narrow, and 
	EDbColText16 if the build is Unicode. */
	EDbColText=EDbColText16,

	/** This is equivalent to EDbColLongText8 if the build is narrow, and 
	EDbColLongText16 if the build is Unicode. */
	EDbColLongText=EDbColLongText16
	};

/** 
Represents different ways of comparing Text and LongText columns.

This affects the ordering of indexes (see CDbKey), the evaluation of SQL 
queries and matching row constraints (see TDbQuery). 
@publishedAll
@released
*/
enum TDbTextComparison 
	{
	/** Use standard text comparison and ordering. */
	EDbCompareNormal,
	/** Use folded text for comparison and ordering. */
	EDbCompareFolded,
	/** Use collation for comparison and ordering. */
	EDbCompareCollated
	};

/** 
Defines a column in a table or rowset. 

The data members are public because this is really a structure to group the 
attributes of the column together. 

Three non-default constructors are provided for convenience. 
@publishedAll
@released
*/
class TDbCol
	{
	friend class CDbColSet;
public:
	enum {ENotNull=0x01,EAutoIncrement=0x02};
public:
	/** Constructs an uninitialised TDbCol. It is necessary because there are 
	also non-default constructors in this class. */
	inline TDbCol() {}
	inline TDbCol(const TDbCol&);
	inline TDbCol& operator=(const TDbCol&);
	IMPORT_C TDbCol(const TDesC& aName,TDbColType aType);
	IMPORT_C TDbCol(const TDesC& aName,TDbColType aType,TInt aMaxLength);
	static inline TBool IsLong(TDbColType aType);
private:
	inline TDbCol(const TDesC& aName);
public:
	/** The column type. */
	TDbColType iType;
	/** The maximum length of data that can be stored in a Text or Binary 
	column. */
	TInt iMaxLength;
	/** A set of flags describing other column attributes. This must be one 
	of the anonymous enum values ENotNull or EAutoIncrement. */
	TUint iAttributes;
	/** The column name. */
	TDbColNameC iName;
	};

/** Manages a set of column definitions which describe a table or rowset 
structure. Column definitions can be added and removed. The set can be 
iterated over, and ordinals for a column can be looked up.

The class is not intended for user derivation. 
@publishedAll
@released
*/
class CDbColSet : public CBase
	{
	friend class TDbColSetIter;
public:
	IMPORT_C CDbColSet();
	IMPORT_C static CDbColSet* NewL();
	IMPORT_C static CDbColSet* NewLC();
	IMPORT_C ~CDbColSet();
	inline const TDbCol& operator[](TDbColNo aCol) const;
	IMPORT_C const TDbCol* Col(const TDesC& aColName) const;
	IMPORT_C TDbColNo ColNo(const TDesC& aColName) const;
	inline TInt Count() const;
	inline void Clear();
	IMPORT_C CDbColSet& AddL(const TDbCol& aCol);
	IMPORT_C void Remove(const TDesC& aColName);
private:
	CArrayPakFlat<TDbCol> iColumns;
	};

/** 
Provides a useful way to iterate over the contents of a column set. 
@publishedAll
@released
*/
class TDbColSetIter
	{
public:
	IMPORT_C TDbColSetIter(const CDbColSet& aColSet);
	inline operator TAny* () const;
	inline const TDbCol& operator*() const;
	inline const TDbCol* operator->() const;
	IMPORT_C TDbColSetIter& operator++();
	inline TDbColSetIter operator++(TInt);
	inline TDbColNo Col() const;
private:
	const TDbCol* iColumn;
	TInt iIndex;
	const CArrayPakFlat<TDbCol>* iArray;
	};

/** Defines a key column in an index.

The data members are public as this is really a structure to group the 
attributes of the key column together. Two non-default constructors are 
provided for convenience. 
@publishedAll
@released
*/
class TDbKeyCol
	{
public:
	/** Specifies whether a key column in an index should be sorted in 
	ascending or descending order. */
	enum TOrder 
		{
		/** Use ascending order. */
		EAsc,
		/** Use descending order. */
		EDesc
		};
public:
	/** Constructs an uninitialised object. Default constructor. It is necessary 
	because there are also non-default constructors in this class. */
	inline TDbKeyCol() {}
	inline TDbKeyCol(const TDbKeyCol&);
	inline TDbKeyCol& operator=(const TDbKeyCol&);
	IMPORT_C TDbKeyCol(const TDesC& aName,TOrder anOrder=EAsc);
	IMPORT_C TDbKeyCol(const TDesC& aName,TInt aLength,TOrder anOrder=EAsc);
public:
	/** The ordering of the key. */
	TOrder iOrder;
	/** The maximum number of characters used to index Text or LongText 
	columns. If equal to KDbUndefinedLength (the default), all the characters 
	in the defined column will be used. */
	TInt iLength;
	/** The column name for the key. */
	TDbColNameC iName;
	};

/** 
Represents the definition of an index. The key may be unique or primary, it 
can specify the sort of comparison which is made for Text columns, and it 
has a list of columns which make up the key. The class is used to construct 
and interrogate index keys.

This class is not intended for user derivation. 
@publishedAll
@released
*/
class CDbKey : public CBase
	{
public:
	enum {EUnique=0x01,EPrimary=0x02};
public:
	IMPORT_C CDbKey();
	IMPORT_C static CDbKey* NewL();
	IMPORT_C static CDbKey* NewLC();
	IMPORT_C ~CDbKey();
	IMPORT_C CDbKey& AddL(const TDbKeyCol& aKeyCol);
	IMPORT_C void Remove(const TDesC& aColName);
	IMPORT_C void Clear();
	inline TInt Count() const;
	inline const TDbKeyCol& operator[](TInt aCol) const;
	inline void MakeUnique();
	inline TBool IsUnique() const;
	inline void SetComparison(TDbTextComparison aComparison);
	inline TDbTextComparison Comparison() const;
	inline void MakePrimary();
	inline TBool IsPrimary() const;
private:
	CArrayPakFlat<TDbKeyCol> iKeys;
	TDbTextComparison iComparison;
	TInt iAttributes;
	};

/** 
Saves the current location of a rowset, enabling rapid navigation back to a 
previously visited row or position in the set.

Instances of this class are returned by RDbRowSet::Bookmark(), which can later 
be passed to RDbRowSet::GotoL() to return to the row which was current at 
the call to Bookmark().

@see RDbRowSet::Bookmark()
@see RDbRowSet::GotoL() 
@publishedAll
@released
*/
class TDbBookmark
	{
	friend class RDbRowSet;
public:
	struct TMark
	/** Stores a database bookmark. */
		{
		enum {ESize=8};
		TUint32 iMark[ESize];
		};
private:
	TMark iMark;
	};

/** 
A wrapper for an SQL string combined with a text comparison mode.

An instance of the class is used as a parameter to RDbView::Prepare(), 
RDbRowSet::FindL() and RDbRowConstraint::Open().

The evaluation of SQL queries or constraints depends on how Text columns are 
compared so every SQL string is accompanied by a member of the TDbTextComparison 
enumeration to indicate how comparison is done.

If the SQL string contains a LIKE clause with * (asterisks) wildcard then the 
characters between them must be no longer than length 255.
If only one * exists then the length is taken from the start and to the end of
the clause. However, if the clause contains a ? (question mark) wildcard
within it then the characters between must be no longer than length 253. 

@see TDbTextComparison
@see RDbView::Prepare()
@see RDbRowSet::FindL()
@see RDbRowConstraint::Open() 
@publishedAll
@released
*/
class TDbQuery
	{
public:
	inline TDbQuery(const TDesC& aQuery,TDbTextComparison aComparison=EDbCompareNormal);
	inline const TDesC& Query() const;
	inline TDbTextComparison Comparison() const;
private:
	const TDesC& iQuery;
	TDbTextComparison iComparison;
	};

/** 
This class is internal and is not intended for use 
@publishedAll
@released
*/
class RDbHandleBase
	{
public:
	inline RDbHandleBase();
	void Close();
protected:
	inline void Set(CDbObject* aObject);
	CDbObject& Object() const;
protected:
	CDbObject* iObject;
	};

/** 
This class is internal and is not intended for use. 
@publishedAll
@released
*/
template <class T>
class RDbHandle : public RDbHandleBase
	{
public:
	inline T* operator=(T* aT);
	inline T* operator->() const;
	inline T& operator*() const;
	inline T* operator()() const;
	};

/** 
Represents a pre-compiled SQL search-condition, which can be used to 
test against the current row in a rowset.

The constraint is compiled against a specific rowset, and can only be matched 
against the same rowset using the RDbRowSet::MatchL() function.

@see RDbRowSet 
@publishedAll
@released
*/
class RDbRowConstraint
	{
	friend class RDbRowSet;
public:
	IMPORT_C TInt Open(const RDbRowSet& aView,TDbQuery aCriteria);
	IMPORT_C void Close();
private:
	RDbHandle<CDbRowConstraint> iConstraint;
	};

/** 
An abstract base class that provides functionality which is shared between 
SQL view objects and Table objects. This functionality includes most of the 
cursor navigation, row retrieval and update behaviour of rowsets.

Rowset objects do not provide the data for the rowset on which they operate. 
It is the responsibility of the derived classes RDbView and RDbTable to specify 
the data source.

This class is not intended for user derivation. 
@publishedAll
@released
*/
class RDbRowSet
	{
	friend class RDbRowConstraint;
	friend class RDbColReadStream;
	friend class RDbColWriteStream;
public:
	/** Specifies where the rowset should navigate to in the GotoL() function. 
	Their use is encapsulated by the respective member functions FirstL(), 
	NextL() etc. */
	enum TPosition 
		{
		/** Move to the first row in the rowset. */
		EFirst,
		/** Move to the next row in the rowset. */
		ENext,
		/** Move to the previous row in the rowset. */
		EPrevious,
		/** Move to the last row in the rowset. */
		ELast,
		/** Move to the position before the first row in the rowset. */
		EBeginning,
		/** Move to the position after the last row in the rowset. */
		EEnd
		};

	/** Specifies which operations can be performed on a rowset. */
	enum TAccess 
		{
		/** All operations can be performed on the rowset. */
		EUpdatable,
		/** Row navigation and reading are permitted. */
		EReadOnly,
		/** Inserting new rows is the only valid operation on the rowset. */
		EInsertOnly
		};

	/** Specifies the direction to search through the rowset when using the 
	FindL() function. */
	enum TDirection 
		{
		/** Search from the current row forwards through the set. */
		EForwards,
		/** Search from the current row backwards through the set. */
		EBackwards
		};

	/** Specifies whether the CountL() function should ensure that it returns 
	the exact value which may be a non-trivial task. */
	enum TAccuracy 
		{
		/** Take the time, if necessary, to return the exact value. */
		EEnsure,
		/** Return any immediately available value. */
		EQuick
		};
public:
	IMPORT_C void Close();
	IMPORT_C void Reset();
//
	IMPORT_C CDbColSet* ColSetL() const;
	IMPORT_C TInt ColCount() const;
	IMPORT_C TDbColType ColType(TDbColNo aCol) const;
	IMPORT_C TDbCol ColDef(TDbColNo aCol) const;
//
	IMPORT_C TBool AtRow() const;
	IMPORT_C TBool AtBeginning() const;
	IMPORT_C TBool AtEnd() const;
//
	IMPORT_C TInt CountL(TAccuracy anAccuracy=EEnsure) const;
	IMPORT_C TBool IsEmptyL() const;
//
	IMPORT_C TBool GotoL(TPosition aPosition);
	inline void BeginningL();
	inline void EndL();
	inline TBool FirstL();
	inline TBool LastL();
	inline TBool NextL();
	inline TBool PreviousL();
//
	IMPORT_C TDbBookmark Bookmark() const;
	IMPORT_C void GotoL(const TDbBookmark& aMark);
//
	IMPORT_C TBool MatchL(const RDbRowConstraint& aConstraint);
	IMPORT_C TInt FindL(TDirection aDirection,TDbQuery aCriteria);
//
	IMPORT_C void GetL();
	IMPORT_C void InsertL();
	IMPORT_C void InsertCopyL();
	IMPORT_C void UpdateL();
	IMPORT_C void PutL();
	IMPORT_C void Cancel();
	IMPORT_C void DeleteL();
//
	inline TBool IsColNull(TDbColNo aCol) const;
	IMPORT_C TInt ColSize(TDbColNo aCol) const;
	IMPORT_C TInt ColLength(TDbColNo aCol) const;
//
	IMPORT_C TInt8 ColInt8(TDbColNo aCol) const;
	IMPORT_C TInt16 ColInt16(TDbColNo aCol) const;
	IMPORT_C TInt32 ColInt32(TDbColNo aCol) const;
	IMPORT_C TInt64 ColInt64(TDbColNo aCol) const;
	inline TInt ColInt(TDbColNo aCol) const;
	IMPORT_C TUint8 ColUint8(TDbColNo aCol) const;
	IMPORT_C TUint16 ColUint16(TDbColNo aCol) const;
	IMPORT_C TUint32 ColUint32(TDbColNo aCol) const;
	inline TUint ColUint(TDbColNo aCol) const;
	IMPORT_C TReal32 ColReal32(TDbColNo aCol) const __SOFTFP;
	IMPORT_C TReal64 ColReal64(TDbColNo aCol) const __SOFTFP;
	inline TReal ColReal(TDbColNo aCol) const;
	IMPORT_C TTime ColTime(TDbColNo aCol) const;
	IMPORT_C TPtrC8 ColDes8(TDbColNo aCol) const;
	IMPORT_C TPtrC16 ColDes16(TDbColNo aCol) const;
	IMPORT_C TPtrC ColDes(TDbColNo aCol) const;
//
	IMPORT_C void SetColNullL(TDbColNo aCol);
	inline void SetColL(TDbColNo aCol,TInt aValue);
	IMPORT_C void SetColL(TDbColNo aCol,TInt32 aValue);
	IMPORT_C void SetColL(TDbColNo aCol,TInt64 aValue);
	inline void SetColL(TDbColNo aCol,TUint aValue);
	IMPORT_C void SetColL(TDbColNo aCol,TUint32 aValue);
	IMPORT_C void SetColL(TDbColNo aCol,TReal32 aValue) __SOFTFP;
	IMPORT_C void SetColL(TDbColNo aCol,TReal64 aValue) __SOFTFP;
	IMPORT_C void SetColL(TDbColNo aCol,TTime aValue);
	IMPORT_C void SetColL(TDbColNo aCol,const TDesC8& aValue);
	IMPORT_C void SetColL(TDbColNo aCol,const TDesC16& aValue);
private:
	inline MStreamBuf* ColSourceL(TDbColNo aCol) const;
	inline MStreamBuf* ColSinkL(TDbColNo aCol);
	CDbCursor& CheckCol(TDbColNo aCol) const;
	TDbColumnC ColumnC(TDbColNo aCol,TDbColType aType) const;
	TDbColumn Column(TDbColNo aCol,TDbColType aType);
protected:
	RDbHandle<CDbCursor> iCursor;
	};

/** 
Allows any column value to be read as stream data.

Note that this is the only way to extract the contents of a Long column from 
a rowset.

Only one column in a rowset object can be opened for reading as a stream at 
a time, and while it is open no column in the same rowset object may be set 
using RDbColWriteStream.

Null columns result in a read stream with no data. Small numeric columns are 
maintained in memory as 32-bit values; hence reading a Uint8 column as a stream 
results in 4 bytes of stream data, not 1. 
@publishedAll
@released
*/
class RDbColReadStream : public RReadStream
	{
public:
	IMPORT_C void OpenL(const RDbRowSet& aView,TDbColNo aCol);
	IMPORT_C void OpenLC(const RDbRowSet& aView,TDbColNo aCol);
	};

/** 
Writes Long columns when inserting or updating rows in a rowset. 

Only one column in a rowset object can be opened for writing as a stream at 
a time. 
@publishedAll
@released
*/
class RDbColWriteStream : public RWriteStream
	{
public:
	/** Constructs this object by invoking the default constructor for 
	RWriteStream. */
	inline RDbColWriteStream() {}
	inline RDbColWriteStream(const MExternalizer<TStreamRef> &anExternalizer);
	IMPORT_C void OpenL(RDbRowSet& aView,TDbColNo aCol);
	IMPORT_C void OpenLC(RDbRowSet& aView,TDbColNo aCol);
	};

/** 
Describes the desired shape of a view's pre-evaluation window.

An instance of this class is passed to the RDbView object as part of the 
Prepare() function. The different constructors for TDbWindow can specify a view:

without pre-evaluation

with full pre-evaluation

with limited pre-evaluation.

@see RDbView 
@publishedAll
@released
*/
class TDbWindow
	{
public:
	/** Denotes a full pre-evaluation window. */
	enum TUnlimited 
		{
		/** No limit on how many rows may be in the window. */
		EUnlimited=KMaxTInt
		};
	enum {ENone=0};
public:
	inline TDbWindow();
	inline TDbWindow(TUnlimited);
	IMPORT_C TDbWindow(TInt aForeSlots,TInt aRearSlots);
	inline TInt Size() const;
	inline TInt PreferredPos() const;
private:
	TInt iSize;
	TInt iPreferredPos;
	};

#if defined(__NO_CLASS_CONSTS__)
/**
@publishedAll
@released
*/
#define KDbUnlimitedWindow TDbWindow(TDbWindow::EUnlimited)
#else
/**
@publishedAll
@released
*/
const TDbWindow KDbUnlimitedWindow=TDbWindow(TDbWindow::EUnlimited);
#endif

/** 
Generates rowsets from an SQL query. The query is prepared and evaluated 
using the interface to this class, while the rowset is manipulated using the 
RDbRowset base class.

There are no special rules to consider when deriving from this class. 
@publishedAll
@released
*/
class RDbView : public RDbRowSet
	{
public:
	IMPORT_C TInt Prepare(RDbDatabase& aDatabase,const TDbQuery& aQuery,TAccess anAccess=EUpdatable);
	IMPORT_C TInt Prepare(RDbDatabase& aDatabase,const TDbQuery& aQuery,const TDbWindow& aWindow);
	IMPORT_C TInt Prepare(RDbDatabase& aDatabase,const TDbQuery& aQuery,const TDbWindow& aWindow,TAccess anAccess);
	IMPORT_C TInt EvaluateAll();
	IMPORT_C TInt Evaluate();
	IMPORT_C void Evaluate(TRequestStatus& aStatus);
	IMPORT_C TBool Unevaluated() const;
	};

/** 
This class is internal and is not intended for use. 
@publishedAll
@released
*/
template <class T>
class TUnion
	{
public:
	inline operator const T&() const;
	inline const T& operator()() const;
	inline T& operator()();
	inline void Set(const T& aT);
private:
	TUint8 iRep[sizeof(T)];
	};

/** 
This class is internal and is not intended for use. 
@publishedAll
@released
*/
class TDbLookupKey
	{
public:
	struct SColumn
		{
		union
			{
			TInt32 iInt32;
			TUint32 iUint32;
			TUnion<TInt64> iInt64;
			TReal32 iReal32;
			TReal64 iReal64;
			TUnion<TTime> iTime;
			struct {const TText8* iPtr;TInt iLength;} iDes8;
			struct {const TText16* iPtr;TInt iLength;} iDes16;
			};
		TDbColType iType;
		};
public:
	inline TDbLookupKey();
	inline TInt Count() const;
	inline const SColumn* First() const;
	void Add(TInt aKey);
	void Add(TUint aKey);
	void Add(TInt64 aKey);
	void Add(TReal32 aKey) __SOFTFP;
	void Add(TReal64 aKey) __SOFTFP;
	void Add(TTime aKey);
	void Add(const TDesC8& aKey);
	void Add(const TDesC16& aKey);
private:
	SColumn& NextKey();
private:
	TInt iCount;
	SColumn iKey[1];	// at least one
	};

/** 
Database key value.

The class encapsulates a key value which is passed to RDbTable::SeekL(), for 
lookup in the currently active index on that rowset.

An instance of this class can store a key value of any type.

Note that the class can only hold a single-column key. Use TDbSeekMultiKey 
for multi-column keys.

@see TDbSeekMultiKey
@see RDbTable 
@publishedAll
@released
*/
class TDbSeekKey
	{
	friend class RDbTable;
public:
	inline TDbSeekKey();
	inline TDbSeekKey(TInt aKey);
	inline TDbSeekKey(TUint aKey);
	inline TDbSeekKey(TInt64 aKey);
	inline TDbSeekKey(TReal32 aKey);
	inline TDbSeekKey(TReal64 aKey);
	inline TDbSeekKey(TTime aKey);
	inline TDbSeekKey(const TDesC8& aKey);
	inline TDbSeekKey(const TDesC16& aKey);
	IMPORT_C TDbSeekKey& Add(TInt aKey);
	IMPORT_C TDbSeekKey& Add(TUint aKey);
	IMPORT_C TDbSeekKey& Add(TInt64 aKey);
	IMPORT_C TDbSeekKey& Add(TReal32 aKey) __SOFTFP;
	IMPORT_C TDbSeekKey& Add(TReal64 aKey) __SOFTFP;
	IMPORT_C TDbSeekKey& Add(TTime aKey);
	IMPORT_C TDbSeekKey& Add(const TDesC8& aKey);
	IMPORT_C TDbSeekKey& Add(const TDesC16& aKey);
private:
	TDbLookupKey& Check();
protected:
	inline TDbSeekKey(TInt aKeys,TInt);
private:
	TInt iMaxKeys;
	TDbLookupKey iKey;
	};

/** 
Database multi-column key value.

The class extends the behaviour of TDbSeekKey by allowing multi-column indexes 
to be searched using multi-column key values.

The template parameter S specifies the maximum number of column values that 
can be added to the key value.

Use the TDbSeekKey::Add() members to append the values for each column in 
the key value. 
@publishedAll
@released
*/
template<TInt S>
class TDbSeekMultiKey : public TDbSeekKey
	{
public:
	inline TDbSeekMultiKey();
private:
	TDbLookupKey::SColumn iExtraKeys[S-1];
	};

/** 
Provides access to table data as a rowset, allowing manipulation of a named 
table in the database. Additionally, a named index can be used to order the 
rowset, and to provide fast key-based row retrieval from the table.

There are no special rules to consider when deriving from this class. 
@publishedAll
@released
*/
class RDbTable : public RDbRowSet
	{
public:
	/** Database table seek comparison types. */
	enum TComparison 
		{
		/** Retrieve the last row which is strictly less than the key value. */
		ELessThan,
		/** Retrieve the last row which is equal to or less than the key 
		value. */
		ELessEqual,
		/** Retrieve the first row which is equal to the key value. */
		EEqualTo,
		/** Retrieve the first row which is equal to or greater than the key 
		value. */
		EGreaterEqual,
		/** Retrieve the first row which is strictly greater than the key 
		value. */
		EGreaterThan
		};
public:
	IMPORT_C TInt Open(RDbDatabase& aDatabase,const TDesC& aName,TAccess anAccess=EUpdatable);
	inline TInt SetIndex(const TDesC& anIndex);
	inline TInt SetNoIndex();
	IMPORT_C TBool SeekL(const TDbSeekKey& aKey,TComparison aComparison=EEqualTo);
private:
	IMPORT_C TInt SetIndex(const TDesC* anIndex);
	};

/**
@publishedAll
@released
*/
class CDbNames : public CBase
	{
private:
	inline CDbNames();
public:
	static CDbNames* NewLC();
	~CDbNames();
	inline TInt Count() const;
	inline const TDesC& operator[](TInt anIndex) const;
	IMPORT_C void AddL(const TDesC& aName);
private:
	CArrayPakFlat<TDbNameC> iList;
	};
/**
@publishedAll
@released
*/
typedef CDbNames CDbTableNames,CDbIndexNames,CDbDatabaseNames;

/**
The maximum length for a generic DBMS string, which might be transferred from DBMS server
to the DBMS client using IPC.
@publishedAll
@released
*/
const TInt KDbMaxStrLen = 256;

/** 
Represents a generic read-only DBMS string. It maps to a non-modifiable buffer descriptor 
with maximum size KDbMaxStrLen.

@see TBufC
@publishedAll
@released
*/
typedef TBufC<KDbMaxStrLen> TDbStringC;

/**
CDbStrings encapsulates functionality used for transferring an array of strings from 
DBMS server to the DBMS client. The maximal length of each element of the array is 
KDbMaxStrLen characters.
The typical usage pattern of CDbStrings class is:
@code
CDbStrings* strings = <a call of an exported DBMS method>;
TInt cnt = strings.Count();
for(TInt i=0;i<cnt;++i)
	{
	const TDesC& str = (*strings)[i];
	<do something with "str" variable>;
	}
delete strings;
@endcode
@publishedAll
@released
*/
class CDbStrings : public CBase
	{
private:
	inline CDbStrings();
public:
	static CDbStrings* NewLC();
	virtual ~CDbStrings();
	inline TInt Count() const;
	inline const TDesC& operator[](TInt aIndex) const;
	void AddL(const TDesC& aStr);
private:
	CArrayPakFlat<TDbStringC> iList;
	};

/**
Abstract class providing the functionality of a database.

The source of the database and the implementation characteristics of a particular
database are provided by a class derived from RDbDatabase. 

DBMS has one such implementation: the store database.

This class is not intended for user derivation.

Note: For functions (i.e. Execute) that take an Sql string, if the string
contains a LIKE clause with * (asterisks) wildcard then the 
characters between them must be no longer than length 255.
If only one * exists then the length is taken from the start and to the end of
the clause. However, if the clause contains a ? (question mark) wildcard
within it then the characters between must be no longer than length 253. 

@publishedAll
@released
*/
class RDbDatabase
	{
	friend class RDbView;
	friend class RDbTable;
	friend class RDbIncremental;
	friend class RDbUpdate;
	friend class RDbNotifier;
public:
	struct TSize {TInt iSize;TInt iUsage;};
public:
	IMPORT_C void Close();
	IMPORT_C TInt Destroy();
//
	IMPORT_C TBool IsDamaged() const;
	IMPORT_C TInt Recover();
//
	IMPORT_C TInt UpdateStats();
	IMPORT_C TSize Size() const;
	IMPORT_C TInt Compact();
	IMPORT_C TInt Begin();
	IMPORT_C TInt Commit();
	IMPORT_C void Rollback();
	IMPORT_C TBool InTransaction() const;
//
	inline TInt CreateTable(const TDesC& aName,const CDbColSet& aDef);
	inline TInt CreateTable(const TDesC& aName,const CDbColSet& aDef,const CDbKey& aPrimaryKey);
	IMPORT_C TInt DropTable(const TDesC& aName);
	IMPORT_C TInt AlterTable(const TDesC& aName,const CDbColSet& aNewDef);
	IMPORT_C TInt CreateIndex(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey);
 	IMPORT_C TInt DropIndex(const TDesC& aName,const TDesC& aTable);
//
	IMPORT_C TInt Execute(const TDesC& aSql,TDbTextComparison aComparison=EDbCompareNormal);
//
	IMPORT_C CDbTableNames* TableNamesL() const;
	IMPORT_C CDbColSet* ColSetL(const TDesC& aName) const;
	IMPORT_C CDbIndexNames* IndexNamesL(const TDesC& aTable) const;
	IMPORT_C CDbKey* KeyL(const TDesC& aName,const TDesC& aTable) const;
private:
	IMPORT_C TInt CreateTable(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey);
protected:
	RDbHandle<CDbDatabase> iDatabase;
	};

/** 
Provides the interface for performing long-running database operations in 
incremental steps, allowing application programs to remain responsive to other 
events.

After an operation has begun, a standard interface is used to continue and 
complete all incremental operations. On successful return from one of the 
initiating functions, a step value is also returned. This gives the progress 
indication and is, initially, positive. This value should be passed into 
subsequent steps in the operation, each of which may decrement the value by 
some amount, although they are allowed to leave it unchanged. The value 
reaches zero, if, and only if the operation completes.

While an incremental operation is in progress, the database cannot be used 
for any other operations such as opening tables or preparing views. 

Starting an incremental operation also requires that no rowsets are open on 
the database and that no commit is pending for data manipulation transactions.

If no explicit transaction has been started by the database, then an automatic 
transaction is begun when any incremental operation is started and is committed 
when complete or rolled back if the operation either fails or is abandoned 
prior to completion. 
@publishedAll
@released
*/
class RDbIncremental
	{
public:
	IMPORT_C void Close();
	IMPORT_C TInt Next(TInt& aStep);
	IMPORT_C void Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus);
//
	IMPORT_C TInt Recover(RDbDatabase& aDatabase,TInt& aStep);
	IMPORT_C TInt Compact(RDbDatabase& aDatabase,TInt& aStep);
	IMPORT_C TInt DropTable(RDbDatabase& aDatabase,const TDesC& aTable,TInt& aStep);
	IMPORT_C TInt AlterTable(RDbDatabase& aDatabase,const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep);
	IMPORT_C TInt CreateIndex(RDbDatabase& aDatabase,const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep);
	IMPORT_C TInt DropIndex(RDbDatabase& aDatabase,const TDesC& aName,const TDesC& aTable,TInt& aStep);
	inline TInt Execute(RDbDatabase& aDatabase,const TDesC& aSql,TInt& aStep);
	IMPORT_C TInt Execute(RDbDatabase& aDatabase,const TDesC& aSql,TDbTextComparison aComparison,TInt& aStep);
	IMPORT_C TInt UpdateStats(RDbDatabase& aDatabase,TInt& aStep);
private:
	RDbHandle<CDbIncremental> iState;
	};

/** 
Provides an interface to allow incremental execution of a DML (SQL data 
update) statement. 

This class offers similar behaviour to the RDbIncremental class, in that it 
allows an application to perform long running operations while remaining 
responsive to events. However, unlike that class, RDbUpdate is restricted to 
executing DML statements.

@see RDbIncremental 
@publishedAll
@released
*/
class RDbUpdate
	{
public:
	IMPORT_C TInt Execute(RDbDatabase& aDatabase,const TDesC& aSql,TDbTextComparison aComparison=EDbCompareNormal);
	IMPORT_C void Close();
//
	IMPORT_C TInt Next();
	IMPORT_C void Next(TRequestStatus& aStatus);
	IMPORT_C TInt RowCount() const;
private:
	RDbHandle<CDbIncremental> iUpdate;
	TPckgBuf<TInt> iRows;
	};

/** 
Provides notification of database changes to clients.

This is useful for shared databases. 
@publishedAll
@released
*/
class RDbNotifier
	{
public:
	/** Defines the events which may be reported by a DBMS change notifier 
	through this RDbNotifier object.

	Each enumerator corresponds to a distinct event type.

	The changes are reported through a TRequestStatus object when an 
	outstanding notification request completes.

	Notes

	If further database events occur while a client is handling the completion 
	of a previous event, the notifier remembers the most significant event. The 
	order of importance is:

	ERecover > ERollback > ECommit

	where the symbol > means "is more important than" */
	enum TEvent 
		{
		/** The database has been closed. */
		EClose,
		/** All read locks have been removed. */
		EUnlock,
		/** A transaction has been committed. */
		ECommit,
		/** A transaction has been rolled back */
		ERollback,
		/** The database has been recovered */
		ERecover
		};
public:
	IMPORT_C TInt Open(RDbDatabase& aDatabase);
	IMPORT_C void Close();
//
	IMPORT_C void NotifyUnlock(TRequestStatus& aStatus);
	IMPORT_C void NotifyChange(TRequestStatus& aStatus);
	IMPORT_C void Cancel();
private:
	RDbHandle<CDbNotifier> iNotifier;
	};

/** 
Client-server databases

Represents a session with the DBMS server. A thread uses this class to set 
up a DBMS server session and this provides the basis for sharing databases 
with other threads. 
@publishedAll
@released
*/
class RDbs : public RSessionBase
	{
public:
	/**
	This enum is used in GetDatabasePolicy/GetTablePolicy/GetTablePolicies calls and specifies
	requested security policy type: read/write/schema.
	@publishedAll
	@released
	*/
	typedef enum {EReadPolicy, EWritePolicy, ESchemaPolicy} TPolicyType;

public:
	IMPORT_C static TVersion Version();
	IMPORT_C TInt Connect();
	IMPORT_C CDbDatabaseNames* DatabaseNamesL(TDriveNumber aDrive, TUid aPolicyUid);
	IMPORT_C TInt CopyDatabase(const TDesC& aSrcDbName, const TDesC& aDestDbName, TUid aPolicyUid);
	IMPORT_C TInt DeleteDatabase(const TDesC& aDbName, TUid aPolicyUid);
	IMPORT_C TInt GetDatabasePolicy(TUid aPolicyUid, TPolicyType aPolicyType, 
									TSecurityPolicy& aDbPolicy);
	IMPORT_C TInt GetTablePolicy(TUid aPolicyUid, const TDesC& aTableName, 
								 TPolicyType aPolicyType, TSecurityPolicy& aTablePolicy);
	IMPORT_C TInt GetTablePolicies(TUid aPolicyUid, const TDesC& aTableName, 
								   TPolicyType aPolicyType, 
								   TSecurityPolicy& aDbPolicy, TSecurityPolicy& aTablePolicy);
	//
	IMPORT_C void ResourceMark();
	IMPORT_C void ResourceCheck();
	IMPORT_C TInt ResourceCount();
	IMPORT_C void SetHeapFailure(TInt aTAllocFail,TInt aRate);

	IMPORT_C TInt ReserveDriveSpace(TInt aDriveNo, TInt aSpace);
	IMPORT_C void FreeReservedSpace(TInt aDriveNo);
	IMPORT_C TInt GetReserveAccess(TInt aDriveNo);
	IMPORT_C TInt ReleaseReserveAccess(TInt aDriveNo);

	IMPORT_C TInt GetBackupPath(TSecureId aRequesterSid, const TDesC& aDbName, 
								TUid aDbPolicyUid, TDes& aBackupPath);
	IMPORT_C CDbStrings* BackupPathsL(TSecureId aRequesterSid, TUid aDbPolicyUid);

private:
	TInt DoConnect();
	TInt SessionMessage(TInt aFunction);
	TInt GetPolicy(TUid aPolicyUid, const TDesC& aTableName, 
				   TUint aMask, TSecurityPolicy& aPolicy);
	};

/**
Generic database implementation
@publishedAll
@released
*/
class RDbNamedDatabase : public RDbDatabase
	{
public:
	/** Specifies which operations can be performed on a rowset. */
	enum TAccess 
	{
	EReadWrite,
	/** Row navigation and reading are permitted. */
	EReadOnly
	};
public:
	IMPORT_C TInt Create(RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat);
	IMPORT_C TInt Create(RFs& aFs,const TDesC& aDatabase,const TDesC& aFormat=TPtrC());
	IMPORT_C TInt Replace(RFs& aFs,const TDesC& aDatabase,const TDesC& aFormat=TPtrC());
	IMPORT_C TInt Open(RFs& aFs,const TDesC& aDatabase,const TDesC& aFormat=TPtrC(),TAccess aMode=EReadWrite);
	IMPORT_C TInt Open(RDbs& aDbs,const TDesC& aDatabase,const TDesC& aFormat=TPtrC());
	};

/**
DBMS Store database implementation
@publishedAll
@released
*/
class RDbStoreDatabase : public RDbDatabase
	{
public:
	IMPORT_C TStreamId CreateL(CStreamStore* aStore);
	IMPORT_C void OpenL(CStreamStore* aStore,TStreamId anId);
	IMPORT_C static void CompressL(CStreamStore& aStore,TStreamId aId);
	IMPORT_C static void DecompressL(CStreamStore& aStore,TStreamId aId);
	};

#include <d32dbms.inl>
#endif
