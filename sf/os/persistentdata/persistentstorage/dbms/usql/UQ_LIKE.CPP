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
// SQL Like predicate node
// 
//

#include "UQ_STD.H"

inline TUint8* TextCopy(TUint8* aDest,const TUint8* aSrc,TInt aLen)
	{return Mem::Copy(aDest,aSrc,aLen);}
inline TUint16* TextCopy(TUint16* aDest,const TUint16* aSrc,TInt aLen)
	{return (TUint16*)Mem::Copy(aDest,aSrc,aLen<<1);}

// template Class HMatcherPattern

template <class T,class D>
inline HMatcherPattern<T,D>* HMatcherPattern<T,D>::Realloc(HMatcherPattern<T,D>* aThis,TInt aSize)
	{return STATIC_CAST(TThis*,User::ReAlloc(aThis,_FOFF(TThis,iPattern[aSize])));}

/**
Creates a HMatcherPattern that converts the pattern into more manageable pieces
based on a delimeter that is the wildcard * (asterisk).

Characters between * (asterisks) must not number greater than KMaxSegmentLength.
If only one * exists then the length is taken from the start and end of pattern.
If these segments contain ? (question mark) wildcard than they must not number
greater than KMaxSegmentLength-2.

The user is responsible for the returned pointer's memory.

@param			aPattern The search pattern to match.
@param			aEscape ESCAPE clause 
@leave			KErrNoMemory if no more memory is available.
@leave			KErrArgument if the search pattern segment length is greater
				than KMaxSegmentLength,
@return			The newly constructed pattern pointer.
@see			KMaxSegmentLength
*/
template <class T,class D>
HMatcherPattern<T,D>* HMatcherPattern<T,D>::NewL(const D& aPattern, TBool aEscape)
	{
	TThis* self=0;
	TInt r=Construct(self,aPattern,aEscape);
	if (r!=KErrNone)
		{
		User::Free(self);
		__LEAVE(r);
		}
	return self;
	}

template <class T,class D>
TInt HMatcherPattern<T,D>::MatchL(const D& aDes,const TTextOps& aTextOp) const
//
// Test the pattern against the supplied descriptor
//
	{
	TDesMatcher<T,D> matcher(aDes);
	return matcher.MatchL(*this,aTextOp);
	}

template <class T,class D>
TInt HMatcherPattern<T,D>::MatchL(MStreamBuf& aBuf,TInt aLength,const TTextOps& aTextOp) const
//
// Test the pattern against the supplied stream
//
	{
	TStreamMatcher<T,D> matcher(aBuf,aLength);
	return matcher.MatchL(*this,aTextOp);
	}

