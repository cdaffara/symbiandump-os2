// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Written by DavidW, October 1996
// Descriptor arrays
// 
//

#if !defined(__BADESCA_H__)
#define __BADESCA_H__

#if !defined(__BAMDESCA_H__)
#include <bamdesca.h>
#endif

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


class CDesC8Array : public CArrayFixBase, public MDesC8Array
/** An implementation base class for 8 bit descriptor arrays. 

It provides some of the behaviour for 8 bit descriptor arrays. The class is 
abstract and cannot be instantiated. 
@publishedAll
@released
*/
	{
protected:
	IMPORT_C CDesC8Array(TBufRep aRep,TInt aGranularity);
public:
    IMPORT_C ~CDesC8Array();
	IMPORT_C void AppendL(const TDesC8& aPtr);
	IMPORT_C void InsertL(TInt aPos,const TDesC8& aPtr);
	IMPORT_C TInt InsertIsqL(const TDesC8& aPtr,TKeyCmpText aTextComparisonType=ECmpFolded);
	IMPORT_C TInt InsertIsqAllowDuplicatesL(const TDesC8& aPtr,TKeyCmpText aTextComparisonType=ECmpFolded);
	IMPORT_C void Sort(TKeyCmpText aTextComparisonType=ECmpFolded);
	IMPORT_C TInt Find(const TDesC8& aPtr,TInt& aPos,TKeyCmpText aTextComparisonType=ECmpFolded) const;
	IMPORT_C TInt FindIsq(const TDesC8& aPtr,TInt& aPos,TKeyCmpText aTextComparisonType=ECmpFolded) const;
	IMPORT_C void Delete(TInt aPos);
	IMPORT_C void Delete(TInt aIndex,TInt aCount);
	IMPORT_C void Reset();
	inline TPtrC8 operator[](TInt aIndex) const;
// from MDesC8Array
    IMPORT_C TInt MdcaCount() const;
	IMPORT_C TPtrC8 MdcaPoint(TInt aIndex) const;
	};

inline TPtrC8 CDesC8Array::operator[](TInt aIndex) const
	{ return(MdcaPoint(aIndex)); }


class CDesC8ArrayFlat : public CDesC8Array
/** An array of 8 bit descriptors implemented using a flat buffer.
"bafl.lib" 
@since 5.0
@publishedAll
@released
*/
	{
public:
	IMPORT_C CDesC8ArrayFlat(TInt aGranularity);
    IMPORT_C ~CDesC8ArrayFlat();
	};


class CDesC8ArraySeg : public CDesC8Array
/** An array of 8 bit descriptors implemented using a segmented buffer. 
"bafl.lib"
@since 5.0
@publishedAll
@released
*/
	{
public:
	IMPORT_C CDesC8ArraySeg(TInt aGranularity);
    IMPORT_C ~CDesC8ArraySeg();
	};


class CDesC16Array : public CArrayFixBase, public MDesC16Array
/** An implementation base class for 16 bit descriptor arrays. It provides some 
of the behaviour for 16 bit descriptor arrays.

The class is abstract and cannot be instantiated. 
" bafl.lib "
@since 5.0
@publishedAll
@released
*/
	{
protected:
	IMPORT_C CDesC16Array(TBufRep aRep,TInt aGranularity);
public:
    IMPORT_C ~CDesC16Array();
	IMPORT_C void AppendL(const TDesC16& aPtr);
	IMPORT_C void InsertL(TInt aPos,const TDesC16& aPtr);
	IMPORT_C TInt InsertIsqL(const TDesC16& aPtr,TKeyCmpText aTextComparisonType=ECmpFolded);
	IMPORT_C TInt InsertIsqAllowDuplicatesL(const TDesC16& aPtr,TKeyCmpText aTextComparisonType=ECmpFolded);
	IMPORT_C void Sort(TKeyCmpText aTextComparisonType=ECmpFolded);
	IMPORT_C TInt Find(const TDesC16& aPtr,TInt& aPos,TKeyCmpText aTextComparisonType=ECmpFolded) const;
	IMPORT_C TInt FindIsq(const TDesC16& aPtr,TInt& aPos,TKeyCmpText aTextComparisonType=ECmpFolded) const;
	IMPORT_C void Delete(TInt aPos);
	IMPORT_C void Delete(TInt aIndex,TInt aCount);
	IMPORT_C void Reset();
	inline TPtrC16 operator[](TInt aIndex) const;
// from MDesC16Array
    IMPORT_C TInt MdcaCount() const;
		IMPORT_C TPtrC16 MdcaPoint(TInt aIndex) const;
	};



inline TPtrC16 CDesC16Array::operator[](TInt aIndex) const
/** Returns a non-modifiable descriptor to represent the descriptor
element located at the specified index within the array.

@param aIndex The position of the descriptor element within the array. 
The position is relative to zero; i.e. zero implies the first descriptor 
element in the array. This value must be non-negative and less than the
number of descriptors currently within the array otherwise the operator 
panics with
@code
EArrayIndexOutOfRange
@endcode
@return A 16 bit non-modifiable pointer descriptor. */
 { return(MdcaPoint(aIndex)); }


