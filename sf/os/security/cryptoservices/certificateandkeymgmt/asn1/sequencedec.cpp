/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* This file contains the implementation of the ASN1 Sequence and Sequence-of 
* object, as described in X208 Sections 20 and 21 and X209 Sections 14 and 
* 15. The sequence object can contain an ordered series of zero, one or more
* other ASN1 objects. The sequence-of object contains zero, one or more of 
* the objects described in its definition. The data encodings of both these
* objects is identical. The ordering of the objects contained within the 
* sequence is defined outside the scope of this object, normally this would
* be handled by an object deriving from this object but it may be handled
* somewhere else. 
*
*/


#include "asn1dec.h"


void TASN1DecSequence::CleanupSequence(TAny* aArray)
	{
	CArrayPtrFlat<TASN1DecGeneric>* array = REINTERPRET_CAST(CArrayPtrFlat<TASN1DecGeneric>*, aArray);
	array->ResetAndDestroy();
	delete array;
	}

EXPORT_C TASN1DecSequence::TASN1DecSequence()
	{
	}
	
EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSequence::DecodeDERLC(const TDesC8& aSource,TInt& aPos)

	{
	CArrayPtrFlat<TASN1DecGeneric>* ret = new(ELeave) CArrayPtrFlat<TASN1DecGeneric> (1);
	TCleanupItem cleanupSeq(TASN1DecSequence::CleanupSequence, ret);
	CleanupStack::PushL(cleanupSeq);
	TASN1DecGeneric gen(aSource.Right(aSource.Length() - aPos));
	gen.InitL();
	aPos += gen.LengthDER();
	DecodeContentsL(gen.GetContentDER(), *ret);
	return ret;
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSequence::DecodeDERLC(const TASN1DecGeneric& aSource)
	{
	CArrayPtrFlat<TASN1DecGeneric>* ret = new(ELeave) CArrayPtrFlat<TASN1DecGeneric> (1);
	TCleanupItem cleanupSeq(TASN1DecSequence::CleanupSequence, ret);
	CleanupStack::PushL(cleanupSeq);
	DecodeContentsL(aSource.GetContentDER(), *ret);
	return ret;
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSequence::DecodeDERLC(const TDesC8& aSource,TInt& aPos, TInt aMin, TInt aMax)

	{
	CArrayPtrFlat<TASN1DecGeneric>* ret = DecodeDERLC(aSource, aPos);
	TInt count = ret->Count();
	if ((count < aMin) || (count > aMax))
		{
		User::Leave(KErrArgument);
		}
	return ret;
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSequence::DecodeDERLC(const TASN1DecGeneric& aSource, TInt aMin, TInt aMax)
	{
	CArrayPtrFlat<TASN1DecGeneric>* ret = DecodeDERLC(aSource);
	TInt count = ret->Count();
	if ((count < aMin) || (count > aMax))
		{
		User::Leave(KErrArgument);
		}
	return ret;
	}

void TASN1DecSequence::DecodeContentsL(const TDesC8& aSource, CArrayPtrFlat<TASN1DecGeneric>& aDest)
	{
	TInt pos = 0;
	while (pos < aSource.Length())
		{
		TPtrC8 p(aSource.Right(aSource.Length() - pos));
		TASN1DecGeneric* Decoded = new(ELeave) TASN1DecGeneric(p);
		CleanupStack::PushL(Decoded);
		Decoded->InitL();
		aDest.AppendL(Decoded);
		pos+=Decoded->LengthDER();
		CleanupStack::Pop();
		}
	}