/**
Breaks up a given search pattern into manageable segments.

The pattern is broken into segments. These segments are organised
from the segment delimeter that is the wildcard * (asterisk).
So in effect a segment may contain other wildcards (i.e. ?) or
the entire pattern (no embedded asterisks) as these at least MUST match.

The cell is created and updated with the segments type and length, and also
the segment itself.

Not including asterisks, the segment must be no longer than length KMaxSegmentLength.
However, if the segment has a ? (question mark) wildcard within it then the 
segment must be no longer than length KMaxSegmentLength-2.

This is due to the way the Find and Match functions of TDes work. Match understands
wildcards whilst Find does not.

The match algorithm depends on KMaxSegmentLength being smaller than the
text read buffer, and for efficiency should not be more than half the size
of the read buffer. Also KMaxSegmentLength must currently fit into 8 bits,
as it is embedded into a single character in an 8-bit text matching buffer. 
[So increasing KMaxSegmentLength is not a simple exercise.]

The search is repeated for each segment within the pattern. Increasing the cell
size and inserting each segment and associated segment data.

On reaching the end of the pattern this data is passed back to the user.
The user is responsible for this returned pointers memory.

@param			aCell On return points to a cell matching this pattern.
@param			aPattern The search pattern to match.
@param			aEscape ESCAPE clause 
@return			KErrNoMemory if no memory is available to create or
				increase the size of a cell,
				KErrArgument if the search pattern segment length is greater
				than KMaxSegmentLength,
				KErrNone if the match was successful.
@see			KMaxSegmentLength
*/
template <class T,class D>
TInt HMatcherPattern<T,D>::Construct(HMatcherPattern<T,D>*& aCell,const D& aPattern,TBool aEscape)
	{
	const T* pM=aPattern.Ptr();
	const T* const eM=pM+aPattern.Length();
	TInt size=(eM-pM)+KPatternGranularity;
	TThis* self=Realloc(0,size);
	if (!self)
		return KErrNoMemory;
	aCell=self;
	T* seg=&self->iPattern[0];
	const T* end=&self->iPattern[size];
	TInt term;	// terminating code
	
	if (eM==pM)
		term=ENull;
	else
		{
		term=EStop;
		do
			{
			//Following code is for the implementation of limited-ESCAPE-clause 
			if(aEscape)
				{
				const T* here=pM;
				TInt len = aPattern.Length();
				if (len>KMaxSegmentLength)
					return KErrArgument;
				*seg++=T(EEscape );
				*seg++=T(len);
				seg=TextCopy(seg,here,len);
				break;
				}
			T m=*pM;
			if (m==KMatchAny)
				{
				term=ESuccess;
				do
					{
					if (++pM==eM)
						break;
					m=*pM;
					} while (m==KMatchAny);
				if (pM==eM)
					break;
				}
			const T* here=pM;
			TInt type;
			if (m==KMatchOne)
				{
				while (++pM<eM && *pM==KMatchOne) {;}
				type=ESkip;
				}
			else
				{
				type=0;
				while (++pM<eM)
					{
					m=*pM;
					if (m==KMatchAny)
						break;
					if (m==KMatchOne)
						type|=EWild;
					}
				if (term==EStop)
					type|=EBeginning;
				else if (pM==eM)
					type|=EEnd;
				else
					{
					type|=EMiddle;
					if (type&EWild)
						{		// include '*'s in segment for matching
						--here;
						++pM;
						}
					}
				}
			*seg++=T(type);
			TInt len=pM-here;
			if (len>KMaxSegmentLength)
				return KErrArgument;
			*seg++=T(len);
			if (type==ESkip)
				len=0;
			if (seg+4+len>end)
				{
				TInt newsize=end-&self->iPattern[0]+KPatternGranularity;
				self=Realloc(self,newsize);
				if (!self)
					return KErrNoMemory;
				seg+=&self->iPattern[0]-&aCell->iPattern[0];
				end=&self->iPattern[newsize];
				aCell=self;
				}
			if (type==(EMiddle|EWild))
				{
				*seg++=T(KMatchAny);
				++here;
				--len;
				}
			seg=TextCopy(seg,here,len);
			} while (pM<eM);
		}
	*seg++=T(term);
	aCell=Realloc(self,seg-&self->iPattern[0]);
	return KErrNone;
	}
	
// class TMatcher