class CDesC16ArrayFlat : public CDesC16Array
/** Array of 16 bit descriptors implemented using a flat buffer. 
"bafl.lib  
@since 5.0
@publishedAll
@released
*/
	{
public:
	IMPORT_C CDesC16ArrayFlat(TInt aGranularity);
    IMPORT_C ~CDesC16ArrayFlat();
	};


class CDesC16ArraySeg : public CDesC16Array
/** An array of 16 bit descriptors implemented using a segmented buffer. 
"bafl.lib"
@since 5.0
@publishedAll
@released
*/
	{
public:
	IMPORT_C CDesC16ArraySeg(TInt aGranularity);
    IMPORT_C ~CDesC16ArraySeg();
	};


class CPtrC8Array : public CArrayFixFlat<TPtrC8>, public MDesC8Array
/** Array of 8 bit non-modifiable pointer descriptors, TPtrC8, implemented using 
a flat buffer.
"bafl.lib" 
@since 5.0
@publishedAll
@released
*/
	{
public:
	IMPORT_C CPtrC8Array(TInt aGranularity);
	IMPORT_C ~CPtrC8Array();
//
	IMPORT_C void CopyL(const MDesC8Array& aArray);
    inline void operator=(const MDesC8Array& aArray);
// Mixin members
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC8 MdcaPoint(TInt aIndex) const;
	};


class CPtrC16Array : public CArrayFixFlat<TPtrC16>, public MDesC16Array
/** An array of 16 bit non-modifiable pointer descriptors, TPtrC16, implemented 
using a flat buffer. 
"bafl.lib"
@since 5.0
@publishedAll
@released
*/
	{
public:
	IMPORT_C CPtrC16Array(TInt aGranularity);
	IMPORT_C ~CPtrC16Array();
//
	IMPORT_C void CopyL(const MDesC16Array& aArray);
    inline void operator=(const MDesC16Array& aArray);
// Mixin members
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC16 MdcaPoint(TInt aIndex) const;
	};


inline void CPtrC8Array::operator=(const MDesC8Array& aArray)
/** Copies a descriptor array into this array, deleting any pre-existing
elements.

The function constructs TPtrC8 elements for each descriptor element
in the specified descriptor array. This operator behaves in the 
same ways as CPtrC8Array::CopyL().

@param aArrayA reference to any descriptor array which satisfies
the protocol defined by this mixin class. */
	{CopyL(aArray);}


inline void CPtrC16Array::operator=(const MDesC16Array& aArray)
/** Copies a descriptor array into this array, deleting any pre-existing
elements.

The function constructs TPtrC16 elements for each descriptor element
in the specified descriptor array.This operator behaves in the 
same ways as CPtrC16Array::CopyL().

@param aArray A reference to any descriptor array which satisfies
the protocol defined by this mixin class. */
	{CopyL(aArray);}

// generic text array definitions

#if defined (_UNICODE)
/** Build independent implementation base class for descriptor arrays.

@see CDesC8Array
@see CDesC16Array 
@publishedAll
@released
*/
typedef CDesC16Array     CDesCArray;

/** Build independent array of descriptors implemented using a flat buffer.

@see CDesC8ArrayFlat
@see CDesC16ArrayFlat
@publishedAll
@released
*/
typedef CDesC16ArrayFlat CDesCArrayFlat;

/** Build independent array of descriptors implemented using a segmented buffer.

@see CDesC16ArraySeg
@see CDesC8ArraySeg 
@publishedAll
@released
*/
typedef CDesC16ArraySeg  CDesCArraySeg;

/** Build independent array of non-modifiable pointer descriptors.

A 16 bit build variant is generated for a Unicode build and an 8 bit build 
variant generated for a non-Unicode build.

This build independent type should always be used unless an explicit 8 bit 
or 16 bit build variant is required.

@see CPtrC16Array
@see CPtrC8Array 
@publishedAll
@released
*/
typedef CPtrC16Array CPtrCArray;
#else
/** Build independent implementation base class for descriptor arrays.

@see CDesC8Array
@see CDesC16Array 
@publishedAll
@released
*/
typedef CDesC8Array      CDesCArray;
/** Build independent array of descriptors implemented using a flat buffer.

@see CDesC8ArrayFlat
@see CDesC16ArrayFlat
@publishedAll
@released
*/
typedef CDesC8ArrayFlat  CDesCArrayFlat;
/** Build independent array of descriptors implemented using a segmented buffer.

@see CDesC16ArraySeg
@see CDesC8ArraySeg 
@publishedAll
@released
*/
typedef CDesC8ArraySeg   CDesCArraySeg;
/** Build independent array of non-modifiable pointer descriptors.

A 16 bit build variant is generated for a Unicode build and an 8 bit build 
variant generated for a non-Unicode build.

This build independent type should always be used unless an explicit 8 bit 
or 16 bit build variant is required.

@see CPtrC16Array
@see CPtrC8Array 
@publishedAll
@released
*/
typedef CPtrC8Array CPtrCArray;
#endif


#endif // __BADESCA_H__




