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
* This file contains the implementation of the ASN1 Object ID base class.
*
*/


#include "asn1dec.h"

const TInt KMaxOIDLength = 164;

EXPORT_C TASN1DecObjectIdentifier::TASN1DecObjectIdentifier(void)
	{
	}

EXPORT_C HBufC* TASN1DecObjectIdentifier::DecodeDERL(const TDesC8& aSource,TInt& aPos)
	{
	TASN1DecGeneric gen(aSource.Right(aSource.Length() - aPos));
	gen.InitL();
	HBufC* res = DecodeDERL(gen);
	aPos+=gen.LengthDER();
	return res;
	}

EXPORT_C HBufC* TASN1DecObjectIdentifier::DecodeDERL(const TASN1DecGeneric& aSource)
	{
	TFixedArray<TInt, KNumberOfIDs>* oid = new(ELeave) TFixedArray<TInt, KNumberOfIDs>;
	CleanupStack::PushL(oid);
	TInt count = DecodeContentsL(*oid, aSource.GetContentDER());
	HBufC* res = HBufC::NewLC(KMaxOIDLength);
	TPtr pRes = res->Des();
	if (count > 0)
		{
		pRes.AppendNum((*oid)[0]);
		for (TInt i = 1; i < count; i++)
			{
			pRes.Append('.');
			pRes.AppendNum((*oid)[i]);
			}
		}
	CleanupStack::Pop();//res
	CleanupStack::PopAndDestroy();//oid
	return res;
	}

TInt TASN1DecObjectIdentifier::DecodeContentsL(TFixedArray<TInt, KNumberOfIDs>& aArray, const TDesC8& aSource)
	{
	TInt l = aSource.Length();
	TInt i=0;
	TInt SubID=0;
	TInt SIDn=0;
	if(l < 1)
		{
		User::Leave(KErrArgument);
		}
	SubID+=aSource[i];
	i++;
	if (SubID>=80)
		{
		aArray[SIDn++]=2;
		SubID-=80;
		}
	else
		{
		if (SubID>=40)
			{
			aArray[SIDn++]=1;
			SubID-=40;
			}
		else
			{
			aArray[SIDn++]=0;
			}
		}
	aArray[SIDn++]=SubID;
	for (;i < l;)
		{
		SubID=0;
		while (aSource[i]&0x80)
			{
			// if shifting by 7 does not increase the value its overflowed
			if ( SubID && ((SubID << 7) <= SubID) )
				{
				User::Leave(KErrOverflow);
				}
				
			SubID<<=7;
			SubID+=aSource[i]&0x7f;
			if(i == (l-1))
				{
				User::Leave(KErrArgument);
				}
			i++;
			}
		if ( SubID && ((SubID << 7) <= SubID) )
			{
			User::Leave(KErrOverflow);
			}
		SubID<<=7;
		SubID+=aSource[i];
		
		i++;
		if (SIDn >= KNumberOfIDs) 
			{
			User::Leave(KErrOverflow);
			}
		aArray[SIDn++]=SubID;
		}
	return SIDn;
	}