template <class T,class D>
TBool TMatcher<T,D>::MatchL(const HMatcherPattern<T,D>& aPattern,const TTextOps& aTextOp)
	{
	const T* pM=&aPattern.iPattern[0];
	const T* eB;
	const T* pB=UnderflowL(eB);
	for (;;)
		{
		TInt segment=*pM;
		switch (segment&EMask)
			{
		case ENull:
			return pB==eB;
		case ESuccess:
			return ETrue;
		case EStop:
			if (pB==eB)
				pB=UnderflowL(eB);
			return pB==eB;
		case ESkip:
			{
			TInt len=*++pM;
			TInt skip=len+pB-eB;
			if (skip>0)
				{
				pB=UnderflowL(eB)+skip;
				if (pB>eB)
					return EFalse;
				}
			else
				pB+=len;
			++pM;
			}
			break;
		//Following code is for the implementation of limited-ESCAPE-clause 
		case EEscape:
			{
			TInt len=*++pM;
			++pM;
			TInt bLen=eB-pB;
			if (bLen<len)
				return EFalse;
			if (aTextOp.Find(pB,bLen,pM,len)<0)
				return EFalse;
			pM+=len;
			pB+=bLen;
			}
			break;
		case EBeginning:
			{
			TInt len=*++pM;
			++pM;
			if (eB-pB<len)
				return EFalse;
			if (segment&EWild)
				{
				if (aTextOp.Match(pB,len,pM,len)<0)
					return EFalse;
				}
			else
				{
				if (aTextOp.Compare(pB,len,pM,len)!=0)
					return EFalse;
				}
			pM+=len;
			pB+=len;
			}
			break;
		case EMiddle:
			{
			TInt len=*++pM;
			++pM;
			TInt match=len;
			if (segment&EWild)
				match-=2;	// the number of characters to match
			TInt left=eB-pB;
			if (left<match)
				pB=UnderflowL(eB,left);
			for (;;)
				{
				TInt bLen=eB-pB;
				if (bLen<match)
					return EFalse;
				TInt pos=segment&EWild ? aTextOp.Match(pB,bLen,pM,len) : aTextOp.Find(pB,bLen,pM,len);
				if (pos>=0)
					{	// found it
					pB+=pos+match;
					break;
					}
				// not found, next chunk of data please.
				pB=UnderflowL(eB,match-1);
				}
			pM+=len;
			}
			break;
		case EEnd:	// match the last segment
			{
			TInt len=*++pM;
			++pM;
			TInt left=eB-pB;
			if (left<len)
				{
				pB=UnderflowL(eB,left);
				if (eB-pB<len)
					return EFalse;
				}
			while (eB-pB>len)
				pB=UnderflowL(eB,len);
			if (segment&EWild)
				{
				if (aTextOp.Match(pB,len,pM,len)<0)
					return EFalse;
				}
			else
				{
				if (aTextOp.Compare(pB,len,pM,len)!=0)
					return EFalse;
				}
			}
			return ETrue;
			}
		}
	}

// Class TDesMatcher

template <class T,class D>
inline TDesMatcher<T,D>::TDesMatcher(const D& aDes)
	{iEnd=(iPtr=aDes.Ptr())+aDes.Length();}

template <class T,class D>
const T* TDesMatcher<T,D>::UnderflowL(const T*& aEnd,TInt aRetain)
	{
	const T* ptr=iPtr-aRetain;
	aEnd=iPtr=iEnd;
	return ptr;
	}

// Class TStreamMatcher

template <class T,class D>
inline TStreamMatcher<T,D>::TStreamMatcher(MStreamBuf& aStreamBuf,TInt aLength)
	:iStream(&aStreamBuf),iRemain(aLength),iEnd(&iBuffer[EBufSize])
	{}

template <class T,class D>
const T* TStreamMatcher<T,D>::UnderflowL(const T*& aEnd,TInt aRetain)
	{
	if (iRemain==0)
		{	// no more stream data, don't move etc.
		aEnd=iEnd;
		return iEnd-aRetain;
		}
	else
		{
		T* rp=&iBuffer[0];
		if (aRetain)
			rp=TextCopy(rp,iEnd-aRetain,aRetain);
		TInt read=Min(EBufSize-aRetain,iRemain);
		iStream.ReadL(rp,read);
		iRemain-=read;
		aEnd=iEnd=rp+read; 
		return iBuffer;
		}
	}

// Class RSqlLiteral

void RSqlLiteral::ToPattern8L(TBool aEscape)
//
// Convert a Buf8 to an 8-bit pattern
//
	{
	__ASSERT(iType==EBuf8);
	HMatcherPattern8* pattern=HMatcherPattern8::NewL(Text8(),aEscape);
	User::Free(iVal.iAlloc);
	iVal.iAlloc=pattern;
	iType=EPattern8;
	}

