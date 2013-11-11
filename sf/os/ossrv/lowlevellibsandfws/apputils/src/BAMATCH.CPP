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
// Started by Brendan, January 1996
// Descended from HCIMATCH.CPP
// Incremental matcher class
// 
//

#include <bamatch.h>
#include <baflpan.h>
#include <bamdesca.h>



EXPORT_C RTextBuf::RTextBuf()
	: iPtr(NULL,0), iText(NULL)
/** Default constructor. */
	{}


EXPORT_C RTextBuf::~RTextBuf()
/** Destructor. */
	{
	Close();
	}


EXPORT_C void RTextBuf::SetMaxLengthL(TInt aMaxLength)
/** Sets the maximum length of the text that the object can store.

Attempts to store text beyond the maximum length cause a panic, as
with descriptors. */
    {
   	__ASSERT_ALWAYS(aMaxLength>0,Panic(EBafPanicTextBufOutOfRange));
	Close();
	iText=new(ELeave) TText[aMaxLength+1];
	iPtr.Set(iText,0,aMaxLength);
	}

EXPORT_C void RTextBuf::Close()
	{
	delete [] iText;
	iText=NULL;
	iPtr.Set(NULL,0,0);
   	}

// class RIncrMatcherBase

EXPORT_C RIncrMatcherBase::~RIncrMatcherBase()
/** Virtual destructor, for reimplementation by derived classes. */
	{}

TBool RIncrMatcherBase::DoIsMatch(const TDesC& aQuery,TCompareFunc aCompare) const
	{
	const TDes& des=MatchDes();
	TInt l=des.Length();
	return l>aQuery.Length() ? EFalse : aCompare(aQuery.Ptr(),l,des.Ptr(),l)==0;
	}

TInt RIncrMatcherBase::DoFirstMatchingIndex(TInt& aIndex,const MDesCArray& aDesArray,TCompareFunc aCompare) const
	{
	TInt noElements=aDesArray.MdcaCount();
	__ASSERT_ALWAYS(aIndex>=0 && aIndex<noElements,Panic(EBafPanicMatcherOutOfRange));
	do
		{
		if (DoIsMatch(aDesArray.MdcaPoint(aIndex),aCompare))
			return KErrNone;
		} while (++aIndex<noElements);
	return KErrNotFound;
	}

void RIncrMatcherBase::DoSetBestMatch(const TDesC& aBuf,TCompareFunc aCompare)
	{
	TDes& des=MatchDes();
	des.SetLength(Min(des.Length(),aBuf.Length()));
	while (!DoIsMatch(aBuf,aCompare))
		DeleteLastChar();
	}


EXPORT_C TBool RIncrMatcherBase::IsMatch(const TDesC& aQuery) const
/**Tests for match, using a binary comparison.

@param aQuery Text to match
@return  ETrue if match found, else EFalse */
    {
	return DoIsMatch(aQuery,Mem::Compare);
	}


EXPORT_C TBool RIncrMatcherBase::IsMatchC(const TDesC& aQuery) const
/** Tests for match, using locale collation rules.

@param aQuery  Text to match
@return ETrue if match found, else EFalse */
    {
	return DoIsMatch(aQuery,Mem::CompareC);
	}

EXPORT_C TBool RIncrMatcherBase::IsMatchF(const TDesC& aQuery) const
/** Tests for match, using locale folding rules.

@param    aQuery  Text to match
@return  ETrue if match found, else EFalse */
	{
	return DoIsMatch(aQuery,Mem::CompareF);
	}

EXPORT_C TInt RIncrMatcherBase::FirstMatchingIndex(TInt& aResultIndex,const MDesCArray& aDesArray,TInt aStartIndex) const
/** Finds the first match in an array, using a binary comparison.

@param aResult On return, index of the first match in aDesArray 
@param aDesArray  Array of descriptors to search
@param aStartIndex Index of aDesArray at which to begin  search
@return  KErrNone if success or KErrNotFound if no match is found */
	{
	aResultIndex=aStartIndex;
	return DoFirstMatchingIndex(aResultIndex,aDesArray,Mem::Compare);
	}

