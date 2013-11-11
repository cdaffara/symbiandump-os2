// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ESTRING_H
#define ESTRING_H

#include <e32std.h>

// this macro is defined only if the target is WINSCW and wchar_t is ON
// (i.e OPTION CW -wchar_t on)
// and  if the target is any thing other than WINSCW 
// it is being assumed that wchar_t is ON(OPTION CW -wchar_t on by 
// default or other wise)
#define NATIVE_WCHAR // most platforms do wchar_t properly
#if defined __cplusplus && defined __WINSCW__
#if !__option(wchar_type) // without the wchar_type option
typedef unsigned short wchar_t; // needs a typedef
#undef NATIVE_WCHAR // which is not native
#endif
#endif

// Since the LStrings can leave in their constructors, we have to make
// sure to avoid the operator new/delete ELeave mismatch problem that
// can result in the top level LString object heap being
// leaked. These operator delete overloads match the global operator
// new overloads applicable to the LString classes (which are
// non-CBase derived).
//
// Note that when allocating a native C++ array of LStrings, only the
// default constructor is invoked which cannot leave. So there are no
// issues related to the array case.

#define LSTRING_CONSTRUCTORS_MAY_LEAVE \
	static void operator delete(TAny* aPtr) __NO_THROW \
		{ \
		return ::operator delete(aPtr); \
		} \
	\
	static void operator delete(TAny* aPtr, TLeave) __NO_THROW \
		{ \
		return ::operator delete(aPtr); \
		} \
	\
	static void operator delete(TAny*, TAny*) __NO_THROW \
		{ \
		} 

/**
	@file

	A self-managing descriptor class

   	@publishedAll
   	@released
*/