void RSqlLiteral::ToPattern16L(TBool aEscape)
//
// Convert a Buf8 to an 8-bit pattern
//
	{
	__ASSERT(iType==EBuf16);
	HMatcherPattern16* pattern=HMatcherPattern16::NewL(Text16(),aEscape);
	User::Free(iVal.iAlloc);
	iVal.iAlloc=pattern;
	iType=EPattern16;
	}


// Class CSqlLikePredicate

LOCAL_C TInt MatchLength(const TText* aPtr,const TText* aEnd)
	{
	TInt match=0;
	while (aPtr<aEnd)
		{
		TText c=*aPtr++;
		if (c!=KMatchAny)
			{
			++match;
			if (c=='\'')
				++aPtr;
			}
		}
	return match;
	}
//Following code is for the implementation of limited-ESCAPE-clause 
LOCAL_C TInt MatchLengthEscape(const TText* aPtr,const TText* aEnd)
	{
	TInt match=0;
	while (aPtr<aEnd)
		{
		TText c=*aPtr++;
			{
			++match;
			if (c=='\'')
				++aPtr;
			}
		}
	return match;
	}


CSqlLikePredicate::CSqlLikePredicate(TType aType,const TDesC& aColumn,const RSqlLiteral& aPattern)
	: CSqlLiteralNode(aType,aColumn,aPattern)
	{
	__ASSERT(aType==ELike||aType==ENotLike);
	}

void CSqlLikePredicate::BindL(const RDbTableRow& aSource)
//
// Compile the Pattern for LongText columns and evaluate the match length
//
	{
	TInt matchsize;
	if(iIsEscape)//Following code is for the implementation of limited-ESCAPE-clause 
		{
		matchsize = MatchLengthEscape(Value().Ptr(),Value().End());				
		}
	else
		{
		matchsize = MatchLength(Value().Ptr(),Value().End());	
		}
	CSqlLiteralNode::BindL(aSource);
	switch (ColType())
		{
	default:	// type mismatch: should be caught by Literal::Bind
		__ASSERT(0);
	case EDbColText8:
		break;
	case EDbColLongText8:
		Value().ToPattern8L(iIsEscape);
		break;
	case EDbColLongText16:
		Value().ToPattern16L(iIsEscape);
		matchsize<<=1;
		break;
	case EDbColText16:
		matchsize<<=1;
		break;
		}
	iMatchSize=matchsize;
	}

TBool CSqlLikePredicate::EvaluateL(const TTextOps& aTextOp) const
	{
	__ASSERT(IsBound());
	TDbColType type=ColType();
	TDbColumnC column(Column());
	TInt match;
	if (TDbCol::IsLong(type))
		{
		const TDbBlob& blob=column.Blob();
		if (blob.Size()<iMatchSize)
			match=EFalse;
		else if (blob.IsInline())
			match=type==EDbColLongText8 ? 
				Value().Pattern8().MatchL(blob.PtrC8(),aTextOp) : 
				Value().Pattern16().MatchL(blob.PtrC16(),aTextOp);
		else
			{
			MStreamBuf& buf=StreamLC(blob);
			match=type==EDbColLongText8 ? 
				Value().Pattern8().MatchL(buf,blob.Size(),aTextOp) : 
				Value().Pattern16().MatchL(buf,blob.Size()>>1,aTextOp);
			CleanupStack::PopAndDestroy();
			}
		}
	else if (column.Size()<iMatchSize)
		match=EFalse;
	else
		{
			if(iIsEscape) //Following code is for the implementation of limited-ESCAPE-clause 
			{
			match=type==EDbColText8 ?
				aTextOp.Find(column.PtrC8(),Value().Text8()) :
				aTextOp.Find(column.PtrC16(),Value().Text16());
			}
			else
			{
			match=type==EDbColText8 ?
				aTextOp.Match(column.PtrC8(),Value().Text8()) :
				aTextOp.Match(column.PtrC16(),Value().Text16());
				
			}
		
		match=match>=0;
		}
	return NodeType()==ELike ? match : !match;
	}
