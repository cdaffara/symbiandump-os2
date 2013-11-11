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

#include "UD_STD.H"

EXPORT_C TInt TTextOps::Compare(const TDesC8& aLeft,const TDesC8& aRight) const
	{
	return Compare(aLeft.Ptr(),aLeft.Length(),aRight.Ptr(),aRight.Length());
	}

EXPORT_C TInt TTextOps::Match(const TText8* aPtr,TInt aLen,const TText8* aPatternPtr,TInt aPatternLen) const
	{
	return Match(TPtrC8(aPtr,aLen),TPtrC8(aPatternPtr,aPatternLen));
	}

EXPORT_C TInt TTextOps::Find(const TDesC8& aDes,const TDesC8& aSub) const
	{
	return iFind8(aDes,aSub.Ptr(),aSub.Length());
	}

EXPORT_C TInt TTextOps::Find(const TText8* aPtr,TInt aLen,const TText8* aSubPtr,TInt aSubLen) const
	{
	return iFind8(TPtrC8(aPtr,aLen),aSubPtr,aSubLen);
	}

EXPORT_C TInt TTextOps::Compare(const TDesC16& aLeft,const TDesC16& aRight) const
	{
	return Compare(aLeft.Ptr(),aLeft.Length(),aRight.Ptr(),aRight.Length());
	}

EXPORT_C TInt TTextOps::Match(const TText16* aPtr,TInt aLen,const TText16* aPatternPtr,TInt aPatternLen) const
	{
	return Match(TPtrC16(aPtr,aLen),TPtrC16(aPatternPtr,aPatternLen));
	}

EXPORT_C TInt TTextOps::Find(const TDesC16& aDes,const TDesC16& aSub) const
	{
	return iFind16(aDes,aSub.Ptr(),aSub.Length());
	}

EXPORT_C TInt TTextOps::Find(const TText16* aPtr,TInt aLen,const TText16* aSubPtr,TInt aSubLen) const
	{
	return iFind16(TPtrC16(aPtr,aLen),aSubPtr,aSubLen);
	}

/**
The method compares aLeft and aRight unicode strings.
Collation level 3 will be used. 
@param aLeft Left string to compare.
@param aRight Right string to compare.
This method is used by sorting algorithms when the key field is a unciode string.
@return Positive. if aLeft is greater than aRight.
        Negative. if aLeft is less than aRight.
        Zero, if aLeft is equal to aRight.
*/
TInt TTextOps::Order(const TDesC16& aLeft, const TDesC16& aRight) const
	{
	return Order(aLeft.Ptr(), aLeft.Length(), aRight.Ptr(), aRight.Length());
	}
	
LOCAL_C TUint DoFold(TUint aChar)
	{
	return aChar;
	}

LOCAL_C TInt DoMatch(const TDesC8& aDes,const TDesC8& aPattern)
	{
	return aDes.Match(aPattern);
	}

LOCAL_C TInt DoMatchF(const TDesC8& aDes,const TDesC8& aPattern)
	{
	return aDes.MatchF(aPattern);
	}

LOCAL_C TInt DoMatchC(const TDesC8& aDes,const TDesC8& aPattern)
	{
	return aDes.MatchC(aPattern);
	}

LOCAL_C TInt DoFind(const TDesC8& aDes,const TText8* aSubPtr,TInt aSubLen)
	{
	return aDes.Find(aSubPtr,aSubLen);
	}

LOCAL_C TInt DoFindF(const TDesC8& aDes,const TText8* aSubPtr,TInt aSubLen)
	{
	return aDes.FindF(aSubPtr,aSubLen);
	}

LOCAL_C TInt DoFindC(const TDesC8& aDes,const TText8* aSubPtr,TInt aSubLen)
	{
	return aDes.FindC(aSubPtr,aSubLen);
	}

LOCAL_C TInt DoMatch(const TDesC16& aDes,const TDesC16& aPattern)
	{
	return aDes.Match(aPattern);
	}

LOCAL_C TInt DoMatchF(const TDesC16& aDes,const TDesC16& aPattern)
	{
	return aDes.MatchF(aPattern);
	}

LOCAL_C TInt DoMatchC(const TDesC16& aDes,const TDesC16& aPattern)
	{
	return aDes.MatchC(aPattern);
	}

LOCAL_C TInt DoFind(const TDesC16& aDes,const TText16* aSubPtr,TInt aSubLen)
	{
	return aDes.Find(aSubPtr,aSubLen);
	}

LOCAL_C TInt DoFindF(const TDesC16& aDes,const TText16* aSubPtr,TInt aSubLen)
	{
	return aDes.FindF(aSubPtr,aSubLen);
	}

LOCAL_C TInt DoFindC(const TDesC16& aDes,const TText16* aSubPtr,TInt aSubLen)
	{
	return aDes.FindC(aSubPtr,aSubLen,0);
	}

LOCAL_C TInt DoCompareC(const TText16* aLeftPtr,TInt aLeftLen,const TText16* aRightPtr,TInt aRightLen)
    {
	return Mem::CompareC(aLeftPtr,aLeftLen,aRightPtr,aRightLen,0,NULL);
	}
	
/**
The method compares aLeftPtr and aRightPtr unicode strings.
Collation level 3 will be used. 
@param aLeftPtr Left string to compare.
@param aLeftLen The length of left string.
@param aRightPtr Right string to compare.
@param aRightLen The length of right string.
This method is used by sorting algorithms when the key field is a unciode string.
@return Positive. if aLeftPtr is greater than aRightPtr.
        Negative. if aLeftPtr is less than aRightPtr.
        Zero, if aLeftPtr is equal to aRightPtr.
*/
LOCAL_C TInt DoOrderC(const TText16* aLeftPtr, TInt aLeftLen, const TText16* aRightPtr, TInt aRightLen)
    {
	return Mem::CompareC(aLeftPtr, aLeftLen, aRightPtr, aRightLen);
	}

LOCAL_D TTextOps const OpTable[3]=
	{
	//iFold          iCompare8      iMatch8   iFind8   iCompare16     iMatch16  iFind16 iOrder16
	{&DoFold,       &Mem::Compare, &DoMatch, &DoFind, &Mem::Compare, &DoMatch, &DoFind, &Mem::Compare}, //EDbCompareNormal
	{&User::Fold,   &Mem::CompareF,&DoMatchF,&DoFindF,&Mem::CompareF,&DoMatchF,&DoFindF,&Mem::CompareF},//EDbCompareFolded
	{&User::Collate,&Mem::CompareC,&DoMatchC,&DoFindC,&DoCompareC,   &DoMatchC,&DoFindC,&DoOrderC}		//EDbCompareCollated
	};

EXPORT_C const TTextOps& TTextOps::Ops(TDbTextComparison aType)
	{
	return OpTable[aType];
	}
