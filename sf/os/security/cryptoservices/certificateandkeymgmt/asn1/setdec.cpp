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
* This file contains the implementation of the ASN1 Set and Set-of object, 
* as described in X208 Sections 22 and 23 and X209 Sections 16 and 17. The 
* set object can contain an ordered series of zero, one or more other ASN1 
* objects. The set-of object contains zero, one or more of the objects 
* described in its definition. The data encodings of both these objects is 
* identical. The ordering of the elements contained within set objects is not
* significant.
* The implementation of sets is pretty much the same as that for sequences 
* (indeed it has started from a straight cut'n'paste) but there are some 
* significant differences with regard to ordering of items within the set,
* comparison and retrieval of items.
*
*/


#include "asn1dec.h"

void TASN1DecSet::CleanupSet(TAny* aArray)
	{
	CArrayPtrFlat<TASN1DecGeneric>* array = REINTERPRET_CAST(CArrayPtrFlat<TASN1DecGeneric>*, aArray);
	array->ResetAndDestroy();
	delete array;
	}

EXPORT_C TASN1DecSet::TASN1DecSet()
	{
	}


EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSet::NewDERLC(const TDesC8& aSource)
// this whole things needs looking at!
	{
	TInt Pos=0;
	CArrayPtrFlat<TASN1DecGeneric>* ret = new(ELeave) CArrayPtrFlat<TASN1DecGeneric> (1);
	TCleanupItem cleanupSeq(TASN1DecSet::CleanupSet, ret);
	CleanupStack::PushL(cleanupSeq);
	TASN1DecGeneric gen(aSource);
	gen.InitL();
	DecodeContentsL(gen.GetContentDER(), Pos, *ret);
	return ret;
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSet::DecodeDERLC(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Ptr=aSource.Right(aSource.Length()-aPos);
	return TASN1DecSet::NewDERLC(Ptr);
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSet::DecodeDERLC(const TASN1DecGeneric& aSource)
	{
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* ret = new(ELeave) CArrayPtrFlat<TASN1DecGeneric> (1);
	TCleanupItem cleanupSeq(TASN1DecSet::CleanupSet, ret);
	CleanupStack::PushL(cleanupSeq);
	DecodeContentsL(aSource.GetContentDER(), pos, *ret);
	return ret;
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSet::DecodeDERLC(const TDesC8& aSource,TInt& aPos, TInt aMin, TInt aMax)

	{
	CArrayPtrFlat<TASN1DecGeneric>* res = DecodeDERLC(aSource, aPos);
	TInt count = res->Count();
	if ((count < aMin) || (count > aMax))
		{
		User::Leave(KErrArgument);
		}
	return res;
	}

EXPORT_C CArrayPtrFlat<TASN1DecGeneric>* TASN1DecSet::DecodeDERLC(const TASN1DecGeneric& aSource,  TInt aMin, TInt aMax)
	{
	CArrayPtrFlat<TASN1DecGeneric>* res = DecodeDERLC(aSource);
	TInt count = res->Count();
	if ((count < aMin) || (count > aMax))
		{
		User::Leave(KErrArgument);
		}
	return res;
	}

void TASN1DecSet::DecodeContentsL(const TDesC8& aSource, TInt& aPos, CArrayPtrFlat<TASN1DecGeneric>& aDest)
	{
	while (aPos < aSource.Length())
		{
		TPtrC8 Ptr=aSource.Right(aSource.Length() - aPos);
		TASN1DecGeneric* Decoded = new(ELeave) TASN1DecGeneric(Ptr);
		CleanupStack::PushL(Decoded);
		Decoded->InitL();
		aDest.AppendL(Decoded);
		aPos+=Decoded->LengthDER();
		CleanupStack::Pop();//i think this should be just 'pop': need to find out
		}
	}