/**
@class LString8

LString8 is a convenient, general-purpose 8 bit string class derived
from RBuf8. LString8 adds automatic cleanup and on-demand buffer
resize facilities.

@par L-Classes

Note: The L prefix denotes that construction, copying, passing and
returning by value, assignment, and manipulation via operators should
all be considered potentially leaving operations unless otherwise
explicitly documented. Code that uses LString8 should be written
accordingly, in leave-safe style.

@par Descriptor Relationships

Like an RBuf8, an LString8 can be passed to any function that is
prototyped to take a TDes8 or a TDesC8 reference. Again like an
RBuf8, an LString8 maintains its string data in a heap buffer.

@par Value-Type Behaviour

Unlike RBuf8, an LString8 may be used much like a simple T class;
LString8 typed variables will automatically clean up after themselves
when they go out of scope, and LString8 typed fields will similarly
automatically release all owned resources when their containing object
is destroyed.

For example, where:

@code
RBuf8 buf;
buf.CleanupClosePushL();
...
CleanupStack::PopAndDestroy();
@endcode

would be required, with LString8:

@code
LString8 buf;
...
@endcode

will suffice. Pushing an LString8 onto the cleanup stack is not
necessary or recommended, but the effects of doing so are
benign. LString8 instances can also be passed and returned by value,
but note that doing so may trigger implicit heap allocation and cause
a leave with KErrNoMemory.

@code
void TakeString(LString8 aString)
	{
	// ...
	}

LString8 ReturnString(LString8 aString)
	{
	TakeString(aString); // Statement may leave with KErrNoMemory
	return aString; // Statement may leave with KErrNoMemory
	}
@endcode

As with other descriptors, passing by reference when possible is more
efficient.

@par On-Demand Growth

In addition to the value-type semantics described above, LString8
also supports automatic in-place resizing. All standard descriptor
methods are available, but for any non-leaving descriptor method that
may panic due to buffer overflow, LString8 adds a corresponding
leaving method that automatically expands the underlying buffer
on-demand. For example, Append() will panic if the new data overflows
available buffer space, while AppendL() will attempt to realloc the
buffer as necessary. The new leaving variants may therefore leave with
KErrNoMemory, may invalidate any existing raw pointers into the data
buffer (e.g. those previously returned by Ptr()), and may change the
value returned by MaxLength().

@code
LString8 message; // Zero length
message.FormatL(_L("This is message %n from %S"), n, &s); // FormatL automatically grows the buffer
User::InfoPrint(message);
@endcode

It is important to note that LString8 only supports automatic growth
when being manipulated directly as an LString8. When an LString8 is
passed to a function accepting a TDes8, that function will operate on
it as if it is a fixed-max-length descriptor. In that case, adequate
capacity must be reserved within the LString8 prior to calling the
function. This can either be achieved using the appropriate constructor or ReserveFreeCapacityL().

@code
extern void GetLastQuery(TDes8& aQuery);
extern void GetLastAuxQuery(TDes8& aQuery);

LString8 query(KMaxQuerySize); // Guarantees query.MaxLength() >= KMaxQuerySize
GetLastQuery(query);

// ...

previousQueryMaxLength = query.MaxLength(); 

query.ReserveFreeCapacityL(KExtraRequiredSize); // Guarantees query.MaxLength() >= KExtraRequiredSize + previousQueryMaxLength;
GetLastAuxQuery(query);

@endcode

@par Relationship with TDes and RBuf

LString8 derives from RBuf8 in order to achieve maximum
interoperability with existing descriptor-accepting APIs. This
derivation forces some unusual API compromises, however, due to the
unique characteristics of LString8 compared to other descriptors.

Some of the mutating methods on the base classes, TDes8 and RBuf8,
panic when called with insufficient buffer space.  Sufficient space is
a precondition of these base classes which LString8 relaxes with
its capability to start with zero length.  LString8 defines new
leaving variants of these methods with auto-growth behaviour
(e.g. AppendL), but at the same time inherits the original methods
(e.g. Append).  This makes it too easy for developers to call the
wrong method inadvertently. In order to address this, the original 
non-leaving methods have been made private in LString8. Developers 
should use the leaving LString8 versions.

Note that, if required for any reason, the non-leaving method variants 
may be accessed by explicitly qualifying the method name to the 
appropriate parent type. For example: aString.TDes::Append(...). When 
working with an LString8 but doing so via a TDes& typed variable, all 
TDes8 APIs are directly available.

Hiding these methods does not remove the problem entirely.  The same
problem can happen if an LString object of insufficient size in passed
into a any API accepting a TDes. The advice is that developers always
ensure there is sufficient space before passing LString as a TDes.

@par Performance Concerns

While being simpler to use than existing descriptors in many cases,
LString8's use of heap allocation and its resizing variant methods
clearly come with associated costs. Their use in performance-critical
code should be carefully considered. On the other hand, LString8's
small stack footprint and ability to better-handle inputs of
unpredictable size may make them a better choice when the alternative
is a large, fixed-max-size TBuf or HBufC.

@par Buffer Ownership

Typically an LString8 will automatically allocate its own buffer, but
like RBuf8 it can also take ownership of a pre-existing raw memory
buffer or heap descriptor.

The class is intended for instantiation.

@see RBuf8
@see TBuf8
@see TPtr8
@see HBufC8
@see TDesC8
@see TDes8
@see LString

*/
class LString8 : public RBuf8
	{
public:
	/*
	 * We add leaving (auto-resizing) variants of the standard TDes
	 * methods where appropriate, and enhance the behaviour of the
	 * corresponding operators to match
	 */

	LSTRING_CONSTRUCTORS_MAY_LEAVE

	IMPORT_C LString8();
	IMPORT_C ~LString8();

	// Construct with initial capacity
	IMPORT_C explicit LString8(TInt aInitialCapacity);

	// Construct by data copying
	IMPORT_C LString8(const TDesC8& aDes);
	IMPORT_C LString8(const TUint8* aZeroTerminatedString);
	IMPORT_C LString8(const LString8& aDes);

	// Construction thru transfer of ownership
	IMPORT_C explicit LString8(HBufC8* aHBuf);
	IMPORT_C explicit LString8(TUint8 *aHeapCell,TInt aMaxLength);
	IMPORT_C explicit LString8(TUint8 *aHeapCell,TInt aLength,TInt aMaxLength);

	// Assignment thru payload copying
	IMPORT_C LString8& operator=(const TDesC8& aDes);
	IMPORT_C LString8& operator=(const LString8& aDes);
	IMPORT_C LString8& operator=(const TUint8* aZeroTerminatedString);

	// Assignment by transfer of payload to this object;
	// supplied object is destroyed or zeroed.
	IMPORT_C LString8& operator=(HBufC8* aHBuf);

	// Assign by transfer of payload ownership to this object;
	// supplied object is destroyed or zeroed.
	IMPORT_C void Assign(const LString8& aString);
	IMPORT_C void Assign(const RBuf8& aRBuf);
	IMPORT_C void Assign(HBufC8* aHBuf);
	IMPORT_C void Assign(TUint8 *aHeapCell,TInt aMaxLength);
	IMPORT_C void Assign(TUint8 *aHeapCell,TInt aLength,TInt aMaxLength);

	IMPORT_C void CreateL(RReadStream& aStream,TInt aMaxLength);

	IMPORT_C LString8& operator+=(TChar aChar);
	IMPORT_C LString8& operator+=(const TDesC8 &aDes);

	IMPORT_C void CopyL(const TDesC8 &aDes);
	IMPORT_C void CopyL(const TDesC16 &aDes);
	IMPORT_C void CopyL(const TUint8 *aZeroTerminatedString);
	IMPORT_C void CopyL(const TUint8 *aBuf,TInt aLength);

 	IMPORT_C void SetLengthL(TInt aLength);
 	IMPORT_C void SetMaxLengthL(TInt aMaxLength);
	IMPORT_C void Compress(); // deprecated; to be removed
	IMPORT_C void ReserveFreeCapacityL(TInt aExtraSpaceLength);
	IMPORT_C void Reset();

	IMPORT_C void AppendL(TChar aChar);
	IMPORT_C void AppendL(const TDesC8 &aDes);
	IMPORT_C void AppendL(const TUint8 *aBuf,TInt aLength);

	IMPORT_C void FillL(TChar aChar,TInt aLength);
	IMPORT_C void FillZL(TInt aLength);

	IMPORT_C void NumFixedWidthL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C void AppendNumFixedWidthL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C const TUint8 *PtrZL();
	IMPORT_C void CopyFL(const TDesC8 &aDes);
	IMPORT_C void CopyCL(const TDesC8 &aDes);
	IMPORT_C void CopyLCL(const TDesC8 &aDes);
	IMPORT_C void CopyUCL(const TDesC8 &aDes);
	IMPORT_C void CopyCPL(const TDesC8 &aDes);
	IMPORT_C void AppendFillL(TChar aChar,TInt aLength);
	IMPORT_C void ZeroTerminateL();
	IMPORT_C void SwapL(TDes8 &aDes);
	IMPORT_C void SwapL(LString8 &aDes); // Added overload
	IMPORT_C void InsertL(TInt aPos,const TDesC8 &aDes);
	IMPORT_C void ReplaceL(TInt aPos,TInt aLength,const TDesC8 &aDes);
	IMPORT_C void JustifyL(const TDesC8 &aDes,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void NumFixedWidthUCL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C void NumUCL(TUint64 aVal, TRadix aRadix=EDecimal);
	IMPORT_C TInt NumL(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	IMPORT_C void NumL(TInt64 aVal);
	IMPORT_C void NumL(TUint64 aVal, TRadix aRadix);
	IMPORT_C void FormatL(TRefByValue<const TDesC8> aFmt,...);
	IMPORT_C void FormatListL(const TDesC8 &aFmt,VA_LIST aList);
	IMPORT_C void AppendJustifyL(const TDesC8 &Des,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const TDesC8 &Des,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const TUint8 *aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const TUint8 *aString,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendNumFixedWidthUCL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C void AppendNumUCL(TUint64 aVal, TRadix aRadix=EDecimal);
	IMPORT_C TInt AppendNumL(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	IMPORT_C void AppendNumL(TInt64 aVal);
	IMPORT_C void AppendNumL(TUint64 aVal, TRadix aRadix);
	IMPORT_C void AppendFormatL(TRefByValue<const TDesC8> aFmt,...);
	IMPORT_C void AppendFormatListL(const TDesC8 &aFmt,VA_LIST aList);
	
	using RBuf8::operator==;
	IMPORT_C TBool operator==( const TUint8* aZeroTerminatedString) const;
	using RBuf8::operator<;
	IMPORT_C TBool operator<( const TUint8* aZeroTerminatedString) const;
	using RBuf8::operator<=;
	IMPORT_C TBool operator<=( const TUint8* aZeroTerminatedString) const;
	using RBuf8::operator>;
	IMPORT_C TBool operator>( const TUint8* aZeroTerminatedString) const;
	using RBuf8::operator>=;
	IMPORT_C TBool operator>=( const TUint8* aZeroTerminatedString) const;
	using RBuf8::operator!=;
	IMPORT_C TBool operator!=( const TUint8* aZeroTerminatedString) const;
	using RBuf8::Compare;
	IMPORT_C TInt Compare(const TUint8* aZeroTerminatedString) const;
	using RBuf8::CompareF;
	IMPORT_C TInt CompareF(const TUint8* aZeroTerminatedString) const;
	using RBuf8::Match;
	IMPORT_C TInt Match(const TUint8* aZeroTerminatedString) const;
	using RBuf8::MatchF;
	IMPORT_C TInt MatchF(const TUint8* aZeroTerminatedString) const;
	using RBuf8::Find;
	IMPORT_C TInt Find(const TUint8* aZeroTerminatedString) const;
	using RBuf8::FindF;
	IMPORT_C TInt FindF(const TUint8* aZeroTerminatedString) const;

	IMPORT_C void CopyFL(const TUint8* aZeroTerminatedString);
	IMPORT_C void CopyLCL(const TUint8* aZeroTerminatedString);
	IMPORT_C void CopyUCL(const TUint8* aZeroTerminatedString);
	IMPORT_C void CopyCPL(const TUint8* aZeroTerminatedString);
	IMPORT_C void InsertL(TInt aPos,const TUint8* aZeroTerminatedString);
	IMPORT_C void ReplaceL(TInt aPos,TInt aLength,const TUint8* aZeroTerminatedString);
	IMPORT_C void JustifyL(const TUint8* aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendL(const TUint8* aZeroTerminatedString);
	IMPORT_C LString8& operator+=(const TUint8* aZeroTerminatedString);
	
	IMPORT_C LString8(const char* aCharStr);
	
	IMPORT_C TBool operator==( const char* aCharStr) const;
	IMPORT_C TBool operator<( const char* aCharStr) const;
	IMPORT_C TBool operator<=( const char* aCharStr) const;
	IMPORT_C TBool operator>( const char* aCharStr) const;
	IMPORT_C TBool operator>=( const char* aCharStr) const;
	IMPORT_C TBool operator!=( const char* aCharStr) const;
	IMPORT_C TInt Compare(const char* aCharStr) const;
	IMPORT_C TInt CompareF(const char* aCharStr) const;
	IMPORT_C TInt Match(const char* aCharStr) const;
	IMPORT_C TInt MatchF(const char* aCharStr) const;
	IMPORT_C TInt Find(const char* aCharStr) const;
	IMPORT_C TInt Find(const char* aCharStr,TInt aLenS ) const;
	IMPORT_C TInt FindF(const char* aCharStr) const;
	IMPORT_C TInt FindF(const char* aCharStr,TInt aLenS) const;
		
	IMPORT_C LString8& operator=(const char* aCharStr);
	IMPORT_C LString8& operator+=(const char* aCharStr);
	IMPORT_C void CopyL(const char* aCharStr);
	IMPORT_C void CopyFL(const char* aCharStr);
	IMPORT_C void CopyLCL(const char* aCharStr);
	IMPORT_C void CopyUCL(const char* aCharStr);
	IMPORT_C void CopyCPL(const char* aCharStr);
	IMPORT_C void InsertL(TInt aPos,const char* aCharStr);
	IMPORT_C void ReplaceL(TInt aPos,TInt aLength,const char* aCharStr);
	IMPORT_C void AppendL(const char* aCharStr, TInt aLength);
	IMPORT_C void AppendL(const char *aCharStr );
	IMPORT_C void JustifyL(const char* aCharStr,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const char* aCharStr,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const char* aCharStr,TInt aWidth,TAlign anAlignment,TChar aFill);

protected:

private:
	/*
	 * capacity management methods
	 */
	void ReserveL(TInt aMinRequiredCapacity);
	void ReserveCapacityGrowExponentialL(TInt aRequiredCapacity);
	void ReserveCapacityGrowExponentialL();
	void ReserveFreeCapacityGrowExponentialL(TInt aRequiredEmptySpace);
	

	//----------------------------------------------------------
	// These mutable TDes8 methods panic when called with an insufficient
	// buffer space.
	// A precondition for TDes8 use is that a sufficient
	// sufficient buffer is allocated before making calls that write to the
	// buffer. LString invalidates this precondition.
	// LString inheriting these TDes methods makes it easy
	// for developers to call them inadvertently. They have been made
	// private to ensure the misunderstanding never happens. Developers should
	// use the leaving (L suffixed) LString equivalents.
	void SetLength(TInt aLength);
	void SetMax();
	void Copy(const TDesC8 &aDes);
	void Copy(const TDesC16 &aDes);
	void Copy(const TUint8 *aBuf,TInt aLength);
	void Copy(const TUint8 *aString);
	void Append(TChar aChar);
	void Append(const TDesC8 &aDes);
	void Append(const TUint8 *aBuf,TInt aLength);
	void Fill(TChar aChar,TInt aLength);
	void FillZ(TInt aLength);
	void NumFixedWidth(TUint aVal,TRadix aRadix,TInt aWidth);
	void AppendNumFixedWidth(TUint aVal,TRadix aRadix,TInt aWidth);
	const TUint8 *PtrZ();
	void CopyF(const TDesC8 &aDes);
	void CopyC(const TDesC8 &aDes);
	void CopyLC(const TDesC8 &aDes);
	void CopyUC(const TDesC8 &aDes);
	void CopyCP(const TDesC8 &aDes);
	void AppendFill(TChar aChar,TInt aLength);
	void ZeroTerminate();
	void Swap(TDes8 &aDes);
	void Insert(TInt aPos,const TDesC8 &aDes);
	void Replace(TInt aPos,TInt aLength,const TDesC8 &aDes);
	void Justify(const TDesC8 &aDes,TInt aWidth,TAlign anAlignment,TChar aFill);
	void NumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
	void NumUC(TUint64 aVal, TRadix aRadix=EDecimal);
	TInt Num(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	void Num(TInt64 aVal);
	void Num(TUint64 aVal, TRadix aRadix);
	void Format(TRefByValue<const TDesC8> aFmt,...);
	void FormatList(const TDesC8 &aFmt,VA_LIST aList);
	void AppendJustify(const TDesC8 &Des,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TDesC8 &Des,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TUint8 *aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TUint8 *aZeroTerminatedString,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendNumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
	void AppendNumUC(TUint64 aVal, TRadix aRadix=EDecimal);
	TInt AppendNum(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	void AppendNum(TInt64 aVal);
	void AppendNum(TUint64 aVal, TRadix aRadix);
	void AppendFormat(TRefByValue<const TDesC8> aFmt,TDes8Overflow *aOverflowHandler,...);
	void AppendFormat(TRefByValue<const TDesC8> aFmt,...);
	void AppendFormatList(const TDesC8 &aFmt,VA_LIST aList,TDes8Overflow *aOverflowHandler=NULL);
	// end of TDes8 methods
	//---------------------------------------------------

	//----------------------------------------------------------
	// These mutable RBuf8 methods have been privatised because
	// they make assumptions which are incompatible with the 
	// general use pattern of LString16.
	void Swap(RBuf8& aRBuf);
	TInt Create(TInt aMaxLength);
	void CreateL(TInt aMaxLength);
	TInt CreateMax(TInt aMaxLength);
	void CreateMaxL(TInt aMaxLength);
	TInt Create(const TDesC8& aDes);
	void CreateL(const TDesC8& aDes);
	TInt Create(const TDesC8& aDes,TInt aMaxLength);
	void CreateL(const TDesC8& aDes,TInt aMaxLength);
	void Close();
	void CleanupClosePushL();
	// end of privated RBuf16 methods
	//---------------------------------------------------

	void EnsureCapacityIncrementL(TInt aLengthIncrement);
	void IncreaseCapacityL();
	
private:
	//reserved data member for future-proofing
	TInt iReserved;
	};

/**
This is the 16-bit version of LString8. All the comments on LString8 apply equally.

@see LString8
@see RBuf16
@see TBuf16
@see TPtr16
@see HBufC16
@see TDesC16
@see TDes16
@see LString
*/
class LString16 : public RBuf16
	{
public:
	/*
	 * We add leaving (auto-resizing) variants of the standard TDes
	 * methods where appropriate, and enhance the behaviour of the
	 * corresponding operators to match
	 */

	LSTRING_CONSTRUCTORS_MAY_LEAVE

	IMPORT_C LString16();
	IMPORT_C ~LString16();

	// Construct with initial capacity
	IMPORT_C explicit LString16(TInt aInitialCapacity);

	// Construct by data copying
	IMPORT_C LString16(const TDesC16& aDes);
	IMPORT_C LString16(const TUint16* aZeroTerminatedString);
	IMPORT_C LString16(const LString16& aDes);

	// Construction thru transfer of ownership
	IMPORT_C explicit LString16(HBufC16* aHBuf);
	IMPORT_C explicit LString16(TUint16 *aHeapCell,TInt aMaxLength);
	IMPORT_C explicit LString16(TUint16 *aHeapCell,TInt aLength,TInt aMaxLength);

	// Assignment thru payload copying
	IMPORT_C LString16& operator=(const TDesC16& aDes);
	IMPORT_C LString16& operator=(const LString16& aDes);
	IMPORT_C LString16& operator=(const TUint16* aZeroTerminatedString);

	// Assignment by transfer of payload to this object;
	// supplied object is destroyed or zeroed.
	IMPORT_C LString16& operator=(HBufC16* aHBuf);

	// Assign by transfer of payload ownership to this object;
	// supplied object is destroyed or zeroed.
	IMPORT_C void Assign(const LString16& aString);
	IMPORT_C void Assign(const RBuf16& aRBuf);
	IMPORT_C void Assign(HBufC16* aHBuf);
	IMPORT_C void Assign(TUint16 *aHeapCell,TInt aMaxLength);
	IMPORT_C void Assign(TUint16 *aHeapCell,TInt aLength,TInt aMaxLength);

	IMPORT_C void CreateL(RReadStream& aStream,TInt aMaxLength);

	IMPORT_C LString16& operator+=(TChar aChar);
	IMPORT_C LString16& operator+=(const TDesC16 &aDes);

	IMPORT_C void CopyL(const TDesC16 &aDes);
	IMPORT_C void CopyL(const TUint16 *aZeroTerminatedString);
	IMPORT_C void CopyL(const TDesC8 &aDes);
	IMPORT_C void CopyL(const TUint16 *aBuf,TInt aLength);

 	IMPORT_C void SetLengthL(TInt aLength);
 	IMPORT_C void SetMaxLengthL(TInt aMaxLength);
	IMPORT_C void Compress(); // deprecated; to be removed
	IMPORT_C void ReserveFreeCapacityL(TInt aExtraSpaceLength);
	IMPORT_C void Reset();

	IMPORT_C void AppendL(TChar aChar);
	IMPORT_C void AppendL(const TDesC16 &aDes);
	IMPORT_C void AppendL(const TUint16 *aBuf,TInt aLength);

	IMPORT_C void FillL(TChar aChar,TInt aLength);
	IMPORT_C void FillZL(TInt aLength);

	IMPORT_C void NumFixedWidthL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C void AppendNumFixedWidthL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C const TUint16 *PtrZL();
	IMPORT_C void CopyFL(const TDesC16 &aDes);
	IMPORT_C void CopyCL(const TDesC16 &aDes);
	IMPORT_C void CopyLCL(const TDesC16 &aDes);
	IMPORT_C void CopyUCL(const TDesC16 &aDes);
	IMPORT_C void CopyCPL(const TDesC16 &aDes);
	IMPORT_C void AppendFillL(TChar aChar,TInt aLength);
	IMPORT_C void ZeroTerminateL();
	IMPORT_C void SwapL(TDes16 &aDes);
	IMPORT_C void SwapL(LString16 &aDes); // Added overload
	IMPORT_C void InsertL(TInt aPos,const TDesC16 &aDes);
	IMPORT_C void ReplaceL(TInt aPos,TInt aLength,const TDesC16 &aDes);
	IMPORT_C void JustifyL(const TDesC16 &aDes,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void NumFixedWidthUCL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C void NumUCL(TUint64 aVal, TRadix aRadix=EDecimal);
	IMPORT_C TInt NumL(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	IMPORT_C void NumL(TInt64 aVal);
	IMPORT_C void NumL(TUint64 aVal, TRadix aRadix);
	IMPORT_C void FormatL(TRefByValue<const TDesC16> aFmt,...);
	IMPORT_C void FormatListL(const TDesC16 &aFmt,VA_LIST aList);
	IMPORT_C void AppendJustifyL(const TDesC16 &Des,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const TDesC16 &Des,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const TUint16 *aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const TUint16 *aString,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendNumFixedWidthUCL(TUint aVal,TRadix aRadix,TInt aWidth);
	IMPORT_C void AppendNumUCL(TUint64 aVal, TRadix aRadix=EDecimal);
	IMPORT_C TInt AppendNumL(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	IMPORT_C void AppendNumL(TInt64 aVal);
	IMPORT_C void AppendNumL(TUint64 aVal, TRadix aRadix);
	IMPORT_C void AppendFormatL(TRefByValue<const TDesC16> aFmt,...);
	IMPORT_C void AppendFormatListL(const TDesC16 &aFmt,VA_LIST aList);
	
	using RBuf16::operator==;
	IMPORT_C TBool operator==( const TUint16* aZeroTerminatedString) const;
	using RBuf16::operator<;
	IMPORT_C TBool operator<( const TUint16* aZeroTerminatedString) const;
	using RBuf16::operator<=;
	IMPORT_C TBool operator<=( const TUint16* aZeroTerminatedString) const;
	using RBuf16::operator>;
	IMPORT_C TBool operator>( const TUint16* aZeroTerminatedString) const;
	using RBuf16::operator>=;
	IMPORT_C TBool operator>=( const TUint16* aZeroTerminatedString) const;
	using RBuf16::operator!=;
	IMPORT_C TBool operator!=( const TUint16* aZeroTerminatedString) const;
	using RBuf16::Compare;
	IMPORT_C TInt Compare(const TUint16* aZeroTerminatedString) const;
	using RBuf16::CompareF;
	IMPORT_C TInt CompareF(const TUint16* aZeroTerminatedString) const;
	using RBuf16::Match;
	IMPORT_C TInt Match(const TUint16* aZeroTerminatedString) const;
	using RBuf16::MatchF;
	IMPORT_C TInt MatchF(const TUint16* aZeroTerminatedString) const;
	using RBuf16::Find;
	IMPORT_C TInt Find(const TUint16* aZeroTerminatedString) const;
	using RBuf16::FindF;
	IMPORT_C TInt FindF(const TUint16* aZeroTerminatedString) const;
	
	IMPORT_C void CopyFL(const TUint16* aZeroTerminatedString);
	IMPORT_C void CopyLCL(const TUint16* aZeroTerminatedString);
	IMPORT_C void CopyUCL(const TUint16* aZeroTerminatedString);
	IMPORT_C void CopyCPL(const TUint16* aZeroTerminatedString);
	IMPORT_C void InsertL(TInt aPos,const TUint16* aZeroTerminatedString);
	IMPORT_C void ReplaceL(TInt aPos,TInt aLength,const TUint16* aZeroTerminatedString);
	IMPORT_C void JustifyL(const TUint16* aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendL(const TUint16* aZeroTerminatedString);
	IMPORT_C LString16& operator+=(const TUint16* aZeroTerminatedString);
	
#ifdef NATIVE_WCHAR 
	IMPORT_C LString16(const wchar_t* aWideCharStr);
	
	IMPORT_C TBool operator==( const wchar_t* aWideCharStr) const;
	IMPORT_C TBool operator<( const wchar_t* aWideCharStr) const;
	IMPORT_C TBool operator<=( const wchar_t* aWideCharStr) const;
	IMPORT_C TBool operator>( const wchar_t* aWideCharStr) const;
	IMPORT_C TBool operator>=( const wchar_t* aWideCharStr) const;
	IMPORT_C TBool operator!=( const wchar_t* aWideCharStr) const;
	IMPORT_C TInt Compare(const wchar_t* aWideCharStr) const;
	IMPORT_C TInt CompareF(const wchar_t* aWideCharStr) const;
	IMPORT_C TInt Match(const wchar_t* aWideCharStr) const;
	IMPORT_C TInt MatchF(const wchar_t* aWideCharStr) const;
	IMPORT_C TInt Find(const wchar_t* aWideCharStr) const;	
	IMPORT_C TInt Find(const wchar_t* aWideCharStr,TInt aLenS ) const;
	IMPORT_C TInt FindF(const wchar_t* aWideCharStr) const;
	IMPORT_C TInt FindF(const wchar_t* aWideCharStr,TInt aLenS) const;
	
	IMPORT_C LString16& operator=(const wchar_t* aWideCharStr);
	IMPORT_C LString16& operator+=(const wchar_t* aWideCharStr);
	IMPORT_C void CopyL(const wchar_t* aWideCharStr);
	IMPORT_C void CopyFL(const wchar_t* aWideCharStr);
	IMPORT_C void CopyLCL(const wchar_t* aWideCharStr);
	IMPORT_C void CopyUCL(const wchar_t* aWideCharStr);
	IMPORT_C void CopyCPL(const wchar_t* aWideCharStr);
	IMPORT_C void InsertL(TInt aPos,const wchar_t* aWideCharStr);
	IMPORT_C void ReplaceL(TInt aPos,TInt aLength,const wchar_t* aWideCharStr);
	IMPORT_C void AppendL(const wchar_t* aWideCharStr, TInt aLength);
	IMPORT_C void AppendL(const wchar_t* aWideCharStr);
	IMPORT_C void JustifyL(const wchar_t* aWideCharStr,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const wchar_t* aWideCharStr,TInt aWidth,TAlign anAlignment,TChar aFill);
	IMPORT_C void AppendJustifyL(const wchar_t* aWideCharStr,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	
#endif// NATIVE_WCHAR 
	

protected:

private:
	/*
	 * New capacity management API
	 */
	void ReserveL(TInt aMinRequiredCapacity);
	void ReserveCapacityGrowExponentialL(TInt aRequiredCapacity);
	void ReserveCapacityGrowExponentialL();
	void ReserveFreeCapacityGrowExponentialL(TInt aRequiredEmptySpace);
	

	//----------------------------------------------------------
	// These mutable TDes16 methods panic when called with an insufficient
	// buffer space.
	// A precondition for TDes16 use is that a sufficient
	// sufficient buffer is allocated before making calls that write to the
	// buffer. LString invalidates this precondition.
	// LString inheriting these TDes methods makes it easy
	// for developers to call them inadvertently. They have been made
	// private to ensure the misunderstanding never happens. Developers should
	// use the leaving (L suffixed) LString equivalents.
	void SetLength(TInt aLength);
	void SetMax();
	void Copy(const TDesC8 &aDes);
	void Copy(const TDesC16 &aDes);
	void Copy(const TUint16 *aBuf,TInt aLength);
	void Copy(const TUint16 *aString);
	void Append(TChar aChar);
	void Append(const TDesC16 &aDes);
	void Append(const TUint16 *aBuf,TInt aLength);
	void Fill(TChar aChar,TInt aLength);
	void FillZ(TInt aLength);
	void NumFixedWidth(TUint aVal,TRadix aRadix,TInt aWidth);
	void AppendNumFixedWidth(TUint aVal,TRadix aRadix,TInt aWidth);
	const TUint16 *PtrZ();
	void CopyF(const TDesC16 &aDes);
	void CopyC(const TDesC16 &aDes);
	void CopyLC(const TDesC16 &aDes);
	void CopyUC(const TDesC16 &aDes);
	void CopyCP(const TDesC16 &aDes);
	void AppendFill(TChar aChar,TInt aLength);
	void ZeroTerminate();
	void Swap(TDes16 &aDes);
	void Insert(TInt aPos,const TDesC16 &aDes);
	void Replace(TInt aPos,TInt aLength,const TDesC16 &aDes);
	void Justify(const TDesC16 &aDes,TInt aWidth,TAlign anAlignment,TChar aFill);
	void NumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
	void NumUC(TUint64 aVal, TRadix aRadix=EDecimal);
	TInt Num(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	void Num(TInt64 aVal);
	void Num(TUint64 aVal, TRadix aRadix);
	void Format(TRefByValue<const TDesC16> aFmt,...);
	void FormatList(const TDesC16 &aFmt,VA_LIST aList);
	void AppendJustify(const TDesC16 &Des,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TDesC16 &Des,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TUint16 *aZeroTerminatedString,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendJustify(const TUint16 *aZeroTerminatedString,TInt aLength,TInt aWidth,TAlign anAlignment,TChar aFill);
	void AppendNumFixedWidthUC(TUint aVal,TRadix aRadix,TInt aWidth);
	void AppendNumUC(TUint64 aVal, TRadix aRadix=EDecimal);
	TInt AppendNum(TReal aVal,const TRealFormat &aFormat) __SOFTFP;
	void AppendNum(TInt64 aVal);
	void AppendNum(TUint64 aVal, TRadix aRadix);
	void AppendFormat(TRefByValue<const TDesC16> aFmt,TDes16Overflow *aOverflowHandler,...);
	void AppendFormat(TRefByValue<const TDesC16> aFmt,...);
	void AppendFormatList(const TDesC16 &aFmt,VA_LIST aList,TDes16Overflow *aOverflowHandler=NULL);
	// end of TDes16 methods
	//---------------------------------------------------

	//----------------------------------------------------------
	// These mutable RBuf16 methods have been privatised because
	// they make assumptions which are incompatible with the 
	// general use pattern of LString16.
	void Swap(RBuf16& aRBuf);
	TInt Create(TInt aMaxLength);
	void CreateL(TInt aMaxLength);
	TInt CreateMax(TInt aMaxLength);
	void CreateMaxL(TInt aMaxLength);
	TInt Create(const TDesC16& aDes);
	void CreateL(const TDesC16& aDes);
	TInt Create(const TDesC16& aDes,TInt aMaxLength);
	void CreateL(const TDesC16& aDes,TInt aMaxLength);
	void Close();
	void CleanupClosePushL();
	// end of privated RBuf16 methods
	//---------------------------------------------------

	void EnsureCapacityIncrementL(TInt aLengthIncrement);
	void IncreaseCapacityL();
	
private:
	//reserved data member for future-proofing
	TInt iReserved;
	};


/**
Type alias for LString16.

LString is typically used in preference to LString16 when manipulating
Unicode text, in the same way that TDesC is typically used in
preference to TDesC16.
*/
typedef LString16 LString;

/**
Type alias for LString8.

LData can be used to better express intent when using an LString8 to
manipulate raw binary (non character) data.
*/
typedef LString8 LData;

#undef LSTRING_CONSTRUCTORS_MAY_LEAVE

#endif // !ESTRING_H