EXPORT_C TInt RIncrMatcherBase::FirstMatchingIndexC(TInt& aResultIndex,const MDesCArray& aDesArray,TInt aStartIndex) const
/** Finds the first match in an array, using locale collation rules.

@param aResult  On return, index of the first match in aDesArray 
@param aDesArray Array of descriptors to search
@param aStartIndex Index of aDesArray  at which to begin search
@return  KErrNone if success or KErrNotFound if no match is found */
    {
	aResultIndex=aStartIndex;
	return DoFirstMatchingIndex(aResultIndex,aDesArray,Mem::CompareC);
	}

EXPORT_C TInt RIncrMatcherBase::FirstMatchingIndexF(TInt& aResultIndex,const MDesCArray& aDesArray,TInt aStartIndex) const
/** Finds the first match in an array, using locale folding rules.

@param aResult  On return, index of the first match in aDesArray
@param     aDesArray Array of descriptors to search
@param     aStartIndex Index of aDesArray at which to begin search
@return  KErrNone if success or KErrNotFound if no match is found */
    {
	aResultIndex=aStartIndex;
	return DoFirstMatchingIndex(aResultIndex,aDesArray,Mem::CompareF);
	}

EXPORT_C void RIncrMatcherBase::SetBestMatch(const TDesC& aBuf)
/** Sets the match text to the best match between the match text and the passed 
buffer, using a binary comparision.

For example, if the original match text is "goose" and the passed buffer is 
"gooSE", the match text would be changed to "goo".

@param aBuf Text to match */
	{
	DoSetBestMatch(aBuf,Mem::Compare);
	}

EXPORT_C void RIncrMatcherBase::SetBestMatchC(const TDesC& aBuf)
/** Sets the match text to the best match between the match text and the passed 
buffer, using locale collation rules.

@param aBuf Text to match */
	{
	DoSetBestMatch(aBuf,Mem::CompareC);
	}

EXPORT_C void RIncrMatcherBase::SetBestMatchF(const TDesC& aBuf)
/** Sets the match text to the best match between the match text and the passed 
buffer, using locale folding rules.

@param aBuf Text to match */
	{
	DoSetBestMatch(aBuf,Mem::CompareF);
	}

EXPORT_C void RIncrMatcherBase::AppendChar(TChar aLetter)
/** Appends a character to the end of the match text.

@param aLetter Character to append */
	{
	TDes& des=MatchDes();
	__ASSERT_ALWAYS(des.Length()<des.MaxLength(),Panic(EBafPanicMatcherOutOfRange));
	des.Append(aLetter);
	}

EXPORT_C void RIncrMatcherBase::DeleteLastChar()
/** Deletes the final character in the match text. */
	{
	TDes& des=MatchDes();
	TInt l=des.Length();
	if (--l>=0)
		{
		des[l]=0;
		des.SetLength(l);
		}
	}

EXPORT_C RIncrMatcherPtr::RIncrMatcherPtr()
    : iDesPtr(NULL)
/** Default constructor. */
    {
    }

EXPORT_C RIncrMatcherPtr::RIncrMatcherPtr(TDes& aDes)
    : iDesPtr(&aDes)
/** Constructor that initialises the object with the text to be matched against.

@param aDes Text to be matched against */
    {
    }

EXPORT_C RIncrMatcherPtr::~RIncrMatcherPtr()
/** Destructor. */
    {
    }

EXPORT_C TDes& RIncrMatcherPtr::MatchDes()
/** Gets the match text.

@return Match text */
    {
    return(*iDesPtr);
    }

EXPORT_C const TDes& RIncrMatcherPtr::MatchDes() const
/** Gets the match text.

@return Match text */
    {
    return(*iDesPtr);
    }


EXPORT_C RIncrMatcherTextBuf::RIncrMatcherTextBuf()
/** Default constructor. */
    {
    }

EXPORT_C RIncrMatcherTextBuf::~RIncrMatcherTextBuf()
/** Destructor.*/
    {
    }

EXPORT_C TDes& RIncrMatcherTextBuf::MatchDes()
/** Gets the match text.

@return Match text */
    {
    return(iTextBuf.Text());
    }

EXPORT_C const TDes& RIncrMatcherTextBuf::MatchDes() const
/** Gets the match text.

@return Match text */
   {
    return(iTextBuf.Text());
    }
